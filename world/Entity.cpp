// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Entity.h"

const Vector3D Entity::getXyz() const
{
    if (loc == NULL) {
        return pos;
    } else {
        return pos + loc->getXyz();
    }
}

void Entity::updateLoc(const Object::MapType & ent)
{
    try {
         Object::MapType::const_iterator I;
         I = ent.find("pos");
         if (I != ent.end()) {
             pos = Vector3D(I->second.AsList());
         }
         I = ent.find("velocity");
         if (I != ent.end()) {
             vel = Vector3D(I->second.AsList());
         }
         I = ent.find("bbox");
         if (I != ent.end()) {
             bbox = Vector3D(I->second.AsList());
         }
         I = ent.find("bmedian");
         if (I != ent.end()) {
             bmedian = Vector3D(I->second.AsList());
         }
    }
    catch (...) {
        std::cerr << "EXCEPTION: Caught when handling location of "
                  << std::endl << std::flush;
    }
}

void Entity::updateAttributes(const Object::MapType & ent)
{
    for (Object::MapType::const_iterator I=ent.begin(); I!=ent.end(); I++) {
        const std::string & key = I->first;
        if ((key == "bbox") || (key == "bmedian")) continue;
        if ((key == "pos") || (key == "loc") || (key == "velocity")) continue;
        if ((key == "face")) continue;
        attributes[key] = I->second;
    }
}
