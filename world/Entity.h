// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Vector3D.h"

#include <string>

class Entity {
  private:
    std::string id;
    Vector3D pos;
    Vector3D vel;
    Vector3D bbox;
    Vector3D bmedian;
    Entity * loc;
    Atlas::Message::Object::MapType attributes;

  public:
    Entity(const std::string & _id) : id(_id), loc(NULL) { }

    const std::string & getId() const { return id; }

    const Vector3D & getPos() const { return pos; }
    const Vector3D & getVel() const { return vel; }
    const Vector3D & getBbox() const { return bbox; }
    const Vector3D & getBmedian() const { return bmedian; }

    const Vector3D getXyz() const;

    void setLoc(Entity * l) { loc = l; }

    Vector3D & Pos() { return pos; }
    Vector3D & Vel() { return vel; }
    Vector3D & Bbox() { return bbox; }
    Vector3D & Bmedian() { return bmedian; }

    void updateLoc(const Object::MapType & ent);
    void updateAttributes(const Object::MapType & ent);
};
