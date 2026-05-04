#ifndef PT_CAMERA_HPP
#define PT_CAMERA_HPP

#include "Ray.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PathTracing {

// Pinhole camera. Identical math to src/Camera.hpp but `pixelRay` accepts
// floating-point pixel coordinates so the path tracer can jitter inside a
// pixel for sub-pixel anti-aliasing (paper §1.1: "Sampling a pixel over
// (x, y) prefilters the image and reduces aliasing").
struct Camera {
    Ray::Vec3 eye;
    Ray::Vec3 topLeft;
    Ray::Vec3 stepX;
    Ray::Vec3 stepY;

    Camera(Ray::Vec3 E, Ray::Vec3 T, int width, int height, double fovDeg = 90.0)
        : eye(E)
    {
        Ray::Vec3 worldUp = {0, 1, 0};
        Ray::Vec3 tn = (T - E).normalized();
        Ray::Vec3 bn = worldUp.cross(tn).normalized();
        Ray::Vec3 vn = tn.cross(bn);

        double gx = std::tan(fovDeg * M_PI / 360.0);
        double gy = gx * (double)(height - 1) / (double)(width - 1);

        topLeft = tn - bn * gx + vn * gy;
        stepX   = bn * ( 2.0 * gx / (width  - 1));
        stepY   = vn * (-2.0 * gy / (height - 1));
    }

    Ray::Ray pixelRay(double col, double row) const {
        Ray::Vec3 dir = topLeft + stepX * col + stepY * row;
        return Ray::Ray(eye, dir);
    }
};

} // namespace PathTracing

#endif // PT_CAMERA_HPP
