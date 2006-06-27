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

#include "CharSelector.h"
#include "Gui.h"

#include "visual/Model.h"
#include "visual/Renderer.h"

#include <wfmath/quaternion.h>
#include <wfmath/point.h>

static int xoffset[] = {0, 80, -80, 160, -160, 240, -240 };
static int yoffset[] = {0, -15, -15, -35, -35, -60, -60 };

CharSelector::~CharSelector()
{
}

void CharSelector::setup()
{
    selectName = m_g.newName();
    createName = m_g.newName();
    selectb.setup();
    create.setup();
}

void CharSelector::draw()
{
    glPushMatrix();
    glTranslatef(0.0f, -100.0f, 0.01f);
    selectb.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    create.draw();
    glPopMatrix();

    std::map<GLuint, std::pair<std::string, std::string> >::const_iterator I;
    int i = 0;
    WFMath::Quaternion rot(1,0,0,0);
    for(I = charNames.begin(); I != charNames.end() && i < 7; ++I, ++i) {
        glPushMatrix();
        glTranslatef(xoffset[i], yoffset[i], 0.0f);
        m_g.print(I->second.second.c_str());
        glTranslatef(0.0f, 20.0f, 0.0f);
        if (selectedChar == I->second.first) {
            glScalef(1.2f, 1.2f, 1.2f);
        }
        glRotatef(-90,1,0,0);
        glPushAttrib(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        m_g.renderer.model->onRender();
        glPopAttrib();
        glPopMatrix();
    }
}

void CharSelector::select()
{
    glPushMatrix();
    glTranslatef(0.0f, -100.0f, 0.01f);
    glPushName(selectName);
    selectb.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    glLoadName(createName);
    create.draw();
    glPopMatrix();

    std::map<GLuint, std::pair<std::string, std::string> >::const_iterator I;
    int i = 0;
    for(I = charNames.begin(); I != charNames.end() && i < 7; ++I, ++i) {
        glPushMatrix();
        glTranslatef(xoffset[i], yoffset[i], 0.0f);
        glLoadName(I->first);
        m_g.print(I->second.second.c_str());
        glTranslatef(0.0f, 20.0f, 0.0f);
        if (selectedChar == I->second.first) {
            glScalef(1.2f, 1.2f, 1.2f);
        }
        glRotatef(-90,1,0,0);
        m_g.renderer.model->onRender();
        glPopMatrix();
    }
    glPopName();
}

void CharSelector::click()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (selectName == *I) {
            selectb.press();
        }
        if (createName == *I) {
            create.press();
        }
        std::map<GLuint,std::pair<std::string,std::string> >::const_iterator J;
        J = charNames.find(*I);
        if (J != charNames.end()) {
            selectedChar = J->second.first;
            std::cout << "SELECTED: " << selectedChar;
        }
    }
}

void CharSelector::release()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (selectName == *I) {
            m_obs = true;
            selectb.release();
            if (!selectedChar.empty()) {
                selectSignal.emit(selectedChar);
            }
        }
        if (createName == *I) {
            m_obs = true;
            create.release();
            createSignal.emit();
        }
    }
}

bool CharSelector::key(int sym, int mod)
{
    return false;
}

void CharSelector::addCharacters(const std::set<std::pair<std::string, std::string> > & cl)
{
    std::set<std::pair<std::string, std::string> >::const_iterator I;
    for (I = cl.begin(); I != cl.end(); ++I) {
        charNames[m_g.newName()] = *I;
    }
}
