// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Alert.h"
#include "Gui.h"

void Alert::setup()
{
    buttonName = m_g.newName();
    button.setup();
}

void Alert::draw()
{
    Item::draw();
    glTranslatef(-211.0f,54.0f,0.01f);
    m_g.print(m_message.c_str());
    glTranslatef(211.0f,-34.0f,0.0f);
    button.draw();
    glTranslatef(0.0f,-20.0f,0.0f);
}

void Alert::select()
{
    Item::select();
    glTranslatef(0.0f,20.0f,0.01f);
    glPushName(buttonName);
    button.draw();
    glPopName();
    glTranslatef(0.0f,-20.0f,0.0f);
}

void Alert::click()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (buttonName == *I) {
            m_obs = true;
        }
    }
}
