#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <SDL2/SDL.h>

#include "asset.h"

class TextureDescriptor : public AssetDescriptor {
public:
  TextureDescriptor() {}
  ~TextureDescriptor() {}
  virtual void *load(const AssetSystem &sys, const pugi::xml_node &node) override;
};

class Texture {
public:
  Texture(std::string path, bool preload = true);
  ~Texture();
  bool loaded();
  bool load();
  void free();
  SDL_Surface *data();
  std::string path();
private:
  SDL_Surface *m_data;
  std::string m_path;
  bool m_loaded;

};

#endif /* __TEXTURE_H__ */