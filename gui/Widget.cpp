// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

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
