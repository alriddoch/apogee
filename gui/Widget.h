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
    const int m_name;

  public:
    Widget(Gui & g, int x, int y);
    virtual ~Widget();

    const int x() const { return m_x; }
    const int y() const { return m_y; }
    const bool obsolete() const { return m_obs; }
    const bool visible() const { return m_vis; }
    int getName() const { return m_name; }

    bool hasFocus() const;
    void releaseFocus() const;
    void takeFocus() const;

    virtual void move(int x, int y); //{ m_x += x; m_y += y; }
    virtual void setup() = 0;
    virtual void draw() = 0;
    virtual void select() = 0;
    virtual void click() = 0;
    virtual void release() = 0;
    virtual bool key(int sym,int mod) = 0;

};

#endif // APOGEE_GUI_WIDGET_H
