// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_VISUAL_SPRITE_H
#define APOGEE_VISUAL_SPRITE_H

#include "Texture.h"

class Renderer;

class Sprite {
  private:
    static unsigned int twoN(unsigned int);

    unsigned int tex_id;
    float m_w, m_h;
    float m_pw, m_ph;
    bool loadedp;
  public:
    Sprite() : m_w(0), m_h(0), m_pw(0), m_ph(0), loadedp(false) { }

    bool load(const std::string & filename);
    void draw();

    bool loaded() const { return loadedp; }
    float w() { return m_w; }
    float h() { return m_h; }
    float pw() { return m_pw; }
    float ph() { return m_ph; }
};

#endif // APOGEE_VISUAL_SPRITE_H
