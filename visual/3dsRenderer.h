// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

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
    void draw3dsNode(Lib3dsNode *);
    void draw3dsMesh(Lib3dsMesh *);
    void draw3dsFile();
  public:
    m3dsRenderer(Renderer &, Eris::Entity & e);
    virtual ~m3dsRenderer();

    void load(const std::string &);

    virtual void render(Renderer &, const PosType &);
    virtual void select(Renderer &, const PosType &);
};

#endif // APOGEE_M3DS_RENDERER_H
