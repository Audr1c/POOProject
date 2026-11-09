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
  Drawable();

  Drawable(const Color &fill, const Color &stroke = Color(255, 255, 255),
           float width = 1.0f, bool is_visible = true);

  Drawable(const std::string &name, const Color &fill,
           const Color &stroke = Color(255, 255, 255), float width = 1.0f,
           bool is_visible = true);

  virtual ~Drawable() = default;

  // Pure virtual method to be implemented by every drawable entity (Particle, Spring, etc.)
  virtual void draw(Renderer &renderer) = 0;

  // Getters (const)
  const Color &getStrokeColor() const;
  const Color &getFillColor() const;
  float getStrokeWidth() const;
  bool isVisible() const;

  // Setters
  void setStrokeColor(const Color &color);
  void setFillColor(const Color &color);
  void setStrokeWidth(float width);
  void setVisible(bool is_visible);
};
