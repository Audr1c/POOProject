#include "world.hpp"

std::unique_ptr<Integrator> makeIntegrator(IntegratorType type) {
  switch (type) {
  case IntegratorType::Euler:
    return std::make_unique<Euler>();
  case IntegratorType::Verlet:
    return std::make_unique<Verlet>();
  }
  return nullptr;
}

/*
 * Add one particle to the particles vector
 */
void World::addPart(const Particle &new_part) { particles.push_back(new_part); }


/*void World::addSpring(Particle &a, Particle &b, float L0, float k,
                         float rho) {
  return springs.push_back(std::make_unique<Spring>(a, b, L0, k, rho));
  
}*/



/*
 * Define which integrator the world uses
 */
void World::setIntegrator(std::unique_ptr<Integrator> new_integrator) {
  integrator = std::move(new_integrator);
}

/*
 * Define the borders of the world
 */
void World::setBorders(Borders b) { world_borders = b; }

/*
 * Update all the forces on each particle
 */
void World::updateForces(float dt) {
  (void)dt;
  for (Particle &p : particles) {
    p.apply_force();
  }
}

/*
 * Use the defined integrator to compute the new position
 */
void World::updatePos(float dt) {
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
    // Verlet keeps its own history: keep it consistent after a clamp
    if (p.pos.y == world_borders.y_min || p.pos.x == world_borders.x_min ||
        p.pos.x == world_borders.x_max || p.pos.y == world_borders.y_max)
      p.previous_pos = p.pos;
  }
}

/*
 * Print the stat of each particle of the world
 */
std::ostream &operator<<(std::ostream &os, const World &w) {
  for (const Particle &p : w.particles) {
    os << p;
  }
  return os;
}

/*
 * Draw the new picture on the screen by calling all drawable objects
 */
void World::draw(Renderer &renderer) {

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
