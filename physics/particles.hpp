/* DEBUG ONLY FOR TESTING WORLD AND INTEGRATORS*/

#pragma once

#include "drawable.hpp"
#include "entity.hpp"
#include "forces/force.hpp"
#include "renderer.hpp"
#include "vec2.hpp"
#include <functional>
#include <ostream>
#include <vector>

class Euler;
class Verlet;
class World;
class Force;
class Spring;
class Gravity;

class Particle : public virtual Entity, Drawable {

private:
  Vec2 pos;
  Vec2 previous_pos; // Used for Verlet integrator
  Vec2 speed;
  Vec2 accel;
  float mass;
  float bounciness; //make rebounds on the edge of the world
  bool moovable;
  std::vector<std::reference_wrapper<Force>> acting_forces;

public:

  const float radius;

  Particle(float x_init, float y_init, float mass, float bounciness, Force &gravity, Force &drag,
           bool moovable = true);

  Particle(const Vec2 &init_pos, float mass, float bounciness, Force &gravity, Force &drag,
           bool moovable = true);

  /*
   * update the acceleration based on all the force acting on the particle
   */
  void apply_force();


  

  friend std::ostream &operator<<(std::ostream &os, const Particle &p);

  void draw(Renderer &renderer);

  friend Euler;
  friend Verlet;
  friend World;
  friend Force;
  friend Spring;
  friend Gravity;
};
