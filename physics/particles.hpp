/* DEBUG ONLY FOR TESTING WORLD AND INTEGRATORS*/

#pragma once

#include "drawable.hpp"
#include "entity.hpp"
#include "forces/force.hpp"
#include "renderer.hpp"
#include "vec2.hpp"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

class Euler;
class Verlet;
class World;
class Force;
class TempForce;

class Particle : public virtual Entity, Drawable {

private:
  Vec2 pos;
  Vec2 previous_pos; // Used for Verlet integrator
  Vec2 speed;
  Vec2 accel;
  float mass;
  bool moovable;
  std::vector<std::reference_wrapper<Force>> acting_forces;

public:
  Particle(float x_init, float y_init, float mass, Force &gravity, Force &drag,
           bool moovable = true)
      : Entity("Particle, starting at : " + std::to_string(x_init) + "," +
               std::to_string(y_init)),
        Drawable(), pos(x_init, y_init), accel(0, 0), mass(mass),
        moovable(moovable), acting_forces({gravity, drag}) {
    previous_pos = pos;
  }

  Particle(const Vec2 &init_pos, float mass, Force &gravity, Force &drag,
           bool moovable = true)
      : Particle(init_pos.x, init_pos.y, mass, gravity, drag, moovable) {}

  /*
   * update the acceleration based on all the force acting on the particle
   */
  void apply_force() {
    if (!this->moovable)
      return;
    this->accel = Vec2(0, 0);
    for (std::reference_wrapper<Force> f : acting_forces) {
      Vec2 force = f.get().compute(*this);
      this->accel += force / mass; // 2nd law of Newton
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const Particle &p) {
    return os << "Particle(" + std::to_string(p.pos.x) + "," +
                     std::to_string(p.pos.y) + ")";
  }

  void draw(Renderer &renderer) { renderer.drawCircle(this->pos, this->mass); };

  friend Euler;
  friend Verlet;
  friend World;
  friend Force;
  friend TempForce;
};
