// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_CAL3D_RENDERER_H
#define APOGEE_CAL3D_RENDERER_H

#include "EntityRenderer.h"

#include <string>

class Model;

class Cal3dRenderer : public EntityRenderer
{
  protected:
    static Model * m_default;
    Model * m_model;

    void drawCal3dModel(Model * m);
    void selectCal3dModel(Model * m);
    void update(float);
  public:
    Cal3dRenderer(Renderer &, Eris::Entity & e);
    virtual ~Cal3dRenderer();

    virtual void load(const std::string &);

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_CAL3D_RENDERER_H
