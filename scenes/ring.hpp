#pragma once
#include "Scene.hpp"
#include "Sphere.hpp"
#include "SceneConfig.hpp"
#include <memory>
#include <cmath>

// Ten spheres arranged in a circle on a ground plane.
namespace Scenes {
    inline SceneConfig ring(int W, int H) {
        Scene::Scene scene(W, H, {-6,-4,0}, {6,4,11}, 24,16,22);

        constexpr int N = 10;
        constexpr double ringRadius = 3.5;
        constexpr double cx = 0, cz = 5.5; // centre of ring in XZ

        for (int i = 0; i < N; ++i) {
            double angle = 2.0 * M_PI * i / N;
            double x = cx + ringRadius * std::cos(angle);
            double z = cz + ringRadius * std::sin(angle);
            scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{x, 0, z}, 0.55));
        }

        // Ground
        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{0,-101,cz}, 100.0));

        return SceneConfig(std::move(scene), Camera({0,3,-4}, {0,0,5.5}, W, H));
    }
}
