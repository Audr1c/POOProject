#include "scenes.hpp"

#include <cmath>
#include <random>
#include <utility>

// ---------------------------------------------------------------------------
// SceneInstance
// ---------------------------------------------------------------------------

SceneInstance::SceneInstance(const Vec2 &size)
    : gravity(), drag(0.f), size(size) {}

Particle &SceneInstance::addParticle(const Vec2 &pos, float mass,
                                     float radius, float bounciness,
                                     bool moovable) {
  return world.addPart(
      Particle(pos, mass, radius, bounciness, gravity, drag, moovable));
}

void SceneInstance::step(float dt) {
  world.updateForces(dt);
  world.updatePos(dt);
  time += dt;
}

std::unique_ptr<SceneInstance> instantiate(const Scene &scene,
                                           IntegratorType integrator) {
  auto inst = std::make_unique<SceneInstance>(scene.size);
  inst->world.setBorders({0.f, scene.size.x, 0.f, scene.size.y});
  inst->world.setIntegrator(makeIntegrator(integrator));
  scene.build(*inst);
  return inst;
}

// ---------------------------------------------------------------------------
// Building blocks shared by several scenes
// ---------------------------------------------------------------------------

namespace {

const float kPi = 3.14159265f;

// Palette
const Color kAnchor(200, 200, 215);
const Color kOrange(255, 140, 0);
const Color kTeal(40, 200, 180);
const Color kPink(240, 90, 160);
const Color kYellow(250, 210, 60);
const Color kViolet(160, 110, 240);
const Color kGreen(110, 210, 90);
const Color kSoftWhite(220, 220, 230);
const Color kDim(150, 150, 170);

Particle &anchor(SceneInstance &s, const Vec2 &pos, float radius = 0.08f) {
  Particle &p = s.addParticle(pos, 1.f, radius, 0.f, false);
  p.setFillColor(kAnchor);
  return p;
}

// Rectangular grid of particles, row-major, row 0 at the top
struct Grid {
  int cols = 0, rows = 0;
  std::vector<Particle *> cells;
  Particle &at(int r, int c) const {
    return *cells[static_cast<std::size_t>(r * cols + c)];
  }
};

Grid makeGrid(SceneInstance &s, const Vec2 &top_left, int cols, int rows,
              float spacing, float mass, float radius, float bounciness,
              const Color &color) {
  Grid g;
  g.cols = cols;
  g.rows = rows;
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++) {
      const Vec2 pos = top_left + Vec2(static_cast<float>(c) * spacing,
                                       -static_cast<float>(r) * spacing);
      Particle &p = s.addParticle(pos, mass, radius, bounciness);
      p.setFillColor(color);
      g.cells.push_back(&p);
    }
  return g;
}

struct GridSprings {
  float k_struct = 0.f; // horizontal and vertical neighbours
  float k_shear = 0.f;  // diagonals (0 = none)
  float k_bend = 0.f;   // neighbours at distance 2 (0 = none)
  float rho = 0.f;      // damping of every spring
  Color color = kSoftWhite;
  bool show_shear = true;
};

void linkGrid(SceneInstance &s, const Grid &g, const GridSprings &o) {
  // Lambda: add a spring between (r1,c1) and (r2,c2) if both exist
  auto link = [&](int r1, int c1, int r2, int c2, float k, bool visible) {
    if (k <= 0.f || r2 < 0 || r2 >= g.rows || c2 < 0 || c2 >= g.cols)
      return;
    Spring &sp = s.world.addSpring(g.at(r1, c1), g.at(r2, c2), k, o.rho);
    sp.setStrokeColor(o.color);
    sp.setVisible(visible);
  };
  for (int r = 0; r < g.rows; r++)
    for (int c = 0; c < g.cols; c++) {
      link(r, c, r, c + 1, o.k_struct, true);
      link(r, c, r + 1, c, o.k_struct, true);
      link(r, c, r + 1, c + 1, o.k_shear, o.show_shear);
      link(r, c, r + 1, c - 1, o.k_shear, o.show_shear);
      link(r, c, r, c + 2, o.k_bend, false);
      link(r, c, r + 2, c, o.k_bend, false);
    }
}

// Ring of n particles around a center particle ("blob")
struct Ring {
  Particle *center = nullptr;
  std::vector<Particle *> rim;
};

Ring makeBlob(SceneInstance &s, const Vec2 &center, float radius, int n,
              float rim_mass, float center_mass, float bounciness,
              const Color &color, float k_rim, float k_spoke, float k_cross,
              float rho) {
  Ring ring;
  ring.center = &s.addParticle(center, center_mass, 0.1f, bounciness);
  ring.center->setFillColor(color);
  for (int i = 0; i < n; i++) {
    const float a = 2.f * kPi * static_cast<float>(i) / static_cast<float>(n);
    Particle &p = s.addParticle(
        center + Vec2(std::cos(a), std::sin(a)) * radius, rim_mass, 0.07f,
        bounciness);
    p.setFillColor(color);
    ring.rim.push_back(&p);
  }
  const std::size_t un = ring.rim.size();
  for (std::size_t i = 0; i < un; i++) {
    Particle &p = *ring.rim[i];
    s.world.addSpring(p, *ring.rim[(i + 1) % un], k_rim, rho)
        .setStrokeColor(color);
    Spring &spoke = s.world.addSpring(p, *ring.center, k_spoke, rho);
    spoke.setStrokeColor(kDim);
    if (k_cross > 0.f) // keeps the shape round
      s.world.addSpring(p, *ring.rim[(i + 2) % un], k_cross, rho)
          .setVisible(false);
  }
  return ring;
}

// Same velocity for a list of particles
void launch(const std::vector<Particle *> &ps, const Vec2 &v) {
  for (Particle *p : ps)
    p->setVelocity(v, kPhysicsDt);
}

// ---------------------------------------------------------------------------
// The scenes
// ---------------------------------------------------------------------------

void buildPendulum(SceneInstance &s) {
  s.drag.setCoefficient(0.f);
  Particle &a = anchor(s, {5.f, 6.f});
  Particle &bob = s.addParticle({8.f, 6.f}, 1.f, 0.2f, 0.5f);
  bob.setFillColor(kOrange);
  s.world.addSpring(a, bob, 5000.f, 0.f); // stiff and undamped
}

void buildDoublePendulum(SceneInstance &s) {
  s.drag.setCoefficient(0.f);
  Particle &a = anchor(s, {5.f, 4.5f});
  Particle &b1 = s.addParticle({6.6f, 4.5f}, 1.f, 0.16f, 0.5f);
  Particle &b2 = s.addParticle({7.56f, 5.78f}, 1.f, 0.16f, 0.5f);
  b1.setFillColor(kTeal);
  b2.setFillColor(kPink);
  s.world.addSpring(a, b1, 6000.f, 0.f);
  s.world.addSpring(b1, b2, 6000.f, 0.f);
}

void buildRope(SceneInstance &s) {
  s.drag.setCoefficient(0.02f);
  const int n = 20;
  const float spacing = 0.45f;
  Particle *prev = &anchor(s, {2.f, 7.f});
  for (int i = 1; i < n; i++) {
    const bool last = i == n - 1;
    Particle &p = s.addParticle({2.f + spacing * static_cast<float>(i), 7.f},
                                last ? 0.5f : 0.1f, last ? 0.18f : 0.06f, 0.4f);
    p.setFillColor(last ? kOrange : kYellow);
    s.world.addSpring(*prev, p, 800.f, 0.3f).setStrokeColor(kYellow);
    prev = &p;
  }
}

void buildBridge(SceneInstance &s) {
  s.drag.setCoefficient(0.03f);
  const int n = 30;
  const Vec2 left(1.f, 5.f), right(11.f, 5.f);
  const float spacing = (right.x - left.x) / static_cast<float>(n - 1);
  std::vector<Particle *> deck;
  for (int i = 0; i < n; i++) {
    const Vec2 pos = left + Vec2(spacing * static_cast<float>(i), 0.f);
    if (i == 0 || i == n - 1) {
      deck.push_back(&anchor(s, pos, 0.12f));
    } else {
      Particle &p = s.addParticle(pos, 0.12f, 0.05f, 0.3f);
      p.setFillColor(kGreen);
      deck.push_back(&p);
    }
  }
  // Rest length 15 % longer than the gap: the bridge sags under its weight
  for (int i = 0; i + 1 < n; i++)
    s.world.addSpring(*deck[i], *deck[i + 1], spacing * 1.15f, 700.f, 0.4f)
        .setStrokeColor(kGreen);
  // Three lanterns hanging under the deck
  for (int i : {7, 15, 22}) {
    Particle &lantern =
        s.addParticle(deck[i]->getPos() + Vec2(0.f, -0.8f), 0.6f, 0.15f, 0.3f);
    lantern.setFillColor(kYellow);
    s.world.addSpring(*deck[i], lantern, 900.f, 0.5f).setStrokeColor(kDim);
  }
}

void buildCloth(SceneInstance &s) {
  s.drag.setCoefficient(0.01f);
  const int n = 20;
  const float spacing = 0.3f;
  const Vec2 top_left(6.f - spacing * (n - 1) / 2.f, 8.5f);
  Grid g = makeGrid(s, top_left, n, n, spacing, 0.05f, 0.04f, 0.2f, kTeal);
  for (Particle *p : g.cells)
    p->setVisible(false);
  // Pinned by its two top corners
  for (Particle *p : {&g.at(0, 0), &g.at(0, n - 1)}) {
    p->setMoovable(false);
    p->setVisible(true);
    p->setFillColor(kAnchor);
  }
  GridSprings o;
  o.k_struct = 250.f;
  o.k_shear = 120.f;
  o.rho = 0.05f;
  o.color = kTeal;
  o.show_shear = false;
  linkGrid(s, g, o);
}

void buildBlob(SceneInstance &s) {
  s.drag.setCoefficient(0.01f);
  Ring b = makeBlob(s, {3.f, 5.5f}, 1.f, 20, 0.1f, 0.5f, 0.7f, kPink, 600.f,
                    250.f, 300.f, 0.4f);
  std::vector<Particle *> all = b.rim;
  all.push_back(b.center);
  launch(all, {4.f, 0.f});
}

void buildWindyCurtain(SceneInstance &s) {
  s.drag.setCoefficient(0.005f);
  const int cols = 24, rows = 14;
  const float spacing = 0.3f;
  Grid g = makeGrid(s, {1.f, 8.5f}, cols, rows, spacing, 0.05f, 0.04f, 0.2f,
                    kViolet);
  for (Particle *p : g.cells)
    p->setVisible(false);
  for (int c = 0; c < cols; c++) {
    Particle &p = g.at(0, c);
    p.setMoovable(false);
    p.setVisible(c % 3 == 0);
    p.setFillColor(kAnchor);
  }
  GridSprings o;
  o.k_struct = 250.f;
  o.k_shear = 120.f;
  o.rho = 0.05f;
  o.color = kViolet;
  o.show_shear = false;
  linkGrid(s, g, o);

  // Gusty wind from the left: a lambda of time (J4 extension)
  Wind &wind = s.addForce<Wind>(
      [](float t) {
        const float gust = 6.f + 5.f * std::sin(0.7f * t) +
                           2.f * std::sin(2.3f * t + 1.f);
        return Vec2(gust, 0.8f * std::sin(1.3f * t));
      },
      s.time, 0.035f);
  for (Particle *p : g.cells)
    p->addForce(wind);
}

void buildJelly(SceneInstance &s) {
  s.drag.setCoefficient(0.01f);
  const int n = 8;
  Grid g = makeGrid(s, {2.f, 7.f}, n, n, 0.3f, 0.08f, 0.06f, 0.5f, kGreen);
  GridSprings o;
  o.k_struct = 400.f;
  o.k_shear = 250.f;
  o.k_bend = 150.f;
  o.rho = 0.3f;
  o.color = kGreen;
  linkGrid(s, g, o);
  launch(g.cells, {5.f, 1.f});
}

void buildOscillators(SceneInstance &s) {
  s.drag.setCoefficient(0.f);
  const float ks[] = {5.f, 10.f, 20.f, 40.f, 80.f, 160.f};
  const Color colors[] = {kOrange, kYellow, kGreen, kTeal, kViolet, kPink};
  for (int i = 0; i < 6; i++) {
    const float x = 1.5f + 1.8f * static_cast<float>(i);
    Particle &a = anchor(s, {x, 7.5f});
    // Released at the rest length: it falls and oscillates around m*g/k
    Particle &m = s.addParticle({x, 6.f}, 0.5f, 0.18f, 0.5f);
    m.setFillColor(colors[i]);
    s.world.addSpring(a, m, 1.5f, ks[i], 0.f).setStrokeColor(colors[i]);
  }
}

void buildBallRain(SceneInstance &s) {
  s.drag.setCoefficient(0.f);
  std::mt19937 rng(42); // fixed seed: the scene is the same at each reset
  std::uniform_real_distribution<float> ux(0.5f, s.size.x - 0.5f),
      uy(3.f, s.size.y - 0.5f), umass(0.1f, 0.6f), ubounce(0.6f, 0.95f),
      uv(-4.f, 4.f);
  const Color palette[] = {kOrange, kTeal, kPink, kYellow, kViolet, kGreen};
  for (int i = 0; i < 40; i++) {
    const float m = umass(rng);
    Particle &p =
        s.addParticle({ux(rng), uy(rng)}, m, 0.08f + 0.35f * m, ubounce(rng));
    p.setFillColor(palette[i % 6]);
    p.setVelocity({uv(rng), uv(rng)}, kPhysicsDt);
  }
}

void buildZeroG(SceneInstance &s) {
  s.gravity.setG(Vec2(0.f, 0.f));
  s.drag.setCoefficient(0.f);

  // Spinning ring: rotation + translation
  Ring big = makeBlob(s, {4.f, 4.f}, 1.2f, 16, 0.1f, 0.3f, 1.f, kTeal, 500.f,
                      200.f, 200.f, 0.f);
  const float omega = 3.f; // rad/s
  for (Particle *p : big.rim) {
    const Vec2 r = p->getPos() - big.center->getPos();
    p->setVelocity(Vec2(-r.y, r.x) * omega + Vec2(1.5f, 0.8f), kPhysicsDt);
  }
  big.center->setVelocity({1.5f, 0.8f}, kPhysicsDt);

  // Small triangle "molecule" going the other way
  Particle &a = s.addParticle({9.f, 5.f}, 0.3f, 0.15f, 1.f);
  Particle &b = s.addParticle({10.f, 5.f}, 0.3f, 0.15f, 1.f);
  Particle &c = s.addParticle({9.5f, 5.87f}, 0.3f, 0.15f, 1.f);
  for (Particle *p : {&a, &b, &c}) {
    p->setFillColor(kYellow);
    p->setVelocity({-2.f, -1.2f}, kPhysicsDt);
  }
  c.setVelocity({-2.f + 2.f, -1.2f}, kPhysicsDt); // makes it tumble
  s.world.addSpring(a, b, 300.f, 0.f).setStrokeColor(kYellow);
  s.world.addSpring(b, c, 300.f, 0.f).setStrokeColor(kYellow);
  s.world.addSpring(c, a, 300.f, 0.f).setStrokeColor(kYellow);
}

void buildSpiderWeb(SceneInstance &s) {
  s.drag.setCoefficient(0.05f);
  const Vec2 center(6.f, 4.5f);
  const int spokes = 16, rings = 4;
  const float ring_step = 0.9f;

  Particle &hub = s.addParticle(center, 0.2f, 0.08f, 0.2f);
  hub.setFillColor(kSoftWhite);

  // web[r][i]: ring r (0 = innermost), spoke i
  std::vector<std::vector<Particle *>> web(rings);
  for (int r = 0; r < rings; r++)
    for (int i = 0; i < spokes; i++) {
      const float a =
          2.f * kPi * static_cast<float>(i) / static_cast<float>(spokes);
      const Vec2 pos = center + Vec2(std::cos(a), std::sin(a)) *
                                    (ring_step * static_cast<float>(r + 1));
      Particle *p;
      if (r == rings - 1) {
        p = &anchor(s, pos, 0.05f); // outer ring is glued to the frame
      } else {
        p = &s.addParticle(pos, 0.05f, 0.04f, 0.2f);
        p->setFillColor(kSoftWhite);
        p->setVisible(false);
      }
      web[static_cast<std::size_t>(r)].push_back(p);
    }

  for (int r = 0; r < rings; r++)
    for (int i = 0; i < spokes; i++) {
      Particle &p = *web[static_cast<std::size_t>(r)][static_cast<std::size_t>(i)];
      Particle &next = *web[static_cast<std::size_t>(r)]
                           [static_cast<std::size_t>((i + 1) % spokes)];
      Particle &inner =
          r == 0 ? hub
                 : *web[static_cast<std::size_t>(r - 1)]
                       [static_cast<std::size_t>(i)];
      // Silk is pre-tensioned: rest length 90 % of the distance
      const float radial = (p.getPos() - inner.getPos()).norm();
      s.world.addSpring(inner, p, radial * 0.9f, 300.f, 0.1f)
          .setStrokeColor(kSoftWhite);
      const float spiral = (next.getPos() - p.getPos()).norm();
      s.world.addSpring(p, next, spiral * 0.9f, 200.f, 0.1f)
          .setStrokeColor(kDim);
    }
}

void buildJellyTower(SceneInstance &s) {
  s.drag.setCoefficient(0.01f);
  const int cols = 4, rows = 14;
  const float spacing = 0.35f;
  const float radius = 0.06f;
  const Vec2 top_left(6.f - spacing * (cols - 1) / 2.f,
                      radius + spacing * (rows - 1));
  Grid g = makeGrid(s, top_left, cols, rows, spacing, 0.1f, radius, 0.1f,
                    kOrange);
  GridSprings o;
  o.k_struct = 600.f;
  o.k_shear = 400.f;
  o.k_bend = 200.f;
  o.rho = 0.4f;
  o.color = kOrange;
  linkGrid(s, g, o);
}

} // namespace

// ---------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------

const std::vector<Scene> &allScenes() {
  static const std::vector<Scene> scenes = {
      {"Pendule", "une ancre, une masse, un ressort raide sans amortissement",
       {10.f, 7.f}, buildPendulum},
      {"Double pendule", "deux masses en chaine : mouvement chaotique",
       {10.f, 8.f}, buildDoublePendulum},
      {"Corde", "20 particules, epinglee a gauche, un poids au bout",
       {12.f, 8.f}, buildRope},
      {"Pont de corde", "corde detendue epinglee aux deux bouts, 3 lanternes",
       {12.f, 8.f}, buildBridge},
      {"Tissu", "grille 20 x 20 epinglee par ses deux coins du haut",
       {12.f, 9.f}, buildCloth},
      {"Blob", "anneau de 20 particules + centre, lance vers la droite",
       {12.f, 8.f}, buildBlob},
      {"Rideau au vent", "extension J4 : vent variable donne par une lambda",
       {14.f, 9.f}, buildWindyCurtain},
      {"Gelee", "carre mou 8 x 8 (structure + cisaillement + flexion)",
       {12.f, 8.f}, buildJelly},
      {"Oscillateurs", "6 masses identiques, raideurs de 5 a 160 N/m",
       {12.f, 8.f}, buildOscillators},
      {"Pluie de balles", "40 balles de tailles et rebonds differents",
       {16.f, 9.f}, buildBallRain},
      {"Apesanteur", "g = 0 : un anneau qui tourne et un triangle",
       {12.f, 8.f}, buildZeroG},
      {"Toile d'araignee", "4 anneaux x 16 rayons, bord exterieur fixe",
       {12.f, 9.f}, buildSpiderWeb},
      {"Tour en gelee", "colonne molle 4 x 14 posee au sol, a pousser",
       {12.f, 8.f}, buildJellyTower},
  };
  return scenes;
}
