// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "Isometric.h"

#include <SDL.h>

Isometric::Isometric(Application & app, int wdth, int hght) :
                                           Renderer(app, wdth, hght)

{
    init();
}

void Isometric::shapeView()
{
    if ((screen = SDL_SetVideoMode(width, height, 0,
            SDL_OPENGL|SDL_RESIZABLE)) == NULL) {
        std::cerr << "Failed to set video mode" << std::endl << std::flush;
        SDL_Quit();
        throw RendererSDLinit();
    }
    SDL_WM_SetCaption("apogee", "isometric");

    glViewport(0, 0, width, height);
    glClearColor(0.0, 0.7, 0.7, 0.0);
    glDepthFunc(GL_LEQUAL);
    glEnableClientState(GL_VERTEX_ARRAY);
}

void Isometric::projection()
{
    // this puts us into orthographic projection
    float xscale = width * scale / meterSize();
    float yscale = height * scale / meterSize();
    glOrtho(-xscale/2, xscale/2, -yscale/2, yscale/2, -2000.0, 2000.0 );
}

void Isometric::viewPoint()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    // GLfloat AmbientColor[] = {1, 0.5, 0.5, 1.0};
    // GLfloat DiffuseColor[] = {0, 1, 0, 1.0};
    // GLfloat LightPos[] = {xscale, yscale, 0.0, 1.0};
    // glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientColor);
    // glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseColor);
    // glLightfv(GL_LIGHT1, GL_POSITION,LightPos);
    // glEnable(GL_LIGHT1);
    // glEnable(GL_LIGHTING);

    glRotatef(elevation-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, -1.0f);      // Aim to centre the character

    glTranslatef(-x_offset,-y_offset,-z_offset);
}
