// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GameClient.h"

#include <visual/Renderer.h>
#include <gui/Gui.h>
#include <gui/Dialogue.h>
#include <gui/Alert.h>

#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Entity.h>

#include <Atlas/Objects/Entity/GameEntity.h>

#include <sigc++/object_slot.h>

using Atlas::Objects::Entity::GameEntity;

void GameClient::lobbyTalk(Eris::Room *r, std::string nm, std::string t)
{
    std::cout << "TALK: " << t << std::endl << std::flush;
}

void GameClient::loginComplete(const Atlas::Objects::Entity::Player &p)
{
    std::cout << "Logged in" << std::endl << std::flush;

    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,renderer.getHeight()/
2);
    d->addField("name", "Apogee Dubneal");
    d->addField("type", "farmer");
    d->oButtonSignal.connect(SigC::slot(this, &GameClient::createCharacter));
    gui->addWidget(d);
}

void GameClient::createCharacter(const std::string & name,
                                 const std::string & type)
{

    GameEntity chrcter(GameEntity::Instantiate());
    chrcter.SetParents(Atlas::Message::Object::ListType(1,type));
    chrcter.SetName(name);
    chrcter.SetAttr("description", "a perigee person");
    chrcter.SetAttr("sex", "female");
    world = player->createCharacter(chrcter);

    lobby->Talk.connect(SigC::slot(this,&GameClient::lobbyTalk));
    lobby->Entered.connect(SigC::slot(this,&GameClient::roomEnter));

    world->EntityCreate.connect(SigC::slot(this,&GameClient::worldEntityCreate));
    world->Entered.connect(SigC::slot(this,&GameClient::worldEnter));
}

void GameClient::roomEnter(Eris::Room *r)
{
    std::cout << "Enter room" << std::endl << std::flush;
}

void GameClient::netFailure(std::string msg)
{
    std::cout << "Got connection failure: " << msg << std::endl << std::flush;
    gui->addWidget(new Alert(*gui,renderer.getWidth()/2,
                                  renderer.getHeight()/2,
                                  msg));
}

void GameClient::connectionLog(std::string msg)
{
    std::cout << "LOG: " << msg << std::endl << std::flush;
}

void GameClient::netConnected()
{
    std::cout << "Connected to server!" << std::endl << std::flush;

    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,renderer.getHeight()/
2);
    d->addField("login", "ajr");
    d->addField("password", "hel");
    d->oButtonSignal.connect(SigC::slot(this, &GameClient::login));
    gui->addWidget(d);

}

void GameClient::login(const std::string & name, const std::string & password)
{
    player = new Eris::Player();
    lobby = player->login(&connection, name, password);
    lobby->LoggedIn.connect(SigC::slot(this, &GameClient::loginComplete));
}

void GameClient::netDisconnected()
{
    std::cout << "Disconnected from the server" << std::endl << std::flush;
    gui->addWidget(new Alert(*gui,renderer.getWidth()/2,
                                  renderer.getHeight()/2,
                                  "Disconnected from server"));
}

void GameClient::worldEntityCreate(Eris::Entity *r)
{
    std::cout << "Created character" << std::endl << std::flush;
}

void GameClient::worldEnter(Eris::Entity *r)
{
    std::cout << "Enter world" << std::endl << std::flush;
    inGame = true;
}
