#pragma once

#include "../physics/color.hpp"
#include "../physics/renderer.hpp"
#include "../physics/vec2.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
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

  static sf::Color toSfColor(const Color &c) {
    return sf::Color(c.r, c.g, c.b, c.a);
  }

public:
  sf::RenderWindow win;

  SFMLRender() = default;

  SFMLRender(unsigned int width, unsigned int height,
             const std::string &name = "Moteur Masses-Ressorts",
             unsigned int fps_limit = 60) {
    create_window(width, height, name, fps_limit);
  }

  ~SFMLRender() override = default;

  // --- Window Management ---

  void create_window(unsigned int width, unsigned int height,
                     const std::string &name = "Moteur Masses-Ressorts",
                     unsigned int fps_limit = 60) {
    if (!win.isOpen()) {
      // Style::Default = titlebar + close + resizable
      win.create(sf::VideoMode(width, height), name, sf::Style::Default);
      win.setFramerateLimit(fps_limit);
      refit();
    } else {
      std::cout << "Window already exists" << std::endl;
    }
  }

  bool isOpen() const override { return win.isOpen(); }

  void close() {
    if (win.isOpen()) {
      win.close();
    }
  }

  void setFramerateLimit(unsigned int fps) { win.setFramerateLimit(fps); }

  sf::Vector2u getWindowSize() const { return win.getSize(); }

  // --- Coordinate Conversion (SI meters <-> SFML pixels) ---

  void setScale(float pixels_per_meter) { scale = pixels_per_meter; }
  float getScale() const { return scale; }

  void setOffset(const Vec2 &pixel_offset) { offset = pixel_offset; }
  void setOffset(float ox, float oy) { offset = Vec2(ox, oy); }
  Vec2 getOffset() const { return offset; }

  // Declare the visible world area (in meters, origin bottom-left). The scale
  // and offset are recomputed so it always fits the window, even when resized.
  void fitWorld(const Vec2 &size_m) {
    world_size = size_m;
    refit();
  }

  void refit() {
    if (world_size.x <= 0.f || world_size.y <= 0.f)
      return;
    Vec2 win_px(static_cast<float>(win.getSize().x),
                static_cast<float>(win.getSize().y));
    scale = std::min(win_px.x / world_size.x, win_px.y / world_size.y);
    Vec2 used = world_size * scale;
    Vec2 margin = (win_px - used) / 2.f; // letterbox: center the world
    offset = Vec2(margin.x, flip_y ? margin.y + used.y : margin.y);
  }

  void setFlipY(bool flip) { flip_y = flip; }
  bool getFlipY() const { return flip_y; }

  sf::Vector2f worldToScreen(const Vec2 &world_pos) const {
    float sx = offset.x + world_pos.x * scale;
    float sy = flip_y ? (offset.y - world_pos.y * scale)
                      : (offset.y + world_pos.y * scale);
    return sf::Vector2f(sx, sy);
  }

  Vec2 screenToWorld(float screen_x, float screen_y) const {
    float wx = (screen_x - offset.x) / scale;
    float wy = flip_y ? ((offset.y - screen_y) / scale)
                      : ((screen_y - offset.y) / scale);
    return Vec2(wx, wy);
  }

  Vec2 screenToWorld(int screen_x, int screen_y) const {
    return screenToWorld(static_cast<float>(screen_x),
                         static_cast<float>(screen_y));
  }

  // --- Default Style Configuration ---

  void setBackgroundColor(const Color &c) { bg_color = c; }
  void setDefaultCircleColor(const Color &c) { default_circle_color = c; }
  void setDefaultLineColor(const Color &c) { default_line_color = c; }

  // --- Renderer Interface Implementation ---

  void clear() override { clear(bg_color); }

  void clear(const Color &color) override { win.clear(toSfColor(color)); }

  void drawCircle(const Vec2 &center, float radius) override {
    drawCircle(center, radius, default_circle_color);
  }

  void drawCircle(const Vec2 &center, float radius,
                  const Color &color) override {
    float pixel_radius = radius * scale;
    sf::CircleShape circle(pixel_radius);
    circle.setOrigin(pixel_radius, pixel_radius);
    circle.setPosition(worldToScreen(center));
    circle.setFillColor(toSfColor(color));
    win.draw(circle);
  }

  void drawLine(const Vec2 &p1, const Vec2 &p2) override {
    drawLine(p1, p2, default_line_color);
  }

  void drawLine(const Vec2 &p1, const Vec2 &p2, const Color &color) override {
    sf::Color sf_color = toSfColor(color);
    const sf::Vertex line[] = {sf::Vertex(worldToScreen(p1), sf_color),
                               sf::Vertex(worldToScreen(p2), sf_color)};
    win.draw(line, 2, sf::Lines);
  }

  void display() override { win.display(); }

  // --- Scene Helpers ---

  // Draw the outline of the world area [0,w]x[0,h] (needs fitWorld)
  void drawWorldFrame(const Color &color = Color(120, 120, 140)) {
    Vec2 a(0.f, 0.f), b(world_size.x, 0.f), c(world_size.x, world_size.y),
        d(0.f, world_size.y);
    drawLine(a, b, color);
    drawLine(b, c, color);
    drawLine(c, d, color);
    drawLine(d, a, color);
  }

  // Draw a horizontal ground line at world coordinate y = ground_y
  void drawGround(float ground_y, const Color &color = Color(120, 120, 140)) {
    float screen_y = worldToScreen(Vec2(0.f, ground_y)).y;
    float width = static_cast<float>(win.getSize().x);
    sf::Color sf_color = toSfColor(color);
    const sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(0.f, screen_y), sf_color),
        sf::Vertex(sf::Vector2f(width, screen_y), sf_color)};
    win.draw(line, 2, sf::Lines);
  }

  // --- Event & Mouse Interaction Helpers ---

  bool pollEvent(sf::Event &event) { return win.pollEvent(event); }

  // Basic event handler that closes the window on Closed event or Escape key
  void handleBasicEvents() {
    sf::Event event;
    while (win.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        win.close();
      } else if (event.type == sf::Event::Resized) {
        // Keep 1 unit = 1 pixel (no stretching), then refit the world
        win.setView(sf::View(sf::FloatRect(0.f, 0.f,
                                           static_cast<float>(event.size.width),
                                           static_cast<float>(event.size.height))));
        refit();
      } else if (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::Escape) {
        win.close();
      }
    }
  }

  sf::Vector2i getMouseScreenPos() const { return sf::Mouse::getPosition(win); }

  Vec2 getMouseWorldPos() const {
    sf::Vector2i mp = sf::Mouse::getPosition(win);
    return screenToWorld(mp.x, mp.y);
  }

  bool isMouseLeftPressed() const {
    return sf::Mouse::isButtonPressed(sf::Mouse::Left);
  }
};

// Alias matching the subject's class diagram naming (SfmlRenderer)
using SfmlRenderer = SFMLRender;
