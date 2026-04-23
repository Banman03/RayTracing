#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Object.hpp"
#include <cmath>

namespace RayTracing {
    class Sphere : public Object {
    public:
        Ray::Vec3 center;
        double radius;

        Sphere(Ray::Vec3 c, double r) : center(c), radius(r) {}

        std::optional<HitRecord> intersect(const Ray::Ray& ray) const override {
            // Quadratic equation for ray-sphere intersection
            Ray::Vec3 oc = ray.origin - center;
            double a = ray.direction.dot(ray.direction);
            double b = 2.0 * oc.dot(ray.direction);
            double c = oc.dot(oc) - radius * radius;
            double discriminant = b * b - 4 * a * c;

            if (discriminant < 0) return std::nullopt;
            
            double t = (-b - std::sqrt(discriminant)) / (2.0 * a);
            if (t < 0) t = (-b + std::sqrt(discriminant)) / (2.0 * a);
            if (t < 0) return std::nullopt;

            HitRecord rec;
            rec.t = t;
            rec.point = ray.point_at(t);
            rec.normal = (rec.point - center).normalized();
            return rec;
        }
    };
}

#endif
