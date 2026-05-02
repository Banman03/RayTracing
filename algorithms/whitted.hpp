#ifndef ALGORITHMS_WHITTED_HPP
#define ALGORITHMS_WHITTED_HPP

#include "Ray.hpp"
#include "Pixel.hpp"
#include "Scene.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "algorithms/common.hpp"
#include "algorithms/blinn_phong.hpp"

// Whitted-style recursive ray tracing.
// Each surface is treated as partially reflective (uniform reflectivity
// across the scene — see note below). At every hit we compute a local
// Blinn-Phong term and recursively trace a perfect-mirror reflection ray,
// then linearly blend the two by the reflectivity coefficient.
//
// FUTURE: per-surface reflectivity. Add `double reflectivity` to Sphere and
// propagate it through HitRecord; replace the constant `k` below with
// `hit.reflectivity`. Scene files would then opt in by passing reflectivity
// to each Sphere they construct.
namespace Algorithms::Whitted {

constexpr int    MAX_DEPTH    = 4;
constexpr double REFLECTIVITY = 0.4;

inline Pixel::Color traceImpl(const Ray::Ray& ray,
                              const Scene::Scene& scene,
                              const Light& light,
                              int depth)
{
    auto sceneHit = scene.trace(ray);
    auto lightHit = light.intersect(ray);

    if (lightHit && (!sceneHit || lightHit->t < sceneHit->t))
        return {1.0, 1.0, 1.0};
    if (!sceneHit)
        return background(ray);

    Pixel::Color local = BlinnPhong::shadeHit(*sceneHit, scene, light, ray);

    if (depth <= 0)
        return local;

    // Mirror reflection: R = D - 2(D·N)N
    Ray::Vec3 D = ray.direction;
    Ray::Vec3 N = sceneHit->normal;
    double    DdotN = D.dot(N);
    Ray::Vec3 R = D - N * (2.0 * DdotN);

    Ray::Ray reflectedRay(sceneHit->point + N * 1e-4, R);
    Pixel::Color reflColor = traceImpl(reflectedRay, scene, light, depth - 1);

    return local * (1.0 - REFLECTIVITY) + reflColor * REFLECTIVITY;
}

inline Pixel::Color trace(const Ray::Ray& ray,
                          const Scene::Scene& scene,
                          const Light& light)
{
    return traceImpl(ray, scene, light, MAX_DEPTH);
}

} // namespace Algorithms::Whitted

#endif // ALGORITHMS_WHITTED_HPP
