// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_GAMECLIENT_H
#define APOGEE_GAMECLIENT_H

#include "Application.h"

#include "common/Vector3D.h"

#include <Eris/Log.h>

#include <Atlas/Objects/ObjectsFwd.h>

class AutonomousEntity;

namespace Eris {
  class Account;
  class Avatar;
  class Lobby;
  class View;
  class Room;
  class Entity;
  class Coord;
  class Connection;
  class ServerInfo;
}

class Compass;
class Console;

class GameClient : public Application {
  protected:
    bool inGame;
    Compass * compassWidget;
    Console * consoleWidget;
    bool clickDown;
    Uint32 clickDownTime;
    Uint16 clickDownX;
    Uint16 clickDownY;

    void doWorld();

    void connectWorldSignals();

    void worldEntityCreate(Eris::Entity *r);
    void worldEnter(Eris::Entity *);
    void gotAvatar(Eris::Avatar *);
  public:
    GameClient(Renderer & rend, Eris::Connection & con) :
               Application(rend, con), inGame(false),
               compassWidget(NULL), clickDown(false),
               clickDownTime(0), clickDownX(0), clickDownY(0),
               m_account(NULL), m_avatar(NULL), m_lobby(NULL), m_view(NULL),
               m_character(NULL) { }
    Eris::Account * m_account;
    Eris::Avatar * m_avatar;
    Eris::Lobby * m_lobby;
    Eris::View * m_view;
    AutonomousEntity * m_character;

    bool setup();
    bool update(float);

    void connectHost(const std::string &);
    void specifyHost();

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
    void loginComplete();

    void charCreator();
    void charListSync();
    void charSelector();

    void roomEnter(Eris::Room *r);
    // void roomAppear(Eris::Room *r, std::string id);
    // void roomLeave(Eris::Room *r, std::string id);

    void charMoved();
    void entitySay(const std::string &, Eris::Entity * e);

    void moveCharacter(const PosType & pos, bool run = false);
    const PosType getAbsCharPos();
};

#endif // APOGEE_GAMECLIENT_H
