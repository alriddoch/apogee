// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

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

void Pie::key(int, int)
{
}

void Pie::addSlice(const std::string & name, const std::string & graphic)
{
    m_slices.push_back(PieSlice(m_g, name));
}
