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

#include "Alert.h"
#include "Gui.h"

#include "common/system.h"

Sprite Alert::background;

Alert::~Alert()
{
}

void Alert::setup()
{
    if (!Alert::background.loaded()) {
        background.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/panels/pnl_greystone_scored_1_up_462x90_ajr.png");
    }

    buttonName = m_g.newName();
    button.setup();
}

void Alert::draw()
{
    Alert::background.draw();
    glTranslatef(-211.0f,54.0f,0.01f);
    m_g.print(m_message.c_str());
    glTranslatef(211.0f,-34.0f,0.0f);
    button.draw();
    glTranslatef(0.0f,-20.0f,0.0f);
}

void Alert::select()
{
    Alert::background.draw();
    glTranslatef(0.0f,20.0f,0.01f);
    glPushName(buttonName);
    button.select();
    glPopName();
    glTranslatef(0.0f,-20.0f,0.0f);
}

void Alert::click()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (buttonName == *I) {
            button.press();
        }
    }
}

void Alert::release()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (buttonName == *I) {
            button.release();
            m_obs = true;
        }
    }
}

bool Alert::key(int sym, int mod)
{
    if (sym == SDLK_RETURN) {
        m_obs = true;
        return true;
    }
    return false;
}
