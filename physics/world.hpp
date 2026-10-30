#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include "forces/force.hpp" //JUST TO DEBUG : NEED TO BE CHANGED
#include "integrators/integrators.hpp"
#include "particles.hpp"

enum class IntegratorType { Euler, Verlet };
inline std::unique_ptr<Integrator> makeIntegrator(IntegratorType type) {
  switch (type) {
  case IntegratorType::Euler:
    return std::make_unique<Euler>();
  case IntegratorType::Verlet:
    return std::make_unique<Verlet>();
  }
  return nullptr;
}

struct Borders {
  float x_min = 0.f, x_max = 10.f, y_min = 0.f, y_max = 10.f;
};

class World {

private:
  std::vector<Particle>
      particles; // Vector containing all the particles of the world
  std::unique_ptr<Integrator>
      integrator;        // Integrator algorithm which is used in the world
  Borders world_borders; // Saved as {x_min, x_max, y_min, y_max}

public:
  /*
   * Add one particle to the particles vector
   */
  void addPart(const Particle &new_part) { particles.push_back(new_part); }

  /*
   * Define which integrator the world uses
   */
  void setIntegrator(std::unique_ptr<Integrator> new_integrator) {
    integrator = std::move(new_integrator);
  }

  /*
   * Define the borders of the world
   */
  void setBorders(Borders b) { world_borders = b; }

  /*
   * Update all the forces on each particle
   */
  void updateForces(float dt) {
    (void)dt;
    for (Particle &p : particles) {
      p.apply_force();
    }
  }

  /*
   * Use the defined integrator to compute the new position
   */
  void updatePos(float dt) {
    if (integrator) {
      integrator->step(particles, dt);
    }

    // check that every particle is inside the world borders, and if that's not
    // the case move them inside
    for (Particle &p : particles) {
      if (p.pos.x < world_borders.x_min) {
        p.pos.x = world_borders.x_min;
        p.speed.x = 0;
      }
      if (p.pos.x > world_borders.x_max) {
        p.pos.x = world_borders.x_max;
        p.speed.x = 0;
      }
      if (p.pos.y < world_borders.y_min) {
        p.pos.y = world_borders.y_min;
        p.speed.y = 0;
      }
      if (p.pos.y > world_borders.y_max) {
        p.pos.y = world_borders.y_max;
        p.speed.y = 0;
      }
    }
  }

  /*
   * Print the stat of each particle of the world
   */
  friend std::ostream &operator<<(std::ostream &os, const World &w) {
    for (const Particle &p : w.particles) {
      os << p;
    }
    return os;
  }

  /*
   * Draw the new picture on the screen by calling all drawable objects
   */
  void draw(Renderer &renderer) {

    // Draw forces
    //  for (Forces &f: forces) {
    //      if (auto* d = dynamic_cast<Drawable*>(f.get())) { //Check if it is a
    //      drawable force d->draw(renderer);
    //      }
    //  }

    // Draw particles
    for (Particle &p : particles) {
      p.draw(renderer);
    }
  }
};
