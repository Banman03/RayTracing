#ifndef PT_OBJECT_HPP
#define PT_OBJECT_HPP

#include "Ray.hpp"
#include "Material.hpp"
#include <optional>

namespace PathTracing {

// Hit record carries a non-owning pointer to the surface's material so that
// the integrator can access ρ, Le, and the BRDF type without re-querying the
// scene. The pointer references storage owned by the Sphere/Scene.
struct HitRecord {
    double           t;
    Ray::Vec3        point;
    Ray::Vec3        normal;
    const Material*  material;
};

class Object {
public:
    virtual ~Object() = default;
    virtual std::optional<HitRecord> intersect(const Ray::Ray& ray) const = 0;
};

} // namespace PathTracing

#endif // PT_OBJECT_HPP
