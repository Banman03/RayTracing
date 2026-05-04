#pragma once
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Material.hpp"
#include "SceneConfig.hpp"
#include <memory>

// Sun (emissive) plus four diffuse planets in the XZ plane. Mirror of
// scenes/solar_system.hpp; the Sun is the only light source so the
// rendering is naturally illuminated by it. No sky.
namespace PathTracing::Scenes {

inline SceneConfig solarSystem(int W, int H, Ray::Vec3 lightPos = {0.0, 0.0, 5.0}) {
    Scene scene(W, H, {-6, -6, -3}, {16, 6, 15}, 40, 20, 32);

    // Sun — emissive. The numbers above 1.0 are radiance, not albedo.
    scene.addSphere(std::make_unique<Sphere>(
        lightPos, 2.5,
        Material::light({9.0, 7.5, 4.0})));

    // Mercury (grey)
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{4.5, 0, 5}, 0.30,
        Material::diffuse({0.55, 0.55, 0.55})));

    // Venus (warm cream)
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{2.5, 0, 10}, 0.55,
        Material::diffuse({0.92, 0.78, 0.55})));

    // Earth (blue-green)
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{-2, 0, 9}, 0.60,
        Material::diffuse({0.25, 0.45, 0.85})));

    // Mars (rust)
    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{8, 0, 9}, 0.40,
        Material::diffuse({0.78, 0.32, 0.22})));

    SceneConfig cfg(std::move(scene), Camera({4, 18, -6}, {2, 0, 6}, W, H, 70.0));
    cfg.skyTop    = {0.0, 0.0, 0.0};
    cfg.skyBottom = {0.0, 0.0, 0.0};
    return cfg;
}

} // namespace PathTracing::Scenes
