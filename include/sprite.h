#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <string>

class SpriteDescriptor : public AssetDescriptor {
public:
  SpriteDescriptor() {}
  ~SpriteDescriptor() {}
  virtual void *load(const AssetSystem &sys, const pugi::xml_node &node) override;
};


class Sprite {
public:
  Sprite(std::string id, int clipX, int clipY, int clipW, int clipH)
      : m_id(id), m_clipH(clipH), m_clipW(clipW), m_clipY(clipY), m_clipX(clipX) {
  }
  ~Sprite() {}

  std::string getId() const {
    return m_id;
  }

//private:
  int m_clipX, m_clipY, m_clipW, m_clipH;
  std::string m_id;
};

#endif /* __SPRITE_H__ */