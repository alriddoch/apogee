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

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Lobby.h>
#include <Eris/View.h>
#include <Eris/Entity.h>
#include <Eris/Avatar.h>
#include <Eris/ServerInfo.h>
#include <Eris/TypeInfo.h>

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/bind.h>

#include <cassert>

using Atlas::Message::Element;

typedef Atlas::Message::Element Element;

using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::GameEntity;

bool GameClient::setup()
{
    Eris::Logged.connect(sigc::mem_fun(*this, &GameClient::connectionLog));

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
    d->oButtonSignal.connect(sigc::mem_fun(*this, &Application::connect));
    gui->addWidget(d);
#else
    ServerSelector * ss = new ServerSelector(*gui, renderer.getWidth()/2,
                                                   renderer.getHeight()/2);
    ss->connectSignal.connect(sigc::mem_fun(*this, &GameClient::connectHost));
    // ss->refreshSignal.connect(sigc::mem_fun(*this, &GameClient::refreshServers));
    ss->specifySignal.connect(sigc::mem_fun(*this, &GameClient::specifyHost));
    // ss->filterSignal.connect(sigc::mem_fun(*this, &GameClient::filterServers));

    // std::set<std::pair<std::string, std::string> > serverList;
    // serverList.insert(std::make_pair(std::string("localhost"), std::string("localhost")));
    // ss->addServers(serverList);

    gui->addWidget(ss);
#endif

    compassWidget = new Compass(*gui, -42, 10);
    gui->addWidget(compassWidget);

    // gui->addWidget(new Pie(*gui, 80, 170));

    consoleWidget = new Console(*gui, 4, 335);
    gui->addWidget(consoleWidget);

    return true;
}

void GameClient::connect(const std::string & host, const std::string & port)
{
    // FIXME Implement getting the port number from port
    connection = new Eris::Connection(appName, host, 6767, true);

    connection->Failure.connect(sigc::mem_fun(*this, &GameClient::netFailure));
    connection->Connected.connect(sigc::mem_fun(*this, &GameClient::netConnected));
    connection->Disconnected.connect(sigc::mem_fun(*this, &GameClient::netDisconnected));

    connection->connect();

    m_account = new Eris::Account(connection);
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
    d->oButtonSignal.connect(sigc::mem_fun(*this, &GameClient::connect));
    gui->addWidget(d);
}

void GameClient::doWorld()
{
    if (!inGame) {
        return;
    }
    assert(m_view != 0);
    Eris::Entity * root = m_view->getTopLevel();
    assert(root != 0);
    renderer.drawWorld(root);
}

bool GameClient::update(float secs)
{
    if (m_view != 0) {
        // m_view->tick();
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

void GameClient::loginComplete()
{
    std::cout << "Logged in" << std::endl << std::flush;

#if 0
    Option * o = new Option(*gui, renderer.getWidth() / 2,
                                  renderer.getHeight() / 2,
                                  "Select", "Create");
    o->buttonOneSignal.connect(sigc::mem_fun(*this, &GameClient::charListSync));
    o->buttonTwoSignal.connect(sigc::mem_fun(*this, &GameClient::charCreator));
    gui->addWidget(o);
#endif
    charListSync();
}

void GameClient::charCreator()
{
    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,renderer.getHeight()/2);
    d->addField("name", "Apogee Bomble");
    d->addField("type", "settler");
    d->oButtonSignal.connect(sigc::mem_fun(*this, &GameClient::createCharacter));
    gui->addWidget(d);
}

void GameClient::charListSync()
{
    std::cout << "charListSync" << std::endl << std::flush;
    assert(m_account != NULL);
    m_account->GotAllCharacters.connect(sigc::mem_fun(*this, &GameClient::charSelector));
    m_account->refreshCharacterInfo();
}

void GameClient::charSelector()
{
    std::cout << "charSelector" << std::endl << std::flush;
    CharSelector * cs = new CharSelector(*gui, renderer.getWidth()/2, renderer.getHeight()/2);
    cs->selectSignal.connect(sigc::mem_fun(*this, &GameClient::takeCharacter));
    cs->createSignal.connect(sigc::mem_fun(*this, &GameClient::charCreator));
    const Eris::CharacterMap & cm = m_account->getCharacters();
    std::set<std::pair<std::string, std::string> > charList;
    for(Eris::CharacterMap::const_iterator I = cm.begin(); I != cm.end(); ++I) {
        const std::string & id = I->first;
        const std::string & name = I->second->getName();
        std::cout << "Selecting from " << id << ":" << name
                  << std::endl << std::flush;
        charList.insert(std::pair<std::string,std::string>(id, name));
    }
    cs->addCharacters(charList);
    
    gui->addWidget(cs);
}


void GameClient::connectWorldSignals()
{
#warning FIXME No lobby talk yet
    // m_lobby->Talk.connect(sigc::mem_fun(*this,&GameClient::lobbyTalk));
    // m_lobby->Entered.connect(sigc::mem_fun(*this,&GameClient::roomEnter));

    m_account->AvatarSuccess.connect(sigc::mem_fun(*this,&GameClient::gotAvatar));
}

void GameClient::createCharacter(const std::string & name,
                                 const std::string & type)
{

    GameEntity chrcter;
    chrcter->setParents(std::list<std::string>(1,type));
    chrcter->setName(name);
    chrcter->setAttr("description", "a perigee person");
    chrcter->setAttr("sex", "female");
    m_account->createCharacter(chrcter);

    connectWorldSignals();
}

void GameClient::takeCharacter(const std::string & chrcter)
{
    std::cout << "takeCharacter" << std::endl << std::flush;
    m_account->takeCharacter(chrcter);

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
    d->oButtonSignal.connect(sigc::mem_fun(*this, &GameClient::login));
    d->cButtonSignal.connect(sigc::mem_fun(*this, &GameClient::loginCancel));
    gui->addWidget(d);

}

void GameClient::loginCancel()
{
    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,renderer.getHeight()/
2);
    d->addField("new login", "ajr");
    d->addField("password", "hel");
    d->oButtonSignal.connect(sigc::mem_fun(*this, &GameClient::create));
    gui->addWidget(d);
}

void GameClient::login(const std::string & name, const std::string & password)
{
    m_account->login(name, password);
    m_account->LoginSuccess.connect(sigc::mem_fun(*this, &GameClient::loginComplete));
    // m_lobby = Eris::Lobby::instance();
}

void GameClient::create(const std::string & name, const std::string & password)
{
    m_account->createAccount(name, "", password);
    m_account->LoginSuccess.connect(sigc::mem_fun(*this, &GameClient::loginComplete));
    // m_lobby = Eris::Lobby::instance();
}

void GameClient::netDisconnected()
{
    std::cout << "Disconnected from the server" << std::endl << std::flush;
    gui->addWidget(new Alert(*gui,renderer.getWidth()/2,
                                  renderer.getHeight()/2,
                                  "Disconnected from server"));
}

void GameClient::worldEntityCreate(Eris::Entity *e)
{
    // std::cout << "Created character" << std::endl << std::flush;
    e->Say.connect(sigc::bind<Eris::Entity*>(sigc::mem_fun(*this, &GameClient::entitySay), e));
}

void GameClient::worldEnter(Eris::Entity * chr)
{
    std::cout << "Enter world" << std::endl << std::flush;
    inGame = true;
    chr->Moved.connect(sigc::mem_fun(*this, &GameClient::charMoved));
    m_character = dynamic_cast<AutonomousEntity*>(chr);

    consoleWidget->lineEntered.connect(sigc::mem_fun(*m_avatar,&Eris::Avatar::say));

}

void GameClient::gotAvatar(Eris::Avatar * a)
{
    m_avatar = a;
    m_view = m_avatar->getView();
    m_view->registerFactory(new WEFactory(*connection->getTypeService(),
                                           renderer));
    m_view->EntityCreated.connect(sigc::mem_fun(*this,&GameClient::worldEntityCreate));
    m_avatar->GotCharacterEntity.connect(sigc::mem_fun(*this,&GameClient::worldEnter));
}

void GameClient::charMoved()
{
    std::cout << "Char moved" << std::endl << std::flush;
}

void GameClient::entitySay(const Atlas::Objects::Root & arg, Eris::Entity * e)
{
    std::cout << "Sound Talk" << std::endl << std::flush;
    Element say;
    if (arg->copyAttr("say", say) != 0 || !say.isString()) {
        std::cerr << "Say without a \"say\" attribute" << std::endl << std::flush;
        return;
    }
    std::string out(e->getName());
    if (out.empty()) {
        out += "[";
        out += e->getType()->getName();
        out += "]";
    }
    out += ": ";
    out += say.String();
    consoleWidget->pushLine(out);
}

void GameClient::moveCharacter(const PosType & pos, bool run)
{
    if (m_character == NULL) {
        return;
    }

    PosType coords(pos);
    Eris::Entity * ref = m_character->getLocation();
    Eris::Entity * r;
    while ((r = ref->getLocation()) != NULL) {
        // FIXME Incorrect usage. Needs real orientation.
        WFMath::Quaternion q = ref->getOrientation();
        if (!q.isValid()) {
            q.identity();
        }
        coords = coords.toLocalCoords(ref->getPosition(), q);
        ref = r;
    };
    
    Move m;

    Atlas::Message::MapType marg;
    marg["id"] = m_character->getId();
    marg["loc"] = m_character->getLocation()->getId();
    marg["pos"] = coords.toAtlas();
    if (!run) {
        marg["velocity"] = VelType(1,0,0).toAtlas();
    }
    m->setArgsAsList(Atlas::Message::ListType(1, marg));
    m->setFrom(m_character->getId());

    connection->send(m);
    
}

const PosType GameClient::getAbsCharPos()
{
    if (!inGame) {
        return PosType();
    }
    float now = SDL_GetTicks();
    PosType pos = m_character->getPosition();
    pos = pos + m_character->getVelocity() * (double)((now - m_character->getTime())/1000.0f);
    Eris::Entity * root = m_view->getTopLevel();
    Eris::Entity * ref = m_character->getLocation();
    if (ref != 0) {
        RenderableEntity * re = dynamic_cast<RenderableEntity *>(ref);
        if (re != 0) {
            re->constrainChild(*m_character, pos);;
        }
    }
    for(; ref != NULL && ref != root; ref = ref->getLocation()) {
        std::cout << pos << ", " << ref->getPosition() << std::endl << std::flush;
        // FIXME Incorrect usage. Needs real orientation.
        WFMath::Quaternion q = ref->getOrientation();
        if (!q.isValid()) {
            q.identity();
        }
        pos = pos.toParentCoords(ref->getPosition(), q);
        std::cout << pos << std::endl << std::flush;
    }
    return pos;
}
