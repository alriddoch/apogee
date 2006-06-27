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

#ifndef APOGEE_GUI_DIALOGUE_H
#define APOGEE_GUI_DIALOGUE_H

#include "Button.h"

#include "TextField.h"
#include "visual/GL.h"

#include <sigc++/signal.h>

class Dialogue : public Widget {
  private:
    GLuint oButtonName, cButtonName, field1Name, field2Name;
    Button oButton;
    Button cButton;
    TextField field1;
    TextField field2;
    std::string field1Tag, field2Tag;
    int fieldCount;
    int focus;

  protected:
    static Sprite background;

  public:
    sigc::signal<void, const std::string &, const std::string &> oButtonSignal;
    sigc::signal<void> cButtonSignal;

    Dialogue(Gui & g, int x, int y) : Widget(g, x, y),
                                      oButton(g, x, y, "OK"),
                                      cButton(g, x, y, "Cancel"),
                                      field1(g, x, y),
                                      field2(g, x, y),
                                      fieldCount(0), focus(-1) { }
    virtual ~Dialogue();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual bool key(int,int);

    void addField(const std::string & name, const std::string & contents);
};

#endif // APOGEE_GUI_DIALOGUE_H
