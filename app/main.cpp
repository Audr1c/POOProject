#include "../physics/particles.hpp"
#include "../physics/vec2.hpp"
#include "../physics/world.hpp"
#include "../render/sfmlrenderer.hpp"
#include "../scenes/scenes.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

namespace {

// Particle held by the mouse
struct Grab {
  Particle *particle = nullptr;
  bool was_moovable = true;
  Vec2 last_mouse;
  Vec2 velocity; // smoothed mouse velocity, given back on release (throw)
};

void printHelp(const std::vector<Scene> &scenes) {
  std::cout << "\n=== Moteur masses-ressorts : choix de la scene ===\n";
  for (std::size_t i = 0; i < scenes.size(); i++) {
    std::string key = i < 9    ? std::to_string(i + 1)
                      : i == 9 ? std::string("0")
                               : std::string("->");
    std::cout << "  [" << key << "] " << scenes[i].name << " : "
              << scenes[i].description << "\n";
  }
  std::cout << "\n  Fleches gauche/droite : scene precedente/suivante\n"
               "  R : recommencer la scene     I : changer d'integrateur\n"
               "  Espace : pause               N : un pas (en pause)\n"
               "  Clic gauche glisse : attraper/lancer une particule\n"
               "  Clic droit : epingler/liberer une particule\n"
               "  Echap : quitter\n"
            << std::endl;
}

Vec2 clampLength(const Vec2 &v, float max_len) {
  const float n = v.norm();
  return n > max_len ? v * (max_len / n) : v;
}

} // namespace

int main() {
  const std::vector<Scene> &scenes = allScenes();
  printHelp(scenes);

  // Renderer: resizable window, each scene is rescaled to fit in it
  SFMLRender rend;
  rend.create_window(1280, 720, "Simulation Masses-Ressorts", 60);

  IntegratorType integrator = IntegratorType::Verlet;
  std::size_t index = 0;
  std::unique_ptr<SceneInstance> scene;
  bool paused = false;
  Grab grab;

  auto updateTitle = [&]() {
    std::string title = "Masses-ressorts | " + std::to_string(index + 1) + "/" +
                        std::to_string(scenes.size()) + " " +
                        scenes[index].name + " | " + toString(integrator) +
                        " | " + std::to_string(scene->world.particleCount()) +
                        " particules, " +
                        std::to_string(scene->world.springCount()) +
                        " ressorts";
    if (paused)
      title += " | PAUSE";
    rend.setTitle(title);
  };

  auto load = [&](std::size_t i) {
    index = i % scenes.size();
    grab = Grab(); // the old particles are about to be destroyed
    scene = instantiate(scenes[index], integrator);
    rend.fitWorld(scenes[index].size);
    std::cout << "Scene " << index + 1 << " : " << scenes[index].name << " ("
              << toString(integrator) << ")" << std::endl;
    updateTitle();
  };

  auto release = [&]() {
    if (!grab.particle)
      return;
    grab.particle->setMoovable(grab.was_moovable);
    if (grab.was_moovable)
      grab.particle->setVelocity(clampLength(grab.velocity, 25.f), kPhysicsDt);
    grab = Grab();
  };

  load(0);

  // Simulation clock: fixed time step + accumulator
  const float dt = kPhysicsDt;
  const float max_frame_time = 0.25f; // avoids the "spiral of death"
  float accumulator = 0.f;
  sf::Clock clock;

  while (rend.isOpen()) {
    // --- Events ---
    sf::Event event;
    while (rend.pollEvent(event)) {
      if (rend.handleWindowEvent(event))
        continue;

      if (event.type == sf::Event::KeyPressed) {
        const sf::Keyboard::Key key = event.key.code;
        if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
          const int d = key - sf::Keyboard::Num0;
          const std::size_t i = d == 0 ? 9 : static_cast<std::size_t>(d - 1);
          if (i < scenes.size())
            load(i);
        } else if (key >= sf::Keyboard::Numpad0 &&
                   key <= sf::Keyboard::Numpad9) {
          const int d = key - sf::Keyboard::Numpad0;
          const std::size_t i = d == 0 ? 9 : static_cast<std::size_t>(d - 1);
          if (i < scenes.size())
            load(i);
        } else if (key == sf::Keyboard::Right) {
          load(index + 1);
        } else if (key == sf::Keyboard::Left) {
          load(index + scenes.size() - 1);
        } else if (key == sf::Keyboard::R) {
          load(index);
        } else if (key == sf::Keyboard::I) {
          integrator = integrator == IntegratorType::Euler
                           ? IntegratorType::Verlet
                           : IntegratorType::Euler;
          scene->world.setIntegrator(makeIntegrator(integrator));
          std::cout << "Integrateur : " << toString(integrator) << std::endl;
          updateTitle();
        } else if (key == sf::Keyboard::Space) {
          paused = !paused;
          updateTitle();
        } else if (key == sf::Keyboard::N && paused) {
          scene->step(dt);
        }
      } else if (event.type == sf::Event::MouseButtonPressed) {
        const Vec2 m =
            rend.screenToWorld(event.mouseButton.x, event.mouseButton.y);
        Particle *p = scene->world.pickParticle(m, 0.4f);
        if (event.mouseButton.button == sf::Mouse::Left && p) {
          release();
          grab.particle = p;
          grab.was_moovable = p->isMoovable();
          grab.last_mouse = m;
          p->setMoovable(false); // the mouse drives it, not the integrator
        } else if (event.mouseButton.button == sf::Mouse::Right && p &&
                   p != grab.particle) {
          p->setMoovable(!p->isMoovable());
          p->setVelocity(Vec2(), dt);
        }
      } else if (event.type == sf::Event::MouseButtonReleased &&
                 event.mouseButton.button == sf::Mouse::Left) {
        release();
      }
    }

    float frame_time = clock.restart().asSeconds();
    if (frame_time > max_frame_time)
      frame_time = max_frame_time; // e.g. after dragging the window

    // --- Mouse drag: the grabbed particle follows the cursor ---
    if (grab.particle) {
      const Borders &b = scene->world.getBorders();
      Vec2 m = rend.getMouseWorldPos();
      m.x = std::clamp(m.x, b.x_min, b.x_max);
      m.y = std::clamp(m.y, b.y_min, b.y_max);
      if (frame_time > 0.f)
        grab.velocity =
            grab.velocity * 0.5f + (m - grab.last_mouse) / frame_time * 0.5f;
      grab.last_mouse = m;
      grab.particle->setPos(m);
      grab.particle->setVelocity(grab.velocity, dt); // seen by the springs
    }

    // --- Physics: as many fixed steps as the elapsed time allows ---
    if (!paused) {
      accumulator += frame_time;
      while (accumulator >= dt) {
        scene->step(dt);
        accumulator -= dt;
      }
    } else {
      accumulator = 0.f;
    }

    // --- Drawing ---
    rend.clear();
    rend.drawWorldFrame();
    scene->world.draw(rend);
    if (grab.particle) {
      const Vec2 &p = grab.particle->getPos();
      rend.drawCircle(p, grab.particle->radius + 0.08f, Color(255, 255, 255, 90));
      rend.drawCircle(p, grab.particle->radius, Color(255, 255, 255));
    }
    rend.display();
  }

  std::cout << "Finished" << std::endl;
}
