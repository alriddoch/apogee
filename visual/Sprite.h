#ifndef APOGEE_VISUAL_SPRITE_H
#define APOGEE_VISUAL_SPRITE_H

#include "Texture.h"

class Sprite {
  private:
    static unsigned int twoN(unsigned int);

    int tex_id;
    double m_w, m_h;
    double m_pw, m_ph;
  public:
    Sprite() : tex_id(-1), m_w(0), m_h(0), m_pw(0), m_ph(0) { }

    bool load(const std::string & filename);
    void draw();
    double w() { return m_w; }
    double h() { return m_h; }
    double pw() { return m_pw; }
    double ph() { return m_ph; }
};

#endif // APOGEE_VISUAL_SPRITE_H
