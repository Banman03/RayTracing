#ifndef PT_VOXEL_GRID_HPP
#define PT_VOXEL_GRID_HPP

#include "Ray.hpp"
#include "Object.hpp"
#include "Sphere.hpp"
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>

namespace PathTracing {

// Amanatides & Woo fast voxel traversal for a uniform 3D grid. This is the
// path-tracing twin of RayTracing::VoxelGrid in src/. Spheres are placed into
// every cell their AABB overlaps; a per-ray rayID prevents testing the same
// sphere twice when it spans multiple cells.
class VoxelGrid {
    Ray::Vec3 _min, _max;
    int       _nx, _ny, _nz;
    double    _vsx, _vsy, _vsz;

    std::vector<const Sphere*>          _spheres;
    std::vector<std::vector<int>>       _cells;

    mutable int              _nextRayID = 1;
    mutable std::vector<int> _lastRayID;

    int cellIdx(int x, int y, int z) const {
        return z * (_nx * _ny) + y * _nx + x;
    }

public:
    VoxelGrid(Ray::Vec3 min, Ray::Vec3 max, int nx, int ny, int nz)
        : _min(min), _max(max), _nx(nx), _ny(ny), _nz(nz)
    {
        _vsx = (max.x - min.x) / nx;
        _vsy = (max.y - min.y) / ny;
        _vsz = (max.z - min.z) / nz;
        _cells.resize(nx * ny * nz);
    }

    void addSphere(const Sphere* s) {
        int id = (int)_spheres.size();
        _spheres.push_back(s);
        _lastRayID.push_back(0);

        int x0 = std::max(0,       (int)std::floor((s->center.x - s->radius - _min.x) / _vsx));
        int y0 = std::max(0,       (int)std::floor((s->center.y - s->radius - _min.y) / _vsy));
        int z0 = std::max(0,       (int)std::floor((s->center.z - s->radius - _min.z) / _vsz));
        int x1 = std::min(_nx - 1, (int)std::floor((s->center.x + s->radius - _min.x) / _vsx));
        int y1 = std::min(_ny - 1, (int)std::floor((s->center.y + s->radius - _min.y) / _vsy));
        int z1 = std::min(_nz - 1, (int)std::floor((s->center.z + s->radius - _min.z) / _vsz));

        for (int z = z0; z <= z1; ++z)
            for (int y = y0; y <= y1; ++y)
                for (int x = x0; x <= x1; ++x)
                    _cells[cellIdx(x, y, z)].push_back(id);
    }

    std::optional<HitRecord> traverse(const Ray::Ray& ray) const {
        int rayID = _nextRayID++;

        double tEntry = 0.0;
        double tExit  = std::numeric_limits<double>::max();

        auto slab = [&](double orig, double dir, double bmin, double bmax) -> bool {
            if (std::abs(dir) < 1e-10)
                return orig >= bmin && orig <= bmax;
            double t0 = (bmin - orig) / dir;
            double t1 = (bmax - orig) / dir;
            if (t0 > t1) std::swap(t0, t1);
            tEntry = std::max(tEntry, t0);
            tExit  = std::min(tExit,  t1);
            return tEntry < tExit;
        };

        if (!slab(ray.origin.x, ray.direction.x, _min.x, _max.x)) return std::nullopt;
        if (!slab(ray.origin.y, ray.direction.y, _min.y, _max.y)) return std::nullopt;
        if (!slab(ray.origin.z, ray.direction.z, _min.z, _max.z)) return std::nullopt;

        Ray::Vec3 entry = ray.point_at(tEntry);
        int X = std::clamp((int)std::floor((entry.x - _min.x) / _vsx), 0, _nx - 1);
        int Y = std::clamp((int)std::floor((entry.y - _min.y) / _vsy), 0, _ny - 1);
        int Z = std::clamp((int)std::floor((entry.z - _min.z) / _vsz), 0, _nz - 1);

        int stepX = (ray.direction.x >= 0) ? 1 : -1;
        int stepY = (ray.direction.y >= 0) ? 1 : -1;
        int stepZ = (ray.direction.z >= 0) ? 1 : -1;

        int justOutX = (stepX > 0) ? _nx : -1;
        int justOutY = (stepY > 0) ? _ny : -1;
        int justOutZ = (stepZ > 0) ? _nz : -1;

        auto initTMax = [](double orig, double dir, double gmin, double vs,
                           int idx, int step) -> double {
            if (std::abs(dir) < 1e-10) return std::numeric_limits<double>::max();
            double boundary = gmin + (step > 0 ? idx + 1 : idx) * vs;
            return (boundary - orig) / dir;
        };
        double tMaxX = initTMax(ray.origin.x, ray.direction.x, _min.x, _vsx, X, stepX);
        double tMaxY = initTMax(ray.origin.y, ray.direction.y, _min.y, _vsy, Y, stepY);
        double tMaxZ = initTMax(ray.origin.z, ray.direction.z, _min.z, _vsz, Z, stepZ);

        double tDeltaX = (std::abs(ray.direction.x) < 1e-10) ? std::numeric_limits<double>::max() : _vsx / std::abs(ray.direction.x);
        double tDeltaY = (std::abs(ray.direction.y) < 1e-10) ? std::numeric_limits<double>::max() : _vsy / std::abs(ray.direction.y);
        double tDeltaZ = (std::abs(ray.direction.z) < 1e-10) ? std::numeric_limits<double>::max() : _vsz / std::abs(ray.direction.z);

        std::optional<HitRecord> closest;
        double closestT = std::numeric_limits<double>::max();

        auto testCell = [&](int x, int y, int z) {
            for (int id : _cells[cellIdx(x, y, z)]) {
                if (_lastRayID[id] == rayID) continue;
                _lastRayID[id] = rayID;
                if (auto hit = _spheres[id]->intersect(ray)) {
                    if (hit->t >= tEntry && hit->t < closestT) {
                        closestT = hit->t;
                        closest  = hit;
                    }
                }
            }
        };

        testCell(X, Y, Z);

        while (true) {
            double tCurExit = std::min({tMaxX, tMaxY, tMaxZ});
            if (closestT <= tCurExit) break;

            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    X += stepX;
                    if (X == justOutX) break;
                    tMaxX += tDeltaX;
                } else {
                    Z += stepZ;
                    if (Z == justOutZ) break;
                    tMaxZ += tDeltaZ;
                }
            } else {
                if (tMaxY < tMaxZ) {
                    Y += stepY;
                    if (Y == justOutY) break;
                    tMaxY += tDeltaY;
                } else {
                    Z += stepZ;
                    if (Z == justOutZ) break;
                    tMaxZ += tDeltaZ;
                }
            }

            testCell(X, Y, Z);
        }

        return closest;
    }
};

} // namespace PathTracing

#endif // PT_VOXEL_GRID_HPP
