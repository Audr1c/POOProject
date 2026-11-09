#include "force.hpp"

Force::Force(const std::string &name) : Entity(name) {}

TempForce::TempForce() : Entity("Tempory Forces") {}

Vec2 TempForce::compute(const Particle &) { return Vec2(0.0f, -9.81f); }
