#include "vec2.hpp"
#include <cmath>

Vec2::Vec2() : x(0), y(0) {}
Vec2::Vec2(float x, float y) : x(x), y(y) {}

// --- Vec2 (op) Vec2 ---
Vec2 Vec2::operator+(const Vec2 &v) const { return Vec2(x + v.x, y + v.y); }
Vec2 Vec2::operator-(const Vec2 &v) const { return Vec2(x - v.x, y - v.y); }
Vec2 Vec2::operator-() const { return Vec2(-x, -y); }

Vec2 &Vec2::operator+=(const Vec2 &v) {
  x += v.x;
  y += v.y;
  return *this;
}

Vec2 &Vec2::operator-=(const Vec2 &v) {
  x -= v.x;
  y -= v.y;
  return *this;
}

// --- Vec2 (op) scalar ---
Vec2 Vec2::operator*(float k) const { return Vec2(x * k, y * k); }
Vec2 Vec2::operator/(float d) const { return Vec2(x / d, y / d); }

Vec2 &Vec2::operator*=(float k) {
  x *= k;
  y *= k;
  return *this;
}

Vec2 &Vec2::operator/=(float d) {
  x /= d;
  y /= d;
  return *this;
}

bool Vec2::operator==(const Vec2 &v) const { return x == v.x && y == v.y; }
bool Vec2::operator!=(const Vec2 &v) const { return !(*this == v); }

// --- Geometry helpers ---
float Vec2::dot(const Vec2 &v) const { return x * v.x + y * v.y; }
float Vec2::normSquared() const { return dot(*this); }
float Vec2::norm() const { return std::sqrt(normSquared()); }

Vec2 Vec2::normalized() const {
  float n = norm();
  return n > 0.f ? *this / n : Vec2();
}

// scalar * Vec2
Vec2 operator*(float k, const Vec2 &v) { return v * k; }
