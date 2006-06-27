// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2003 Alistair Riddoch
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

#ifndef APOGEE_GUI_COMPASS_H
#define APOGEE_GUI_COMPASS_H

#include "Widget.h"

#include "visual/Sprite.h"

class Compass : public Widget {
  protected:
    static Sprite compass_case;
    static Sprite compass_needle;
    static Sprite needle_shadow;

    float m_angle;

  public:
    Compass(Gui & g, int x, int y) : Widget(g, x, y), m_angle(90) { }
    virtual ~Compass();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual bool key(int,int);

    void setAngle(float angle) { m_angle = angle; }
    int width();
};

#endif // APOGEE_GUI_COMPASS_H
