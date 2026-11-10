#include "../physics/entity.hpp"
#include "../physics/forces/force.hpp"
#include "../physics/forces/springs.hpp"
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
  int idsOfParticles[8];
  const Vec2 first(2.f, 8.f);
  const Vec2 step(1.5f, -0.6f);
  for (int i = 0; i < 8; i++) {
    float mass = 0.15f + 0.05f * static_cast<float>(i);
    float bounciness = 1;
    Particle p = Particle(first + step * static_cast<float>(i), mass,
                          bounciness, gravity, drag);
    w.addPart(p);
    idsOfParticles[i] = p.getId();
  }
  w.addPart(Particle(world_size.x / 2.f, world_size.y - 0.5f, 0.3f, 0, gravity,
                     drag, false));

  // add springs
  for (int i = 0; i < 7; i++) {
    w.addSpring(w.getParticle(idsOfParticles[i]),
                w.getParticle(idsOfParticles[i + 1]), 1, 1, .01);
  }
  // 5. Simulation clock: fixed time step (required by Verlet) + accumulator
  const float dt = 1.f / 120.f; // physics step, independent of the frame rate
  const float max_frame_time = 0.25f; // avoids the "spiral of death"
  float accumulator = 0.f;
  sf::Clock clock;

  // 6. Main loop
  while (rend.isOpen()) {
    rend.handleBasicEvents(); // close / ESC / resize

    float frame_time = clock.restart().asSeconds();
    if (frame_time > max_frame_time)
      frame_time = max_frame_time; // e.g. after dragging the window
    accumulator += frame_time;

    // Run as many fixed steps as the real time elapsed allows
    while (accumulator >= dt) {
      w.updateForces(dt);
      w.updatePos(dt);
      accumulator -= dt;
    }

    rend.clear();
    rend.drawWorldFrame();
    w.draw(rend);
    rend.display();
  }

  cout << "Finished" << endl;
}
