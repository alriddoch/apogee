// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IxClient.h"

#include <visual/Renderer.h>
#include <visual/Model.h>

#include <gui/Gui.h>
#include <gui/Dialogue.h>
#include <gui/Compass.h>

#include <Eris/World.h>
#include <Eris/Entity.h>

#include <coal/isoloader.h>

#include <sigc++/object_slot.h>

using Atlas::Message::Object;

IxClient::IxClient(Renderer & rend, Eris::Connection & con)
                   : GameClient(rend, con),
                     terrain_detail(true), terrain_over(false)
{
}

IxClient::~IxClient()
{
}

bool IxClient::setup()
{
    Coal::IsoLoader loader;
    loader.loadMap("moraf.map", &map_database);

    map_height.load("moraf_hm.png");

    gui = new Gui(renderer);
    gui->setup();

    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,renderer.getHeight()/2);
    d->addField("host", "localhost");
    d->addField("port", "6767");
    d->oButtonSignal.connect(SigC::slot(this, &Application::connect));
    gui->addWidget(d);

    compassWidget = new Compass(*gui, 42, 10);
    gui->addWidget(compassWidget);
    
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
    renderer.drawWorld(root);
}

bool IxClient::update(float secs)
{
    renderer.update(secs);
    if (inGame) {
        Point3D offset = getAbsCharPos();
        renderer.setXoffset(offset.x());
        renderer.setYoffset(offset.y());
        renderer.setZoffset(offset.z());
    }
    renderer.clear();
    // renderer.lightOn();
    renderer.drawMap(map_database, map_height);
    renderer.origin();

    doWorld();

    compassWidget->setAngle(-renderer.getRotation());
    renderer.lightOff();
    renderer.drawGui();
    gui->draw();
    renderer.flip();
    return false;
}

bool IxClient::event(SDL_Event & event)
{
    static int oldy = 0;
    static float oldScl = 0;
    switch(event.type) {
        case SDL_MOUSEMOTION:
            if (event.motion.state & SDL_BUTTON(3)) {
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
                oldy = event.button.y;
                oldScl = renderer.getScale();
                // return false;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_0:
                    renderer.setElevation(0.0);
                    renderer.setRotation(45.0);
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
                case SDLK_q:
                    terrain_detail = terrain_detail ? false : true;
                    break;
                case SDLK_w:
                    terrain_over = terrain_over ? false : true;
                    break;
                default:
                    return false;
                    break;
            }
            return true;
            break;
    }
    return gui->event(event);
}

void IxClient::mouse(int dx, int dy)
{
    float newRot = renderer.getRotation() + dx;
    while (newRot >= 360) { newRot -= 360; };
    renderer.setRotation(newRot);

    float newElv = renderer.getElevation() + dy;
    if (newElv < -90) { newElv = -90; }
    if (newElv > 90) { newElv = 90; }
    renderer.setElevation(newElv);
}
