// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_BUTTON_H
#define APOGEE_GUI_BUTTON_H

#include "Widget.h"
#include <visual/Sprite.h>

class Button : public Widget {
  protected:
    static Sprite m_small;
    static Sprite m_medium;
    static Sprite m_large;

    std::string m_label;
    int size;

  public:
    Button(Gui & g, int x, int y, const std::string & label) : Widget(g, x, y),
                                              m_label(label), size(2) { }

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
};

#endif // APOGEE_GUI_BUTTON_H
