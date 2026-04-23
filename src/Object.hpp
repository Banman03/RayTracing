#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Ray.hpp"
#include "Pixel.hpp"
#include <optional>

namespace RayTracing {
    struct HitRecord {
        double t;
        Ray::Vec3 point;
        Ray::Vec3 normal;
    };

    class Object {
    public:
        virtual ~Object() = default;
        virtual std::optional<HitRecord> intersect(const Ray::Ray& ray) const = 0;
    };
}

#endif
