// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GameClient.h"
#include "WorldEntity.h"

#include <visual/Renderer.h>
#include <gui/Gui.h>
#include <gui/Dialogue.h>
#include <gui/Option.h>
#include <gui/CharSelector.h>
#include <gui/Alert.h>

#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Entity.h>
#include <Eris/Avatar.h>

#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Move.h>

#include <sigc++/object_slot.h>

using Atlas::Message::Object;

using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::GameEntity;

Point3D & operator+=(Point3D & lhs, const Point3D & rhs)
{
    lhs[0] += rhs[0];
    lhs[1] += rhs[1];
    lhs[2] += rhs[2];
    return lhs;
}

Point3D & operator-=(Point3D & lhs, const Point3D & rhs)
{
    lhs[0] -= rhs[0];
    lhs[1] -= rhs[1];
    lhs[2] -= rhs[2];
    return lhs;
}

Atlas::Message::Object Point3D::toAtlas() const
{
    Atlas::Message::Object::ListType ret(3);
    ret[0] = m_elem[0];
    ret[1] = m_elem[1];
    ret[2] = m_elem[2];
    return Atlas::Message::Object(ret);
}

const Point3D operator+(const Point3D & lhs, const Point3D & rhs)
{
    return Point3D(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

void GameClient::lobbyTalk(Eris::Room *r, const std::string& nm,
                                          const std::string& t)
{
    std::cout << "TALK: " << t << std::endl << std::flush;
}

void GameClient::loginComplete(const Atlas::Objects::Entity::Player &p)
{
    std::cout << "Logged in" << std::endl << std::flush;

    Option * o = new Option(*gui, renderer.getWidth() / 2,
                                  renderer.getHeight() / 2,
                                  "Select", "Create");
    o->buttonOneSignal.connect(SigC::slot(*this, &GameClient::charListSync));
    o->buttonTwoSignal.connect(SigC::slot(*this, &GameClient::charCreator));
    gui->addWidget(o);
}

void GameClient::charCreator()
{
    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,renderer.getHeight()/2);
    d->addField("name", "Apogee Bomble");
    d->addField("type", "settler");
    d->oButtonSignal.connect(SigC::slot(*this, &GameClient::createCharacter));
    gui->addWidget(d);
}

void GameClient::charListSync()
{
    std::cout << "charListSync" << std::endl << std::flush;
    assert(player != NULL);
    player->GotAllCharacters.connect(SigC::slot(*this, &GameClient::charSelector));
    player->refreshCharacterInfo();
}

void GameClient::charSelector()
{
    std::cout << "charSelector" << std::endl << std::flush;
    CharSelector * cs = new CharSelector(*gui, renderer.getWidth()/2, renderer.getHeight()/2);
    cs->selectSignal.connect(SigC::slot(*this, &GameClient::takeCharacter));
    cs->createSignal.connect(SigC::slot(*this, &GameClient::charCreator));
    Eris::CharacterList cl = player->getCharacters();
    std::set<std::pair<std::string, std::string> > charList;
    for(Eris::CharacterList::const_iterator I = cl.begin(); I != cl.end(); ++I){
        std::cout << "Selecting from " << I->GetId() << ":" << I->GetName() << std::endl << std::flush;
        charList.insert(std::pair<std::string,std::string>(I->GetId(),I->GetName()));
    }
    cs->addCharacters(charList);
    
    gui->addWidget(cs);
}

void GameClient::createCharacter(const std::string & name,
                                 const std::string & type)
{

    GameEntity chrcter(GameEntity::Instantiate());
    chrcter.SetParents(Atlas::Message::Object::ListType(1,type));
    chrcter.SetName(name);
    chrcter.SetAttr("description", "a perigee person");
    chrcter.SetAttr("sex", "female");
    world = player->createCharacter(chrcter)->getWorld();

    lobby->Talk.connect(SigC::slot(*this,&GameClient::lobbyTalk));
    lobby->Entered.connect(SigC::slot(*this,&GameClient::roomEnter));

    world->EntityCreate.connect(SigC::slot(*this,&GameClient::worldEntityCreate));
    world->Entered.connect(SigC::slot(*this,&GameClient::worldEnter));
    world->registerFactory(new WEFactory());
}

void GameClient::takeCharacter(const std::string & chrcter)
{
    std::cout << "takeCharacter" << std::endl << std::flush;
    world = player->takeCharacter(chrcter)->getWorld();
    std::cout << "Character taken, world = " << world << std::endl << std::flush;

    lobby->Talk.connect(SigC::slot(*this,&GameClient::lobbyTalk));
    lobby->Entered.connect(SigC::slot(*this,&GameClient::roomEnter));

    world->EntityCreate.connect(SigC::slot(*this,&GameClient::worldEntityCreate));
    world->Entered.connect(SigC::slot(*this,&GameClient::worldEnter));
    world->registerFactory(new WEFactory());
}

void GameClient::roomEnter(Eris::Room *r)
{
    std::cout << "Enter room" << std::endl << std::flush;
}

void GameClient::netFailure(const std::string & msg)
{
    std::cout << "Got connection failure: " << msg << std::endl << std::flush;
    gui->addWidget(new Alert(*gui,renderer.getWidth()/2,
                                  renderer.getHeight()/2,
                                  msg));
}

void GameClient::connectionLog(Eris::LogLevel level, const std::string & msg)
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
    d->oButtonSignal.connect(SigC::slot(*this, &GameClient::login));
    gui->addWidget(d);

}

void GameClient::login(const std::string & name, const std::string & password)
{
    player = new Eris::Player(&connection);
    player->login(name, password);
    lobby = Eris::Lobby::instance();
    lobby->LoggedIn.connect(SigC::slot(*this, &GameClient::loginComplete));
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

void GameClient::worldEnter(Eris::Entity * chr)
{
    std::cout << "Enter world" << std::endl << std::flush;
    inGame = true;
    chr->Moved.connect(SigC::slot(*this, &GameClient::charMoved));
    character = dynamic_cast<WorldEntity*>(chr);

}

void GameClient::charMoved(const Point3D &)
{
    std::cout << "Char moved" << std::endl << std::flush;
}

void GameClient::moveCharacter(const Point3D & pos)
{
    if (character == NULL) {
        return;
    }

    Point3D coords(pos);
    Eris::Entity * ref = character->getContainer();
    Eris::Entity * r;
    while ((r = ref->getContainer()) != NULL) {
        coords -= ref->getPosition();
        ref = r;
    };
    
    Move m(Move::Instantiate());

    Atlas::Message::Object::MapType marg;
    marg["id"] = character->getID();
    marg["loc"] = character->getContainer()->getID();
    marg["pos"] = coords.toAtlas();
    marg["velocity"] = Point3D(1,0,0).toAtlas();
    m.SetArgs(Atlas::Message::Object::ListType(1, marg));
    m.SetFrom(character->getID());

    connection.send(m);
    
}

const Point3D GameClient::getAbsCharPos()
{
    if (!inGame) {
        return Point3D();
    }
    float now = SDL_GetTicks();
    Point3D pos = character->getPosition();
    pos = pos + character->getVelocity() * (double)((now - character->getTime())/1000.0f);
    Eris::Entity * root = world->getRootEntity();
    for(Eris::Entity * ref = character->getContainer();
        ref != NULL && ref != root;
        ref = ref->getContainer()) {
        pos = pos + ref->getPosition();
    }
    return pos;
}
