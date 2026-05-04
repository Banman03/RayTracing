#ifndef PT_RANDOM_HPP
#define PT_RANDOM_HPP

#include "Ray.hpp"
#include <cmath>
#include <cstdint>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PathTracing {

// Per-thread Mersenne Twister wrapper with helpers for the unit interval and
// for cosine-weighted hemisphere sampling.
struct RNG {
    std::mt19937_64 engine;
    std::uniform_real_distribution<double> uniform{0.0, 1.0};

    explicit RNG(uint64_t seed = 0x9E3779B97F4A7C15ull) : engine(seed) {}

    double next() { return uniform(engine); }
};

// Build an orthonormal basis (u, v, w) with `w == n`. Frisvad's branchless
// construction would be slightly faster, but the cross-product version is
// easier to read and only runs once per scatter.
inline void orthonormalBasis(const Ray::Vec3& n, Ray::Vec3& u, Ray::Vec3& v, Ray::Vec3& w) {
    w = n;
    Ray::Vec3 a = (std::abs(w.x) > 0.9) ? Ray::Vec3{0, 1, 0} : Ray::Vec3{1, 0, 0};
    v = w.cross(a).normalized();
    u = w.cross(v);
}

// Cosine-weighted hemisphere sample around `n`. The pdf is cos(θ)/π, which
// matches the BRDF*cos factor for a Lambertian surface and so cancels in
// the path-tracing estimator (paper §1.3 mentions this is the natural
// distribution for diffuse sampling).
inline Ray::Vec3 cosineHemisphere(const Ray::Vec3& n, RNG& rng) {
    double u1 = rng.next();
    double u2 = rng.next();
    double r   = std::sqrt(u1);
    double phi = 2.0 * M_PI * u2;
    double x = r * std::cos(phi);
    double y = r * std::sin(phi);
    double z = std::sqrt(std::max(0.0, 1.0 - u1));

    Ray::Vec3 ub, vb, wb;
    orthonormalBasis(n, ub, vb, wb);
    return (ub * x + vb * y + wb * z).normalized();
}

// Mirror reflection: r = d - 2(d·n)n.
inline Ray::Vec3 reflect(const Ray::Vec3& d, const Ray::Vec3& n) {
    return d - n * (2.0 * d.dot(n));
}

} // namespace PathTracing

#endif // PT_RANDOM_HPP
