#include "drawable.hpp"

Drawable::Drawable()
    : Entity("Drawable"), stroke_color(35, 68, 93), fill_color(231, 111, 81),
      stroke_width(1.0f), visible(true) {}

Drawable::Drawable(const Color &fill, const Color &stroke, float width,
                   bool is_visible)
    : Entity("Drawable"), stroke_color(stroke), fill_color(fill),
      stroke_width(width), visible(is_visible) {}

Drawable::Drawable(const std::string &name, const Color &fill,
                   const Color &stroke, float width, bool is_visible)
    : Entity(name), stroke_color(stroke), fill_color(fill),
      stroke_width(width), visible(is_visible) {}

// Getters (const)
const Color &Drawable::getStrokeColor() const { return stroke_color; }
const Color &Drawable::getFillColor() const { return fill_color; }
float Drawable::getStrokeWidth() const { return stroke_width; }
bool Drawable::isVisible() const { return visible; }

// Setters
void Drawable::setStrokeColor(const Color &color) { stroke_color = color; }
void Drawable::setFillColor(const Color &color) { fill_color = color; }
void Drawable::setStrokeWidth(float width) { stroke_width = width; }
void Drawable::setVisible(bool is_visible) { visible = is_visible; }
