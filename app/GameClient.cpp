// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "GameClient.h"
#include "WorldEntity.h"

#include <visual/Renderer.h>
#include <gui/Gui.h>
#include <gui/Dialogue.h>
#include <gui/Option.h>
#include <gui/CharSelector.h>
#include <gui/ServerSelector.h>
#include <gui/Alert.h>
#include <gui/Compass.h>
#include <gui/Pie.h>
#include <gui/Console.h>

#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Entity.h>
#include <Eris/Avatar.h>
#include <Eris/ServerInfo.h>

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Move.h>

#include <sigc++/object_slot.h>

#include <cassert>

using Atlas::Message::Element;

typedef Atlas::Message::Element Element;

using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::GameEntity;

bool GameClient::setup()
{
    connection.Failure.connect(SigC::slot(*this, &GameClient::netFailure));
    connection.Connected.connect(SigC::slot(*this, &GameClient::netConnected));
    connection.Disconnected.connect(SigC::slot(*this, &GameClient::netDisconnected));
    Eris::Logged.connect(SigC::slot(*this, &GameClient::connectionLog));

    if (!renderer.init()) {
        return false;
    }

    gui = new Gui(renderer);
    gui->setup();
    // bag = new Sprite();
    // bag->load("bag.png");

#if 0
    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,
                                     renderer.getHeight()/2);
    d->addField("host", "localhost");
    d->addField("port", "6767");
    d->oButtonSignal.connect(SigC::slot(*this, &Application::connect));
    gui->addWidget(d);
#else
    ServerSelector * ss = new ServerSelector(*gui, renderer.getWidth()/2,
                                                   renderer.getHeight()/2);
    ss->connectSignal.connect(SigC::slot(*this, &GameClient::connectHost));
    // ss->refreshSignal.connect(SigC::slot(*this, &GameClient::refreshServers));
    ss->specifySignal.connect(SigC::slot(*this, &GameClient::specifyHost));
    // ss->filterSignal.connect(SigC::slot(*this, &GameClient::filterServers));

    // std::set<std::pair<std::string, std::string> > serverList;
    // serverList.insert(std::make_pair(std::string("localhost"), std::string("localhost")));
    // ss->addServers(serverList);

    gui->addWidget(ss);
#endif

    compassWidget = new Compass(*gui, -42, 10);
    gui->addWidget(compassWidget);

    // gui->addWidget(new Pie(*gui, 80, 170));

    gui->addWidget(new Console(*gui, 4, 4));

    return true;
}

void GameClient::connectHost(const std::string & hostName)
{
    connect(hostName, "6767");
}

void GameClient::specifyHost()
{
    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,
                                     renderer.getHeight()/2);
    d->addField("host", "localhost");
    d->addField("port", "6767");
    d->oButtonSignal.connect(SigC::slot(*this, &Application::connect));
    gui->addWidget(d);
}

void GameClient::doWorld()
{
    if (!inGame) {
        return;
    }
    assert(m_world != 0);
    Eris::Entity * root = m_world->getRootEntity();
    assert(root != 0);
    renderer.drawWorld(root);
}

bool GameClient::update(float secs)
{
    if (m_world != 0) {
        m_world->tick();
    }

    renderer.update(secs);
    if (inGame) {
        PosType offset = getAbsCharPos();
        renderer.setXoffset(offset.x());
        renderer.setYoffset(offset.y());
        renderer.setZoffset(offset.z());
    }
    renderer.clear();

    renderer.drawSky();

    renderer.lightOn();

    renderer.origin();

    doWorld();

    renderer.lightOff();

    compassWidget->setAngle(-renderer.getRotation());
    renderer.drawGui();
    gui->draw();
    renderer.flip();
    return false;
}

void GameClient::lobbyTalk(Eris::Room *r, const std::string& nm,
                                          const std::string& t)
{
    std::cout << "TALK: " << t << std::endl << std::flush;
}

void GameClient::loginComplete(const Atlas::Objects::Entity::Player &p)
{
    std::cout << "Logged in" << std::endl << std::flush;

#if 0
    Option * o = new Option(*gui, renderer.getWidth() / 2,
                                  renderer.getHeight() / 2,
                                  "Select", "Create");
    o->buttonOneSignal.connect(SigC::slot(*this, &GameClient::charListSync));
    o->buttonTwoSignal.connect(SigC::slot(*this, &GameClient::charCreator));
    gui->addWidget(o);
#endif
    charListSync();
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
    assert(m_player != NULL);
    m_player->GotAllCharacters.connect(SigC::slot(*this, &GameClient::charSelector));
    m_player->refreshCharacterInfo();
}

void GameClient::charSelector()
{
    std::cout << "charSelector" << std::endl << std::flush;
    CharSelector * cs = new CharSelector(*gui, renderer.getWidth()/2, renderer.getHeight()/2);
    cs->selectSignal.connect(SigC::slot(*this, &GameClient::takeCharacter));
    cs->createSignal.connect(SigC::slot(*this, &GameClient::charCreator));
    const Eris::CharacterMap & cm = m_player->getCharacters();
    std::set<std::pair<std::string, std::string> > charList;
    for(Eris::CharacterMap::const_iterator I = cm.begin(); I != cm.end(); ++I) {
        const std::string & id = I->first;
        const std::string & name = I->second.getName();
        std::cout << "Selecting from " << id << ":" << name
                  << std::endl << std::flush;
        charList.insert(std::pair<std::string,std::string>(id, name));
    }
    cs->addCharacters(charList);
    
    gui->addWidget(cs);
}


void GameClient::connectWorldSignals()
{
    m_lobby->Talk.connect(SigC::slot(*this,&GameClient::lobbyTalk));
    m_lobby->Entered.connect(SigC::slot(*this,&GameClient::roomEnter));

    m_world->EntityCreate.connect(SigC::slot(*this,&GameClient::worldEntityCreate));
    m_world->Entered.connect(SigC::slot(*this,&GameClient::worldEnter));
    m_world->registerFactory(new WEFactory(*connection.getTypeService(),
                                           renderer));
}

void GameClient::createCharacter(const std::string & name,
                                 const std::string & type)
{

    GameEntity chrcter;
    chrcter.setParents(Atlas::Message::Element::ListType(1,type));
    chrcter.setName(name);
    chrcter.setAttr("description", "a perigee person");
    chrcter.setAttr("sex", "female");
    m_avatar = m_player->createCharacter(chrcter);
    m_world = m_avatar->getWorld();

    connectWorldSignals();
}

void GameClient::takeCharacter(const std::string & chrcter)
{
    std::cout << "takeCharacter" << std::endl << std::flush;
    m_avatar = m_player->takeCharacter(chrcter);
    m_world = m_avatar->getWorld();
    std::cout << "Character taken, world = " << m_world << std::endl << std::flush;

    connectWorldSignals();
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
    d->cButtonSignal.connect(SigC::slot(*this, &GameClient::loginCancel));
    gui->addWidget(d);

}

void GameClient::loginCancel()
{
    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,renderer.getHeight()/
2);
    d->addField("new login", "ajr");
    d->addField("password", "hel");
    d->oButtonSignal.connect(SigC::slot(*this, &GameClient::create));
    gui->addWidget(d);
}

void GameClient::login(const std::string & name, const std::string & password)
{
    m_player = new Eris::Player(&connection);
    m_player->login(name, password);
    m_lobby = Eris::Lobby::instance();
    m_lobby->LoggedIn.connect(SigC::slot(*this, &GameClient::loginComplete));
}

void GameClient::create(const std::string & name, const std::string & password)
{
    m_player = new Eris::Player(&connection);
    m_player->createAccount(name, "", password);
    m_lobby = Eris::Lobby::instance();
    m_lobby->LoggedIn.connect(SigC::slot(*this, &GameClient::loginComplete));
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
    // std::cout << "Created character" << std::endl << std::flush;
}

void GameClient::worldEnter(Eris::Entity * chr)
{
    std::cout << "Enter world" << std::endl << std::flush;
    inGame = true;
    chr->Moved.connect(SigC::slot(*this, &GameClient::charMoved));
    m_character = dynamic_cast<AutonomousEntity*>(chr);

}

void GameClient::charMoved(const PosType &)
{
    std::cout << "Char moved" << std::endl << std::flush;
}

void GameClient::moveCharacter(const PosType & pos)
{
    if (m_character == NULL) {
        return;
    }

    PosType coords(pos);
    Eris::Entity * ref = m_character->getContainer();
    Eris::Entity * r;
    while ((r = ref->getContainer()) != NULL) {
        // FIXME Incorrect usage. Needs real orientation.
        coords = coords.toLocalCoords(ref->getPosition(), WFMath::Quaternion().identity());
        ref = r;
    };
    
    Move m;

    Atlas::Message::Element::MapType marg;
    marg["id"] = m_character->getID();
    marg["loc"] = m_character->getContainer()->getID();
    marg["pos"] = coords.toAtlas();
    marg["velocity"] = VelType(1,0,0).toAtlas();
    m.setArgs(Atlas::Message::Element::ListType(1, marg));
    m.setFrom(m_character->getID());

    connection.send(m);
    
}

const PosType GameClient::getAbsCharPos()
{
    if (!inGame) {
        return PosType();
    }
    float now = SDL_GetTicks();
    PosType pos = m_character->getPosition();
    pos = pos + m_character->getVelocity() * (double)((now - m_character->getTime())/1000.0f);
    Eris::Entity * root = m_world->getRootEntity();
    Eris::Entity * ref = m_character->getContainer();
    if (ref != 0) {
        RenderableEntity * re = dynamic_cast<RenderableEntity *>(ref);
        if (re != 0) {
            re->constrainChild(*m_character, pos);;
        }
    }
    for(; ref != NULL && ref != root; ref = ref->getContainer()) {
        std::cout << pos << ", " << ref->getPosition() << std::endl << std::flush;
        // FIXME Incorrect usage. Needs real orientation.
        pos = pos.toParentCoords(ref->getPosition(), WFMath::Quaternion().identity());
        std::cout << pos << std::endl << std::flush;
    }
    return pos;
}
