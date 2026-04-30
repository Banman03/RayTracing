#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <memory>
#include <limits>
#include "Object.hpp"
#include "Sphere.hpp"
#include "VoxelGrid.hpp"

namespace Scene {

class Scene {
    int _width, _height;

    // Scene owns the spheres; VoxelGrid holds non-owning pointers into this vector.
    std::vector<std::unique_ptr<RayTracing::Sphere>> _spheres;
    RayTracing::VoxelGrid _grid;

public:
    // gridMin/gridMax define the world-space extent of the voxel grid.
    // nx, ny, nz control grid resolution (more cells = fewer objects per cell).
    Scene(int w, int h,
          Ray::Vec3 gridMin, Ray::Vec3 gridMax,
          int nx = 10, int ny = 10, int nz = 10)
        : _width(w), _height(h), _grid(gridMin, gridMax, nx, ny, nz)
    {}

    void addSphere(std::unique_ptr<RayTracing::Sphere> s) {
        _grid.addSphere(s.get());
        _spheres.push_back(std::move(s));
    }

    std::optional<RayTracing::HitRecord> trace(const Ray::Ray& ray) const {
        return _grid.traverse(ray);
    }

    int width()  const { return _width; }
    int height() const { return _height; }
};

} // namespace Scene

#endif // SCENE_HPP
