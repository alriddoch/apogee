// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Option.h"
#include "Gui.h"

#include <algorithm>

Sprite Option::background;

Option::~Option()
{
}

void Option::setup()
{
    if (!Option::background.loaded()) {
        background.load("media/media-2d/collection-alriddoch/ui/panels/pnl_greystone_scored_1_up_462x90_ajr.png");
    } 

    buttonOneName = m_g.newName();
    buttonTwoName = m_g.newName();
    buttonOne.setup();
    buttonTwo.setup();
}

void Option::draw()
{
    Option::background.draw();
    glPushMatrix();
    glTranslatef(-211.0f, 54.0f,0.01f);
    m_g.print(question.c_str());
    glPopMatrix();

    glPushMatrix();
    int offset = std::max(buttonOne.width(), buttonTwo.width());
    glTranslatef(211.0f - offset / 2, 49.0f, 0.01f);
    buttonOne.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    buttonTwo.draw();
    glPopMatrix();
}

void Option::select()
{
    Option::background.draw();

    glPushMatrix();
    int offset = std::max(buttonOne.width(), buttonTwo.width());
    glTranslatef(211.0f - offset / 2, 49.0f, 0.01f);
    glPushName(buttonOneName);
    buttonOne.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    glLoadName(buttonTwoName);
    buttonTwo.draw();
    glPopName();
    glPopMatrix();
}

void Option::click()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (buttonOneName == *I) {
            buttonOne.press();
        }
        if (buttonTwoName == *I) {
            buttonTwo.press();
        }
    }
}

void Option::release()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (buttonOneName == *I) {
            m_obs = true;
            buttonOne.release();
            buttonOneSignal.emit();
        }
        if (buttonTwoName == *I) {
            m_obs = true;
            buttonTwo.release();
            buttonTwoSignal.emit();
        }
    }
}

void Option::key(int sym, int mod)
{
}
