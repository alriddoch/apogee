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

void TextField::key(int)
{
}
