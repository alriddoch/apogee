// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "DemeterScene.h"

#include <SDL.h>

const float PI = 3.14159f;
const float FOG_RED = 0.5f;
const float FOG_GREEN = 0.75f;
const float FOG_BLUE = 1.0f;
const float FOG_ALPHA = 0.0f;

DemeterScene::DemeterScene(Application & app, int wdth, int hght) :
                                                 Renderer(app, wdth, hght)
{
    elevation = 10;
    rotation = 45;

    init();
}

void DemeterScene::shapeView()
{
    const float maxViewDistance = 4500.0f;

    if ((screen = SDL_SetVideoMode(width, height, 16,
            SDL_OPENGL|SDL_DOUBLEBUF|SDL_RESIZABLE)) == NULL) {
        std::cerr << "Failed to set video mode" << std::endl << std::flush;
        SDL_Quit();
        throw RendererSDLinit();
    }
    SDL_WM_SetCaption("perigee", "perspective");

    glViewport(0, 0, width, height);
    glClearColor(0.5f, 0.75f, 1.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_NORMALIZE);
    //glShadeModel(GL_SMOOTH);
    float fogColor[4];
    fogColor[0] = FOG_RED;
    fogColor[1] = FOG_GREEN;
    fogColor[2] = FOG_BLUE;
    fogColor[3] = FOG_ALPHA;
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE,GL_LINEAR);
    glFogfv(GL_FOG_COLOR,fogColor);
    glFogf(GL_FOG_START,100.0f);
    glFogf(GL_FOG_END,maxViewDistance - 100.0f);
    glHint(GL_FOG_HINT,GL_FASTEST);
}

void DemeterScene::projection()
{
    const float maxViewDistance = 4500.0f;
    // Not sure how to use this one to get a decent projection no matter
    // what the aspect ratio. Keep it here for reference.
    //gluPerspective(45.0f, (float)width/(float)height,0.65f, maxViewDistance);

    // Sort out perspective projection, with scale tied to height of the window
    // this allow a very wide window to give a nice widescreen view, and
    // allows a narrow window to be usable.
    float s = ((float)width / (float)height) * 3.0f / 8.0f;
    glViewport(0,0,width,height);
    glFrustum(-s,s,-0.375f,0.375f,0.65f,maxViewDistance);
}

void DemeterScene::viewPoint()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    glEnable(GL_DEPTH_TEST);
    glTranslatef(0.0f, 0.0f, -5.0f);
}

void DemeterScene::reorient()
{
    // FIXME See orient(); we are not its inverse. Is this a problem?
    glRotatef(-rotation, 0.0, 0.0, 1.0);
    glRotatef(90-elevation, 1.0, 0.0, 0.0);
}

void DemeterScene::orient()
{
    glRotatef(elevation-90, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, -2.5f);
}

void DemeterScene::translate()
{
    glTranslatef(-x_offset,-y_offset,-z_offset);
}
