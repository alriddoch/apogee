// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IsoClient.h"

#include <visual/Isometric.h>

#include <gui/Gui.h>

#include <Eris/World.h>
#include <Eris/Entity.h>
#include <Eris/Avatar.h>

IsoClient::IsoClient(Eris::Connection & con) : GameClient(*new Isometric(*this), con)
{
}

IsoClient::~IsoClient()
{
}

bool IsoClient::event(SDL_Event & event)
{
    static int oldx = 0;
    static int oldy = 0;
    static float oldRot = 0;
    static float oldElv = 0;
    static float oldScl = 0;

    bool eaten = gui->event(event);
    
    if (eaten) {
        return true;
    }
    
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
                if (event.button.button == SDL_BUTTON_LEFT && inGame) {
                    Eris::Entity * we = m_world->getRootEntity();
                    Eris::Entity * e = renderer.selectWorld(we, event.motion.x,
                                                                event.motion.y);
                    if (e == we) {
                        const int x = event.motion.x;
                        const int y = renderer.getHeight() - event.motion.y;
                        renderer.origin();
                        const float z = renderer.getZ(x, y);
                        // Check that the point clicked on is not in the far
                        // distance
                        if (z < 0.9) {
                            moveCharacter(renderer.getWorldCoord(x, y, z));
                        }
                    } else {
                        m_avatar->touch(e);
                    }
                }
                return true;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_0:
                    renderer.setElevation(30.0);
                    renderer.setRotation(45.0);
                    return true;
                    break;
                case SDLK_RETURN:
                    if ((event.key.keysym.mod & KMOD_LALT) ||
                        (event.key.keysym.mod & KMOD_LALT)) {
                        renderer.toggleFullscreen();
                    }
                    break;
                default:
                    break;
            }
            break;
    }
    return false;
}
