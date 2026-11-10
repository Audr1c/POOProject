#include "force.hpp"
#include "../particles.hpp"

Force::Force(const std::string &name) : Entity(name) {}

Gravity::Gravity() : Entity("Tempory Forces") {}

Vec2 Gravity::compute(const Particle &a) { return Vec2(0.0f, -9.81f) * a.mass; }
