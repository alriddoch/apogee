// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IxClient.h"

#include <visual/DemeterScene.h>
#include <visual/Model.h>

#include <gui/Gui.h>
#include <gui/Dialogue.h>
#include <gui/Compass.h>

#include <Eris/World.h>
#include <Eris/Entity.h>

#include <coal/isoloader.h>

#include <sigc++/object_slot.h>

using Atlas::Message::Object;

IxClient::IxClient(Eris::Connection & con)
                   : GameClient(* new DemeterScene(*this), con),
                     m_use_mouse(false)
{
}

IxClient::~IxClient()
{
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
                (event.button.state & SDL_PRESSED)) {
                if ((event.button.button == SDL_BUTTON_MIDDLE) ||
                    (event.button.button == SDL_BUTTON_RIGHT)) {
                    oldy = event.button.y;
                    oldScl = renderer.getScale();
                }
                if (event.button.button == SDL_BUTTON_LEFT) {
                    const int x = event.motion.x;
                    const int y = renderer.getHeight() - event.motion.y;
                    renderer.origin();
                    const float z = renderer.getZ(x, y);
                    // Check that the point clicked on is not in the far
                    // distance
                    std::cout << "LEFT CLICK" << x << ":" << y << " "
                              << z << std::endl << std::flush;
                    if (z < 0.999) {
                        std::cout << "Lets move" << std::endl << std::flush;
                        moveCharacter(renderer.getWorldCoord(x, y, z));
                    }
                }
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
                case SDLK_q:
                    m_use_mouse = m_use_mouse ? false : true;
                    return true;
                    break;
                default:
                    break;
            }
            break;
    }
    return gui->event(event);
}

void IxClient::mouse(int dx, int dy)
{
    if (m_use_mouse) { return; }
    float newRot = renderer.getRotation() + dx;
    while (newRot >= 360) { newRot -= 360; };
    renderer.setRotation(newRot);

    float newElv = renderer.getElevation() + dy;
    if (newElv < -90) { newElv = -90; }
    if (newElv > 90) { newElv = 90; }
    renderer.setElevation(newElv);
}
