// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "TextField.h"
#include "Gui.h"

Sprite TextField::graphic;

TextField::~TextField()
{
}

void TextField::setup()
{
    if (!TextField::graphic.loaded()) {
        graphic.load("media/media-2d/ui-collection-pegasus/ui_buttons_christals/button_text_christals_grey_1_dn_115x25_us.png");
    }
}

void TextField::draw()
{
    TextField::graphic.draw();
    glTranslatef(-50, 5.0f, 0.01f);
    m_g.print(m_contents.c_str());
    glTranslatef(50, -5.0f, 0.0f);
}

void TextField::select()
{
    TextField::graphic.draw();
    // No need to draw contents, as it is not pickable
}

void TextField::click()
{
}

void TextField::release()
{
}

void TextField::key(int sym,int mod)
{
    int k = Gui::keyToAscii(sym,mod);
    if (k != -1) {
        m_contents.push_back(k);
        return;
    }
    switch (sym) {
        case SDLK_BACKSPACE:
            if (m_contents.size() != 0) {
                m_contents.erase(m_contents.end() - 1);
            }
            break;
        default:
            cout << "WEIRD keypress" << endl << flush;
            break;
    };
}
