// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Button.h"
#include "Gui.h"

Sprite Button::m_small;
Sprite Button::m_medium;
Sprite Button::m_large;

void Button::setup()
{
    int len = m_label.size();
    if (len < 2) {
        size = 0;
        if (!Button::m_small.loaded()) {
            m_small.load("media/media-2d/ui-collection-pegasus/ui_buttons_christals/button_horiz_christal_1_orig_up_25x25_us.png");
        }
    } else if (len < 11) {
        size = 1;
        if (!Button::m_medium.loaded()) {
            m_medium.load("media/media-2d/ui-collection-pegasus/ui_buttons_christals/button_horiz_christal_1_orig_up_115x25_us.png");
        }
    } else {
        size = 2;
        if (!Button::m_large.loaded()) {
            m_large.load("media/media-2d/ui-collection-pegasus/ui_buttons_christals/button_horiz_christal_1_orig_up_170x25_us.png");
        }
    }
}

void Button::draw()
{
    int len = m_label.size();
    switch (size) {
        case 0:
            Button::m_small.draw();
            break;
        case 1:
            Button::m_medium.draw();
            break;
        //case 2:
        default:
            Button::m_large.draw();
            break;
    }
    glTranslatef(-5 * len, 5.0f, 0.01f);
    m_g.print(m_label.c_str());
    glTranslatef(5 * len, -5.0f, 0.0f);
}

void Button::select()
{
    int len = m_label.size();
    switch (size) {
        case 0:
            Button::m_small.draw();
            break;
        case 1:
            Button::m_medium.draw();
            break;
        //case 2:
        default:
            Button::m_large.draw();
            break;
    }
    // No need to draw label, as it is not pickable
}

void Button::click()
{
    
}
