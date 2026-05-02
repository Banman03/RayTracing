#ifndef SCENE_CONFIG_HPP
#define SCENE_CONFIG_HPP

#include "Scene.hpp"
#include "Camera.hpp"

struct SceneConfig {
    Scene::Scene scene;
    Camera       camera;

    SceneConfig(Scene::Scene s, Camera c)
        : scene(std::move(s)), camera(std::move(c)) {}

    SceneConfig(SceneConfig&&)            = default;
    SceneConfig& operator=(SceneConfig&&) = default;
};

#endif // SCENE_CONFIG_HPP
