// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_WIDGET_H
#define APOGEE_GUI_WIDGET_H

class Gui;
class Widget;

class Widget {
  private:
    int m_x, m_y;

  protected:
    Gui & m_g;
    bool m_obs;
    bool m_vis;

  public:
    Widget(Gui & g, int x, int y) : m_x(x), m_y(y), m_g(g),
                                    m_obs(false), m_vis(true) { }
    virtual ~Widget();

    const int x() const { return m_x; }
    const int y() const { return m_y; }
    const bool obsolete() const { return m_obs; }
    const bool visible() const { return m_vis; }

    virtual void move(int x, int y); //{ m_x += x; m_y += y; }
    virtual void setup() = 0;
    virtual void draw() = 0;
    virtual void select() = 0;
    virtual void click() = 0;
    virtual void release() = 0;
    virtual bool key(int sym,int mod) = 0;

};

#endif // APOGEE_GUI_WIDGET_H
