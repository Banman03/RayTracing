#include <vector>
#include <memory>
#include "Object.hpp"
#include "Sphere.hpp"

namespace Scene {
    class Scene {
        private:
        int _width;
        int _height;
        int _horizontal_steps;
        int _vertical_steps;
        std::vector<std::unique_ptr<RayTracing::Object>> _objects;

        public:
        Scene(int w, int h, int hs, int vs) : _width(w), _height(h), _horizontal_steps(hs), _vertical_steps(vs) {}

        void add_object(std::unique_ptr<RayTracing::Object> obj) {
            _objects.push_back(std::move(obj));
        }

        std::optional<RayTracing::HitRecord> trace(const Ray::Ray& ray) const {
            std::optional<RayTracing::HitRecord> closest_hit;
            double min_t = std::numeric_limits<double>::max();

            for (const auto& obj : _objects) {
                if (auto hit = obj->intersect(ray)) {
                    if (hit->t < min_t) {
                        min_t = hit->t;
                        closest_hit = hit;
                    }
                }
            }
            return closest_hit;
        }
    };
}
