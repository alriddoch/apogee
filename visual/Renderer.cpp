// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Renderer.h"
#include "Texture.h"
#include "Sprite.h"
#include "Model.h"

#include <SDL_image.h>

#include <GL/glu.h>

#include <iostream>

Renderer * Renderer::instance = NULL;

Renderer::Renderer(int wdth, int hght) : screen(NULL),
                                         width(wdth), height(hght),
                                         elevation(30), rotation(45),
                                         scale(1), x_offset(0), y_offset(0)
{
}

float Renderer::getZ(int x, int y)
{
    float z = 0;
    glReadPixels (x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    return z;
}

const Point3D Renderer::getWorldCoord(int x, int y, float z)
{
    GLint viewport[4];

    GLdouble mvmatrix[16], projmatrix[16];

    GLint realy;  /*  OpenGL y coordinate position  */

    GLdouble wx, wy, wz;  /*  returned world x, y, z coords  */

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

    gluUnProject (x, y, z, mvmatrix, projmatrix, viewport, &wx, &wy, &wz);

    std::cout << "{" << wx << ";" << wy << ";" << wz << std::endl << std::flush;
    return Point3D(wx, wy, wz);
}
