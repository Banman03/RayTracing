#pragma once
#include "Scene.hpp"
#include "Sphere.hpp"
#include "SceneConfig.hpp"
#include <memory>

// One large sphere on a ground plane
namespace Scenes {
    inline SceneConfig singleSphere(int W, int H) {
        Scene::Scene scene(W, H, {-4,-4,-2}, {4,4,9}, 16,16,22);

        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 0, 0,  5}, 1.5));
        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 0,-101.5,5}, 100.0));

        return SceneConfig(std::move(scene), Camera({0,1,-3}, {0,0,5}, W, H));
    }
}
