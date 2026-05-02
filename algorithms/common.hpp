#ifndef ALGORITHMS_COMMON_HPP
#define ALGORITHMS_COMMON_HPP

#include "Ray.hpp"
#include "Pixel.hpp"
#include "Scene.hpp"

namespace Algorithms {

// Sky-blue gradient keyed on the ray direction's y component.
// Used by every algorithm when a ray escapes the scene.
inline Pixel::Color background(const Ray::Ray& ray) {
    double t = 0.5 * (ray.direction.y + 1.0);
    return {1.0 - 0.5 * t, 1.0 - 0.3 * t, 1.0};
}

// Standard shadow-ray test. The origin is offset along `normal` to avoid
// self-intersection. Returns true if any object lies between origin and a
// surface at distance `maxDist` along `dir` (a normalized direction).
inline bool occluded(const Scene::Scene& scene,
                     const Ray::Vec3& origin,
                     const Ray::Vec3& normal,
                     const Ray::Vec3& dir,
                     double maxDist)
{
    Ray::Vec3 shadowOrig = origin + normal * 1e-4;
    Ray::Ray  shadowRay(shadowOrig, dir);
    if (auto blocker = scene.trace(shadowRay))
        return blocker->t < maxDist - 1e-4;
    return false;
}

} // namespace Algorithms

#endif // ALGORITHMS_COMMON_HPP
