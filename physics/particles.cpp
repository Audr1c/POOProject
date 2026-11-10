#include "particles.hpp"
#include "drawable.hpp"
#include "entity.hpp"
#include <string>

Particle::Particle() : Entity("Particule error"), Drawable(), radius(0) {};

Particle::Particle(float x_init, float y_init, float mass, float bounciness,
                   Force &gravity, Force &drag, bool moovable)
    : Particle(Vec2(x_init, y_init), mass, mass, bounciness, gravity, drag,
               moovable) {}

Particle::Particle(const Vec2 &init_pos, float mass, float bounciness,
                   Force &gravity, Force &drag, bool moovable)
    : Particle(init_pos, mass, mass, bounciness, gravity, drag, moovable) {}

Particle::Particle(const Vec2 &init_pos, float mass, float radius,
                   float bounciness, Force &gravity, Force &drag, bool moovable)
    : Entity("Particle, starting at : " + std::to_string(init_pos.x) + "," +
             std::to_string(init_pos.y)),
      Drawable(), pos(init_pos), previous_pos(init_pos), accel(0, 0),
      mass(mass), bounciness(bounciness), moovable(moovable),
      acting_forces({gravity, drag}), radius(radius) {}

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

void Particle::addForce(Force &f) { acting_forces.push_back(f); }

const Vec2 &Particle::getPos() const { return pos; }
const Vec2 &Particle::getSpeed() const { return speed; }
float Particle::getMass() const { return mass; }
bool Particle::isMoovable() const { return moovable; }

void Particle::setPos(const Vec2 &p) {
  const Vec2 shift = p - pos;
  pos = p;
  previous_pos += shift; // keep the Verlet speed unchanged
}

void Particle::setVelocity(const Vec2 &v, float dt) {
  speed = v;
  previous_pos = pos - v * dt; // Verlet deduces the speed from this
}

void Particle::setMoovable(bool m) { moovable = m; }

std::ostream &operator<<(std::ostream &os, const Particle &p) {
  return os << "Particle(" + std::to_string(p.pos.x) + "," +
                   std::to_string(p.pos.y) + ")";
}

void Particle::draw(Renderer &renderer) {
  if (!visible)
    return;
  renderer.drawCircle(this->pos, this->radius, fill_color);
}
