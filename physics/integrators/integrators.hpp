#pragma once

#include "../particles.hpp"
#include "../vec2.hpp"

class Integrator {

public:
  virtual ~Integrator() = default;
  virtual void step(ParticleList &parts, float dt) = 0;
};

class Euler : public Integrator {

public:
  void step(ParticleList &parts, float dt) override;
};

class Verlet : public Integrator {

public:
  void step(ParticleList &parts, float dt) override;
};
