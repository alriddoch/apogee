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

#ifndef APOGEE_M3DS_RENDERER_H
#define APOGEE_M3DS_RENDERER_H

#include "GL.h"

#include "EntityRenderer.h"

#include <lib3ds/file.h>

#include <string>

#define BUF_VERTEX 0
#define BUF_TCOORD 1
#define BUF_NORMAL 2

class VertexBuffer
{
  public:
    float * buffers[3];
    GLuint bobject;
};

class m3dsRenderer : public EntityRenderer
{
  protected:
    Lib3dsFile * m_model;

    void compileVertexBuffer(Lib3dsMesh *, VertexBuffer *);
    void draw3dsNode(Renderer&, Lib3dsNode *);
    void draw3dsMesh(Lib3dsMesh *);
    void draw3dsFile(Renderer&);
  public:
    m3dsRenderer(Renderer &, RenderableEntity & e);
    virtual ~m3dsRenderer();

    void load(const std::string &);

    virtual void render(Renderer &, const PosType &);
    virtual void select(Renderer &, const PosType &);
};

#endif // APOGEE_M3DS_RENDERER_H
