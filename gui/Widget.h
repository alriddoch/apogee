// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_WIDGET_H
#define APOGEE_GUI_WIDGET_H

class Gui;

class Widget {
  private:
    int m_x, m_y;

  public:
    Widget(int x, int y) : m_x(x), m_y(y) { }

    virtual void setup(Gui &) = 0;
    virtual void draw() = 0;
    virtual void select() = 0;

};

#endif // APOGEE_GUI_WIDGET_H
