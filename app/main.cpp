#include "../physics/entity.hpp"
#include "../physics/particles.hpp"
#include "../physics/renderer.hpp"
#include "../physics/vec2.hpp"
#include "../physics/world.hpp"
#include "../render/sfmlrenderer.hpp"
#include <iostream>
using namespace std;

// Placeholder force that does nothing (used while real drag is not written)
class NoForce : public Force {
public:
  NoForce() : Entity("No force") {}
  Vec2 compute(const Particle &) override { return Vec2(); }
};

int main() {

  cout << "Setup !" << endl;

  // World size in meters (origin bottom-left, +y up)
  const Vec2 world_size(16.f, 9.f);

  // 1. Renderer: resizable window, the world is rescaled to always fit in it
  SFMLRender rend;
  rend.create_window(1280, 720, "Simulation Masses-Ressorts", 60);
  rend.fitWorld(world_size);

  // 2. World and borders
  World w;
  w.setBorders({0.f, world_size.x, 0.f, world_size.y});

  // 3. Forces and integrator
  Gravity gravity; // (0, -9.81)
  NoForce drag;
  w.setIntegrator(makeIntegrator(IntegratorType::Euler));

  // 4. Scene: a row of particles of growing size dropped from different
  //    heights, plus a fixed anchor at the top (the particle radius is its
  //    mass)
  const Vec2 first(2.f, 8.f);
  const Vec2 step(1.5f, -0.6f);
  for (int i = 0; i < 8; i++) {
    float mass = 0.15f + 0.05f * static_cast<float>(i);
    w.addPart(
        Particle(first + step * static_cast<float>(i), mass, gravity, drag));
  }
  w.addPart(Particle(world_size.x / 2.f, world_size.y - 0.5f, 0.3f, gravity,
                     drag, false));

  // 5. Simulation clock
  sf::Clock clock;

  // 6. Main loop
  while (rend.isOpen()) {
    rend.handleBasicEvents(); // close / ESC / resize

    float dt = clock.restart().asSeconds();
    if (dt > 0.05f)
      dt = 0.05f; // clamp delta time

    w.updateForces(dt);
    w.updatePos(dt);

    rend.clear();
    rend.drawWorldFrame();
    w.draw(rend);
    rend.display();
  }

  cout << "Finished" << endl;
}
