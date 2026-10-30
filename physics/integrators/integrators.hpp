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
      // Speed += acceleration * dt
      p.speed.x += p.accel.x * dt;
      p.speed.y += p.accel.y * dt;

      // Position += speed * dt
      p.pos.x += p.speed.x * dt;
      p.pos.y += p.speed.y * dt;
    }
  }
};

class Verlet : public Integrator {

public:
  void step(std::vector<Particle> &parts, float dt) override {
    for (Particle &p : parts) {

      Vec2 tmp_old_pos = p.pos;

      // x(n+1) = 2*x(n)     - x(n-1)            + a*dt*dt
      p.pos.x = 2 * p.pos.x - p.previous_pos.x + p.accel.x * dt * dt;
      p.pos.y = 2 * p.pos.y - p.previous_pos.y + p.accel.y * dt * dt;

      // Updating particle's speed attribute
      p.speed.x = (p.pos.x - tmp_old_pos.x) / dt;
      p.speed.y = (p.pos.y - tmp_old_pos.y) / dt;

      p.previous_pos = tmp_old_pos;
    }
  }
};
