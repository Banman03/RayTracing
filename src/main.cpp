#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>
#include "SceneConfig.hpp"
#include "Pixel.hpp"
#include "Display.hpp"

#include "scenes/single_sphere.hpp"
#include "scenes/solar_system.hpp"
#include "scenes/ring.hpp"
#include "scenes/random_field.hpp"

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

// Shading

static Pixel::Color background(const Ray::Ray& ray) {
    double t = 0.5 * (ray.direction.y + 1.0);
    return {1.0 - 0.5 * t, 1.0 - 0.3 * t, 1.0};
}

static Pixel::Color shade(const RayTracing::HitRecord& hit,
                          const Ray::Vec3& lightPos,
                          const Scene::Scene& scene)
{
    Ray::Vec3 toLight   = lightPos - hit.point;
    double    distLight = toLight.norm();
    Ray::Vec3 L = {toLight.x / distLight,
                   toLight.y / distLight,
                   toLight.z / distLight};

    double NdotL = hit.normal.x * L.x +
                   hit.normal.y * L.y +
                   hit.normal.z * L.z;

    bool inShadow = false;
    if (NdotL > 0.0) {
        Ray::Vec3 shadowOrig = hit.point + hit.normal * 1e-4;
        Ray::Ray  shadowRay(shadowOrig, L);
        if (auto blocker = scene.trace(shadowRay))
            if (blocker->t < distLight - 1e-4)
                inShadow = true;
    }

    double ambient   = 0.12;
    double diffuse   = (!inShadow && NdotL > 0.0) ? NdotL : 0.0;
    double intensity = ambient + (1.0 - ambient) * diffuse;

    double r = intensity * (0.5 + 0.5 * hit.normal.x);
    double g = intensity * (0.3 + 0.5 * hit.normal.y);
    double b = intensity * (0.4 + 0.3 * hit.normal.z);
    return {r, g, b};
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
    if (argc == 2) {
        sceneName = argv[1];
    } else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " [scene]\n"
                  << "Scenes: single  solar_system  ring  random_field\n";
        return 1;
    }

    constexpr int W = 800;
    constexpr int H = 450;
    constexpr int NUM_LIGHTS = 20;
    constexpr double lightSphereRadius = 0.35;

    auto lights = generateLights(NUM_LIGHTS);
    SceneConfig cfg = buildScene(sceneName, W, H);
    Display display(W, H, "");

    int  idx  = 0;
    bool quit = false;

    while (!quit) {
        const Ray::Vec3& lightPos = lights[idx];
        RayTracing::Sphere lightSphere(lightPos, lightSphereRadius);

        char title[128];
        std::snprintf(title, sizeof(title),
            "RayTracing — %s  [%d/%d]  light (%.1f, %.1f, %.1f)  ← → to navigate",
            sceneName.c_str(), idx + 1, NUM_LIGHTS,
            lightPos.x, lightPos.y, lightPos.z);
        display.setTitle(title);

        display.clear();
        display.present();

        bool interrupted = false;
        for (int row = 0; row < H && !quit && !interrupted; ++row) {
            for (int col = 0; col < W; ++col) {
                Ray::Ray ray = cfg.camera.pixelRay(col, row);

                auto sceneHit = cfg.scene.trace(ray);
                auto lightHit = lightSphere.intersect(ray);

                Pixel::Color c;
                if (lightHit && (!sceneHit || lightHit->t < sceneHit->t))
                    c = {1.0, 1.0, 1.0};
                else if (sceneHit)
                    c = shade(*sceneHit, lightPos, cfg.scene);
                else
                    c = background(ray);

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
