// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_ENTITY_RENDERER_H
#define APOGEE_ENTITY_RENDERER_H

#include "common/Vector3D.h"

#include <sigc++/object.h>

class Renderer;

namespace Eris {
  class Entity;
}

class EntityRenderer : virtual public SigC::Object
{
  public:
    Eris::Entity & m_ent;
    bool m_drawContents;

    EntityRenderer(Renderer &, Eris::Entity & e);
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
    BBoxRenderer(Renderer &, Eris::Entity & e);
    virtual ~BBoxRenderer();

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_ENTITY_RENDERER_H
