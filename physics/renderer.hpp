#pragma once

#include "color.hpp"
#include "vec2.hpp"

class Renderer {
public:
  virtual ~Renderer() = default;

  // Core rendering interface required by the engine
  virtual void clear() = 0;
  virtual void drawCircle(const Vec2 &center, float radius) = 0;
  virtual void drawLine(const Vec2 &p1, const Vec2 &p2) = 0;
  virtual void display() = 0;

  // Color-aware overrides using Color class (default to standard methods if not overridden)
  virtual void clear(const Color &color);
  virtual void drawCircle(const Vec2 &center, float radius,
                          const Color &color);
  virtual void drawLine(const Vec2 &p1, const Vec2 &p2, const Color &color);

  // Convenience overloads with float coordinates
  void drawCircle(float x, float y, float radius);
  void drawCircle(float x, float y, float radius, const Color &color);
  void drawLine(float x1, float y1, float x2, float y2);
  void drawLine(float x1, float y1, float x2, float y2, const Color &color);

  // Window state helper
  virtual bool isOpen() const;
};
