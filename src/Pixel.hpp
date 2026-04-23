#ifndef PIXEL_HPP
#define PIXEL_HPP

namespace Pixel {
    struct Color {
        double r, g, b;

        // Basic color operations
        Color operator+(const Color& other) const { return {r + other.r, g + other.g, g + other.b}; }
        Color operator*(double scalar) const { return {r * scalar, g * scalar, b * scalar}; }
    };

    class Pixel {
    private:
        Color _color;

    public:
        Pixel() : _color({0.0, 0.0, 0.0}) {}
        Pixel(Color c) : _color(c) {}

        void set_color(Color c) { _color = c; }
        Color get_color() const { return _color; }
    };
}

#endif // PIXEL_HPP
