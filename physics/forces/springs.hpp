#pragma once

#include "../drawable.hpp"
#include "force.hpp"
#include <array>

class Particle;

class Spring : public Force, public Drawable {
private:
  std::array<Particle *, 2> ends; // only 2 particles can be connected

  float L0;  // rest length
  float k;   // stiffness
  float rho; // damping coefficient

public:
  Spring(Particle &first, Particle &second, float L0, float k, float rho);

  Vec2 compute(const Particle &particle) override;

  // Drawn with its stroke color, turning red when stretched and blue when
  // compressed
  void draw(Renderer &renderer) override;

  float getRestLength() const;
  float getLength() const;
};
