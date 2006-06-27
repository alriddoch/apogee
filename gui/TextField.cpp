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

#include "TextField.h"
#include "Gui.h"

#include "common/system.h"

#include <iostream>

Sprite TextField::graphic;

TextField::~TextField()
{
}

void TextField::setup()
{
    if (!TextField::graphic.loaded()) {
        graphic.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/buttons/button_horiz_greystone_scored_1_dn_115x25_ajr.png");
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

bool TextField::key(int sym,int mod)
{
    int k = Gui::keyToAscii(sym,mod);
    if (k != -1) {
        m_contents.push_back(k);
        return true;
    }
    switch (sym) {
        case SDLK_BACKSPACE:
            if (m_contents.size() != 0) {
                m_contents.erase(m_contents.end() - 1);
            }
            return true;
            break;
        default:
            std::cout << "WEIRD keypress" << std::endl << std::flush;
            break;
    };
    return false;
}
