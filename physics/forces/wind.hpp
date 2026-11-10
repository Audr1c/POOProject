#pragma once
#include "force.hpp"
#include <functional>

// Variable wind (J4 extension). The wind speed is a function of time given as
// a lambda; the force pushes the particle towards the wind speed:
//   F = c * (w(t) - v)
// The force only reads the clock, it never modifies World.
class Wind : public Force {
private:
  std::function<Vec2(float)> profile; // wind speed (m/s) at time t (s)
  const float &time;                  // simulation clock, owned by the scene
  float c;                            // coupling coefficient (kg/s)

public:
  Wind(std::function<Vec2(float)> profile, const float &time, float c);
  Vec2 compute(const Particle &p) override;
  Vec2 current() const; // wind speed right now
};
