// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_CAL3D_RENDERER_H
#define APOGEE_CAL3D_RENDERER_H

#include "EntityRenderer.h"

#include <string>

class Cal3dModel;
class CalBone;
class CalCoreBone;

class Cal3dRenderer : public EntityRenderer
{
  protected:
    static Cal3dModel * m_default;
    Cal3dModel * m_model;
    CalBone * cb1;
    CalCoreBone * ccb1;

    void drawCal3dModel(Cal3dModel * m);
    void selectCal3dModel(Cal3dModel * m);
    void update(float);
  public:
    Cal3dRenderer(Renderer &, RenderableEntity & e);
    virtual ~Cal3dRenderer();

    void load(const std::string &);

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_CAL3D_RENDERER_H
