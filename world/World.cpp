// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "World.h"



using Atlas::Message::Object;

void World::updateObject(const Object::MapType & ent)
{
    Object::MapType::const_iterator ID = ent.find("id");
    if (ID == ent.end()) {
        std::cerr << "ERROR: Cannot update entity with no id"
                  << std::endl << std::flush;
        return;
    }
    try {
        const std::string & id = ID->second.AsString();
        edict::const_iterator I = m_entities.find(id);
        Entity * e;
        if (I != m_entities.end()) {
            e = I->second;
        } else {
            e = new Entity(id);
            addObject(e);
        }
        e->updateLoc(ent);
        e->updateAttributes(ent);
        Object::MapType::const_iterator J = ent.find("loc");
        if (J != ent.end() && J->second.IsString()) {
            const std::string & ref = J->second.AsString();
            I = m_entities.find(ref);
            Entity * r;
            if (I != m_entities.end()) {
                r = I->second;
            } else {
                r = new Entity(ref);
                addObject(e);
                m_lookIds.push_back(ref);
            }
            e->setLoc(r);

        }
        J = ent.find("contains");
        if (J != ent.end() && J->second.IsList()) {
            const Object::ListType & contains = J->second.AsList();
            Object::ListType::const_iterator K;
            for (K = contains.begin(); K != contains.end(); K++) {
                const std::string & cid = K->AsString();
                if (m_entities.find(cid) == m_entities.end()) {
                    m_lookIds.push_back(cid);
                }
            }
        }
    }
    catch (...) {
        std::cerr << "EXCEPTION: Caught when handling sight of entity"
                  << std::endl << std::flush;
    }

}
