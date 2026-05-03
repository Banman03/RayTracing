#ifndef PT_SCENE_HPP
#define PT_SCENE_HPP

#include "Sphere.hpp"
#include "VoxelGrid.hpp"
#include <memory>
#include <vector>

namespace PathTracing {

class Scene {
    int _width, _height;

    std::vector<std::unique_ptr<Sphere>> _spheres;
    VoxelGrid                            _grid;

public:
    Scene(int w, int h,
          Ray::Vec3 gridMin, Ray::Vec3 gridMax,
          int nx = 10, int ny = 10, int nz = 10)
        : _width(w), _height(h), _grid(gridMin, gridMax, nx, ny, nz)
    {}

    void addSphere(std::unique_ptr<Sphere> s) {
        _grid.addSphere(s.get());
        _spheres.push_back(std::move(s));
    }

    std::optional<HitRecord> trace(const Ray::Ray& ray) const {
        return _grid.traverse(ray);
    }

    int width()  const { return _width; }
    int height() const { return _height; }
};

} // namespace PathTracing

#endif // PT_SCENE_HPP
