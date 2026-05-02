#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Ray.hpp"
#include <cmath>

struct Camera {
    Ray::Vec3 eye;
    Ray::Vec3 topLeft;  // direction to the top-left pixel
    Ray::Vec3 stepX;    // per-column step (rightward)
    Ray::Vec3 stepY;    // per-row    step (downward)

    // E: eye position, T: target, fovDeg: horizontal FOV.
    // Derivation follows math/overview.md (t_n, b_n, v_n basis).
    Camera(Ray::Vec3 E, Ray::Vec3 T, int width, int height, double fovDeg = 90.0)
        : eye(E)
    {
        Ray::Vec3 worldUp = {0, 1, 0};
        Ray::Vec3 tn = (T - E).normalized();           // forward  (t_n)
        Ray::Vec3 bn = worldUp.cross(tn).normalized(); // right    (b_n)
        Ray::Vec3 vn = tn.cross(bn);                   // true up  (v_n)

        double gx = std::tan(fovDeg * M_PI / 360.0);
        double gy = gx * (double)(height - 1) / (double)(width - 1);

        topLeft = tn - bn * gx + vn * gy;
        stepX   = bn * ( 2.0 * gx / (width  - 1));
        stepY   = vn * (-2.0 * gy / (height - 1));  // negate: rows go top-to-bottom
    }

    Ray::Ray pixelRay(int col, int row) const {
        Ray::Vec3 dir = topLeft + stepX * col + stepY * row;
        return Ray::Ray(eye, dir);
    }
};

#endif // CAMERA_HPP
