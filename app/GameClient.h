// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Application.h"

namespace Eris {
  class Player;
  class Lobby;
  class World;
  class Room;
  class Entity;
}

namespace Atlas {
 namespace Objects {
  namespace Entity {
   class Player;
  }
 }
}

class GameClient : public Application {
  public:
    GameClient(Renderer & rend, Eris::Connection & con) :
               Application(rend, con), player(NULL), lobby(NULL),
                                       world(NULL) { }
    Eris::Player * player;
    Eris::Lobby * lobby;
    Eris::World * world;

    void netConnected();
    void netFailure(std::string msg);
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
};
