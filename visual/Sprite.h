// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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
    std::string m_filename;

    bool loadData();
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
