#include "drag.hpp"
#include "../particles.hpp"

Drag::Drag(float b) : Entity("Drag"), Force("Drag"), b(b) {}

Vec2 Drag::compute(const Particle &p) { return -b * p.getSpeed(); }

void Drag::setCoefficient(float new_b) { b = new_b; }
float Drag::getCoefficient() const { return b; }
