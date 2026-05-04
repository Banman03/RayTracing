#pragma once
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Material.hpp"
#include "SceneConfig.hpp"
#include <memory>
#include <cstdint>

// 5x5 grid of randomized colored spheres on a ground plane, with two
// emissive lights overhead. Mirror of scenes/random_field.hpp.
namespace PathTracing::Scenes {

inline SceneConfig randomField(int W, int H) {
    Scene scene(W, H, {-8, -4, -2}, {8, 8, 11}, 28, 14, 22);

    uint32_t rng = 42;
    auto next = [&]() -> double {
        rng = rng * 1664525u + 1013904223u;
        return (double)(rng >> 16) / 65535.0;          // [0, 1)
    };

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            double r = 0.25 + next() * 0.35;
            double x = (i - 2) * 2.0 + (next() - 0.5) * 0.5;
            double z = j * 1.8 + 1.5  + (next() - 0.5) * 0.5;
            double y = -1.0 + r;

            // Random saturated color.
            Pixel::Color albedo{
                0.30 + 0.65 * next(),
                0.30 + 0.65 * next(),
                0.30 + 0.65 * next()
            };

            // Roughly one in seven spheres is a mirror.
            Material mat = (next() < 0.15)
                ? Material::mirror({0.95, 0.95, 0.95})
                : Material::diffuse(albedo);

            scene.addSphere(std::make_unique<Sphere>(Ray::Vec3{x, y, z}, r, mat));
        }
    }

    // Ground.
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{0, -101, 4}, 100.0,
        Material::diffuse({0.65, 0.65, 0.70})));

    // Two emissive lights for nicer shadows.
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{-4, 5.5, 3.5}, 1.5,
        Material::light({16.0, 12.0, 8.0})));
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{4.5, 5.5, 6.5}, 1.5,
        Material::light({8.0, 12.0, 16.0})));

    SceneConfig cfg(std::move(scene), Camera({0, 4, -5}, {0, 0, 4}, W, H));
    cfg.skyTop    = {0.18, 0.22, 0.40};
    cfg.skyBottom = {0.40, 0.45, 0.60};
    return cfg;
}

} // namespace PathTracing::Scenes
