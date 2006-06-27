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

#include "Dialogue.h"
#include "Gui.h"

#include "common/system.h"

#include <algorithm>

Sprite Dialogue::background;

Dialogue::~Dialogue()
{
}

void Dialogue::setup()
{
    if (!Dialogue::background.loaded()) {
        background.load(getMediaPath() + "/media/media-2d/collection-alriddoch/ui/panels/pnl_greystone_scored_1_up_462x90_ajr.png");
    }

    cButtonName = m_g.newName();
    oButtonName = m_g.newName();
    field1Name = m_g.newName();
    field2Name = m_g.newName();
    oButton.setup();
    cButton.setup();
    field1.setup();
    field2.setup();
}

void Dialogue::draw()
{
    Dialogue::background.draw();
    glPushMatrix();
    glTranslatef(-211.0f,54.0f,0.01f);
    m_g.print(field1Tag.c_str());
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-211.0f,29.0f,0.01f);
    m_g.print(field2Tag.c_str());
    glPopMatrix();

    glPushMatrix();
    int offset = std::max(cButton.width(), oButton.width());
    glTranslatef(211.0f - offset / 2,49.0f,0.01f);
    oButton.draw();
    glTranslatef(0.0f,-25.0f,0.01f);
    cButton.draw();
    glPopMatrix();

    if (fieldCount < 1) { return; }
    offset = std::max(field1Tag.size(), field2Tag.size());
    glPushMatrix();
    glTranslatef(-145.0f + offset * 10,49.0f,0.01f);
    field1.draw();
    if (fieldCount < 2) { 
        glPopMatrix();
        return;
    }
    glTranslatef(0.0f,-25.0f,0.01f);
    field2.draw();
    glPopMatrix();
}

void Dialogue::select()
{
    Dialogue::background.draw();

    glPushMatrix();
    int offset = std::max(cButton.width(), oButton.width());
    glTranslatef(211.0f - offset / 2,49.0f,0.01f);
    glPushName(oButtonName);
    oButton.select();
    glTranslatef(0.0f,-25.0f,0.01f);
    glLoadName(cButtonName);
    cButton.select();
    glPopMatrix();
    glPopName();

    if (fieldCount < 1) { return; }
    offset = std::max(field1Tag.size(), field2Tag.size());
    glPushMatrix();
    glTranslatef(-145.0f + offset * 10,49.0f,0.01f);
    glPushName(field1Name);
    field1.select();
    glPopName();
    if (fieldCount < 2) { 
        glPopMatrix();
        return;
    }
    glTranslatef(0.0f,-25.0f,0.01f);
    glPushName(field2Name);
    field2.select();
    glPopName();
    glPopMatrix();
}

void Dialogue::click()
{
    const hitlist & h = m_g.getHits();
    focus = -1;

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (oButtonName == *I) {
            oButton.press();
        }
        if (cButtonName == *I) {
            cButton.press();
        }
        if (field1Name == *I) {
            focus = 1;
        }
        if (field2Name == *I) {
            focus = 2;
        }
    }
}

void Dialogue::release()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (oButtonName == *I) {
            m_obs = true;
            oButton.release();
            oButtonSignal.emit(field1.getContents(), field2.getContents());
        }
        if (cButtonName == *I) {
            m_obs = true;
            cButton.release();
            cButtonSignal.emit();
        }
    }
}

bool Dialogue::key(int sym,int mod)
{
    //int k = Gui::keyToAscii(sym,mod);
    //if (k != -1) { cout << "[" << k << "]" << endl << flush; }
    if (focus == 1) {
        return field1.key(sym, mod);
    } else if (focus == 2) {
        return field2.key(sym, mod);
    }
    return false;
}

void Dialogue::addField(const std::string & name, const std::string & contents)
{
    switch (++fieldCount) {
        case 1:
            field1Tag = name;
            field1.setContents(contents);
            break;
        case 2:
            field2Tag = name;
            field2.setContents(contents);
            break;
        default:
            break;
    }
}
