#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Pixel.hpp"
#include "Display.hpp"

#include "SceneConfig.hpp"
#include "PathTracer.hpp"
#include "Random.hpp"

#include "scenes/single_sphere.hpp"
#include "scenes/solar_system.hpp"
#include "scenes/ring.hpp"
#include "scenes/random_field.hpp"

namespace pt = PathTracing;

constexpr int W            = 800;
constexpr int H            = 900;
constexpr int MAX_DEPTH    = 6;
constexpr int RR_START     = 3;
constexpr int MAX_SAMPLES  = 4096;
constexpr int PRESENT_ROWS = 32;
constexpr int NUM_LIGHTS   = 20;

// Same LCG seed as the ray tracer so light positions are comparable.
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

// Reinhard tone-map + gamma 2.2.
static int toSRGB(double x) {
    if (x < 0.0) x = 0.0;
    x = x / (1.0 + x);
    x = std::pow(x, 1.0 / 2.2);
    return std::clamp((int)(x * 255.999), 0, 255);
}

static pt::SceneConfig buildScene(const std::string& name, Ray::Vec3 lightPos) {
    if (name == "single")       return pt::Scenes::singleSphere(W, H, lightPos);
    if (name == "solar_system") return pt::Scenes::solarSystem(W, H, lightPos);
    if (name == "ring")         return pt::Scenes::ring(W, H, lightPos);
    if (name == "random_field") return pt::Scenes::randomField(W, H, lightPos);
    std::cerr << "Unknown scene \"" << name << "\".\n"
              << "Available: single  solar_system  ring  random_field\n";
    std::exit(1);
}

int main(int argc, char* argv[]) {
    std::string sceneName = "solar_system";
    if (argc == 2) {
        sceneName = argv[1];
    } else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " [scene]\n"
                  << "Scenes: single  solar_system  ring  random_field\n";
        return 1;
    }

    auto lightPositions = generateLights(NUM_LIGHTS);

    Display display(W, H, "");

    std::vector<double> sumR(W * H, 0.0);
    std::vector<double> sumG(W * H, 0.0);
    std::vector<double> sumB(W * H, 0.0);

    int  lightIdx = 0;
    int  spp      = 0;
    bool quit     = false;

    pt::SceneConfig cfg    = buildScene(sceneName, lightPositions[lightIdx]);
    pt::PathTracer  tracer(cfg, MAX_DEPTH, RR_START);
    pt::RNG         rng(0xC0FFEEu);

    // Switch to a new light index: rebuild scene, clear accumulators.
    auto resetLight = [&](int newIdx) {
        lightIdx = newIdx;
        cfg    = buildScene(sceneName, lightPositions[lightIdx]);
        tracer = pt::PathTracer(cfg, MAX_DEPTH, RR_START);
        std::fill(sumR.begin(), sumR.end(), 0.0);
        std::fill(sumG.begin(), sumG.end(), 0.0);
        std::fill(sumB.begin(), sumB.end(), 0.0);
        spp = 0;
        display.clear();
        display.present();
    };

    while (!quit) {
        const Ray::Vec3& lp = lightPositions[lightIdx];
        char title[200];
        std::snprintf(title, sizeof(title),
            "PathTracing — %s   [%d/%d]  light (%.1f, %.1f, %.1f)   spp=%d   depth=%d   ← → to navigate",
            sceneName.c_str(), lightIdx + 1, NUM_LIGHTS,
            lp.x, lp.y, lp.z, spp, MAX_DEPTH);
        display.setTitle(title);

        if (spp >= MAX_SAMPLES) {
            switch (display.waitEvent()) {
                case Event::Quit:
                    quit = true;
                    break;
                case Event::PrevScene:
                    resetLight((lightIdx + NUM_LIGHTS - 1) % NUM_LIGHTS);
                    break;
                case Event::NextScene:
                    resetLight((lightIdx + 1) % NUM_LIGHTS);
                    break;
                case Event::None:
                    break;
            }
            continue;
        }

        bool interrupted = false;
        int  thisSample  = spp + 1;

        for (int row = 0; row < H && !quit && !interrupted; ++row) {
            for (int col = 0; col < W; ++col) {
                double jx = rng.next();
                double jy = rng.next();
                Ray::Ray ray = cfg.camera.pixelRay(col + jx, row + jy);

                Pixel::Color L = tracer.trace(ray, rng);

                int idx = row * W + col;
                sumR[idx] += L.r;
                sumG[idx] += L.g;
                sumB[idx] += L.b;

                double inv = 1.0 / (double)thisSample;
                display.setPixel(col, row,
                    toSRGB(sumR[idx] * inv),
                    toSRGB(sumG[idx] * inv),
                    toSRGB(sumB[idx] * inv));
            }

            if ((row % PRESENT_ROWS) == (PRESENT_ROWS - 1) || row == H - 1) {
                display.present();
                switch (display.pollEvent()) {
                    case Event::Quit:
                        quit = true;
                        break;
                    case Event::PrevScene:
                        resetLight((lightIdx + NUM_LIGHTS - 1) % NUM_LIGHTS);
                        interrupted = true;
                        break;
                    case Event::NextScene:
                        resetLight((lightIdx + 1) % NUM_LIGHTS);
                        interrupted = true;
                        break;
                    case Event::None:
                        break;
                }
            }
        }

        if (!interrupted && !quit)
            spp = thisSample;
    }

    return 0;
}
