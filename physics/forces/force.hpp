#pragma once
#include "../entity.hpp"
#include "../vec2.hpp"
// #include "../particles.hpp"
#include <string>

class Particle;

class Force : public virtual Entity {
public:
  Force(const std::string &name = "Force");
  virtual ~Force() = default;
  virtual Vec2 compute(const Particle &p) = 0;
};

class TempForce : public Force {
public:
  TempForce();
  Vec2 compute(const Particle &) override;
};
