// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GAMECLIENT_H
#define APOGEE_GAMECLIENT_H

#include "Application.h"

#include <common/Vector3D.h>

#include <Mercator/Terrain.h>

#include <Eris/Log.h>

class WorldEntity;

namespace Eris {
  class Player;
  class Lobby;
  class World;
  class Room;
  class Entity;
  class Coord;
  class Connection;
}

namespace Atlas {
 namespace Objects {
  namespace Entity {
   class Player;
  }
 }
}

class Compass;

class GameClient : public Application {
  protected:
    bool inGame;
    bool haveTerrain;
    Mercator::Terrain mterrain;
    Compass * compassWidget;

    void doWorld();
  public:
    GameClient(Renderer & rend, Eris::Connection & con) :
               Application(rend, con), inGame(false), haveTerrain(false),
               player(NULL), lobby(NULL), world(NULL), character(NULL) { }
    Eris::Player * player;
    Eris::Lobby * lobby;
    Eris::World * world;
    WorldEntity * character;

    bool setup();
    bool update(float);

    void netConnected();
    void netFailure(const std::string & msg);
    void connectionLog(Eris::LogLevel level, const std::string & msg);
    void netDisconnected();

    void login(const std::string &, const std::string &);
    void createCharacter(const std::string &, const std::string &);
    void takeCharacter(const std::string &);

    void lobbyTalk(Eris::Room *r, const std::string& nm, const std::string& t);
    void loginComplete(const Atlas::Objects::Entity::Player &p);

    void charCreator();
    void charListSync();
    void charSelector();

    void roomEnter(Eris::Room *r);
    // void roomAppear(Eris::Room *r, std::string id);
    // void roomLeave(Eris::Room *r, std::string id);

    void readTerrain(Eris::Entity * ent);

    void worldEntityCreate(Eris::Entity *r);
    void worldEnter(Eris::Entity *r);

    void charMoved(const Point3D &);

    void moveCharacter(const Point3D & pos);
    const Point3D getAbsCharPos();
};

#endif // APOGEE_GAMECLIENT_H
