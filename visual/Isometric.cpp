// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "Isometric.h"

Isometric::Isometric(Application & app, int wdth, int hght) :
                                           Renderer(app, wdth, hght)
{
    m_windowName = "apogee";
    m_iconName = "isometric";

    init();
}

static const float maxViewDistance = 250.0f;

void Isometric::projection()
{
    // this puts us into orthographic projection
    float xscale = width * scale / meterSize();
    float yscale = height * scale / meterSize();
    glOrtho(-xscale/2, xscale/2, -yscale/2, yscale/2, -200.0, 200.0 );
}

void Isometric::viewPoint()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View

    glRotatef(elevation-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    glTranslatef(0.0f, 0.0f, -1.0f);      // Aim to centre the character

    glTranslatef(-x_offset,-y_offset,-z_offset);

    static GLfloat AmbientColor[] = {0.3f, 0.3f, 0.3f, 1.f};
    static GLfloat DiffuseColor[] = {1.f, 1.f, 0.95f, 1.f};
    static GLfloat LightPos[] = {0.f, 1.f, 1.f, 0.f};
    static GLfloat lmodel_ambient[] = {0.f, 0.f, 0.f, 1.f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientColor);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseColor);
    glLightfv(GL_LIGHT1, GL_POSITION,LightPos);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

void Isometric::drawSky()
{
}
