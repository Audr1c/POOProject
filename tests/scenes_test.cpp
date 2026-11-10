// Headless check of every scene (no SFML): each scene is simulated for 10 s
// with both integrators, and must stay finite and reasonably calm.
#include "../physics/particles.hpp"
#include "../physics/world.hpp"
#include "../scenes/scenes.hpp"

#include <cmath>
#include <cstdio>

int main() {
  int failures = 0;
  for (const Scene &scene : allScenes()) {
    for (IntegratorType type : {IntegratorType::Euler, IntegratorType::Verlet}) {
      auto inst = instantiate(scene, type);
      float max_speed = 0.f;
      bool finite = true;
      const int steps = static_cast<int>(10.f / kPhysicsDt);
      for (int i = 0; i < steps && finite; i++) {
        inst->step(kPhysicsDt);
        for (Entity *e : inst->world.entities())
          if (const Particle *p = dynamic_cast<const Particle *>(e)) {
            const Vec2 &x = p->getPos();
            const Vec2 &v = p->getSpeed();
            if (!std::isfinite(x.x) || !std::isfinite(x.y)) {
              finite = false;
              break;
            }
            max_speed = std::fmax(max_speed, v.norm());
          }
      }
      const bool ok = finite && max_speed < 60.f;
      if (!ok)
        failures++;
      std::printf("%-4s %-18s %-6s  max |v| = %6.2f m/s\n", ok ? "OK" : "FAIL",
                  scene.name.c_str(), toString(type), max_speed);
    }
  }
  std::printf("%d echec(s)\n", failures);
  return failures == 0 ? 0 : 1;
}
