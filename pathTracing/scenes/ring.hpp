#pragma once
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Material.hpp"
#include "SceneConfig.hpp"
#include <memory>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Ten colored spheres arranged in a circle, with a mirror sphere at the
// centre. An emissive sphere floats above to act as the area light. Mirror
// of scenes/ring.hpp.
namespace PathTracing::Scenes {

inline SceneConfig ring(int W, int H) {
    Scene scene(W, H, {-7, -4, -1}, {7, 9, 12}, 24, 16, 22);

    constexpr int    N           = 10;
    constexpr double ringRadius  = 3.5;
    constexpr double cx = 0.0, cz = 5.5;

    Pixel::Color palette[10] = {
        {0.85, 0.20, 0.20}, {0.85, 0.55, 0.15}, {0.85, 0.85, 0.20},
        {0.20, 0.80, 0.30}, {0.20, 0.80, 0.75}, {0.20, 0.45, 0.85},
        {0.50, 0.25, 0.85}, {0.85, 0.25, 0.75}, {0.92, 0.92, 0.92},
        {0.30, 0.30, 0.30},
    };

    for (int i = 0; i < N; ++i) {
        double angle = 2.0 * M_PI * i / N;
        double x = cx + ringRadius * std::cos(angle);
        double z = cz + ringRadius * std::sin(angle);
        scene.addSphere(std::make_unique<Sphere>(
            Ray::Vec3{x, 0, z}, 0.55,
            Material::diffuse(palette[i])));
    }

    // Mirror sphere in the center of the ring for nice interreflections.
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{cx, 0.4, cz}, 0.95,
        Material::mirror({0.95, 0.95, 0.97})));

    // Ground.
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{0, -101, cz}, 100.0,
        Material::diffuse({0.70, 0.70, 0.70})));

    // Overhead area light.
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{cx, 6.0, cz}, 1.6,
        Material::light({14.0, 14.0, 12.0})));

    SceneConfig cfg(std::move(scene), Camera({0, 3, -4}, {0, 0, 5.5}, W, H));
    cfg.skyTop    = {0.20, 0.30, 0.55};
    cfg.skyBottom = {0.55, 0.65, 0.85};
    return cfg;
}

} // namespace PathTracing::Scenes
