// Energie totale d'une scene au cours du temps, avec les deux integrateurs.
//
//   ./energie <fichier.csv> [numero de scene = 1] [duree en s = 10]
//
// Ecrit t, E_euler, E_verlet (en joules) tous les 1/60 s :
//   E = sum(1/2 m v^2) - sum(m g.x) + sum(1/2 k (L - L0)^2)
// Les ressorts sont retrouves parmi les Entity* du monde par dynamic_cast,
// comme la particule attrapee a la souris.
#include "../physics/forces/springs.hpp"
#include "../scenes/scenes.hpp"

#include <cstdio>
#include <string>
#include <vector>

namespace {

float totalEnergy(SceneInstance &s) {
  const Vec2 g = s.gravity.getG();
  float e = 0.f;
  for (Entity *ent : s.world.entities()) {
    if (const Particle *p = dynamic_cast<const Particle *>(ent)) {
      if (!p->isMoovable())
        continue;
      const float m = p->getMass();
      e += 0.5f * m * p->getSpeed().normSquared() - m * g.dot(p->getPos());
    } else if (const Spring *sp = dynamic_cast<const Spring *>(ent)) {
      e += sp->energy();
    }
  }
  return e;
}

} // namespace

int main(int argc, char **argv) {
  if (argc < 2) {
    std::fprintf(stderr, "usage : energie <fichier.csv> [scene] [duree]\n");
    return 1;
  }
  const std::size_t index = argc > 2 ? std::stoul(argv[2]) - 1 : 0;
  const float duration = argc > 3 ? std::stof(argv[3]) : 10.f;
  const Scene &scene = allScenes().at(index);

  auto euler = instantiate(scene, IntegratorType::Euler);
  auto verlet = instantiate(scene, IntegratorType::Verlet);

  std::FILE *out = std::fopen(argv[1], "w");
  if (!out)
    return 1;
  std::fprintf(out, "t,euler,verlet\n");
  const int steps = static_cast<int>(duration / kPhysicsDt);
  for (int i = 0; i <= steps; i++) {
    if (i % 2 == 0)
      std::fprintf(out, "%.4f,%.5f,%.5f\n", static_cast<double>(euler->time),
                   static_cast<double>(totalEnergy(*euler)),
                   static_cast<double>(totalEnergy(*verlet)));
    euler->step(kPhysicsDt);
    verlet->step(kPhysicsDt);
  }
  std::fclose(out);
  std::printf("%s : %d pas, ecrit dans %s\n", scene.name.c_str(), steps, argv[1]);
  return 0;
}
