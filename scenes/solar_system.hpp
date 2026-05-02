#pragma once
#include "Scene.hpp"
#include "Sphere.hpp"
#include "SceneConfig.hpp"
#include <memory>

// Sun + four planets spread in the XZ plane, viewed from above at an angle.
// No ground — open space.
namespace Scenes {
    inline SceneConfig solarSystem(int W, int H) {
        Scene::Scene scene(W, H, {-5,-5,-2}, {15,5,14}, 40,20,32);

        // Sun
        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 0, 0, 5}, 2.5));
        // Mercury
        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 4.5, 0, 5}, 0.30));
        // Venus
        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 2.5, 0,10}, 0.55));
        // Earth
        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{-2,  0, 9}, 0.60));
        // Mars
        scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 8,  0, 9}, 0.40));

        // High-angle camera looking down at the system
        return SceneConfig(std::move(scene), Camera({4,18,-6}, {2,0,6}, W, H, 70.0));
    }
}
