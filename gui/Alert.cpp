// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Alert.h"
#include "Gui.h"

void Alert::setup()
{
    messageName = m_g.newName();
}

void Alert::draw()
{
    Item::draw();
    glTranslatef(0.0f,0.0f,0.01f);
    m_g.print("HELLO");
}

void Alert::select()
{
    Item::draw();
    glTranslatef(0.0f,0.0f,0.01f);
    m_g.print("HELLO");
}
