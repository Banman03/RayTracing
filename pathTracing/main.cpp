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

// Output settings — kept smaller than the rasterized ray tracer because
// Monte Carlo path tracing requires many samples per pixel to converge.
constexpr int    W           = 480;
constexpr int    H           = 270;
constexpr int    MAX_DEPTH   = 6;
constexpr int    RR_START    = 3;
constexpr int    MAX_SAMPLES = 4096;
constexpr int    PRESENT_ROWS = 32;

// Reinhard tone-mapping followed by gamma 2.2 — the simplest pair that
// keeps emissive surfaces from clipping while still producing a perceptually
// linear-ish output.
static int toSRGB(double x) {
    if (x < 0.0) x = 0.0;
    x = x / (1.0 + x);
    x = std::pow(x, 1.0 / 2.2);
    return std::clamp((int)(x * 255.999), 0, 255);
}

static pt::SceneConfig buildScene(const std::string& name) {
    if (name == "single")       return pt::Scenes::singleSphere(W, H);
    if (name == "solar_system") return pt::Scenes::solarSystem(W, H);
    if (name == "ring")         return pt::Scenes::ring(W, H);
    if (name == "random_field") return pt::Scenes::randomField(W, H);
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

    Display display(W, H, "PathTracing");

    // Per-pixel accumulators; we average sums across all completed samples
    // to produce a progressively-refined image.
    std::vector<double> sumR(W * H, 0.0);
    std::vector<double> sumG(W * H, 0.0);
    std::vector<double> sumB(W * H, 0.0);

    // Each scene gets its own seed so reloading produces the same image.
    auto loadScene = [&](const std::string& name) -> pt::SceneConfig {
        std::fill(sumR.begin(), sumR.end(), 0.0);
        std::fill(sumG.begin(), sumG.end(), 0.0);
        std::fill(sumB.begin(), sumB.end(), 0.0);
        display.clear();
        display.present();
        return buildScene(name);
    };

    pt::SceneConfig cfg = loadScene(sceneName);
    pt::PathTracer  tracer(cfg, MAX_DEPTH, RR_START);
    pt::RNG         rng(0xC0FFEEu);

    constexpr const char* SCENE_LIST[] = {
        "single", "solar_system", "ring", "random_field"
    };
    constexpr int NUM_SCENES = sizeof(SCENE_LIST) / sizeof(SCENE_LIST[0]);

    int sceneIdx = 0;
    for (int i = 0; i < NUM_SCENES; ++i)
        if (SCENE_LIST[i] == sceneName) sceneIdx = i;

    bool quit  = false;
    int  spp   = 0;          // completed samples per pixel

    while (!quit) {
        char title[160];
        std::snprintf(title, sizeof(title),
            "PathTracing — %s   spp=%d   depth=%d   ← → to switch scenes",
            sceneName.c_str(), spp, MAX_DEPTH);
        display.setTitle(title);

        if (spp >= MAX_SAMPLES) {
            // Converged enough; idle on the input queue until a key event.
            switch (display.waitEvent()) {
                case Event::Quit:      quit = true; break;
                case Event::PrevScene: sceneIdx = (sceneIdx + NUM_SCENES - 1) % NUM_SCENES;
                                       sceneName = SCENE_LIST[sceneIdx];
                                       cfg = loadScene(sceneName);
                                       tracer = pt::PathTracer(cfg, MAX_DEPTH, RR_START);
                                       spp = 0;
                                       break;
                case Event::NextScene: sceneIdx = (sceneIdx + 1) % NUM_SCENES;
                                       sceneName = SCENE_LIST[sceneIdx];
                                       cfg = loadScene(sceneName);
                                       tracer = pt::PathTracer(cfg, MAX_DEPTH, RR_START);
                                       spp = 0;
                                       break;
                case Event::None:      break;
            }
            continue;
        }

        // ---- one sample-per-pixel pass over the image -------------------------
        bool interrupted = false;
        int  thisSample  = spp + 1;          // 1-based sample index after this pass

        for (int row = 0; row < H && !quit && !interrupted; ++row) {
            for (int col = 0; col < W; ++col) {
                // Subpixel jitter inside [0, 1) on each axis. (paper §1.1)
                double jx = rng.next();
                double jy = rng.next();
                Ray::Ray ray = cfg.camera.pixelRay(col + jx, row + jy);

                Pixel::Color L = tracer.trace(ray, rng);

                int idx = row * W + col;
                sumR[idx] += L.r;
                sumG[idx] += L.g;
                sumB[idx] += L.b;

                double inv = 1.0 / (double)thisSample;
                int ir = toSRGB(sumR[idx] * inv);
                int ig = toSRGB(sumG[idx] * inv);
                int ib = toSRGB(sumB[idx] * inv);
                display.setPixel(col, row, ir, ig, ib);
            }

            if ((row % PRESENT_ROWS) == (PRESENT_ROWS - 1) || row == H - 1) {
                display.present();
                switch (display.pollEvent()) {
                    case Event::Quit:      quit = true; break;
                    case Event::PrevScene: sceneIdx = (sceneIdx + NUM_SCENES - 1) % NUM_SCENES;
                                           sceneName = SCENE_LIST[sceneIdx];
                                           cfg = loadScene(sceneName);
                                           tracer = pt::PathTracer(cfg, MAX_DEPTH, RR_START);
                                           spp = 0;
                                           interrupted = true;
                                           break;
                    case Event::NextScene: sceneIdx = (sceneIdx + 1) % NUM_SCENES;
                                           sceneName = SCENE_LIST[sceneIdx];
                                           cfg = loadScene(sceneName);
                                           tracer = pt::PathTracer(cfg, MAX_DEPTH, RR_START);
                                           spp = 0;
                                           interrupted = true;
                                           break;
                    case Event::None:      break;
                }
            }
        }

        if (!interrupted && !quit) {
            spp = thisSample;
        }
    }

    return 0;
}
