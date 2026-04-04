#include <vector>

namespace Scene {
    class Ray {

    };

    class Pixel {

    };
    
    class Scene {
        private:
        int _width;
        int _height;
        int _horizontal_steps;
        int _vertical_steps;
        std::vector<Ray> _scene_ray_vec;
        
        void change_scene() {
            
        }

        public:
        Scene(int w, int h, int hs, int vs) : _width(w), _height(h), _horizontal_steps(hs), _vertical_steps(vs) {}
        
    };
}