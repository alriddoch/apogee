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
