#ifndef ALGORITHMS_BLINN_PHONG_HPP
#define ALGORITHMS_BLINN_PHONG_HPP

#include "Ray.hpp"
#include "Pixel.hpp"
#include "Scene.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "algorithms/common.hpp"
#include <algorithm>
#include <cmath>

namespace Algorithms::BlinnPhong {

// Blinn-Phong: Lambert diffuse + a specular highlight using the halfway
// vector H = normalize(L + V). Cheaper than classic Phong (no reflection
// vector) and visually equivalent for most cases.
inline Pixel::Color shadeHit(const RayTracing::HitRecord& hit,
                             const Scene::Scene& scene,
                             const Light& light,
                             const Ray::Ray& ray)
{
    Ray::Vec3 toLight = light.position - hit.point;
    double    dist    = toLight.norm();
    Ray::Vec3 L       = {toLight.x / dist, toLight.y / dist, toLight.z / dist};

    double NdotL = hit.normal.dot(L);

    bool inShadow = false;
    if (NdotL > 0.0)
        inShadow = occluded(scene, hit.point, hit.normal, L, dist);

    constexpr double ambient   = 0.12;
    constexpr double shininess = 64.0;
    constexpr double specStrength = 0.6;

    double diffuse  = (!inShadow && NdotL > 0.0) ? NdotL : 0.0;
    double specular = 0.0;
    if (!inShadow && NdotL > 0.0) {
        // V points from hit toward camera, opposite the incoming ray.
        Ray::Vec3 V = ray.direction * -1.0;
        Ray::Vec3 H = (L + V).normalized();
        double NdotH = std::max(0.0, hit.normal.dot(H));
        specular = std::pow(NdotH, shininess);
    }

    double diffuseIntensity = ambient + (1.0 - ambient) * diffuse;
    double s = specular * specStrength;

    return {
        diffuseIntensity * (0.5 + 0.5 * hit.normal.x) + s,
        diffuseIntensity * (0.3 + 0.5 * hit.normal.y) + s,
        diffuseIntensity * (0.4 + 0.3 * hit.normal.z) + s
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
    if (sceneHit)
        return shadeHit(*sceneHit, scene, light, ray);
    return background(ray);
}

} // namespace Algorithms::BlinnPhong

#endif // ALGORITHMS_BLINN_PHONG_HPP
