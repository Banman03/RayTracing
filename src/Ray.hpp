#ifndef RAY_HPP
#define RAY_HPP

#include <cmath>

namespace Ray {
    struct Vec3 {
        double x, y, z;

        Vec3 operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
        Vec3 operator-(const Vec3& other) const { return {x - other.x, y - other.y, z - other.z}; }
        Vec3 operator*(double scalar) const { return {x * scalar, y * scalar, z * scalar}; }
        
        double dot(const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }
        Vec3 cross(const Vec3& other) const {
            return {
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            };
        }
        double norm() const { return std::sqrt(x * x + y * y + z * z); }
        Vec3 normalized() const {
            double n = norm();
            return {x / n, y / n, z / n};
        }
    };

    class Ray {
    public:
        Vec3 origin;
        Vec3 direction;

        Ray(Vec3 o, Vec3 d) : origin(o), direction(d.normalized()) {}

        Vec3 point_at(double t) const {
            return origin + direction * t;
        }
    };
}

#endif // RAY_HPP
