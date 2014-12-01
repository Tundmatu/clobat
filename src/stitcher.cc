#include "stitcher.h"

#include <chrono>

Stitcher::Stitcher(Uint32 width, Uint32 height) : m_width(width), m_height(height), m_packer(width, height), m_begun(false) {
  // allocate our texture on the GPU
  auto start = std::chrono::system_clock::now();
  glGenTextures(1, &m_sheet);
  glBindTexture(GL_TEXTURE_2D, m_sheet);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Allocated %ix%i texture sheet (%i bytes) for stitcher in %llu ms", width, height, width * height, duration.count());

  // create our buffers
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ibo);

  // create our pbo
  glGenBuffers(1, &m_pbo);
}

Stitcher::~Stitcher() {
  glDeleteBuffers(1, &m_pbo);
  glDeleteBuffers(1, &m_ibo);
  glDeleteBuffers(1, &m_vbo);

  glDeleteTextures(1, &m_sheet);
}

void Stitcher::begin() {
  if (m_begun) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Stitcher must end its batch before it can start a new one");
    return;
  }

  // todo: use vbo

  glBegin(GL_QUADS);

  m_begun = true;
}

void Stitcher::draw(const Sprite &sprite, float x, float y) {
  if (m_usedPatches.count(sprite.getId()) == 0)
    stitch(sprite);

  TexturePatch data = m_usedPatches.at(sprite.getId());

  float u1 = (data.m_x + sprite.m_clipX) / (float) m_width;
  float v1 = (data.m_y + sprite.m_clipY) / (float) m_height;
  float u2 = (data.m_x + sprite.m_clipX + sprite.m_clipW) / (float) m_width;
  float v2 = (data.m_y + sprite.m_clipY + sprite.m_clipH) / (float) m_height;

  float vx = x;
  float vy = y;
  float vw = x + sprite.m_clipW;
  float vh = x + sprite.m_clipH;

  Vec4 p1(vx, vy, u1, v1);
  Vec4 p2(vx, vy, u1, v1);
  Vec4 p3(vx, vy, u1, v1);
  Vec4 p4(vx, vy, u1, v1);

  m_activePatches.emplace(sprite.getId());

  // upload vertex data
  // todo: use vbo
  glTexCoord2f(u1, v1); glVertex2f(vx, vy);
  glTexCoord2f(u1, v2); glVertex2f(vx, vh);
  glTexCoord2f(u2, v2); glVertex2f(vw, vh);
  glTexCoord2f(u2, v1); glVertex2f(vw, vy);

}

void Stitcher::end() {
  if (!m_begun) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Stitcher must start its batch before it can end one");
    return;
  }

  // todo: use vbo
  glEnd();

  // handle overflow

  m_activePatches.clear();

  m_begun = false;
}

void Stitcher::stitch(const Sprite &sprite) {
  // todo: remove
  end();

  std::shared_ptr<Texture> texture = gAssetSystem->getAsset<Texture>(sprite.getId());
  if (!texture) return;

  int tw = texture->data()->w;
  int th = texture->data()->h;

  auto start = std::chrono::system_clock::now();
  Rect rect = m_packer.insert(sprite.getId(), tw, th);
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Packed %ix%i texture in %llu ms", tw, th, duration.count());

  if (rect.m_height > 0) {
    TexturePatch patch(rect.m_x, rect.m_y, tw, th);

    m_usedPatches.insert(std::pair<std::string, TexturePatch>(sprite.getId(), patch));

    start = std::chrono::system_clock::now();
    upload(patch, texture->data()->pixels);
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Uploaded %ix%i texture in %llu ms", tw, th, duration.count());
  }

  // todo: remove
  begin();
}

void Stitcher::upload(const TexturePatch &patch, void *data) {
  Uint32 size = patch.m_w * patch.m_h * 4;

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);

  // we can probably get away with resizing the pbo but we can orphan it just
  // in case!
  glBufferData(GL_PIXEL_UNPACK_BUFFER, size, NULL, GL_STREAM_DRAW);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, size, data, GL_STREAM_DRAW);

  glBindTexture(GL_TEXTURE_2D, m_sheet);
  glTexSubImage2D(GL_TEXTURE_2D, 0, patch.m_x, patch.m_y, patch.m_w, patch.m_h, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Uploaded %ix%i texture (%i bytes) into internal sheet at x: %i, y: %i", patch.m_w, patch.m_h, size, patch.m_x, patch.m_y);
}

void Stitcher::prune() {

}

void Stitcher::sort() {

}