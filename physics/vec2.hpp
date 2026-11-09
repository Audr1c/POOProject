#pragma once

class Vec2 {
public:
  float x;
  float y;

  Vec2();
  Vec2(float x, float y);

  // --- Vec2 (op) Vec2 ---
  Vec2 operator+(const Vec2 &v) const;
  Vec2 operator-(const Vec2 &v) const;
  Vec2 operator-() const;

  Vec2 &operator+=(const Vec2 &v);
  Vec2 &operator-=(const Vec2 &v);

  // --- Vec2 (op) scalar ---
  Vec2 operator*(float k) const;
  Vec2 operator/(float d) const;

  Vec2 &operator*=(float k);
  Vec2 &operator/=(float d);

  bool operator==(const Vec2 &v) const;
  bool operator!=(const Vec2 &v) const;

  // --- Geometry helpers ---
  float dot(const Vec2 &v) const;
  float normSquared() const;
  float norm() const;
  Vec2 normalized() const;
};

// scalar * Vec2
Vec2 operator*(float k, const Vec2 &v);
