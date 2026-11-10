#pragma once

#include "drawable.hpp"
#include "entity.hpp"
#include "forces/force.hpp"
#include "renderer.hpp"
#include "vec2.hpp"
#include <deque>
#include <functional>
#include <ostream>
#include <vector>

class Euler;
class Verlet;
class World;
class Force;
class Spring;
class Gravity;

class Particle : public virtual Entity, public Drawable {

private:
  Vec2 pos;
  Vec2 previous_pos; // Used for Verlet integrator
  Vec2 speed;
  Vec2 accel;
  float mass;
  float bounciness; // make rebounds on the edge of the world
  bool moovable;
  std::vector<std::reference_wrapper<Force>> acting_forces;

public:
  const float radius;

  Particle();

  Particle(float x_init, float y_init, float mass, float bounciness,
           Force &gravity, Force &drag, bool moovable = true);

  Particle(const Vec2 &init_pos, float mass, float bounciness, Force &gravity,
           Force &drag, bool moovable = true);

  // Same, but the drawn / collision radius is chosen instead of being the mass
  Particle(const Vec2 &init_pos, float mass, float radius, float bounciness,
           Force &gravity, Force &drag, bool moovable = true);

  /*
   * update the acceleration based on all the force acting on the particle
   */
  void apply_force();

  // Attach one more force (wind, ...) to this particle
  void addForce(Force &f);

  // --- Read access (used by the forces, the scenes and the mouse) ---
  const Vec2 &getPos() const;
  const Vec2 &getSpeed() const;
  float getMass() const;
  bool isMoovable() const;

  // --- Write access (scenes and mouse) ---
  // Teleport the particle: the Verlet history follows so it does not jump
  void setPos(const Vec2 &p);
  // Give a speed to the particle, for both integrators (dt = physics step)
  void setVelocity(const Vec2 &v, float dt);
  void setMoovable(bool m);

  friend std::ostream &operator<<(std::ostream &os, const Particle &p);

  void draw(Renderer &renderer) override;

  friend Euler;
  friend Verlet;
  friend World;
  friend Force;
  friend Spring;
  friend Gravity;
};

// Container of the particles of a World. A deque never moves its elements on
// push_back, so the springs can safely keep pointers to the particles.
using ParticleList = std::deque<Particle>;
