// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_TEXTFIELD_H
#define APOGEE_GUI_TEXTFIELD_H

#include "Widget.h"
#include <visual/Sprite.h>

class TextField : public Widget {
  protected:
    static Sprite graphic;

    std::string m_contents;
    int size;

  public:
    TextField(Gui & g, int x, int y, const std::string & contents = "") :
              Widget(g, x, y), m_contents(contents), size(1) { }
    virtual ~TextField();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual void key(int);

    void key();

    void setContents(const std::string & c) { m_contents = c; }
    const std::string getContents() const { return m_contents; }
};

#endif // APOGEE_GUI_BUTTON_H
