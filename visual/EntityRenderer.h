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

#ifndef APOGEE_ENTITY_RENDERER_H
#define APOGEE_ENTITY_RENDERER_H

#include "common/Vector3D.h"

#include <sigc++/trackable.h>

class Renderer;
class RenderableEntity;

class EntityRenderer : virtual public sigc::trackable
{
  public:
    RenderableEntity & m_ent;
    bool m_drawContents;

    EntityRenderer(Renderer &, RenderableEntity & e);
    virtual ~EntityRenderer();

    bool drawContents() const { return m_drawContents; }

    virtual void render(Renderer &, const PosType & camPos) = 0;
    virtual void select(Renderer &, const PosType & camPos) = 0;
};

namespace WFMath {
  template<const int dim> class AxisBox;
}

class BBoxRenderer : public EntityRenderer
{
  protected:
    void draw3DBox(const WFMath::AxisBox<3> & bbox);
    void select3DBox(const WFMath::AxisBox<3> & bbox);
  public:
    BBoxRenderer(Renderer &, RenderableEntity & e);
    virtual ~BBoxRenderer();

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_ENTITY_RENDERER_H
