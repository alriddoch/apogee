// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "Button.h"
#include "Gui.h"

#include "common/system.h"

Sprite Button::m_smallUp;
Sprite Button::m_mediumUp;
Sprite Button::m_largeUp;
Sprite Button::m_smallDown;
Sprite Button::m_mediumDown;
Sprite Button::m_largeDown;

Button::~Button()
{
}

void Button::setup()
{
    int len = m_label.size();
    if (len < 2) {
        size = 0;
        if (!Button::m_smallUp.loaded()) {
            m_smallUp.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/buttons/button_horiz_greystone_scored_1_up_115x25_ajr.png");
        }
        if (!Button::m_smallDown.loaded()) {
            m_smallDown.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/buttons/button_horiz_greystone_scored_1_dn_115x25_ajr.png");
        }
    } else if (len < 11) {
        size = 1;
        if (!Button::m_mediumUp.loaded()) {
            m_mediumUp.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/buttons/button_horiz_greystone_scored_1_up_115x25_ajr.png");
        }
        if (!Button::m_mediumDown.loaded()) {
            m_mediumDown.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/buttons/button_horiz_greystone_scored_1_dn_115x25_ajr.png");
        }
    } else {
        size = 2;
        if (!Button::m_largeUp.loaded()) {
            m_largeUp.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/buttons/button_horiz_greystone_scored_1_up_115x25_ajr.png");
        }
        if (!Button::m_largeDown.loaded()) {
            m_largeDown.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/buttons/button_horiz_greystone_scored_1_dn_115x25_ajr.png");
        }
    }
}

void Button::draw()
{
    int len = m_label.size();
    switch (size) {
        case 0:
            if (pressed) {
                Button::m_smallDown.draw();
            } else {
                Button::m_smallUp.draw();
            }
            break;
        case 1:
            if (pressed) {
                Button::m_mediumDown.draw();
            } else {
                Button::m_mediumUp.draw();
            }
            break;
        //case 2:
        default:
            if (pressed) {
                Button::m_largeDown.draw();
            } else {
                Button::m_largeUp.draw();
            }
            break;
    }
    glTranslatef(-5 * len, 5.0f, 0.01f);
    m_g.print(m_label.c_str());
    glTranslatef(5 * len, -5.0f, 0.0f);
}

void Button::select()
{
    // It doesn't matter if we draw up or down, as both are the same size
    switch (size) {
        case 0:
            Button::m_smallUp.draw();
            break;
        case 1:
            Button::m_mediumUp.draw();
            break;
        //case 2:
        default:
            Button::m_largeUp.draw();
            break;
    }
    // No need to draw label, as it is not pickable
}

void Button::click()
{
}

void Button::release()
{
}

bool Button::key(int,int)
{
    return false;
}

int Button::width()
{
    switch (size) {
        case 0:
            return 25;
        case 1:
            return 115;
        case 2:
        default:
            return 170;
    };
}
