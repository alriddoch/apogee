// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "Isometric.h"

#include <common/debug.h>

#include <SDL.h>

#include <iostream>

static const bool debug_flag = false;

bool have_GL_EXT_compiled_vertex_array = false;

Isometric::Isometric(Application & app, int wdth, int hght) :
                                           Renderer(app, wdth, hght)

{
    init();
}

void Isometric::viewPoint()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // this puts us in perpective projection
    //gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    // this puts us into orthographic projection
    float xscale = width * scale / meterSize();
    float yscale = height * scale / meterSize();
    glOrtho(-xscale/2, xscale/2, -yscale/2, yscale/2, -2000.0, 2000.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    glTranslatef(0.0f, -1.0f, 0.0f);      // Aim to centre the character
    // GLfloat AmbientColor[] = {1, 0.5, 0.5, 1.0};
    // GLfloat DiffuseColor[] = {0, 1, 0, 1.0};
    // GLfloat LightPos[] = {xscale, yscale, 0.0, 1.0};
    // glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientColor);
    // glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseColor);
    // glLightfv(GL_LIGHT1, GL_POSITION,LightPos);
    // glEnable(GL_LIGHT1);
    // glEnable(GL_LIGHTING);
}

inline void Isometric::reorient()
{
    glRotatef(-rotation, 0.0, 0.0, 1.0);
    glRotatef(90-elevation, 1.0, 0.0, 0.0);
}

inline void Isometric::orient()
{
    glRotatef(elevation-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
}

inline void Isometric::translate()
{
    glTranslatef(-x_offset,-y_offset,-z_offset);
}

void Isometric::shapeView()
{
    //if (screen != NULL) {
        //SDL_FreeSurface(screen);
    //}
    if ((screen = SDL_SetVideoMode(width, height, 0,
            SDL_OPENGL|SDL_RESIZABLE)) == NULL) {
        std::cerr << "Failed to set video mode" << std::endl << std::flush;
        SDL_Quit();
        throw RendererSDLinit();
    }
    SDL_WM_SetCaption("apogee", "isometric");

    glViewport(0, 0, width, height);
    glClearColor(0.0, 0.7, 0.7, 0.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // glShadeModel(GL_SMOOTH);
    viewPoint();
}
