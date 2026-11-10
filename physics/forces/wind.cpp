#include "wind.hpp"
#include "../particles.hpp"
#include <utility>

Wind::Wind(std::function<Vec2(float)> profile, const float &time, float c)
    : Entity("Wind"), Force("Wind"), profile(std::move(profile)), time(time),
      c(c) {}

Vec2 Wind::compute(const Particle &p) {
  return c * (current() - p.getSpeed());
}

Vec2 Wind::current() const { return profile(time); }
