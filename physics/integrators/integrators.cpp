#include "integrators.hpp"

void Euler::step(ParticleList &parts, float dt) {
  for (Particle &p : parts) {
    if (!p.moovable)
      continue;
    // Keep the previous position up to date too, so that the integrator can
    // be switched to Verlet at any time without a jump
    p.previous_pos = p.pos;
    p.speed += p.accel * dt; // Speed += acceleration * dt
    p.pos += p.speed * dt;   // Position += speed * dt
  }
}

void Verlet::step(ParticleList &parts, float dt) {
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
