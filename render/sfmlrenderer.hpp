#pragma once

#include "../physics/color.hpp"
#include "../physics/renderer.hpp"
#include "../physics/vec2.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class SFMLRender : public Renderer {
private:
  float scale = 1.0f;      // Pixels per meter (SI -> pixels)
  Vec2 offset{0.0f, 0.0f}; // Origin offset in pixels
  Vec2 world_size{0.0f, 0.0f}; // Visible world area in meters (0 = no auto-fit)
  bool flip_y = true;      // If true, +y in physics points upwards on screen
  Color bg_color{30, 30, 40, 255};
  Color default_circle_color{255, 140, 0, 255};
  Color default_line_color{255, 255, 255, 255};

  static sf::Color toSfColor(const Color &c);

public:
  sf::RenderWindow win;

  SFMLRender() = default;

  SFMLRender(unsigned int width, unsigned int height,
             const std::string &name = "Moteur Masses-Ressorts",
             unsigned int fps_limit = 60);

  ~SFMLRender() override = default;

  // --- Window Management ---

  void create_window(unsigned int width, unsigned int height,
                     const std::string &name = "Moteur Masses-Ressorts",
                     unsigned int fps_limit = 60);

  bool isOpen() const override;

  void close();

  void setFramerateLimit(unsigned int fps);

  sf::Vector2u getWindowSize() const;

  // --- Coordinate Conversion (SI meters <-> SFML pixels) ---

  void setScale(float pixels_per_meter);
  float getScale() const;

  void setOffset(const Vec2 &pixel_offset);
  void setOffset(float ox, float oy);
  Vec2 getOffset() const;

  // Declare the visible world area (in meters, origin bottom-left). The scale
  // and offset are recomputed so it always fits the window, even when resized.
  void fitWorld(const Vec2 &size_m);

  void refit();

  void setFlipY(bool flip);
  bool getFlipY() const;

  sf::Vector2f worldToScreen(const Vec2 &world_pos) const;

  Vec2 screenToWorld(float screen_x, float screen_y) const;
  Vec2 screenToWorld(int screen_x, int screen_y) const;

  // --- Default Style Configuration ---

  void setBackgroundColor(const Color &c);
  void setDefaultCircleColor(const Color &c);
  void setDefaultLineColor(const Color &c);

  // --- Renderer Interface Implementation ---

  void clear() override;
  void clear(const Color &color) override;

  void drawCircle(const Vec2 &center, float radius) override;
  void drawCircle(const Vec2 &center, float radius,
                  const Color &color) override;

  void drawLine(const Vec2 &p1, const Vec2 &p2) override;
  void drawLine(const Vec2 &p1, const Vec2 &p2, const Color &color) override;

  void display() override;

  // --- Scene Helpers ---

  // Draw the outline of the world area [0,w]x[0,h] (needs fitWorld)
  void drawWorldFrame(const Color &color = Color(120, 120, 140));

  // Draw a horizontal ground line at world coordinate y = ground_y
  void drawGround(float ground_y, const Color &color = Color(120, 120, 140));

  // --- Event & Mouse Interaction Helpers ---

  bool pollEvent(sf::Event &event);

  // Close / Escape / resize. Returns true if the event was consumed.
  bool handleWindowEvent(const sf::Event &event);

  // Basic event handler that closes the window on Closed event or Escape key
  void handleBasicEvents();

  void setTitle(const std::string &title);

  sf::Vector2i getMouseScreenPos() const;

  Vec2 getMouseWorldPos() const;

  bool isMouseLeftPressed() const;
};

// Alias matching the subject's class diagram naming (SfmlRenderer)
using SfmlRenderer = SFMLRender;
