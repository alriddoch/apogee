// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_ITEM_H
#define APOGEE_GUI_ITEM_H

#include "Widget.h"

#include <visual/Sprite.h>

class Item : public Widget {
  private:
    Sprite graphic;

  public:
    Item(Gui & g, int x, int y) : Widget(g,x,y) { }

    bool load(const char *);

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
};

#endif // APOGEE_GUI_ITEM_H
