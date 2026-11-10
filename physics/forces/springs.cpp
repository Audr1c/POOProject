#include "springs.hpp"

#include "../particles.hpp"

#include <algorithm>

Spring::Spring(Particle &first, Particle &second, float L0, float k, float rho)
    : Entity("Spring"), Force("Spring"), Drawable(), ends{&first, &second},
      L0(L0), k(k), rho(rho) {
  // Registers itself on both particles
  first.addForce(*this);
  second.addForce(*this);
}

Vec2 Spring::compute(const Particle &particle) {
  const Vec2 displacement = ends[1]->getPos() - ends[0]->getPos();
  const float length = displacement.norm();
  if (length == 0.0f)
    return Vec2();

  const Vec2 direction = displacement / length;
  const float relative_speed =
      (ends[1]->getSpeed() - ends[0]->getSpeed()).dot(direction);
  const Vec2 force_on_first =
      direction * (k * (length - L0) + rho * relative_speed);

  if (&particle == ends[0])
    return force_on_first;
  if (&particle == ends[1])
    return -force_on_first;
  return Vec2();
}

float Spring::getRestLength() const { return L0; }

float Spring::getLength() const {
  return (ends[1]->getPos() - ends[0]->getPos()).norm();
}

void Spring::draw(Renderer &renderer) {
  if (!visible)
    return;

  // strain in [-1, 1]: 25 % of stretch gives the full tint
  float strain = L0 > 0.f ? (getLength() - L0) / L0 * 4.f : 0.f;
  strain = std::clamp(strain, -1.f, 1.f);

  const Color target = strain > 0.f ? Color(255, 60, 60) : Color(80, 140, 255);
  const float t = strain > 0.f ? strain : -strain;
  auto mix = [t](unsigned char a, unsigned char b) {
    return static_cast<unsigned char>(static_cast<float>(a) * (1.f - t) +
                                      static_cast<float>(b) * t);
  };
  const Color c(mix(stroke_color.r, target.r), mix(stroke_color.g, target.g),
                mix(stroke_color.b, target.b), stroke_color.a);
  renderer.drawLine(ends[0]->getPos(), ends[1]->getPos(), c);
}
