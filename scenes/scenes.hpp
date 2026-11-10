#pragma once

#include "../physics/forces/drag.hpp"
#include "../physics/forces/force.hpp"
#include "../physics/forces/wind.hpp"
#include "../physics/world.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

// Fixed physics time step, shared by every scene (subject: dt = 1/120 s)
constexpr float kPhysicsDt = 1.f / 120.f;

// A running scene: it owns the World and every force the particles refer to.
// The forces are declared BEFORE the world, so they are destroyed after it
// (the particles keep references to them until the very end).
class SceneInstance {
private:
  std::vector<std::unique_ptr<Force>> extra_forces; // wind, ...

public:
  Gravity gravity;  // shared by every particle of the scene
  Drag drag;        // shared by every particle of the scene
  float time = 0.f; // simulation clock (s), read by time-dependent forces
  Vec2 size;        // visible area in meters, origin bottom-left
  World world;

  explicit SceneInstance(const Vec2 &size);

  SceneInstance(const SceneInstance &) = delete;
  SceneInstance &operator=(const SceneInstance &) = delete;

  // Keep ownership of an extra force and return it, to attach it to particles
  template <typename F, typename... Args> F &addForce(Args &&...args) {
    auto f = std::make_unique<F>(std::forward<Args>(args)...);
    F &ref = *f;
    extra_forces.push_back(std::move(f));
    return ref;
  }

  // Shortcut: a particle using the scene's gravity and drag
  Particle &addParticle(const Vec2 &pos, float mass, float radius,
                        float bounciness = 0.5f, bool moovable = true);

  // One fixed physics step
  void step(float dt);
};

// Description of a scene: a name, a short text and the function (usually a
// lambda) that fills a fresh SceneInstance.
struct Scene {
  std::string name;
  std::string description;
  Vec2 size;
  std::function<void(SceneInstance &)> build;
};

// Every available scene, in menu order
const std::vector<Scene> &allScenes();

// Build a fresh, ready to run instance of a scene
std::unique_ptr<SceneInstance> instantiate(const Scene &scene,
                                           IntegratorType integrator);
