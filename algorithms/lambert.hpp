#ifndef ALGORITHMS_LAMBERT_HPP
#define ALGORITHMS_LAMBERT_HPP

#include "Ray.hpp"
#include "Pixel.hpp"
#include "Scene.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "algorithms/common.hpp"
#include <algorithm>

namespace Algorithms::Lambert {

// Local shading at a confirmed hit point. Exposed so other algorithms
// (e.g. Whitted's local term) can reuse the diffuse model.
inline Pixel::Color shadeHit(const RayTracing::HitRecord& hit,
                             const Scene::Scene& scene,
                             const Light& light)
{
    Ray::Vec3 toLight = light.position - hit.point;
    double    dist    = toLight.norm();
    Ray::Vec3 L       = {toLight.x / dist, toLight.y / dist, toLight.z / dist};

    double NdotL = hit.normal.dot(L);

    bool inShadow = false;
    if (NdotL > 0.0)
        inShadow = occluded(scene, hit.point, hit.normal, L, dist);

    constexpr double ambient = 0.12;
    double diffuse   = (!inShadow && NdotL > 0.0) ? NdotL : 0.0;
    double intensity = ambient + (1.0 - ambient) * diffuse;

    return {
        intensity * (0.5 + 0.5 * hit.normal.x),
        intensity * (0.3 + 0.5 * hit.normal.y),
        intensity * (0.4 + 0.3 * hit.normal.z)
    };
}

// Top-level entry: handles light-sphere visualization, scene miss, and shading.
inline Pixel::Color trace(const Ray::Ray& ray,
                          const Scene::Scene& scene,
                          const Light& light)
{
    auto sceneHit = scene.trace(ray);
    auto lightHit = light.intersect(ray);

    if (lightHit && (!sceneHit || lightHit->t < sceneHit->t))
        return {1.0, 1.0, 1.0};
    if (sceneHit)
        return shadeHit(*sceneHit, scene, light);
    return background(ray);
}

} // namespace Algorithms::Lambert

#endif // ALGORITHMS_LAMBERT_HPP
