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
        Pixel::Color L          = {0.0, 0.0, 0.0};
        Pixel::Color throughput = {1.0, 1.0, 1.0};

        for (int depth = 0; depth < _maxDepth; ++depth) {
            auto hit = _cfg->scene.trace(ray);
            if (!hit) {
                L = L + hadamard(throughput, skyEmission(ray.direction));
                break;
            }

            const Material* m = hit->material;

            // Step 3A — accumulate emission from the surface we just hit.
            L = L + hadamard(throughput, m->emission);

            // Step 3B — Russian-Roulette absorption based on max-channel ρ.
            // Always continue for the first few bounces to avoid dimming.
            double p = std::max({m->albedo.r, m->albedo.g, m->albedo.b});
            p = std::clamp(p, 0.0, 0.95);
            if (depth >= _rrStart) {
                if (p <= 0.0 || rng.next() > p) break;
                throughput = throughput * (1.0 / p);
            }

            // Fold the surface reflectance into throughput. For both
            // Lambertian (cosine-weighted) and mirror BRDFs the
            // fr·cos(θ)/pdf factor equals 1, so only ρ remains.
            throughput = hadamard(throughput, m->albedo);

            // Sample the next direction.
            Ray::Vec3 dir;
            switch (m->type) {
                case BRDF::Specular:
                    dir = reflect(ray.direction, hit->normal);
                    break;
                case BRDF::Diffuse:
                default:
                    dir = cosineHemisphere(hit->normal, rng);
                    break;
            }

            // Offset the new origin along the normal to dodge self-intersection.
            Ray::Vec3 origin = hit->point + hit->normal * 1e-4;
            ray = Ray::Ray(origin, dir);
        }

        return L;
    }
};

} // namespace PathTracing

#endif // PT_PATH_TRACER_HPP
