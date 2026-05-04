#pragma once
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Material.hpp"
#include "SceneConfig.hpp"
#include <memory>

// One colored sphere on a ground plane plus an overhead emissive sphere.
// Path-tracing version of scenes/single_sphere.hpp — same camera, same
// sphere positions; the area light replaces the point lights from the
// rasterized ray tracer.
namespace PathTracing::Scenes {

inline SceneConfig singleSphere(int W, int H) {
    Scene scene(W, H, {-6, -4, -3}, {6, 12, 12}, 16, 16, 22);

    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{0, 0, 5}, 1.5,
        Material::diffuse({0.85, 0.55, 0.30})));

    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{0, -101.5, 5}, 100.0,
        Material::diffuse({0.75, 0.75, 0.75})));

    scene.addSphere(std::make_unique<Sphere>(
        Ray::Vec3{2.5, 6.0, 4.0}, 1.4,
        Material::light({18.0, 17.0, 14.0})));

    SceneConfig cfg(std::move(scene), Camera({0, 1, -3}, {0, 0, 5}, W, H));
    cfg.skyTop    = {0.50, 0.65, 1.00};
    cfg.skyBottom = {0.85, 0.90, 1.00};
    return cfg;
}

} // namespace PathTracing::Scenes
