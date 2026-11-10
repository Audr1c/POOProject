#pragma once
#include "../entity.hpp"
#include "../vec2.hpp"
#include <string>

class Particle;

class Force : public virtual Entity {
public:
  Force(const std::string &name = "Force");
  virtual ~Force() = default;
  virtual Vec2 compute(const Particle &p) = 0;
};

// Weight: F = m * g
class Gravity : public Force {
private:
  Vec2 g;

public:
  Gravity(const Vec2 &g = Vec2(0.0f, -9.81f));
  Vec2 compute(const Particle &) override;
  void setG(const Vec2 &new_g);
  const Vec2 &getG() const;
};
