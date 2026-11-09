#pragma once

class Color {
public:
  unsigned char r, g, b, a;

  Color();
  Color(unsigned char r, unsigned char g, unsigned char b,
        unsigned char a = 255);
};
