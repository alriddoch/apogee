// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Pie.h"

#include <iostream>

Sprite Pie::m_sliceGraphic;

Pie::~Pie()
{
}

void Pie::setup()
{
    if (!m_sliceGraphic.loaded()) {
        m_sliceGraphic.load("pieslice_32.png");
    }

    for(int i = 0; i < 5; ++i) {
        PieSlice foo;
        m_slices.push_back(foo);
    }
}

void Pie::draw()
{
    int numSlices = m_slices.size();

    if (12 < numSlices) {
        std::cout << "Can't handle a pie with more than 8 slices."
                  << std::endl << std::flush;
        return;
    }

    float angle = 360.f / numSlices;

    glPushMatrix();
    for(int i = 0; i < numSlices; ++i) {
        glRotatef(angle, 0.f, 0.f, 1.f);
        glPushMatrix();
        glTranslatef(40.f, 0.f, 0.f);
        m_sliceGraphic.draw();
        glPopMatrix();
    }

    glPopMatrix();
}

void Pie::select()
{
    draw();
}

void Pie::click()
{
}

void Pie::release()
{
}

void Pie::key(int, int)
{
}

void Pie::addSlice(const std::string & name, const std::string & graphic)
{
}
