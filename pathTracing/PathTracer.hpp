#ifndef PT_PATH_TRACER_HPP
#define PT_PATH_TRACER_HPP

#include "Ray.hpp"
#include "Pixel.hpp"
#include "Material.hpp"
#include "SceneConfig.hpp"
#include "Random.hpp"
#include <algorithm>
#include <cmath>

namespace PathTracing {

// Monte Carlo path tracer following §1.2 of monte_carlo_path_tracing.pdf.
//
// The estimator follows the rendering equation
//     L(x, ω) = Le(x, ω) + ∫ fr(x, ωi, ω) L(x', ωi') cos θi dωi
// and turns the recursive integral into an iterative random walk:
//
//     trace(ray):
//       L = 0; throughput = 1
//       loop:
//         hit = intersect(ray)
//         if no hit: L += throughput * sky;  break
//         L += throughput * Le                              (paper Step 3A)
//         apply Russian-Roulette absorption with prob 1 - p
//         throughput *= ρ / p                               (paper Step 3B)
//         sample new direction ω' from BRDF
//           Lambertian: cosine-weighted hemisphere — fr·cos/pdf = 1
//           Specular  : mirror reflection            — fr·cos/pdf = 1
//
// The continuation probability p is derived from the surface reflectance
// (a "max-channel reflectance" Russian-Roulette) which keeps the estimator
// unbiased while guaranteeing termination.
class PathTracer {
    // Held by pointer rather than reference so the tracer remains
    // move-assignable when the caller swaps in a new scene.
    const SceneConfig* _cfg;
    int                _maxDepth;
    int                _rrStart;

    // Smooth top-to-bottom sky used as the environment radiance when a ray
    // escapes the scene. Mirrors the gradient in src/main.cpp's `background`.
    Pixel::Color skyEmission(const Ray::Vec3& dir) const {
        double t = 0.5 * (dir.y + 1.0);                 // [0, 1]
        return _cfg->skyBottom * (1.0 - t) + _cfg->skyTop * t;
    }

public:
    explicit PathTracer(const SceneConfig& cfg, int maxDepth = 8, int rrStart = 3)
        : _cfg(&cfg), _maxDepth(maxDepth), _rrStart(rrStart) {}

    Pixel::Color trace(Ray::Ray ray, RNG& rng) const {
        Pixel::Color L            = {0.0, 0.0, 0.0};
        Pixel::Color throughput   = {1.0, 1.0, 1.0};
        bool         lastSpecular = true;  // camera ray: always see emitters directly

        for (int depth = 0; depth < _maxDepth; ++depth) {
            auto hit = _cfg->scene.trace(ray);
            if (!hit) {
                L = L + hadamard(throughput, skyEmission(ray.direction));
                break;
            }

            const Material* m = hit->material;

            // Add emission only on camera rays or after specular bounces.
            // After diffuse bounces the light is already counted by NEE below.
            if (lastSpecular)
                L = L + hadamard(throughput, m->emission);

            // Russian-Roulette absorption based on max-channel ρ.
            double p = std::max({m->albedo.r, m->albedo.g, m->albedo.b});
            p = std::clamp(p, 0.0, 0.95);
            if (depth >= _rrStart) {
                if (p <= 0.0 || rng.next() > p) break;
                throughput = throughput * (1.0 / p);
            }

            // Fold albedo into throughput. After this line throughput includes
            // this surface's reflectance, which is what sampleLight needs.
            throughput = hadamard(throughput, m->albedo);

            Ray::Vec3 dir;
            if (m->type == BRDF::Specular) {
                dir = reflect(ray.direction, hit->normal);
                lastSpecular = true;
            } else {
                // NEE: explicitly sample each area light for direct illumination.
                for (const Sphere* light : _cfg->scene.lights())
                    sampleLight(*hit, light, throughput, rng, L);

                dir = cosineHemisphere(hit->normal, rng);
                lastSpecular = false;
            }

            ray = Ray::Ray(hit->point + hit->normal * 1e-4, dir);
        }

        return L;
    }

private:
    // Direct-lighting estimator for one area light (spherical emitter).
    // Samples a uniform random point on the light surface and computes the
    // unoccluded contribution using the area-form rendering equation:
    //   L_direct = fr * Le * cos(θ) * cos(θ') * A / dist²
    // where fr = albedo/π (Lambertian) is already folded into `throughput`
    // (albedo part) so we divide by π here.
    void sampleLight(const HitRecord& hit, const Sphere* light,
                     const Pixel::Color& throughput, RNG& rng,
                     Pixel::Color& L) const
    {
        Ray::Vec3 lightPt = light->center + uniformSphere(rng) * light->radius;
        Ray::Vec3 toLight = lightPt - hit.point;
        double    dist    = toLight.norm();
        if (dist < 1e-8) return;
        Ray::Vec3 dir = toLight * (1.0 / dist);

        double cosAtHit = hit.normal.dot(dir);
        if (cosAtHit <= 0.0) return;  // light below surface horizon

        // Outward normal at the sampled point on the light sphere.
        Ray::Vec3 lightNormal = (lightPt - light->center) * (1.0 / light->radius);
        double cosAtLight = lightNormal.dot(dir * -1.0);
        if (cosAtLight <= 0.0) return;  // sampled the back face

        // Visibility: any opaque surface between hit point and light sample?
        Ray::Ray shadowRay(hit.point + hit.normal * 1e-4, dir);
        auto blocker = _cfg->scene.trace(shadowRay);
        if (blocker && blocker->t < dist - 1e-3) return;

        double area   = 4.0 * M_PI * light->radius * light->radius;
        double weight = cosAtHit * cosAtLight * area / (M_PI * dist * dist);
        L = L + hadamard(throughput, light->material.emission) * weight;
    }
};

} // namespace PathTracing

#endif // PT_PATH_TRACER_HPP
