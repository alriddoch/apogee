// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IxClient.h"

#include <visual/Renderer.h>
#include <visual/Sprite.h>
#include <visual/Model.h>
#include <visual/widgets.h>

#include <gui/Gui.h>

#include <Atlas/Objects/Entity/GameEntity.h>

#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Entity.h>

#include <sigc++/object_slot.h>

#include <iostream>

#include <math.h>

#include <coal/debug.h>

using Atlas::Objects::Entity::GameEntity;

using Atlas::Message::Object;

void IxClient::grid()
{
    widgets::grid(renderer);
}

void IxClient::compass()
{
    widgets::compass(renderer);
}

void IxClient::axis()
{
    widgets::axis(renderer);
}

bool IxClient::setup()
{
    CoalBladeLoader loader (map_database);
    loader.LoadMap ("blade2.xml");
    CoalDebug debug;
    // debug.Dump (map_database);
    character = new Sprite();
    character->load("swinesherd_female_1_us_E_0.png",renderer);

    model = new Model();
    if (!model->onInit(Datapath() + "paladin.cfg")) {
        std::cerr << "Loading paladin model failed" << std::endl << std::flush;
    }

    gui = new Gui(renderer);
    gui->setup();
    
    return 0;
}

void IxClient::doWorld()
{
    if (world == NULL) {
        cout << "No world" << endl << flush;
        return;
    }
    Eris::Entity * root = world->getRootEntity();
    if (root == NULL) {
        cout << "No root" << endl << flush;
        return;
    }
    int numEnts = root->getNumMembers();
    cout << numEnts << " root" << endl << flush;
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * ent = root->getMember(i);
        std::cout << ":" << ent->getID() << ent->getPosition() << ":"
                  << ent->getBBox() << std::endl << std::flush;
        renderer.draw3DBox(ent->getPosition(), ent->getBBox());
    }
#if 0
    const World::edict & ents = world.getWorld();

    World::edict::const_iterator I;
    for (I = ents.begin(); I != ents.end(); I++) {
        std::cout << ":" << I->first << I->second->getPos() << ":"
                  << I->second->getBbox() << I->second->getBmedian()
                  << std::endl << std::flush;
        renderer.draw3DBox(I->second->getXyz(),
                           I->second->getBbox(),
                           I->second->getBmedian());
    }
#endif
}

bool IxClient::update()
{
    renderer.clear();
    renderer.lightOn();
    renderer.drawMap(map_database);
    renderer.origin();
    model->onUpdate(0.1);
    renderer.drawCal3DModel(model, 0, 0);
    doWorld();
    renderer.lightOff();
    renderer.drawGui();
    glTranslatef(200.0f, 0.0f,10.0f);
    gui->draw();
    renderer.flip();
    return false;
}

bool IxClient::event(SDL_Event & event)
{
    static int oldx = 0;
    static int oldy = 0;
    static int oldRot = 0;
    static int oldElv = 0;
    static float oldScl = 0;
    switch(event.type) {
        case SDL_MOUSEMOTION:
            if (event.motion.state & SDL_BUTTON(2)) {
                //int w = renderer.getWidth();
                //int h = renderer.getHeight();
                const int x = event.motion.x;
                const int y = event.motion.y;
                int newRot = ((x - oldx) * 360) / renderer.getWidth();
                if (oldy > renderer.getHeight()/2) {
                    newRot = oldRot + newRot;
                } else {
                    newRot = oldRot - newRot;
                }
                while (newRot >= 360) { newRot -= 360; };
                int newElv = ((y - oldy) * 90) / renderer.getHeight();
                newElv = oldElv + newElv;
                if (newElv < 0) { newElv = 0; }
                if (newElv > 90) { newElv = 90; }
                renderer.setRotation(newRot);
                renderer.setElevation(newElv);
                return true;
            } else if (event.motion.state & SDL_BUTTON(3)) {
                const int y = event.motion.y;
                const int h = renderer.getHeight();
                float newScl = ((float)(h + y - oldy) / h) * oldScl;
                renderer.setScale(newScl);
                return true;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if ((event.button.type & SDL_MOUSEBUTTONDOWN) &&
                ((event.button.button & SDL_BUTTON_MIDDLE) ||
                 (event.button.button & SDL_BUTTON_RIGHT)) &&
                (event.button.state & SDL_PRESSED)) {
                oldx = event.button.x;
                oldy = event.button.y;
                oldRot = renderer.getRotation();
                oldElv = renderer.getElevation();
                oldScl = renderer.getScale();
                return false;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_0:
                    renderer.setElevation(0);
                    renderer.setRotation(45);
                    return true;
                    break;
                case SDLK_UP:
                    renderer.setYoffset(renderer.getYoffset() + 1);
                    break;
                case SDLK_DOWN:
                    renderer.setYoffset(renderer.getYoffset() - 1);
                    break;
                case SDLK_LEFT:
                    renderer.setXoffset(renderer.getXoffset() - 1);
                    break;
                case SDLK_RIGHT:
                    renderer.setXoffset(renderer.getXoffset() + 1);
                    break;
                default:
                    return false;
                    break;
            }
            return true;
            break;
    }
    return false;
}

void IxClient::mouse(int dx, int dy)
{
    int newRot = renderer.getRotation() + dx;
    while (newRot >= 360) { newRot -= 360; };
    renderer.setRotation(newRot);

    int newElv = renderer.getElevation() + dy;
    if (newElv < -90) { newElv = -90; }
    if (newElv > 90) { newElv = 90; }
    renderer.setElevation(newElv);
}

void IxClient::lobbyTalk(Eris::Room *r, std::string nm, std::string t)
{
    std::cout << "TALK: " << t << std::endl << std::flush;
}

void IxClient::loginComplete(const Atlas::Objects::Entity::Player &p)
{
    std::cout << "Logged in" << std::endl << std::flush;

    GameEntity chrcter(GameEntity::Instantiate());
    chrcter.SetParents(Atlas::Message::Object::ListType(1,"farmer"));
    chrcter.SetName("Apogee Dubneal");
    chrcter.SetAttr("description", "a perigee person");
    chrcter.SetAttr("sex", "female");
    world = player->createCharacter(chrcter);

    lobby->Talk.connect(SigC::slot(this, &IxClient::lobbyTalk));
    lobby->Entered.connect(SigC::slot(this, &IxClient::roomEnter));

    world->EntityCreate.connect(SigC::slot(this, &IxClient::worldEntityCreate));
    world->Entered.connect(SigC::slot(this, &IxClient::worldEnter));
}

void IxClient::roomEnter(Eris::Room *r)
{
    std::cout << "Enter room" << std::endl << std::flush;
}

void IxClient::netFailure(std::string msg)
{
    std::cout << "Got connection failure: " << msg << std::endl << std::flush;
}

void IxClient::netConnected()
{
    std::cout << "Connected to server!" << std::endl << std::flush;

    player = new Eris::Player();
    lobby = player->login(&connection, "ajr", "hel");
    lobby->LoggedIn.connect(SigC::slot(this, &IxClient::loginComplete));
}

void IxClient::netDisconnected()
{
    std::cout << "Disconnected from the server" << std::endl << std::flush;
}

void IxClient::worldEntityCreate(Eris::Entity *r)
{
    std::cout << "Created character" << std::endl << std::flush;
}

void IxClient::worldEnter(Eris::Entity *r)
{
    std::cout << "Enter world" << std::endl << std::flush;
}
