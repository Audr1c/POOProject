#pragma once
#include "../entity.hpp"
#include "../vec2.hpp"
// #include "../particles.hpp"
#include <vector>

class Particle;

class Force : public virtual Entity {
public:
  Force(const std::string &name = "Force") : Entity(name) {}
  virtual ~Force() = default;
  virtual Vec2 compute(const Particle &p) = 0;
};

class TempForce : public Force {
public:
  TempForce() : Entity("Tempory Forces") {}
  Vec2 compute(const Particle &) override { return Vec2(0.0f, -9.81f); };
};