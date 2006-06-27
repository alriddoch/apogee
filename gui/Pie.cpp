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

#include "Pie.h"

#include <iostream>

Sprite Pie::m_sliceGraphic;
Sprite Pie::m_slicePressedGraphic;

Pie::~Pie()
{
}

void Pie::setup()
{
    if (!m_sliceGraphic.loaded()) {
        m_sliceGraphic.load("pieslice_32.png");
        m_slicePressedGraphic.load("pieslice_32_inverted.png");
    }

    for(int i = 0; i < 10; ++i) {
        PieSlice foo(m_g, "test");
        m_slices.push_back(foo);
    }
}

void Pie::draw()
{
    int numSlices = m_slices.size();

    float angle = 360.f / numSlices;

    glPushMatrix();
    std::vector<PieSlice>::const_iterator I = m_slices.begin();
    for(int i = 0; I != m_slices.end(); ++I, ++i) {
        glPushMatrix();
        glRotatef(angle * i, 0.f, 0.f, 1.f);
        glTranslatef(60.f, 0.f, 0.f);
        glRotatef(angle * i, 0.f, 0.f, -1.f);
        if (I->isPressed()) {
            m_slicePressedGraphic.draw();
        } else {
            m_sliceGraphic.draw();
        }
        glPopMatrix();
    }

    glPopMatrix();
}

void Pie::select()
{
    int numSlices = m_slices.size();

    float angle = 360.f / numSlices;

    glPushMatrix();
    std::vector<PieSlice>::const_iterator I = m_slices.begin();
    glPushName(1);
    for(int i = 0; I != m_slices.end(); ++I, ++i) {
        glLoadName(I->getName());
        glPushMatrix();
        glRotatef(angle * i, 0.f, 0.f, 1.f);
        glTranslatef(60.f, 0.f, 0.f);
        glRotatef(angle * i, 0.f, 0.f, -1.f);
        m_sliceGraphic.draw();
        glPopMatrix();
    }
    glPopName();

    glPopMatrix();
}

void Pie::click()
{
    const hitlist & h = m_g.getHits();

    std::vector<PieSlice>::iterator I = m_slices.begin();
    for(; I != m_slices.end(); ++I) {
        hitlist::const_iterator J = h.begin();
        for (;J != h.end(); ++J) {
            if (*J == I->getName()) {
                I->press();
                SliceSelected.emit(I->getId());
            }
        }
    }
}

void Pie::release()
{
}

bool Pie::key(int, int)
{
    return false;
}

void Pie::addSlice(const std::string & name, const std::string & graphic)
{
    m_slices.push_back(PieSlice(m_g, name));
}
