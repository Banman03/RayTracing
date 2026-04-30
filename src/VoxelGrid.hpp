#ifndef VOXEL_GRID_HPP
#define VOXEL_GRID_HPP

#include "Ray.hpp"
#include "Object.hpp"
#include "Sphere.hpp"
#include <vector>
#include <memory>
#include <limits>
#include <cmath>
#include <algorithm>

namespace RayTracing {

// Amanatides & Woo fast voxel traversal for a uniform 3D grid.
// Spheres are placed into every cell their AABB overlaps.
// Per-ray rayID deduplication prevents testing the same sphere twice
// when it spans multiple cells.
class VoxelGrid {
    Ray::Vec3 _min, _max;
    int _nx, _ny, _nz;
    double _vsx, _vsy, _vsz;

    std::vector<const Sphere*> _spheres;       // non-owning; Scene owns the objects
    std::vector<std::vector<int>> _cells;       // cell -> sphere indices

    mutable int _nextRayID = 1;
    mutable std::vector<int> _lastRayID;        // per-sphere, last rayID that tested it

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

        // All cells overlapped by the sphere's AABB
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

    // Returns the closest intersection along the ray, or nullopt if none.
    std::optional<HitRecord> traverse(const Ray::Ray& ray) const {
        int rayID = _nextRayID++;

        // --- Initialization: clip ray to grid AABB (slab method) ---
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

        // Starting voxel from the grid entry point
        Ray::Vec3 entry = ray.point_at(tEntry);
        int X = std::clamp((int)std::floor((entry.x - _min.x) / _vsx), 0, _nx - 1);
        int Y = std::clamp((int)std::floor((entry.y - _min.y) / _vsy), 0, _ny - 1);
        int Z = std::clamp((int)std::floor((entry.z - _min.z) / _vsz), 0, _nz - 1);

        // Step direction per axis (+1 or -1)
        int stepX = (ray.direction.x >= 0) ? 1 : -1;
        int stepY = (ray.direction.y >= 0) ? 1 : -1;
        int stepZ = (ray.direction.z >= 0) ? 1 : -1;

        // Sentinels: first index that is outside the grid in each direction
        int justOutX = (stepX > 0) ? _nx : -1;
        int justOutY = (stepY > 0) ? _ny : -1;
        int justOutZ = (stepZ > 0) ? _nz : -1;

        // tMax: t at which the ray first crosses the next voxel boundary on each axis
        auto initTMax = [](double orig, double dir, double gmin, double vs,
                           int idx, int step) -> double {
            if (std::abs(dir) < 1e-10) return std::numeric_limits<double>::max();
            double boundary = gmin + (step > 0 ? idx + 1 : idx) * vs;
            return (boundary - orig) / dir;
        };
        double tMaxX = initTMax(ray.origin.x, ray.direction.x, _min.x, _vsx, X, stepX);
        double tMaxY = initTMax(ray.origin.y, ray.direction.y, _min.y, _vsy, Y, stepY);
        double tMaxZ = initTMax(ray.origin.z, ray.direction.z, _min.z, _vsz, Z, stepZ);

        // tDelta: how much t advances when crossing one full voxel on each axis
        double tDeltaX = (std::abs(ray.direction.x) < 1e-10) ? std::numeric_limits<double>::max() : _vsx / std::abs(ray.direction.x);
        double tDeltaY = (std::abs(ray.direction.y) < 1e-10) ? std::numeric_limits<double>::max() : _vsy / std::abs(ray.direction.y);
        double tDeltaZ = (std::abs(ray.direction.z) < 1e-10) ? std::numeric_limits<double>::max() : _vsz / std::abs(ray.direction.z);

        // --- Traversal ---
        std::optional<HitRecord> closest;
        double closestT = std::numeric_limits<double>::max();

        // Test all spheres in the current cell, deduplicating via rayID.
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

        // Check the starting voxel before entering the loop.
        testCell(X, Y, Z);

        // Incremental traversal (Amanatides & Woo §"The New Traversal Algorithm").
        while (true) {
            // tCurExit is the t at which the ray exits the current voxel.
            // If the closest hit lies within the current voxel we are done.
            double tCurExit = std::min({tMaxX, tMaxY, tMaxZ});
            if (closestT <= tCurExit) break;

            // Advance to the neighbouring voxel along the axis with the smallest tMax.
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

} // namespace RayTracing

#endif // VOXEL_GRID_HPP
