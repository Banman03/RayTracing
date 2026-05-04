#include <iostream>
#include <functional>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdio>
#include "SceneConfig.hpp"
#include "Pixel.hpp"
#include "Display.hpp"
#include "Light.hpp"

#include "scenes/single_sphere.hpp"
#include "scenes/solar_system.hpp"
#include "scenes/ring.hpp"
#include "scenes/random_field.hpp"

#include "algorithms/lambert.hpp"
#include "algorithms/blinn_phong.hpp"
#include "algorithms/soft_shadows.hpp"
#include "algorithms/whitted.hpp"

// Light position generation

static std::vector<Ray::Vec3> generateLights(int n) {
    uint32_t rng = 9001;
    auto next = [&](double lo, double hi) -> double {
        rng = rng * 1664525u + 1013904223u;
        return lo + ((rng >> 16) / 65535.0) * (hi - lo);
    };
    std::vector<Ray::Vec3> lights(n);
    for (auto& l : lights)
        l = { next(-7, 7), next(3, 12), next(-4, 9) };
    return lights;
}

// Algorithm dispatcher

using TraceFn = std::function<Pixel::Color(const Ray::Ray&,
                                           const Scene::Scene&,
                                           const Light&)>;

static TraceFn pickAlgorithm(const std::string& name) {
    if (name == "lambert") return Algorithms::Lambert::trace;
    if (name == "phong")   return Algorithms::BlinnPhong::trace;
    if (name == "soft")    return Algorithms::SoftShadows::trace;
    if (name == "whitted") return Algorithms::Whitted::trace;
    std::cerr << "Unknown algorithm \"" << name << "\".\n"
              << "Available: lambert  phong  soft  whitted\n";
    std::exit(1);
}

// Scene registry

static SceneConfig buildScene(const std::string& name, int W, int H) {
    if (name == "single")       return Scenes::singleSphere(W, H);
    if (name == "solar_system") return Scenes::solarSystem(W, H);
    if (name == "ring")         return Scenes::ring(W, H);
    if (name == "random_field") return Scenes::randomField(W, H);
    std::cerr << "Unknown scene \"" << name << "\".\n"
              << "Available: single  solar_system  ring  random_field\n";
    std::exit(1);
}

// Main

int main(int argc, char* argv[]) {
    std::string sceneName = "solar_system";
    std::string algoName  = "lambert";

    if (argc == 2) {
        sceneName = argv[1];
    } else if (argc == 3) {
        sceneName = argv[1];
        algoName  = argv[2];
    } else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " [scene [algo]]\n"
                  << "Scenes: single  solar_system  ring  random_field\n"
                  << "Algos:  lambert  phong  soft  whitted\n";
        return 1;
    }

    TraceFn algo = pickAlgorithm(algoName);

    constexpr int W = 800;
    constexpr int H = 900;
    constexpr int NUM_LIGHTS = 20;
    constexpr double lightSphereRadius = 0.35;

    auto lights = generateLights(NUM_LIGHTS);
    SceneConfig cfg = buildScene(sceneName, W, H);
    Display display(W, H, "");

    int  idx  = 0;
    bool quit = false;

    while (!quit) {
        const Ray::Vec3& lightPos = lights[idx];
        Light light{lightPos, lightSphereRadius};

        char title[160];
        std::snprintf(title, sizeof(title),
            "RayTracing — %s [%s]  [%d/%d]  light (%.1f, %.1f, %.1f)  arrows to navigate",
            sceneName.c_str(), algoName.c_str(), idx + 1, NUM_LIGHTS,
            lightPos.x, lightPos.y, lightPos.z);
        display.setTitle(title);

        display.clear();
        display.present();

        bool interrupted = false;
        for (int row = 0; row < H && !quit && !interrupted; ++row) {
            for (int col = 0; col < W; ++col) {
                Ray::Ray ray = cfg.camera.pixelRay(col, row);

                Pixel::Color c = algo(ray, cfg.scene, light);

                int ir = std::clamp((int)(c.r * 255.999), 0, 255);
                int ig = std::clamp((int)(c.g * 255.999), 0, 255);
                int ib = std::clamp((int)(c.b * 255.999), 0, 255);
                display.setPixel(col, row, ir, ig, ib);
            }

            display.present();

            switch (display.pollEvent()) {
                case Event::Quit:      quit         = true; break;
                case Event::PrevScene: idx = (idx + NUM_LIGHTS - 1) % NUM_LIGHTS;
                                       interrupted  = true; break;
                case Event::NextScene: idx = (idx + 1) % NUM_LIGHTS;
                                       interrupted  = true; break;
                case Event::None:      break;
            }
        }

        if (!interrupted && !quit) {
            switch (display.waitEvent()) {
                case Event::Quit:      quit = true; break;
                case Event::PrevScene: idx  = (idx + NUM_LIGHTS - 1) % NUM_LIGHTS; break;
                case Event::NextScene: idx  = (idx + 1) % NUM_LIGHTS; break;
                case Event::None:      break;
            }
        }
    }

    return 0;
}
