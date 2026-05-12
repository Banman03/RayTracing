#ifndef ALGORITHMS_SOFT_SHADOWS_HPP
#define ALGORITHMS_SOFT_SHADOWS_HPP

#include "Ray.hpp"
#include "Pixel.hpp"
#include "Scene.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "algorithms/common.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Algorithms::SoftShadows {

// Lambert with area-light sampling. Casts N shadow rays toward random points
// on the light's surface and averages the unblocked fraction. The penumbra
// width grows with the light radius.
//
// The RNG is per-pixel-seeded so the noise pattern is stable across frames
// (no flickering when navigating between light positions).
constexpr int N_SAMPLES = 256;

// Uniform random point on the unit sphere (Marsaglia 1972 — rejection on
// the unit disk, then lifted to 3D). Two uniforms in, unit vector out.
inline Ray::Vec3 randomUnitVec(uint32_t& rng) {
    auto next = [&]() -> double {
        rng = rng * 1664525u + 1013904223u;
        return ((rng >> 16) / 65535.0) * 2.0 - 1.0;  // [-1, 1)
    };
    double x1, x2, s;
    do {
        x1 = next();
        x2 = next();
        s  = x1 * x1 + x2 * x2;
    } while (s >= 1.0 || s == 0.0);
    double f = 2.0 * std::sqrt(1.0 - s);
    return { x1 * f, x2 * f, 1.0 - 2.0 * s };
}

inline Pixel::Color shadeHit(const RayTracing::HitRecord& hit,
                             const Scene::Scene& scene,
                             const Light& light,
                             uint32_t rngSeed)
{
    Ray::Vec3 toCenter = light.position - hit.point;
    double    dist0    = toCenter.norm();

    // Continuous shadow factor: average over N rays to random points on the
    // light sphere's surface.
    double unblocked = 0.0;
    uint32_t rng = rngSeed;
    for (int i = 0; i < N_SAMPLES; ++i) {
        Ray::Vec3 sample = light.position + randomUnitVec(rng) * light.radius;
        Ray::Vec3 toSample = sample - hit.point;
        double    dist = toSample.norm();
        Ray::Vec3 L    = {toSample.x / dist, toSample.y / dist, toSample.z / dist};

        if (hit.normal.dot(L) <= 0.0) continue;  // light is below horizon
        if (!occluded(scene, hit.point, hit.normal, L, dist))
            unblocked += 1.0;
    }
    double shadowFactor = unblocked / (double)N_SAMPLES;

    // Diffuse uses the direction to the light center (single dot product
    // is fine — the soft factor is what produces the penumbra).
    Ray::Vec3 Lc = {toCenter.x / dist0, toCenter.y / dist0, toCenter.z / dist0};
    double NdotL = std::max(0.0, hit.normal.dot(Lc));

    constexpr double ambient = 0.12;
    double intensity = ambient + (1.0 - ambient) * NdotL * shadowFactor;

    return {
        intensity * (0.5 + 0.5 * hit.normal.x),
        intensity * (0.3 + 0.5 * hit.normal.y),
        intensity * (0.4 + 0.3 * hit.normal.z)
    };
}

inline Pixel::Color trace(const Ray::Ray& ray,
                          const Scene::Scene& scene,
                          const Light& light)
{
    auto sceneHit = scene.trace(ray);
    auto lightHit = light.intersect(ray);

    if (lightHit && (!sceneHit || lightHit->t < sceneHit->t))
        return {1.0, 1.0, 1.0};
    if (sceneHit) {
        // Stable per-pixel seed: hash the ray direction. This is a rough but
        // cheap way to make the noise consistent across frames.
        uint32_t seed =
            (uint32_t)((ray.direction.x + 2.0) * 73856093.0) ^
            (uint32_t)((ray.direction.y + 2.0) * 19349663.0) ^
            (uint32_t)((ray.direction.z + 2.0) * 83492791.0);
        if (seed == 0) seed = 1;  // LCG must not be seeded with 0 in the bad path
        return shadeHit(*sceneHit, scene, light, seed);
    }
    return background(ray);
}

} // namespace Algorithms::SoftShadows

#endif // ALGORITHMS_SOFT_SHADOWS_HPP
