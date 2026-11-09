#pragma once

#include "../particles.hpp" //JUST TO DEBUG : NEED TO BE CHANGED
#include "../vec2.hpp"
#include <vector>

class Integrator {

public:
  virtual ~Integrator() = default;
  virtual void step(std::vector<Particle> &parts, float dt) = 0;
};

class Euler : public Integrator {

public:
  void step(std::vector<Particle> &parts, float dt) override;
};

class Verlet : public Integrator {

public:
  void step(std::vector<Particle> &parts, float dt) override;
};
