// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GAMECLIENT_H
#define APOGEE_GAMECLIENT_H

#include "Application.h"

#include <common/Vector3D.h>

#include <Eris/Log.h>

class AutonomousEntity;

namespace Eris {
  class Player;
  class Avatar;
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
    Compass * compassWidget;
    bool clickDown;
    Uint32 clickDownTime;
    Uint16 clickDownX;
    Uint16 clickDownY;

    void doWorld();

    void connectWorldSignals();
  public:
    GameClient(Renderer & rend, Eris::Connection & con) :
               Application(rend, con), inGame(false),
               compassWidget(NULL), clickDown(false),
               clickDownTime(0), clickDownX(0), clickDownY(0),
               m_player(NULL), m_avatar(NULL), m_lobby(NULL), m_world(NULL),
               m_character(NULL) { }
    Eris::Player * m_player;
    Eris::Avatar * m_avatar;
    Eris::Lobby * m_lobby;
    Eris::World * m_world;
    AutonomousEntity * m_character;

    bool setup();
    bool update(float);

    void netConnected();
    void netFailure(const std::string & msg);
    void connectionLog(Eris::LogLevel level, const std::string & msg);
    void netDisconnected();

    void loginCancel();

    void login(const std::string &, const std::string &);
    void create(const std::string &, const std::string &);
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

    void worldEntityCreate(Eris::Entity *r);
    void worldEnter(Eris::Entity *r);

    void charMoved(const Point3D &);

    void moveCharacter(const Point3D & pos);
    const Point3D getAbsCharPos();
};

#endif // APOGEE_GAMECLIENT_H
