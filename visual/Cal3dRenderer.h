// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_CAL3D_RENDERER_H
#define APOGEE_CAL3D_RENDERER_H

#include "EntityRenderer.h"

class Model;

class Cal3dRenderer : public EntityRenderer
{
  protected:
    void drawCal3dModel(Model * m);
    void selectCal3dModel(Model * m);
  public:
    Cal3dRenderer(Eris::Entity & e);
    virtual ~Cal3dRenderer();

    virtual void render(Renderer &);
    virtual void select(Renderer &);
};

#endif // APOGEE_CAL3D_RENDERER_H
