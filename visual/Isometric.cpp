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
    Uint32 flags = SDL_OPENGL|SDL_DOUBLEBUF|SDL_ANYFORMAT;
    if (fullscreen) {
        flags |= SDL_FULLSCREEN;
    } else {
        flags |= SDL_RESIZABLE;
    }
    if ((screen = SDL_SetVideoMode(width, height, 0, flags)) == NULL) {
        std::cerr << "Failed to set video mode" << std::endl << std::flush;
        SDL_Quit();
        throw RendererSDLinit();
    }
    SDL_WM_SetCaption("apogee", "isometric");

    glViewport(0, 0, width, height);
    glClearColor(0.0, 0.7, 0.7, 0.0);
}

void Isometric::drawSky()
{
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
