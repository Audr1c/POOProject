#include "world.hpp"
#include "forces/springs.hpp"
#include <algorithm>

std::unique_ptr<Integrator> makeIntegrator(IntegratorType type) {
  switch (type) {
  case IntegratorType::Euler:
    return std::make_unique<Euler>();
  case IntegratorType::Verlet:
    return std::make_unique<Verlet>();
  }
  return nullptr;
}

const char *toString(IntegratorType type) {
  switch (type) {
  case IntegratorType::Euler:
    return "Euler";
  case IntegratorType::Verlet:
    return "Verlet";
  }
  return "?";
}

/*
 * Add one particle to the particles container
 */
Particle &World::addPart(const Particle &new_part) {
  particles.push_back(new_part);
  return particles.back();
}

Spring &World::addSpring(Particle &a, Particle &b, float L0, float k,
                         float rho) {
  // built in place: the Spring registers itself on a and b, it must not be
  // copied afterwards
  return springs.emplace_back(a, b, L0, k, rho);
}

Spring &World::addSpring(Particle &a, Particle &b, float k, float rho) {
  return addSpring(a, b, (b.getPos() - a.getPos()).norm(), k, rho);
}

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
const Borders &World::getBorders() const { return world_borders; }

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
  // the case move them inside and make them rebound
  for (Particle &p : particles) {
    if (p.pos.x - p.radius < world_borders.x_min) {
      p.pos.x = world_borders.x_min + p.radius + p.bounciness*(world_borders.x_min - (p.pos.x - p.radius));
      p.speed.x = -p.speed.x * p.bounciness;
      p.previous_pos = p.pos - p.speed * dt;
    }
    if (p.pos.x + p.radius > world_borders.x_max) {
      p.pos.x = world_borders.x_max - p.radius - p.bounciness*((p.pos.x + p.radius) - world_borders.x_max );
      p.speed.x = -p.speed.x * p.bounciness;
      p.previous_pos = p.pos - p.speed * dt;
    }
    if (p.pos.y - p.radius < world_borders.y_min) {
      p.pos.y = world_borders.y_min + p.radius + p.bounciness*(world_borders.y_min - (p.pos.y - p.radius));
      p.speed.y = -p.speed.y * p.bounciness;
      p.previous_pos = p.pos - p.speed * dt;
    }
    if (p.pos.y + p.radius > world_borders.y_max) {
      p.pos.y = world_borders.y_max - p.radius - p.bounciness*((p.pos.y + p.radius) - world_borders.y_max );
      p.speed.y = -p.speed.y * p.bounciness;
      p.previous_pos = p.pos - p.speed * dt;
    }
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

  // Draw springs
  for (Spring &s : springs) {
    s.draw(renderer);
  }

  // Draw particles
  for (Particle &p : particles) {
    p.draw(renderer);
  }
}

std::vector<Entity *> World::entities() {
  std::vector<Entity *> all;
  all.reserve(particles.size() + springs.size());
  for (Particle &p : particles)
    all.push_back(&p); // upcast to the single shared Entity sub-object
  for (Spring &s : springs)
    all.push_back(&s);
  return all;
}

Particle *World::pickParticle(const Vec2 &at, float max_dist) {
  Particle *best = nullptr;
  float best_dist = max_dist;

  // Lambda: keep the particle closest to the click. dynamic_cast filters the
  // Entity* that really are particles (springs are skipped).
  auto closer = [&](Entity *e) {
    if (Particle *p = dynamic_cast<Particle *>(e)) {
      const float d = (p->getPos() - at).norm() - p->radius;
      if (d < best_dist) {
        best_dist = d;
        best = p;
      }
    }
  };

  std::vector<Entity *> all = entities();
  std::for_each(all.begin(), all.end(), closer);
  return best;
}

std::size_t World::particleCount() const { return particles.size(); }
std::size_t World::springCount() const { return springs.size(); }

Particle &World::getParticle(int id) {
  for (Particle &p : particles) {
    if (p.getId() == id)
      return p;
  }
  throw std::out_of_range("Particle id " + std::to_string(id) + " introuvable");
}
