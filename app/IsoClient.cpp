// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IsoClient.h"

#include <visual/Renderer.h>
#include <visual/Sprite.h>

#include <gui/Gui.h>
#include <gui/Dialogue.h>
#include <gui/Compass.h>

#include <Atlas/Objects/Entity/GameEntity.h>

#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Entity.h>

#include <coal/isoloader.h>

#include <sigc++/object_slot.h>

#include <SDL.h>

#include <iostream>

#include <math.h>

using Atlas::Objects::Entity::GameEntity;

using Atlas::Message::Object;

bool IsoClient::setup()
{
    Coal::IsoLoader loader;
    loader.loadMap ("moraf.map", &map_database);
    // CoalDebug debug;
    // debug.Dump (map_database);

    map_height.load("moraf_hm.png");
    mterrain.setBasePoint(-1, -1, -8.f);
    mterrain.setBasePoint(-1, 0, -6.f);
    mterrain.setBasePoint(0, -1, -10.f);
    mterrain.setBasePoint(0, 0, 0.f);
    mterrain.setBasePoint(0, 1, 8.f);
    mterrain.setBasePoint(1, 0, 4.f);
    mterrain.setBasePoint(1, 1, 14.f);
    mterrain.setBasePoint(1, -1, -4.f);
    mterrain.setBasePoint(-1, 1, -4.f);
    mterrain.refresh(0, 0);
    mterrain.refresh(0, 1);
    mterrain.refresh(1, 0);
    mterrain.refresh(1, 1);

    gui = new Gui(renderer);
    gui->setup();
    // bag = new Sprite();
    // bag->load("bag.png");

    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,
                                     renderer.getHeight()/2);
    d->addField("host", "localhost");
    d->addField("port", "6767");
    d->oButtonSignal.connect(SigC::slot(*this, &Application::connect));
    gui->addWidget(d);

    compassWidget = new Compass(*gui, 42, 10);
    gui->addWidget(compassWidget);

    return 0;
}

void IsoClient::doWorld()
{
    if ((world == NULL) || !inGame) {
        // std::cout << "No world" << std::endl << std::flush;
        return;
    }
    Eris::Entity * root = world->getRootEntity();
    if (root == NULL) {
        std::cout << "No root" << std::endl << std::flush;
        return;
    }
    renderer.drawWorld(root);
}

bool IsoClient::update(float secs)
{
    renderer.update(secs);
    if (inGame) {
        Point3D offset = getAbsCharPos();
        renderer.setXoffset(offset.x());
        renderer.setYoffset(offset.y());
        renderer.setZoffset(offset.z());
    }
    renderer.clear();
    renderer.lightOff();
    // renderer.drawMap(map_database, map_height);
    renderer.drawMap(mterrain);
    renderer.origin();

    doWorld();

    compassWidget->setAngle(-renderer.getRotation());
    renderer.lightOff();
    renderer.drawGui();
    gui->draw();
    renderer.flip();
    return false;
}

bool IsoClient::event(SDL_Event & event)
{
    static int oldx = 0;
    static int oldy = 0;
    static float oldRot = 0;
    static float oldElv = 0;
    static float oldScl = 0;
    switch(event.type) {
        case SDL_MOUSEMOTION:
            if (event.motion.state & SDL_BUTTON(2)) {
                //int w = renderer.getWidth();
                //int h = renderer.getHeight();
                const float x = event.motion.x;
                const float y = event.motion.y;
                float newRot = ((x - oldx) * 360.0f) / renderer.getWidth();
                if (oldy > renderer.getHeight()/2) {
                    newRot = oldRot + newRot;
                } else {
                    newRot = oldRot - newRot;
                }
                while (newRot >= 360.0f) { newRot -= 360.0f; };
                float newElv = ((y - oldy) * 90.0f) / renderer.getHeight();
                newElv = oldElv + newElv;
                if (newElv < 0) { newElv = 0.0f; }
                if (newElv > 90) { newElv = 90.0f; }
                renderer.setRotation(newRot);
                renderer.setElevation(newElv);
                return true;
            } else if (event.motion.state & SDL_BUTTON(3)) {
                const float y = event.motion.y;
                const float h = renderer.getHeight();
                float newScl = ((h + y - oldy) / h) * oldScl;
                renderer.setScale(newScl);
                return true;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if ((event.button.type & SDL_MOUSEBUTTONDOWN) &&
                (event.button.state & SDL_PRESSED)) {
                if ((event.button.button == SDL_BUTTON_MIDDLE) ||
                    (event.button.button == SDL_BUTTON_RIGHT)) {
                    oldx = event.button.x;
                    oldy = event.button.y;
                    oldRot = renderer.getRotation();
                    oldElv = renderer.getElevation();
                    oldScl = renderer.getScale();
                }
                if (event.button.button == SDL_BUTTON_LEFT) {
                    const float x = event.motion.x;
                    const float y = renderer.getHeight() - event.motion.y;
                    renderer.origin();
                    const float z = renderer.getZ(x, y);
                    // Check that the point clicked on is not in the far
                    // distance
                    if (z < 0.9) {
                        moveCharacter(renderer.getWorldCoord(x, y, z));
                    }
                }
                //return false;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_0:
                    renderer.setElevation(30.0);
                    renderer.setRotation(45.0);
                    return true;
                    break;
                case SDLK_UP:
                    renderer.setYoffset(renderer.getYoffset() + 1);
                    return true;
                    break;
                case SDLK_DOWN:
                    renderer.setYoffset(renderer.getYoffset() - 1);
                    return true;
                    break;
                case SDLK_LEFT:
                    renderer.setXoffset(renderer.getXoffset() - 1);
                    return true;
                    break;
                case SDLK_RIGHT:
                    renderer.setXoffset(renderer.getXoffset() + 1);
                    return true;
                    break;
                default:
                    break;
            }
            break;
    }
    return gui->event(event);
}
