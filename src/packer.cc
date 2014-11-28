#include "packer.h"

Packer::Packer(Uint32 width, Uint32 height) : m_width(width), m_height(height) {
  resize(width, height);
}

Packer::~Packer() {

}

void Packer::resize(Uint32 width, Uint32 height) {
  m_width = width;
  m_height = height;
  Rect node(0, 0, width, height);

  m_used = 0;
  m_usedRects.clear();
  m_freeRects.clear();
  m_freeRects.push_back(node);
}

Vec2 Packer::insert(std::string id, Uint32 width, Uint32 height) {
  Uint32 first, second;
  Rect node = find(width, height, first, second);

  if (node.m_height == 0) return Vec2(node.m_x, node.m_y);

  /*
  auto it = m_freeRects.begin();
  while (it != m_freeRects.end()) {
    if (split(*it, node) m_freeRects.erase(it++);
    ++it;
  }
  */

  size_t numRects = m_freeRects.size();
  for (size_t i = 0; i < numRects; ++i) {
    if (split(m_freeRects[i], node)) {
      m_freeRects.erase(m_freeRects.begin() + i);
    }
  }

  prune();

  m_usedRects.push_back(node);
  m_used += node.m_width * node.m_height;

  return Vec2(node.m_x, node.m_y);
}

Rect Packer::score(Uint32 width, Uint32 height, Uint32 &first, Uint32 &second) const {
  first = std::numeric_limits<int>::max();
  second = std::numeric_limits<int>::max();

  Rect node = find(width, height, first, second);

  if (node.m_height == 0) {
    first = std::numeric_limits<int>::max();
    second = std::numeric_limits<int>::max();
  }

  return node;
}

Rect Packer::find(Uint32 width, Uint32 height, Uint32 &first, Uint32 &second) const {
  Rect node(0, 0, 0, 0);

  first = std::numeric_limits<int>::max();

  for (size_t i = 0; i < m_freeRects.size(); ++i) {
    Rect r = m_freeRects[i];
    if (r.m_width >= width && r.m_height >= height) {
      Uint32 loh = abs(r.m_width - width);
      Uint32 lov = abs(r.m_height - height);
      Uint32 ssf = loh < lov ? loh : lov; // min
      Uint32 lsf = loh > lov ? loh : lov; // max

      if (ssf < first || (ssf == first && lsf < first)) {
        node.m_x = r.m_x;
        node.m_y = r.m_y;
        node.m_width = width;
        node.m_height = height;

        first = ssf;
        second = lsf;
      }
    }
  }

  return node;
}

bool Packer::split(Rect node, const Rect &used) {
  if (used.m_x >= node.m_x + node.m_width || used.m_x + used.m_width <= node.m_x ||
      used.m_y >= node.m_y + node.m_height || used.m_y + used.m_height <= node.m_y)
    return false;

  if (used.m_x < node.m_x + node.m_width && used.m_x + used.m_width > node.m_x) {
    if (used.m_y > node.m_y && used.m_y < node.m_y + node.m_height) {
      Rect newNode = node;
      newNode.m_height = used.m_y - newNode.m_y;
      m_freeRects.push_back(newNode);
    }

    if (used.m_y + used.m_height < node.m_y + node.m_height) {
      Rect newNode = node;
      newNode.m_y = used.m_y + used.m_height;
      newNode.m_height = node.m_y + node.m_height - (used.m_y + used.m_height);
      m_freeRects.push_back(newNode);
    }
  }

  if (used.m_y < node.m_y + node.m_height && used.m_y + used.m_height > node.m_y) {
    if (used.m_x > node.m_x && used.m_x < node.m_x + node.m_width) {
      Rect newNode = node;
      newNode.m_width = used.m_x - newNode.m_x;
      m_freeRects.push_back(newNode);
    }

    if (used.m_x + used.m_width < node.m_x + node.m_width) {
      Rect newNode = node;
      newNode.m_x = used.m_x + used.m_width;
      newNode.m_width = node.m_x + node.m_width - (used.m_x + used.m_width);
      m_freeRects.push_back(newNode);
    }
  }

  return true;
}

void Packer::prune() {
  for(size_t i = 0; i < m_freeRects.size(); ++i) {
    for (size_t j = i + 1; j < m_freeRects.size(); ++j) {
      if (Rect::overlaps(m_freeRects[i], m_freeRects[j])) {
        m_freeRects.erase(m_freeRects.begin() + i);
        --i;
        break;
      }
      if (Rect::overlaps(m_freeRects[j], m_freeRects[i])) {
        m_freeRects.erase(m_freeRects.begin() + j);
        --j;
      }
    }
  }
}