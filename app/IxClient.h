#ifndef APOGEE_EDITOR_H
#define APOGEE_EDITOR_H

#include <coal/bladeloader.h>

#include <string>

#include "Application.h"

class Sprite;
class Model;

namespace Atlas {
 namespace Objects {
  namespace Entity {
   class Player;
  }
 }
}

namespace Eris {
 class World;
 class Player;
 class Lobby;
 class BaseConnection;
 class Connection;
 class Room;
 class Entity;
}

class IxClient: public Application {
  private:
    void grid();
    void doWorld();
    void compass();
    void axis();

    CoalDatabase map_database;
    Sprite * character;
    Model * model;
    Eris::Connection & connection;
  public:
    IxClient(Renderer & rend, Eris::Connection & con) : Application(rend),
                                                      character(NULL),
                                                      connection(con),
                                                      player(NULL),
                                                      lobby(NULL),
                                                      world(NULL) { }
    virtual ~IxClient() { }

    bool setup();
    bool update();
    bool event(SDL_Event &);
    void mouse(int x, int y);

    void netConnected();
    void netFailure(std::string msg);
    void netDisconnected();

    void lobbyTalk(Eris::Room *r, std::string nm, std::string t);
    void loginComplete(const Atlas::Objects::Entity::Player &p);

    void roomEnter(Eris::Room *r);
    // void roomAppear(Eris::Room *r, std::string id);
    // void roomLeave(Eris::Room *r, std::string id);

    void worldEntityCreate(Eris::Entity *r);
    void worldEnter(Eris::Entity *r);

    Eris::Player * player;
    Eris::Lobby * lobby;
    Eris::World * world;
};

#endif // APOGEE_EDITOR_H
