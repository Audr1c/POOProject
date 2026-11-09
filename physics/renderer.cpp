#include "renderer.hpp"

// Color-aware overrides using Color class (default to standard methods if not overridden)
void Renderer::clear(const Color &color) {
  (void)color;
  clear();
}

void Renderer::drawCircle(const Vec2 &center, float radius,
                          const Color &color) {
  (void)color;
  drawCircle(center, radius);
}

void Renderer::drawLine(const Vec2 &p1, const Vec2 &p2, const Color &color) {
  (void)color;
  drawLine(p1, p2);
}

// Convenience overloads with float coordinates
void Renderer::drawCircle(float x, float y, float radius) {
  drawCircle(Vec2(x, y), radius);
}

void Renderer::drawCircle(float x, float y, float radius, const Color &color) {
  drawCircle(Vec2(x, y), radius, color);
}

void Renderer::drawLine(float x1, float y1, float x2, float y2) {
  drawLine(Vec2(x1, y1), Vec2(x2, y2));
}

void Renderer::drawLine(float x1, float y1, float x2, float y2,
                        const Color &color) {
  drawLine(Vec2(x1, y1), Vec2(x2, y2), color);
}

// Window state helper
bool Renderer::isOpen() const { return true; }
