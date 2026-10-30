#include "../physics/entity.hpp"
#include "../physics/particles.hpp"
#include "../physics/renderer.hpp"
#include "../physics/world.hpp"
#include "../render/sfmlrenderer.hpp"
#include <iostream>
using namespace std;

int Entity::current_ids = 0;

int main() {

  cout << "Hello Poop! DRIC" << endl;
  cout << "Setup !" << endl;

  // 1. Instantiating a Renderer and opening the window
  SFMLRender rend;
  rend.create_window(800, 600, "Simulation Masses-Ressorts", 60);

  // 2. Creating the world and setting borders
  World w;
  w.setBorders({0.f, 800.f, 0.f, 600.f});

  // 3. Setting up forces and integrator
  TempForce gravity;
  TempForce drag;
  w.setIntegrator(makeIntegrator(IntegratorType::Euler));

  // 4. Creating a particle
  Particle p1(400.f, 100.f, 15.f, gravity, drag);
  w.addPart(p1);

  // 5. Simulation clock
  sf::Clock clock;

  // 6. Main render loop
  while (rend.isOpen()) {
    // Handle close / ESC events
    cout << "In the loop" << endl;
    rend.handleBasicEvents();

    float dt = clock.restart().asSeconds();
    if (dt > 0.05f)
      dt = 0.05f; // clamp delta time

    // Physics step
    w.updateForces(dt);
    w.updatePos(dt);

    // Render step
    rend.clear();
    w.draw(rend);
    rend.display();
  }

  cout << "Finished" << endl;
}
