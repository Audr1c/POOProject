#include "sfmlrenderer.hpp"
#include <algorithm>
#include <cmath>
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
    // Style::Default = titlebar + close + resizable, antialiased shapes
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    win.create(sf::VideoMode(width, height), name, sf::Style::Default,
               settings);
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
void SFMLRender::setLineWidth(float pixels) { line_px = pixels; }
void SFMLRender::setOutlineWidth(float pixels) { outline_px = pixels; }

// --- Renderer Interface Implementation ---

void SFMLRender::clear() { clear(bg_color); }

void SFMLRender::clear(const Color &color) { win.clear(toSfColor(color)); }

void SFMLRender::drawCircle(const Vec2 &center, float radius) {
  drawCircle(center, radius, default_circle_color);
}

void SFMLRender::drawCircle(const Vec2 &center, float radius,
                            const Color &color) {
  float pixel_radius = radius * scale;
  sf::CircleShape circle(pixel_radius, 48);
  circle.setOrigin(pixel_radius, pixel_radius);
  circle.setPosition(worldToScreen(center));
  circle.setFillColor(toSfColor(color));
  // Contour plus sombre de la meme teinte, comme les noeuds des schemas
  if (pixel_radius > 3.f) {
    auto dark = [](sf::Uint8 v) { return static_cast<sf::Uint8>(v * 0.62f); };
    circle.setOutlineThickness(-outline_px);
    circle.setOutlineColor(
        sf::Color(dark(color.r), dark(color.g), dark(color.b), color.a));
  }
  win.draw(circle);
}

void SFMLRender::drawLine(const Vec2 &p1, const Vec2 &p2) {
  drawLine(p1, p2, default_line_color);
}

void SFMLRender::drawLine(const Vec2 &p1, const Vec2 &p2, const Color &color) {
  const sf::Color sf_color = toSfColor(color);
  const sf::Vector2f a = worldToScreen(p1), b = worldToScreen(p2);
  const sf::Vector2f d = b - a;
  const float len = std::sqrt(d.x * d.x + d.y * d.y);
  if (len < 1e-3f)
    return;
  // Segment epais : un quadrilatere de largeur line_px autour de [a, b]
  const sf::Vector2f n(-d.y / len * line_px / 2.f, d.x / len * line_px / 2.f);
  const sf::Vertex quad[] = {sf::Vertex(a + n, sf_color),
                             sf::Vertex(b + n, sf_color),
                             sf::Vertex(b - n, sf_color),
                             sf::Vertex(a - n, sf_color)};
  win.draw(quad, 4, sf::Quads);
}

void SFMLRender::display() { win.display(); }

// --- Scene Helpers ---

// Draw the outline of the world area [0,w]x[0,h] (needs fitWorld)
void SFMLRender::drawWorldFrame(const Color &color) {
  const Vec2 corners[] = {Vec2(0.f, 0.f), Vec2(world_size.x, 0.f),
                          Vec2(world_size.x, world_size.y),
                          Vec2(0.f, world_size.y)};
  // Pointilles : traits de 9 px, espaces de 6 px
  const float on = 4.5f * line_px / scale, off = 3.f * line_px / scale;
  const float saved = line_px;
  line_px = 0.75f * line_px;
  for (int i = 0; i < 4; i++) {
    const Vec2 &p = corners[i];
    const Vec2 &q = corners[(i + 1) % 4];
    const float len = (q - p).norm();
    const Vec2 u = (q - p) / len;
    for (float t = 0.f; t < len; t += on + off)
      drawLine(p + u * t, p + u * std::min(t + on, len), color);
  }
  line_px = saved;
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
