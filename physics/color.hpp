#pragma once

class Color {
public:
  unsigned char r, g, b, a;

  Color() : r(0), g(0), b(0), a(255) {}
  Color(unsigned char r, unsigned char g, unsigned char b,
        unsigned char a = 255)
      : r(r), g(g), b(b), a(a) {}
};
