#ifndef __STITCH_H__
#define __STITCH_H__

#include <SDL2/SDL_opengl.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "texture.h"
#include "sprite.h"
#include "globals.h"

class Packer {
  // todo: implement
};

struct TexturePatch {
  TexturePatch(int m_x, int m_y, int m_w, int m_h) : m_x(m_x), m_y(m_y), m_w(m_w), m_h(m_h) {}
  ~TexturePatch() {}
  int m_x, m_y, m_w, m_h;
};

class Stitcher {
public:
  Stitcher(size_t size) : m_width(size), m_height(size), m_begun(false) {}
  Stitcher(size_t width, size_t height) : m_width(width), m_height(height), m_begun(false) {}
  ~Stitcher() {}

  void begin() {
    if (m_begun) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Stitcher must end its batch before it can start a new one");
      return;
    }

    m_begun = true;
  }

  void draw(const Sprite &sprite, float x, float y) {
    if (m_usedPatches.count(sprite.getId()) == 0)
      stitch(sprite);

    TexturePatch data = m_patches.at(sprite.getId());

    float u1 = (data->m_x + sprite.m_clipX) / (float) m_width;
    float v1 = (data->m_y + sprite.m_clipY) / (float) m_height;
    float u2 = (data->m_x + sprite.m_clipX + sprite.m_clipW) / (float) m_width;
    float v2 = (data->m_y + sprite.m_clipY + sprite.m_clipH) / (float) m_height;

    m_activePatches.emplace(sprite.getId());

    // upload vertex data
  }

  void end() {
    if (!m_begun) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Stitcher must start its batch before it can end one");
      return;
    }

    // handle overflow

    m_activePatches.clear();

    m_begun = false;
  }

private:
  void stitch(const Sprite &sprite) {
    std::shared_ptr<Texture> texture = gAssetSystem->getAsset(sprite.getId());
    if (!texture) return;

    int tw = texture->data()->w;
    int th = texture->data()->h;

    // sync box sorting

    m_usedPatches.insert(sprite.getId(), TexturePatch(0, 0, tw, th));

    // async texture upload
  }

  void prune() {

  }

  void sort() {
    // todo: implement
  }

  /* vertex buffer handle, index buffer handle. */
  GLuint m_vbo, m_ibo;

  /* pixel buffer handle for streaming upload. */
  GLuint m_pbo;

  /* texture handle for the internal sprite sheet */
  GLuint m_sheet;

  /* internal texture size. */
  int m_width, m_height;

  /* keeps track of draw state. */
  bool m_begun;

  /* map of all textures stitched into internal texture. */
  std::unordered_map<std::string, TexturePatch> m_usedPatches;

  /* map of all actively used textures during a batch draw. */
  std::unordered_set<std::string> m_activePatches;

  /* list of textures that couldn't be placed inside the internal
     texture. scheduled for insertion. */
  std::vector<TexturePatch> m_overflowPatches;
};

#endif /* __STITCH_H__ */