// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_BUTTON_H
#define APOGEE_GUI_BUTTON_H

#include "Widget.h"
#include <visual/Sprite.h>

class Button : public Widget {
  protected:
    static Sprite m_smallUp;
    static Sprite m_mediumUp;
    static Sprite m_largeUp;
    static Sprite m_smallDown;
    static Sprite m_mediumDown;
    static Sprite m_largeDown;

    std::string m_label;
    int size;
    bool pressed;

  public:
    Button(Gui & g, int x, int y, const std::string & label) : Widget(g, x, y),
                                              m_label(label), size(2),
                                              pressed(false) { }
    virtual ~Button();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual void key(int);

    void press() { pressed = true; }
    void unpress() { pressed = false; }
    int width();
};

#endif // APOGEE_GUI_BUTTON_H
