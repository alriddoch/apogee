// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Widget.h"

void Widget::move(int x, int y)
{
    m_x += x;
    m_y += y;
}
