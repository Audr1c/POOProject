#pragma once
class Vec2 {
public:
  float x;
  float y;

  Vec2() : x(0), y(0) {}
  Vec2(float x, float y) : x(x), y(y) {}

  Vec2 operator+(const Vec2 &v) const { return Vec2(x + v.x, y + v.y); }
  Vec2 &operator+=(const Vec2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  Vec2 operator/(float d) const { return Vec2(x / d, y / d); }
};