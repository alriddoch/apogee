// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "WorldEntity.h"

#include <sigc++/object_slot.h>

#include <SDL.h>

WorldEntity::WorldEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World * w)
              : Eris::Entity(ge, w), visEntity(*new VisualEntity())
{
    Moved.connect(SigC::slot(*this, &WorldEntity::movedSignal));
    updateTime = SDL_GetTicks();
}

void WorldEntity::movedSignal(const Point3D &)
{
    updateTime = SDL_GetTicks();
}

bool WEFactory::accept(const Atlas::Objects::Entity::GameEntity&, Eris::World *)
{
    return true;
}

Eris::EntityPtr WEFactory::instantiate(const Atlas::Objects::Entity::GameEntity & ge,
                                       Eris::World * w)
{
    return new WorldEntity(ge, w);
}
