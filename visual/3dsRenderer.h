// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_M3DS_RENDERER_H
#define APOGEE_M3DS_RENDERER_H

#include "EntityRenderer.h"

#include <lib3ds/file.h>

class m3dsRenderer : public EntityRenderer
{
  protected:
    Lib3dsFile * m_model;
    unsigned long m_model_list;

    void draw3dsModel();
  public:
    m3dsRenderer(Renderer &, Eris::Entity & e);
    virtual ~m3dsRenderer();

    virtual void render(Renderer &, const PosType &);
    virtual void select(Renderer &, const PosType &);
};

#endif // APOGEE_M3DS_RENDERER_H
