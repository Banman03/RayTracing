#pragma once
#include "Scene.hpp"
#include "Sphere.hpp"
#include "SceneConfig.hpp"
#include <memory>
#include <cstdint>

// 5x5 grid of spheres sitting on a ground plane, with deterministic
namespace Scenes {
    inline SceneConfig randomField(int W, int H) {
        Scene::Scene scene(W, H, {-7,-4,-1}, {7,3,10}, 28,14,22);

        uint32_t rng = 42;
        auto next = [&]() -> double {
            rng = rng * 1664525u + 1013904223u;
            return (double)(rng >> 16) / 65535.0;  // [0, 1)
        };

        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                double r  = 0.25 + next() * 0.35;
                double x  = (i - 2) * 2.0 + (next() - 0.5) * 0.5;
                double z  = j * 1.8 + 1.5  + (next() - 0.5) * 0.5;
                double y  = -1.0 + r;
                scene.addSphere(
                    std::make_unique<RayTracing::Sphere>(Ray::Vec3{x, y, z}, r));
            }
        }

        // Ground
        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{0,-101,4}, 100.0));

        return SceneConfig(std::move(scene), Camera({0,4,-5}, {0,0,4}, W, H));
    }
}
