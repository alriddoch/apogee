// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Entity.h"

#include <map>
#include <list>

class World {
  public:
    typedef std::map<std::string, Entity *> edict;

  private:
    edict m_entities;
    std::list<std::string> m_lookIds;

  public:
    void addObject(Entity * e) {
        m_entities[e->getId()] = e;
    }
    Entity * getObject(const std::string & id) const {
        edict::const_iterator I = m_entities.find(id);
        if (I == m_entities.end()) {
            return NULL;
        }
        return I->second;
    }
    const edict & getWorld() const { return m_entities; }
    const std::list<std::string> & getIds() const { return m_lookIds; }

    void clearLookIds() { m_lookIds = std::list<std::string>(); }

    void updateObject(const Atlas::Message::Object::MapType & ent);

    
};

