#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "entity.hpp"
#include "forces/force.hpp"
#include "forces/springs.hpp"
#include "integrators/integrators.hpp"
#include "particles.hpp"

enum class IntegratorType { Euler, Verlet };
std::unique_ptr<Integrator> makeIntegrator(IntegratorType type);
const char *toString(IntegratorType type);

struct Borders {
  float x_min = 0.f, x_max = 10.f, y_min = 0.f, y_max = 10.f;
};

class World {

private:
  // deque: adding a particle never moves the others, so the springs (and the
  // mouse) can keep pointers to them
  ParticleList particles;
  // deque too: a Spring must never move (particles keep a reference to it),
  // and push_back/emplace_back on a deque never moves the existing elements
  std::deque<Spring> springs;
  // Integrator algorithm which is used in the world
  std::unique_ptr<Integrator> integrator;
  Borders world_borders; // Saved as {x_min, x_max, y_min, y_max}

public:
  World() = default;

  // Rule of three: copying a World is forbidden. The springs point to the
  // particles of THIS world and the particles hold references to its
  // springs: a copy would have to rebuild every link, and copying a whole
  // simulation is never needed (a scene is rebuilt from its function).
  World(const World &) = delete;
  World &operator=(const World &) = delete;
  ~World() = default;

  /*
   * Add one particle to the world. Returns the particle stored in the world
   * (not the argument, which is copied), to attach springs to it.
   */
  Particle &addPart(const Particle &new_part);

  // Spring with an explicit rest length
  Spring &addSpring(Particle &a, Particle &b, float L0, float k, float rho);
  // Spring whose rest length is the current distance between a and b
  Spring &addSpring(Particle &a, Particle &b, float k, float rho);

  /*
   * Define which integrator the world uses
   */
  void setIntegrator(std::unique_ptr<Integrator> new_integrator);

  /*
   * Define the borders of the world
   */
  void setBorders(Borders b);
  const Borders &getBorders() const;

  /*
   * Update all the forces on each particle
   */
  void updateForces(float dt);

  /*
   * Use the defined integrator to compute the new position
   */
  void updatePos(float dt);

  /*
   * Print the stat of each particle of the world
   */
  friend std::ostream &operator<<(std::ostream &os, const World &w);

  /*
   * Draw the new picture on the screen by calling all drawable objects
   */
  void draw(Renderer &renderer);

  // Every entity of the world (particles and springs), seen through the base
  std::vector<Entity *> entities();

  // Particle closest to `at` among all entities (lambda + dynamic_cast), or
  // nullptr if none is closer than max_dist
  Particle *pickParticle(const Vec2 &at, float max_dist);

  std::size_t particleCount() const;
  std::size_t springCount() const;

  // getter a particule
  Particle &getParticle(int id);
};
