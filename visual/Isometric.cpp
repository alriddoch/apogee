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

#include "GL.h"

#include "Isometric.h"

#include "common/configuration.h"

Isometric::Isometric(Application & app, int wdth, int hght) :
                                           Renderer(wdth, hght)
{
    if (map_mode) {
        elevation = 90;
        rotation = 0;
        scale = 30;
    }

    m_windowName = "apogee";
    m_iconName = "isometric";

}

void Isometric::projection()
{
    if (map_mode) {
        glOrtho(0,64,0,64,-300,300);
    } else {
        // this puts us into orthographic projection
        float xscale = width * scale / meterSize();
        float yscale = height * scale / meterSize();
        glOrtho(-xscale/2, xscale/2, -yscale/2, yscale/2, -200.0, 200.0 );
    }
}

void Isometric::viewPoint()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View

    glRotatef(elevation-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    glTranslatef(0.0f, 0.0f, -1.0f);      // Aim to centre the character

    glTranslatef(-x_offset,-y_offset,-z_offset);

    static const GLfloat lightPos[] = {1.f, 0.f, 1.f, 0.f};
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
}

void Isometric::drawSky()
{
}
