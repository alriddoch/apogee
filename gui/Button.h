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

#ifndef APOGEE_GUI_BUTTON_H
#define APOGEE_GUI_BUTTON_H

#include "Widget.h"

#include "visual/Sprite.h"

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
    virtual bool key(int, int);

    void press() { pressed = true; }
    void unpress() { pressed = false; }
    int width();
};

#endif // APOGEE_GUI_BUTTON_H
