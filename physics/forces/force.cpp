#include "force.hpp"
#include "../particles.hpp"

Force::Force(const std::string &name) : Entity(name) {}

Gravity::Gravity(const Vec2 &g) : Entity("Gravity"), Force("Gravity"), g(g) {}

Vec2 Gravity::compute(const Particle &a) { return g * a.getMass(); }

void Gravity::setG(const Vec2 &new_g) { g = new_g; }
const Vec2 &Gravity::getG() const { return g; }
