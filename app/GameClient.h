// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GAMECLIENT_H
#define APOGEE_GAMECLIENT_H

#include "Application.h"

#include <Eris/Connection.h>

class Vector3D;
class WorldEntity;

namespace Eris {
  class Player;
  class Lobby;
  class World;
  class Room;
  class Entity;
  class Coord;
}

namespace Atlas {
 namespace Objects {
  namespace Entity {
   class Player;
  }
 }
}

class GameClient : public Application {
  protected:
    bool inGame;
  public:
    GameClient(Renderer & rend, Eris::Connection & con) :
               Application(rend, con), inGame(false), player(NULL),
               lobby(NULL), world(NULL), character(NULL) { }
    Eris::Player * player;
    Eris::Lobby * lobby;
    Eris::World * world;
    WorldEntity * character;

    void netConnected();
    void netFailure(std::string msg);
    void connectionLog(Eris::LogLevel level, std::string msg);
    void netDisconnected();

    void login(const std::string &, const std::string &);
    void createCharacter(const std::string &, const std::string &);

    void lobbyTalk(Eris::Room *r, std::string nm, std::string t);
    void loginComplete(const Atlas::Objects::Entity::Player &p);

    void roomEnter(Eris::Room *r);
    // void roomAppear(Eris::Room *r, std::string id);
    // void roomLeave(Eris::Room *r, std::string id);

    void worldEntityCreate(Eris::Entity *r);
    void worldEnter(Eris::Entity *r);

    void charMoved(Eris::Entity *, Eris::Coord);

    void moveCharacter(const Vector3D & pos);
    const Vector3D getAbsCharPos();
};

#endif // APOGEE_GAMECLIENT_H
