// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_WORLDENTITY_H
#define APOGEE_WORLDENTITY_H

#include <Eris/Entity.h>
#include <Eris/Factory.h>

#include <visual/VisualEntity.h>

class WorldEntity : public Eris::Entity
{
  public:
    VisualEntity & visEntity;

    WorldEntity(const Atlas::Objects::Entity::GameEntity &ge)
              : Eris::Entity(ge), visEntity(*new VisualEntity()) { }
};

class WEFactory : public Eris::Factory
{
  public:
    virtual bool accept(const Atlas::Objects::Entity::GameEntity &);
    virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity & ge);
};

#endif // APOGEE_WORLDENTITY_H
