// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IxClient.h"

#include <visual/Renderer.h>
#include <visual/Model.h>
#include <visual/HeightMap.h>

#include <gui/Gui.h>
#include <gui/Dialogue.h>

#include <Eris/World.h>
#include <Eris/Entity.h>

#include <sigc++/object_slot.h>

using Atlas::Message::Object;

bool IxClient::setup()
{
    model = new Model();
    if (!model->onInit(Datapath() + "paladin.cfg")) {
        std::cerr << "Loading paladin model failed" << std::endl << std::flush;
    }

    gui = new Gui(renderer);
    gui->setup();

    Dialogue * d = new Dialogue(*gui,renderer.getWidth()/2,renderer.getHeight()/2);
    d->addField("host", "localhost");
    d->addField("port", "6767");
    d->oButtonSignal.connect(SigC::slot(this, &Application::connect));
    gui->addWidget(d);
    
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
                  << ent->getBBox().v << std::endl << std::flush;
        renderer.draw3DBox(Vector3D(ent->getPosition()), ent->getBBox());
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
    HeightMap foo;
    renderer.clear();
    renderer.lightOn();
    renderer.drawMap(map_database, foo);
    renderer.origin();
    model->onUpdate(0.1);
    renderer.drawCal3DModel(model, Vector3D(0,0,0), Eris::Quaternion());
    doWorld();
    renderer.lightOff();
    renderer.drawGui();
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
                // return false;
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
    return gui->event(event);
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
