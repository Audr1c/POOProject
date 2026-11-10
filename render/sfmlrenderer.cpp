#include "sfmlrenderer.hpp"
#include <algorithm>
#include <iostream>

sf::Color SFMLRender::toSfColor(const Color &c) {
  return sf::Color(c.r, c.g, c.b, c.a);
}

SFMLRender::SFMLRender(unsigned int width, unsigned int height,
                       const std::string &name, unsigned int fps_limit) {
  create_window(width, height, name, fps_limit);
}

// --- Window Management ---

void SFMLRender::create_window(unsigned int width, unsigned int height,
                               const std::string &name,
                               unsigned int fps_limit) {
  if (!win.isOpen()) {
    // Style::Default = titlebar + close + resizable
    win.create(sf::VideoMode(width, height), name, sf::Style::Default);
    win.setFramerateLimit(fps_limit);
    refit();
  } else {
    std::cout << "Window already exists" << std::endl;
  }
}

bool SFMLRender::isOpen() const { return win.isOpen(); }

void SFMLRender::close() {
  if (win.isOpen()) {
    win.close();
  }
}

void SFMLRender::setFramerateLimit(unsigned int fps) {
  win.setFramerateLimit(fps);
}

sf::Vector2u SFMLRender::getWindowSize() const { return win.getSize(); }

// --- Coordinate Conversion (SI meters <-> SFML pixels) ---

void SFMLRender::setScale(float pixels_per_meter) { scale = pixels_per_meter; }
float SFMLRender::getScale() const { return scale; }

void SFMLRender::setOffset(const Vec2 &pixel_offset) { offset = pixel_offset; }
void SFMLRender::setOffset(float ox, float oy) { offset = Vec2(ox, oy); }
Vec2 SFMLRender::getOffset() const { return offset; }

// Declare the visible world area (in meters, origin bottom-left). The scale
// and offset are recomputed so it always fits the window, even when resized.
void SFMLRender::fitWorld(const Vec2 &size_m) {
  world_size = size_m;
  refit();
}

void SFMLRender::refit() {
  if (world_size.x <= 0.f || world_size.y <= 0.f)
    return;
  Vec2 win_px(static_cast<float>(win.getSize().x),
              static_cast<float>(win.getSize().y));
  scale = std::min(win_px.x / world_size.x, win_px.y / world_size.y);
  Vec2 used = world_size * scale;
  Vec2 margin = (win_px - used) / 2.f; // letterbox: center the world
  offset = Vec2(margin.x, flip_y ? margin.y + used.y : margin.y);
}

void SFMLRender::setFlipY(bool flip) { flip_y = flip; }
bool SFMLRender::getFlipY() const { return flip_y; }

sf::Vector2f SFMLRender::worldToScreen(const Vec2 &world_pos) const {
  float sx = offset.x + world_pos.x * scale;
  float sy = flip_y ? (offset.y - world_pos.y * scale)
                    : (offset.y + world_pos.y * scale);
  return sf::Vector2f(sx, sy);
}

Vec2 SFMLRender::screenToWorld(float screen_x, float screen_y) const {
  float wx = (screen_x - offset.x) / scale;
  float wy = flip_y ? ((offset.y - screen_y) / scale)
                    : ((screen_y - offset.y) / scale);
  return Vec2(wx, wy);
}

Vec2 SFMLRender::screenToWorld(int screen_x, int screen_y) const {
  return screenToWorld(static_cast<float>(screen_x),
                       static_cast<float>(screen_y));
}

// --- Default Style Configuration ---

void SFMLRender::setBackgroundColor(const Color &c) { bg_color = c; }
void SFMLRender::setDefaultCircleColor(const Color &c) {
  default_circle_color = c;
}
void SFMLRender::setDefaultLineColor(const Color &c) { default_line_color = c; }

// --- Renderer Interface Implementation ---

void SFMLRender::clear() { clear(bg_color); }

void SFMLRender::clear(const Color &color) { win.clear(toSfColor(color)); }

void SFMLRender::drawCircle(const Vec2 &center, float radius) {
  drawCircle(center, radius, default_circle_color);
}

void SFMLRender::drawCircle(const Vec2 &center, float radius,
                            const Color &color) {
  float pixel_radius = radius * scale;
  sf::CircleShape circle(pixel_radius);
  circle.setOrigin(pixel_radius, pixel_radius);
  circle.setPosition(worldToScreen(center));
  circle.setFillColor(toSfColor(color));
  win.draw(circle);
}

void SFMLRender::drawLine(const Vec2 &p1, const Vec2 &p2) {
  drawLine(p1, p2, default_line_color);
}

void SFMLRender::drawLine(const Vec2 &p1, const Vec2 &p2, const Color &color) {
  sf::Color sf_color = toSfColor(color);
  const sf::Vertex line[] = {sf::Vertex(worldToScreen(p1), sf_color),
                             sf::Vertex(worldToScreen(p2), sf_color)};
  win.draw(line, 2, sf::Lines);
}

void SFMLRender::display() { win.display(); }

// --- Scene Helpers ---

// Draw the outline of the world area [0,w]x[0,h] (needs fitWorld)
void SFMLRender::drawWorldFrame(const Color &color) {
  Vec2 a(0.f, 0.f), b(world_size.x, 0.f), c(world_size.x, world_size.y),
      d(0.f, world_size.y);
  drawLine(a, b, color);
  drawLine(b, c, color);
  drawLine(c, d, color);
  drawLine(d, a, color);
}

// Draw a horizontal ground line at world coordinate y = ground_y
void SFMLRender::drawGround(float ground_y, const Color &color) {
  float screen_y = worldToScreen(Vec2(0.f, ground_y)).y;
  float width = static_cast<float>(win.getSize().x);
  sf::Color sf_color = toSfColor(color);
  const sf::Vertex line[] = {
      sf::Vertex(sf::Vector2f(0.f, screen_y), sf_color),
      sf::Vertex(sf::Vector2f(width, screen_y), sf_color)};
  win.draw(line, 2, sf::Lines);
}

// --- Event & Mouse Interaction Helpers ---

bool SFMLRender::pollEvent(sf::Event &event) { return win.pollEvent(event); }

// Handle the window events every program needs: close button, Escape and
// resize. Returns true if the event was consumed.
bool SFMLRender::handleWindowEvent(const sf::Event &event) {
  if (event.type == sf::Event::Closed) {
    win.close();
    return true;
  }
  if (event.type == sf::Event::Resized) {
    // Keep 1 unit = 1 pixel (no stretching), then refit the world
    win.setView(sf::View(sf::FloatRect(0.f, 0.f,
                                       static_cast<float>(event.size.width),
                                       static_cast<float>(event.size.height))));
    refit();
    return true;
  }
  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Escape) {
    win.close();
    return true;
  }
  return false;
}

// Basic event handler that closes the window on Closed event or Escape key
void SFMLRender::handleBasicEvents() {
  sf::Event event;
  while (win.pollEvent(event))
    handleWindowEvent(event);
}

void SFMLRender::setTitle(const std::string &title) { win.setTitle(title); }

sf::Vector2i SFMLRender::getMouseScreenPos() const {
  return sf::Mouse::getPosition(win);
}

Vec2 SFMLRender::getMouseWorldPos() const {
  sf::Vector2i mp = sf::Mouse::getPosition(win);
  return screenToWorld(mp.x, mp.y);
}

bool SFMLRender::isMouseLeftPressed() const {
  return sf::Mouse::isButtonPressed(sf::Mouse::Left);
}
