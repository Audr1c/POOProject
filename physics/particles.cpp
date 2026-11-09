#include "particles.hpp"
#include <string>

Particle::Particle(float x_init, float y_init, float mass, Force &gravity,
                   Force &drag, bool moovable)
    : Entity("Particle, starting at : " + std::to_string(x_init) + "," +
             std::to_string(y_init)),
      Drawable(), pos(x_init, y_init), accel(0, 0), mass(mass),
      moovable(moovable), acting_forces({gravity, drag}) {
  previous_pos = pos;
}

Particle::Particle(const Vec2 &init_pos, float mass, Force &gravity,
                   Force &drag, bool moovable)
    : Particle(init_pos.x, init_pos.y, mass, gravity, drag, moovable) {}

/*
 * update the acceleration based on all the force acting on the particle
 */
void Particle::apply_force() {
  if (!this->moovable)
    return;
  this->accel = Vec2(0, 0);
  for (std::reference_wrapper<Force> f : acting_forces) {
    Vec2 force = f.get().compute(*this);
    this->accel += force / mass; // 2nd law of Newton
  }
}

std::ostream &operator<<(std::ostream &os, const Particle &p) {
  return os << "Particle(" + std::to_string(p.pos.x) + "," +
                   std::to_string(p.pos.y) + ")";
}

void Particle::draw(Renderer &renderer) {
  renderer.drawCircle(this->pos, this->mass);
}
