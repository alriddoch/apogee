// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Compass.h"
#include "Gui.h"

Sprite Compass::compass_case;
Sprite Compass::compass_needle;
Sprite Compass::needle_shadow;

Compass::~Compass()
{
}

void Compass::setup()
{
    if (!Compass::compass_case.loaded()) {
        compass_case.load("compass_case.png");
    }
    if (!Compass::compass_needle.loaded()) {
        compass_needle.load("compass_needle.png");
    }
    if (!Compass::needle_shadow.loaded()) {
        needle_shadow.load("compass_needle_shadow.png");
    }
}

void Compass::draw()
{
    compass_case.draw();
    glTranslatef(0.0f, 32.0f, 0.01f);
    glTranslatef(-3.0f, -3.0f, 0.01f);
    glRotatef(-m_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, -32.0f, 0.01f);
    needle_shadow.draw();
    glTranslatef(0.0f, 32.0f, 0.01f);
    glRotatef(m_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(3.0f, 3.0f, 0.01f);
    glRotatef(-m_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, -32.0f, 0.01f);
    compass_needle.draw();
    glTranslatef(0.0f, 32.0f, 0.01f);
    glRotatef(m_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, -32.0f, 0.01f);
}

void Compass::select()
{
    compass_case.draw();
    // No need to draw needle, as it is not pickable
}

void Compass::click()
{
}

void Compass::release()
{
}

void Compass::key(int,int)
{
}
