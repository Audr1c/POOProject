#pragma once

#include "color.hpp"
#include "entity.hpp"
#include "renderer.hpp"
#include <string>

class Drawable : public virtual Entity {
protected:
  Color stroke_color;
  Color fill_color;
  float stroke_width;
  bool visible;

public:
  Drawable()
      : Entity("Drawable"), stroke_color(255, 255, 255),
        fill_color(255, 140, 0), stroke_width(1.0f), visible(true) {}

  Drawable(const Color &fill, const Color &stroke = Color(255, 255, 255),
           float width = 1.0f, bool is_visible = true)
      : Entity("Drawable"), stroke_color(stroke), fill_color(fill),
        stroke_width(width), visible(is_visible) {}

  Drawable(const std::string &name, const Color &fill,
           const Color &stroke = Color(255, 255, 255), float width = 1.0f,
           bool is_visible = true)
      : Entity(name), stroke_color(stroke), fill_color(fill),
        stroke_width(width), visible(is_visible) {}

  virtual ~Drawable() = default;

  // Pure virtual method to be implemented by every drawable entity (Particle, Spring, etc.)
  virtual void draw(Renderer &renderer) = 0;

  // Getters (const)
  const Color &getStrokeColor() const { return stroke_color; }
  const Color &getFillColor() const { return fill_color; }
  float getStrokeWidth() const { return stroke_width; }
  bool isVisible() const { return visible; }

  // Setters
  void setStrokeColor(const Color &color) { stroke_color = color; }
  void setFillColor(const Color &color) { fill_color = color; }
  void setStrokeWidth(float width) { stroke_width = width; }
  void setVisible(bool is_visible) { visible = is_visible; }
};

