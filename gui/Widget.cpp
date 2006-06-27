// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2001 Alistair Riddoch
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

#include "Widget.h"

#include "Gui.h"

Widget::Widget(Gui & g, int x, int y) : m_x(x), m_y(y), m_g(g),
                                        m_obs(false), m_vis(true),
                                        m_name(g.newName())
{
}

Widget::~Widget()
{
}

void Widget::releaseFocus() const
{
    if (hasFocus()) {
        m_g.clearFocus();
    }
}

void Widget::takeFocus() const
{
    if (!hasFocus()) {
        m_g.takeFocus(*this);
    }
}

void Widget::move(int x, int y)
{
    m_x += x;
    m_y += y;
}

bool Widget::hasFocus() const
{
    return (m_name == m_g.getFocus());
}
