// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_ENTITY_RENDERER_H
#define APOGEE_ENTITY_RENDERER_H

class Renderer;

namespace Eris {
  class Entity;
}

class EntityRenderer
{
  public:
    Eris::Entity & ent;

    EntityRenderer(Eris::Entity & e);
    virtual ~EntityRenderer();

    virtual void render(Renderer &) = 0;
    virtual void select(Renderer &) = 0;
};

#endif // APOGEE_ENTITY_RENDERER_H
