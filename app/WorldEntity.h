// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_WORLDENTITY_H
#define APOGEE_WORLDENTITY_H

#include "common/Vector3D.h"

#include <Eris/Entity.h>
#include <Eris/Factory.h>

namespace Eris {
  class TypeInfo;
}

class EntityRenderer;

class RenderableEntity : public Eris::Entity
{
  public:
    EntityRenderer * m_drawer;

    RenderableEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);
    virtual void constrainChild(PosType & pos);
};

class MovableEntity : public RenderableEntity
{
    float updateTime;
  public:
    void movedSignal(const PosType &);

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

    virtual void constrainChild(PosType & pos);
};

class TreeEntity : public RenderableEntity
{
  public:
    TreeEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

};

class Renderer;

class RenderFactory
{
  public:
    RenderFactory() { }
    virtual ~RenderFactory();

    virtual EntityRenderer * newRenderer(Renderer &, Eris::Entity &) const = 0;
};

template <class R>
class RendererFactory : public RenderFactory
{
  private:
    std::string m_filename;
  public:
    explicit RendererFactory(const std::string & fname) : m_filename(fname) { }

    EntityRenderer * newRenderer(Renderer & r, Eris::Entity & e) const {
        R * er = new R(r, e);
        er->load(m_filename);
        return er;
    }
};

class Application;

class WEFactory : public Eris::Factory
{
  public:
    typedef std::map<std::string, RenderFactory *> RendererMap;
  private:
    static Eris::TypeInfo * autonomousType;
    static Eris::TypeInfo * terrainType;
    static Eris::TypeInfo * treeType;

    RendererMap m_renderFactories;
  public:
    Application & m_app;

    explicit WEFactory(Application & a);
    virtual ~WEFactory();
    
    virtual bool accept(const Atlas::Objects::Entity::GameEntity &, Eris::World *);
    virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity &, Eris::World *);
};

#endif // APOGEE_WORLDENTITY_H
