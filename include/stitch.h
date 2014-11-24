#ifndef __STITCH_H__
#define __STITCH_H__

#include <SDL2/SDL_opengl.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "texture.h"
#include "sprite.h"
#include "globals.h"
#include "types.h"

struct TexturePatch {
  TexturePatch(Uint32 x, Uint32 y, Uint32 w, Uint32 h) : m_x(x), m_y(y), m_w(w), m_h(h) {}
  ~TexturePatch() {}
  Uint32 m_x, m_y, m_w, m_h;
};

class Packer {
public:
  Packer(Uint32 width, Uint32 height);
  ~Packer();
  TexturePatch pack(std::string id, Uint32 width, Uint32 height);
private:
  Uint32 m_width, m_height;
};

class Stitcher {
public:
  Stitcher(Uint32 width, Uint32 height);
  virtual ~Stitcher();

  void begin();
  void draw(const Sprite &sprite, float x, float y);
  void end();
private:
  void stitch(const Sprite &sprite);
  void upload(const TexturePatch &patch, void *data);

  void prune();

  void sort();


  /* vertex buffer handle, index buffer handle. */
  GLuint m_vbo, m_ibo;

  /* pixel buffer handle for streaming upload. */
  GLuint m_pbo;

  /* texture handle for the internal sprite sheet */
  GLuint m_sheet;

  Uint32 m_count;

  /* internal texture size. */
  Uint32 m_width, m_height;

  /* keeps track of draw state. */
  bool m_begun;

  /* rectangle packer to fit patches inside the sheet */
  Packer m_packer;

  /* map of all textures stitched into internal texture. */
  std::unordered_map<std::string, TexturePatch> m_usedPatches;

  /* map of all actively used textures during a batch draw. */
  std::unordered_set<std::string> m_activePatches;

  /* list of textures that couldn't be placed inside the internal
     texture. scheduled for insertion. */
  std::vector<TexturePatch> m_overflowPatches;
};

#endif /* __STITCH_H__ */
