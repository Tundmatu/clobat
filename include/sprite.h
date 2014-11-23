#ifndef __SPRITE_H__
#define __SPRITE_H__

class Sprite {
public:
  Sprite(string m_id, int m_clipH, int m_clipW, int m_clipY, int m_clipX)
      : m_id(m_id), m_clipH(m_clipH), m_clipW(m_clipW), m_clipY(m_clipY), m_clipX(m_clipX) {
  }
  ~Sprite() {}

  std::string getId() const {
    return m_id;
  }

private:
  int m_clipX, m_clipY, m_clipW, m_clipH;
  std::string m_id;
};

#endif /* __SPRITE_H__ */