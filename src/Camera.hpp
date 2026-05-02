#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Ray.hpp"
#include <cmath>

struct Camera {
    Ray::Vec3 eye;
    Ray::Vec3 topLeft;
    Ray::Vec3 stepX;
    Ray::Vec3 stepY;

    // E: eye position, T: target, fovDeg: horizontal FOV.
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

    Ray::Ray pixelRay(int col, int row) const {
        Ray::Vec3 dir = topLeft + stepX * col + stepY * row;
        return Ray::Ray(eye, dir);
    }
};

#endif // CAMERA_HPP
