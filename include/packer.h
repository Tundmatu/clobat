#pragma once

#include "types.h"

#include <vector>
#include <string>

struct Rect;

/*
 * Basic implementation of the maxrects-bssf-bnf packer as described by
 * Jukka Jylänki in his survey of different bin packing algorithms:
 * http://clb.demon.fi/files/RectangleBinPack.pdf
 */
class Packer {
public:
  Packer(Uint32 width, Uint32 height);
  ~Packer();

  void resize(Uint32 width, Uint32 height);
  Rect insert(std::string id, Uint32 width, Uint32 height);

  Uint32 getPackerWidth() { return m_width; }
  Uint32 getPackerHeight() { return m_height; }

  Uint32 getArea() { return m_width * m_height; }
  Uint32 getAreaOccupied() { return m_used; }
private:
  Uint32 m_width, m_height, m_used;

  std::vector<Rect> m_usedRects;
  std::vector<Rect> m_freeRects;

  Rect score(Uint32 width, Uint32 height, Uint32 &first, Uint32 &second) const;
  Rect find(Uint32 width, Uint32 height, Uint32 &bestShort, Uint32 &bestLong) const;
  bool split(Rect node, const Rect &used);
  void prune();
};

struct Rect {
  Rect(Uint32 x, Uint32 y, Uint32 width, Uint32 height) : m_x(x), m_y(y), m_width(width), m_height(height) {}
  ~Rect() {}

  Uint32 m_x;
  Uint32 m_y;
  Uint32 m_width;
  Uint32 m_height;

  static bool overlaps(const Rect &a, const Rect &b) {
    return a.m_x >= b.m_x
        && a.m_y >= b.m_y
        && a.m_x + a.m_width <= b.m_x + b.m_width
        && a.m_y + a.m_height <= b.m_y + b.m_height;
  }
};
