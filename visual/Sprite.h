#ifndef APOGEE_VISUAL_SPRITE_H
#define APOGEE_VISUAL_SPRITE_H

#include "Texture.h"

class Renderer;

class Sprite {
  private:
    static unsigned int twoN(unsigned int);

    int tex_id;
    float m_w, m_h;
    float m_pw, m_ph;
  public:
    Sprite() : tex_id(-1), m_w(0), m_h(0), m_pw(0), m_ph(0) { }

    bool load(const std::string & filename);
    bool load(const std::string & filename, const Renderer & renderer);
    void draw();

    bool loaded() const { return (tex_id != -1); }
    float w() { return m_w; }
    float h() { return m_h; }
    float pw() { return m_pw; }
    float ph() { return m_ph; }
};

#endif // APOGEE_VISUAL_SPRITE_H
