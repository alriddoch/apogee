// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_ENTITY_RENDERER_H
#define APOGEE_ENTITY_RENDERER_H

#include <sigc++/object.h>

class Renderer;

namespace Eris {
  class Entity;
}

namespace WFMath {
  template<const int dim> class Vector;
}

class EntityRenderer : virtual public SigC::Object
{
  public:
    Eris::Entity & m_ent;

    EntityRenderer(Renderer &, Eris::Entity & e);
    virtual ~EntityRenderer();

    virtual void render(Renderer &, const WFMath::Vector<3> & camPos) = 0;
    virtual void select(Renderer &) = 0;
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

    virtual void render(Renderer &, const WFMath::Vector<3> & camPos);
    virtual void select(Renderer &);
};

#endif // APOGEE_ENTITY_RENDERER_H
