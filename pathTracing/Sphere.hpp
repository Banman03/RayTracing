#ifndef PT_SPHERE_HPP
#define PT_SPHERE_HPP

#include "Object.hpp"
#include <cmath>

namespace PathTracing {

class Sphere : public Object {
public:
    Ray::Vec3 center;
    double    radius;
    Material  material;

    Sphere(Ray::Vec3 c, double r, Material m)
        : center(c), radius(r), material(std::move(m)) {}

    std::optional<HitRecord> intersect(const Ray::Ray& ray) const override {
        Ray::Vec3 oc = ray.origin - center;
        double a = ray.direction.dot(ray.direction);
        double b = 2.0 * oc.dot(ray.direction);
        double c = oc.dot(oc) - radius * radius;
        double disc = b * b - 4.0 * a * c;
        if (disc < 0.0) return std::nullopt;

        // Reject hits that are essentially at the ray origin (self-intersection
        // after a scatter event). Without this, a freshly scattered ray would
        // immediately re-hit the surface it just left.
        constexpr double kEps = 1e-4;
        double sqrtDisc = std::sqrt(disc);
        double t = (-b - sqrtDisc) / (2.0 * a);
        if (t < kEps) t = (-b + sqrtDisc) / (2.0 * a);
        if (t < kEps) return std::nullopt;

        HitRecord rec;
        rec.t        = t;
        rec.point    = ray.point_at(t);
        rec.normal   = (rec.point - center).normalized();
        rec.material = &material;
        return rec;
    }
};

} // namespace PathTracing

#endif // PT_SPHERE_HPP
