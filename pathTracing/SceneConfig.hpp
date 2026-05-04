#ifndef PT_SCENE_CONFIG_HPP
#define PT_SCENE_CONFIG_HPP

#include "Scene.hpp"
#include "Camera.hpp"
#include "Pixel.hpp"

namespace PathTracing {

// A scene's geometry, camera, and the environment radiance the integrator
// should return when a ray escapes the scene without hitting any surface.
// `skyTop` is the radiance towards +Y, `skyBottom` is towards -Y; the
// integrator interpolates between them along ray.direction.y. Set both to
// {0,0,0} for a closed scene that is illuminated only by emissive surfaces.
struct SceneConfig {
    Scene        scene;
    Camera       camera;
    Pixel::Color skyTop    = {0.0, 0.0, 0.0};
    Pixel::Color skyBottom = {0.0, 0.0, 0.0};

    SceneConfig(Scene s, Camera c)
        : scene(std::move(s)), camera(std::move(c)) {}

    SceneConfig(SceneConfig&&)            = default;
    SceneConfig& operator=(SceneConfig&&) = default;
};

} // namespace PathTracing

#endif // PT_SCENE_CONFIG_HPP
