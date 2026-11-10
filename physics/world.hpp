#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include "forces/force.hpp" //JUST TO DEBUG : NEED TO BE CHANGED
#include "forces/springs.hpp"
#include "integrators/integrators.hpp"
#include "particles.hpp"

enum class IntegratorType { Euler, Verlet };
std::unique_ptr<Integrator> makeIntegrator(IntegratorType type);

struct Borders {
  float x_min = 0.f, x_max = 10.f, y_min = 0.f, y_max = 10.f;
};

class World {

private:
  std::vector<Particle>
      particles; // Vector containing all the particles of the world
  std::vector<Spring> springs;
  std::unique_ptr<Integrator>
      integrator;        // Integrator algorithm which is used in the world
  Borders world_borders; // Saved as {x_min, x_max, y_min, y_max}

public:
  /*
   * Add one particle to the particles vector
   */
  void addPart(const Particle &new_part);

  void addSpring(Particle &a, Particle &b, float L0, float k, float rho);

  /*
   * Define which integrator the world uses
   */
  void setIntegrator(std::unique_ptr<Integrator> new_integrator);

  /*
   * Define the borders of the world
   */
  void setBorders(Borders b);

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
};
