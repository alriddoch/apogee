// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Alert.h"
#include "Gui.h"

Sprite Alert::background;

Alert::~Alert()
{
}

void Alert::setup()
{
    if (!Alert::background.loaded()) {
        background.load("media/media-2d/ui-collection-pegasus/ui_panels_text/panels_text_christal/pnl_text_christals_grey_1_up_462x90_us.png");
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

void Alert::key(int sym, int mod)
{
    if (sym == SDLK_RETURN) {
        m_obs = true;
    }
}
