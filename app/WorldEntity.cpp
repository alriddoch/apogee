// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "WorldEntity.h"

bool WEFactory::accept(const Atlas::Objects::Entity::GameEntity&)
{
    return true;
}

Eris::EntityPtr WEFactory::instantiate(const Atlas::Objects::Entity::GameEntity & ge)
{
    return new WorldEntity(ge);
}
