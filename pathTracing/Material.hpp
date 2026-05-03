#ifndef PT_MATERIAL_HPP
#define PT_MATERIAL_HPP

#include "Pixel.hpp"

namespace PathTracing {

// BRDF families described in §1.2 of monte_carlo_path_tracing.pdf:
//   D - diffuse, S - specular. Glossy is not modelled here.
enum class BRDF { Diffuse, Specular };

// A surface's material data: reflectance ρ (albedo) and emission Le.
// An emissive surface is the only kind of light source the path tracer
// recognizes — see paper §1.2 ("we assume all light sources are
// described by emission terms attached to surfaces").
struct Material {
    Pixel::Color albedo   = {0.0, 0.0, 0.0};
    Pixel::Color emission = {0.0, 0.0, 0.0};
    BRDF         type     = BRDF::Diffuse;

    static Material diffuse(Pixel::Color albedo) {
        Material m; m.albedo = albedo; m.type = BRDF::Diffuse;  return m;
    }
    static Material mirror(Pixel::Color albedo) {
        Material m; m.albedo = albedo; m.type = BRDF::Specular; return m;
    }
    static Material light(Pixel::Color emission) {
        Material m; m.emission = emission; m.type = BRDF::Diffuse; return m;
    }
};

// Component-wise color multiplication (used to fold albedo into throughput).
inline Pixel::Color hadamard(const Pixel::Color& a, const Pixel::Color& b) {
    return {a.r * b.r, a.g * b.g, a.b * b.b};
}

} // namespace PathTracing

#endif // PT_MATERIAL_HPP
