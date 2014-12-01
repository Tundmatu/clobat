#include "texture.h"

#include <SDL2/SDL_image.h>

void* TextureDescriptor::load(const AssetSystem &sys, const pugi::xml_node &node) {
  Texture *tex = new Texture(node.attribute("src").as_string("null"));
  return tex;
}

Texture::Texture(std::string path, bool preload) : m_path(path), m_loaded(false), m_data(NULL) {
  if (preload) load();
}

Texture::~Texture() {
  SDL_FreeSurface(m_data);
}


bool Texture::loaded() {
  return m_data != NULL;
}

bool Texture::load() {
  if (loaded()) free();

  SDL_Surface *img = IMG_Load(m_path.c_str());
  SDL_PixelFormat pf;
  pf.palette = 0;
  pf.BitsPerPixel = 32;
  pf.BytesPerPixel = 4;
  pf.Rshift = pf.Rloss = pf.Gloss = pf.Bloss = pf.Aloss = 0;
  pf.Rmask = 0x000000ff;
  pf.Gshift = 8;
  pf.Gmask = 0x0000ff00;
  pf.Bshift = 16;
  pf.Bmask = 0x00ff0000;
  pf.Ashift = 24;
  pf.Amask = 0xff000000;

  m_data = SDL_ConvertSurface(img, &pf, SDL_SWSURFACE);
  if (!m_data) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error while loading texture '%s': %s", m_path.c_str(), IMG_GetError());
    return false;
  } else {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loaded texture '%s': %ix%i", m_path.c_str(), m_data->w, m_data->h);
    return true;
  }
}

void Texture::free() {
  SDL_FreeSurface(m_data);
}

std::string Texture::path() {
  return m_path;
}

SDL_Surface *Texture::data() {
  return m_data;
}
