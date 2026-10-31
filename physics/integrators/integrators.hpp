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
  void step(std::vector<Particle> &parts, float dt) override {
    for (Particle &p : parts) {
      if (!p.moovable)
        continue;
      p.speed += p.accel * dt; // Speed += acceleration * dt
      p.pos += p.speed * dt;   // Position += speed * dt
    }
  }
};

class Verlet : public Integrator {

public:
  void step(std::vector<Particle> &parts, float dt) override {
    for (Particle &p : parts) {
      if (!p.moovable)
        continue;

      Vec2 tmp_old_pos = p.pos;

      // x(n+1) = 2*x(n) - x(n-1) + a*dt*dt
      p.pos = 2.f * p.pos - p.previous_pos + p.accel * (dt * dt);

      // Updating particle's speed attribute
      p.speed = (p.pos - tmp_old_pos) / dt;

      p.previous_pos = tmp_old_pos;
    }
  }
};
