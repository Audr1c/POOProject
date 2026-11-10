#pragma once
#include "force.hpp"

// Air friction: F = -b * v
class Drag : public Force {
private:
  float b;

public:
  Drag(float b = 0.0f);
  Vec2 compute(const Particle &p) override;
  void setCoefficient(float new_b);
  float getCoefficient() const;
};
