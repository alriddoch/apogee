// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_CONSOLE_H
#define APOGEE_GUI_CONSOLE_H

#include "Widget.h"

#include <string>

class Console : public Widget {
  private:
    std::string cmdLine;
  public:
    Console(Gui & g, int x, int y) : Widget(g, x, y) { }
    virtual ~Console();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual bool key(int,int);

    void addChannel(const std::string & name);
};

#endif // APOGEE_GUI_CONSOLE_H
