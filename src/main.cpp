#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <string>
#include "SceneConfig.hpp"
#include "Pixel.hpp"
#include "Display.hpp"

#include "scenes/single_sphere.hpp"
#include "scenes/solar_system.hpp"
#include "scenes/ring.hpp"
#include "scenes/random_field.hpp"

// ── Shading ────────────────────────────────────────────────────────────────

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

// ── Scene registry ─────────────────────────────────────────────────────────

static SceneConfig buildScene(const std::string& name, int W, int H) {
    if (name == "single")       return Scenes::singleSphere(W, H);
    if (name == "solar_system") return Scenes::solarSystem(W, H);
    if (name == "ring")         return Scenes::ring(W, H);
    if (name == "random_field") return Scenes::randomField(W, H);
    std::cerr << "Unknown scene \"" << name << "\".\n"
              << "Available: single  solar_system  ring  random_field\n";
    std::exit(1);
}

// ── Main ───────────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    std::string sceneName = "solar_system";
    Ray::Vec3   lightPos  = {4.0, 8.0, -2.0};

    if (argc == 2) {
        sceneName = argv[1];
    } else if (argc == 5) {
        sceneName = argv[1];
        try {
            lightPos = {std::stod(argv[2]),
                        std::stod(argv[3]),
                        std::stod(argv[4])};
        } catch (const std::exception&) {
            std::cerr << "Usage: " << argv[0] << " [scene] [lx ly lz]\n";
            return 1;
        }
    } else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " [scene] [lx ly lz]\n"
                  << "Scenes: single  solar_system  ring  random_field\n";
        return 1;
    }

    std::cerr << "Scene: " << sceneName
              << "  Light: (" << lightPos.x << ", "
                              << lightPos.y << ", "
                              << lightPos.z << ")\n";

    constexpr int W = 800, H = 450;
    SceneConfig cfg = buildScene(sceneName, W, H);

    Display      display(W, H, "RayTracing — " + sceneName);
    std::ofstream out("output.ppm");
    out << "P3\n" << W << " " << H << "\n255\n";

    bool quit = false;
    for (int row = 0; row < H && !quit; ++row) {
        for (int col = 0; col < W; ++col) {
            Ray::Ray ray = cfg.camera.pixelRay(col, row);

            Pixel::Color c;
            if (auto hit = cfg.scene.trace(ray))
                c = shade(*hit, lightPos, cfg.scene);
            else
                c = background(ray);

            int ir = std::clamp((int)(c.r * 255.999), 0, 255);
            int ig = std::clamp((int)(c.g * 255.999), 0, 255);
            int ib = std::clamp((int)(c.b * 255.999), 0, 255);

            display.setPixel(col, row, ir, ig, ib);
            out << ir << ' ' << ig << ' ' << ib << '\n';
        }

        display.present();   // show completed row immediately
        quit = display.pollQuit();
    }

    std::cerr << "Done. Close the window to exit.\n";
    display.waitUntilClosed();
    return 0;
}
