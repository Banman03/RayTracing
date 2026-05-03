#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Ray.hpp"
#include "Object.hpp"
#include "Sphere.hpp"
#include <optional>

// Point light with a finite radius.
// The radius is used both for visualization (the white sphere drawn at the
// light position) and for area-light sampling in the soft-shadows algorithm.
struct Light {
    Ray::Vec3 position;
    double    radius;

    // Visualization-sphere intersection. Used by every algorithm to draw a
    // white emissive disc where the light is, before falling through to scene
    // shading or the sky background.
    std::optional<RayTracing::HitRecord> intersect(const Ray::Ray& ray) const {
        return RayTracing::Sphere(position, radius).intersect(ray);
    }
};

#endif // LIGHT_HPP
