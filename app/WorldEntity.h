// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_WORLDENTITY_H
#define APOGEE_WORLDENTITY_H

#include <Eris/Entity.h>
#include <Eris/Factory.h>

#include <visual/VisualEntity.h>

namespace Eris {
  class TypeInfo;
}

class EntityRenderer;

class RenderableEntity : public Eris::Entity
{
  public:
    EntityRenderer * m_drawer;

    RenderableEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);
};

class MovableEntity : public RenderableEntity
{
    float updateTime;
  public:
    void movedSignal(const Point3D &);

    MovableEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

    const float getTime() {
        return updateTime;
    }
};

class AutonomousEntity : public MovableEntity
{
  public:
    AutonomousEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

};

class TerrainEntity : public RenderableEntity
{
  public:
    TerrainEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

};

class TreeEntity : public RenderableEntity
{
  public:
    TreeEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

};

class Application;

class WEFactory : public Eris::Factory
{
  private:
    static Eris::TypeInfo * autonomousType;
    static Eris::TypeInfo * terrainType;
    static Eris::TypeInfo * treeType;
  public:
    Application & m_app;

    explicit WEFactory(Application & a);
    
    virtual bool accept(const Atlas::Objects::Entity::GameEntity &, Eris::World *);
    virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity &, Eris::World *);
};

#endif // APOGEE_WORLDENTITY_H
