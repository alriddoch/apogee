// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_COMPASS_H
#define APOGEE_GUI_COMPASS_H

#include "Widget.h"
#include <visual/Sprite.h>

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
    virtual void key(int,int);

    void setAngle(float angle) { m_angle = angle; }
    int width();
};

#endif // APOGEE_GUI_COMPASS_H
