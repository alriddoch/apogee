// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "Compass.h"
#include "Gui.h"

#include "common/system.h"
#include "common/configuration.h"

#include <varconf/Config.h>

Sprite Compass::compass_case;
Sprite Compass::compass_needle;
Sprite Compass::needle_shadow;

Compass::~Compass()
{
}

void Compass::setup()
{
    if (!Compass::compass_case.loaded()) {
        std::string ccase;
        if (global_conf->findItem(m_g.getSkinName(), "compass_case")) {
            ccase = global_conf->getItem(m_g.getSkinName(), "compass_case").as_string();
        }
        compass_case.load(getMediaPath() + "/" + ccase);
    }
    if (!Compass::compass_needle.loaded()) {
        std::string needle;
        if (global_conf->findItem(m_g.getSkinName(), "compass_needle")) {
            needle = global_conf->getItem(m_g.getSkinName(), "compass_needle").as_string();
        }
        compass_needle.load(getMediaPath() + "/" + needle);
    }
    if (!Compass::needle_shadow.loaded()) {
        std::string shadow;
        if (global_conf->findItem(m_g.getSkinName(), "compass_shadow")) {
            shadow = global_conf->getItem(m_g.getSkinName(), "compass_shadow").as_string();
        }
        needle_shadow.load(getMediaPath() + "/" + shadow);
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

bool Compass::key(int,int)
{
    return false;
}
