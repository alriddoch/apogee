// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "WorldEntity.h"

#include "Application.h"

#include "visual/Cal3dRenderer.h"

#include <Eris/World.h>
#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>

#include <sigc++/object_slot.h>

#include <SDL.h>

using Atlas::Objects::Entity::GameEntity;

RenderableEntity::RenderableEntity(const GameEntity &ge,
                             Eris::World * w) : Eris::Entity(ge, w), m_drawer(0)
{
}

MovableEntity::MovableEntity(const GameEntity &ge,
                             Eris::World * w) : RenderableEntity(ge, w)
{
    Moved.connect(SigC::slot(*this, &MovableEntity::movedSignal));
    updateTime = SDL_GetTicks();
}

void MovableEntity::movedSignal(const Point3D &)
{
    updateTime = SDL_GetTicks();
}

CharacterEntity::CharacterEntity(const GameEntity &ge,
                             Eris::World * w) : MovableEntity(ge, w)
{
}

TerrainEntity::TerrainEntity(const GameEntity &ge,
                             Eris::World * w) : RenderableEntity(ge, w)
{
}

TreeEntity::TreeEntity(const GameEntity &ge,
                             Eris::World * w) : RenderableEntity(ge, w)
{
}

WEFactory::WEFactory(Application & a) : m_app(a)
{
}

Eris::TypeInfo * WEFactory::characterType = 0;
Eris::TypeInfo * WEFactory::terrainType = 0;
Eris::TypeInfo * WEFactory::treeType = 0;

bool WEFactory::accept(const GameEntity&, Eris::World * w)
{
    if (characterType == 0) {
        characterType = w->getConnection()->getTypeInfoEngine()->findSafe("character");
    }
    if (terrainType == 0) {
        terrainType = w->getConnection()->getTypeInfoEngine()->findSafe("terrain");
    }
    if (treeType == 0) {
        treeType = w->getConnection()->getTypeInfoEngine()->findSafe("tree");
    }

    return true;
}

Eris::EntityPtr WEFactory::instantiate(const GameEntity & ge,
                                       Eris::World * w)
{
    RenderableEntity * re = 0;
    Eris::TypeInfoPtr type = w->getConnection()->getTypeInfoEngine()->getSafe(ge);
    if (type->safeIsA(characterType)) {
        re = new CharacterEntity(ge,w);
        re->m_drawer = new Cal3dRenderer(m_app.renderer, *re);
    } else if (type->safeIsA(terrainType)) {
        re = new TerrainEntity(ge,w);
    } else if (type->safeIsA(treeType)) {
        re = new TreeEntity(ge,w);
    } else {
        re = new RenderableEntity(ge, w);
    }
    if (re->m_drawer == 0) {
        re->m_drawer = new BBoxRenderer(m_app.renderer, *re);
    }
    return re;
}
