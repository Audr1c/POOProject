// Captures d'ecran des scenes pour la presentation (sans interaction).
//
//   ./capture <dossier> [largeur hauteur]
//
// Pour chaque scene : on la construit, on la simule pendant un temps choisi
// (pas fixe 1/120 s, integrateur de Verlet), puis on enregistre l'image de la
// fenetre dans <dossier>/scene_XX.png. Le rendu passe par le meme SFMLRender
// que le programme principal, l'image est donc identique a ce qu'on voit.
#include "../render/sfmlrenderer.hpp"
#include "../scenes/scenes.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "usage : capture <dossier> [largeur hauteur]\n";
    return 1;
  }
  const std::string out = argv[1];
  const unsigned w = argc > 3 ? static_cast<unsigned>(std::stoi(argv[2])) : 1600;
  const unsigned h = argc > 3 ? static_cast<unsigned>(std::stoi(argv[3])) : 900;

  // Instant de la capture pour chaque scene (s), dans l'ordre de allScenes()
  const float when[] = {0.55f, 2.2f, 1.6f, 3.f, 2.5f, 1.1f,
                        3.5f,  0.9f, 0.6f, 1.2f, 1.5f, 3.f, 2.f};

  SFMLRender rend;
  rend.create_window(w, h, "capture", 0);
  // Traits proportionnels a la taille : on peut capturer en 2x puis reduire
  // l'image (anticrenelage par sur-echantillonnage, utile sans carte graphique)
  const float k = static_cast<float>(w) / 1600.f;
  rend.setLineWidth(2.f * k);
  rend.setOutlineWidth(1.5f * k);

  const std::vector<Scene> &scenes = allScenes();
  for (std::size_t i = 0; i < scenes.size(); i++) {
    auto scene = instantiate(scenes[i], IntegratorType::Verlet);
    rend.fitWorld(scenes[i].size);
    const float t_end = i < sizeof(when) / sizeof(when[0]) ? when[i] : 2.f;
    while (scene->time < t_end)
      scene->step(kPhysicsDt);

    rend.clear();
    rend.drawWorldFrame();
    scene->world.draw(rend);

    sf::Texture tex;
    tex.create(w, h);
    tex.update(rend.win);
    // On ne garde que le monde (sans les bandes laterales), plus une marge
    const sf::Vector2f tl = rend.worldToScreen(Vec2(0.f, scenes[i].size.y));
    const sf::Vector2f br = rend.worldToScreen(Vec2(scenes[i].size.x, 0.f));
    const int m = static_cast<int>(6.f * k);
    const int x0 = std::max(0, static_cast<int>(tl.x) - m);
    const int y0 = std::max(0, static_cast<int>(tl.y) - m);
    const int x1 = std::min(static_cast<int>(w), static_cast<int>(br.x) + m);
    const int y1 = std::min(static_cast<int>(h), static_cast<int>(br.y) + m);
    const sf::Image full = tex.copyToImage();
    sf::Image crop;
    crop.create(static_cast<unsigned>(x1 - x0), static_cast<unsigned>(y1 - y0));
    crop.copy(full, 0, 0, sf::IntRect(x0, y0, x1 - x0, y1 - y0));
    char name[64];
    std::snprintf(name, sizeof(name), "/scene_%02zu.png", i + 1);
    crop.saveToFile(out + name);
    rend.display();
    std::cout << name + 1 << " : " << scenes[i].name << " ("
              << scene->world.particleCount() << " particules, "
              << scene->world.springCount() << " ressorts)\n";
  }
  rend.close();
  return 0;
}
