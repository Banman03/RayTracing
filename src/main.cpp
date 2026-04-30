#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include "Scene.hpp"
#include "Pixel.hpp"

// ── Camera ─────────────────────────────────────────────────────────────────

struct Camera {
    Ray::Vec3 eye;
    Ray::Vec3 topLeft;
    Ray::Vec3 stepX;
    Ray::Vec3 stepY;

    // Follows the derivation in math/overview.md.
    // E: camera position, T: target, fovDeg: horizontal FOV.
    Camera(Ray::Vec3 E, Ray::Vec3 T, int width, int height, double fovDeg = 90.0)
        : eye(E)
    {
        Ray::Vec3 worldUp = {0, 1, 0};
        Ray::Vec3 tn = (T - E).normalized();           // forward  (t_n)
        Ray::Vec3 bn = worldUp.cross(tn).normalized(); // right    (b_n = worldUp × t_n)
        Ray::Vec3 vn = tn.cross(bn);                   // true up  (v_n = t_n × b_n)

        double gx = std::tan(fovDeg * M_PI / 360.0);
        double gy = gx * (double)(height - 1) / (double)(width - 1);

        Ray::Vec3 qx = bn * (2.0 * gx / (width  - 1));
        Ray::Vec3 qy = vn * (2.0 * gy / (height - 1));

        topLeft = tn - bn * gx + vn * gy; // top-left pixel direction (p_{1,m})
        stepX   = qx;
        stepY   = qy * -1.0;              // rows run top-to-bottom, so negate v_n step
    }

    Ray::Ray pixelRay(int col, int row) const {
        Ray::Vec3 dir = topLeft + stepX * col + stepY * row;
        return Ray::Ray(eye, dir);
    }
};

// ── Shading ────────────────────────────────────────────────────────────────

static Pixel::Color background(const Ray::Ray& ray) {
    double t = 0.5 * (ray.direction.y + 1.0);
    return {1.0 - 0.5 * t, 1.0 - 0.3 * t, 1.0};
}

// Point-light shading with a shadow ray.
// lightPos: world-space position of the light source.
static Pixel::Color shade(const RayTracing::HitRecord& hit,
                          const Ray::Vec3& lightPos,
                          const Scene::Scene& scene)
{
    // Direction and distance from hit surface to the light
    Ray::Vec3 toLight   = lightPos - hit.point;
    double    distLight = toLight.norm();
    Ray::Vec3 L         = {toLight.x / distLight,
                           toLight.y / distLight,
                           toLight.z / distLight};

    double NdotL = hit.normal.x * L.x +
                   hit.normal.y * L.y +
                   hit.normal.z * L.z;

    // Shadow ray: offset along the normal to avoid self-intersection,
    // then fire toward the light. Occlusion only matters if the light
    // is on the correct side of the surface (NdotL > 0).
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

    // Surface tint derived from the outward normal (gives distinct colours
    // to differently oriented faces without needing per-sphere materials).
    double r = intensity * (0.5 + 0.5 * hit.normal.x);
    double g = intensity * (0.3 + 0.5 * hit.normal.y);
    double b = intensity * (0.4 + 0.3 * hit.normal.z);
    return {r, g, b};
}

// ── Main ───────────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    // Light position: default overhead-front, overridden by CLI args.
    Ray::Vec3 lightPos = {4.0, 8.0, -2.0};

    if (argc == 4) {
        try {
            lightPos = {std::stod(argv[1]),
                        std::stod(argv[2]),
                        std::stod(argv[3])};
        } catch (const std::exception&) {
            std::cerr << "Usage: " << argv[0] << " <light_x> <light_y> <light_z>\n";
            return 1;
        }
    } else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " <light_x> <light_y> <light_z>\n";
        return 1;
    }

    std::cerr << "Light position: ("
              << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << ")\n";

    constexpr int W = 800, H = 450;

    Scene::Scene scene(W, H,
                       {-8, -5, -4},
                       { 8,  6, 12},
                       32, 22, 32);

    scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 0.0,  0.0, 5.0}, 1.0));
    scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 2.5,  0.3, 6.5}, 0.8));
    scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{-2.2, -0.2, 4.5}, 0.7));
    scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{ 0.5,  1.2, 7.0}, 0.5));
    scene.addSphere(std::make_unique<RayTracing::Sphere>(Ray::Vec3{0, -101, 5}, 100.0)); // ground

    Camera cam({0, 1, -3}, {0, 0, 5}, W, H, 90.0);

    std::ofstream out("output.ppm");
    out << "P3\n" << W << " " << H << "\n255\n";

    for (int row = 0; row < H; ++row) {
        if (row % 50 == 0)
            std::cerr << "Row " << row << " / " << H << "\r" << std::flush;

        for (int col = 0; col < W; ++col) {
            Ray::Ray ray = cam.pixelRay(col, row);

            Pixel::Color c;
            if (auto hit = scene.trace(ray))
                c = shade(*hit, lightPos, scene);
            else
                c = background(ray);

            int ir = std::clamp((int)(c.r * 255.999), 0, 255);
            int ig = std::clamp((int)(c.g * 255.999), 0, 255);
            int ib = std::clamp((int)(c.b * 255.999), 0, 255);
            out << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    std::cerr << "\nWrote output.ppm (" << W << "x" << H << ")\n";
    return 0;
}
