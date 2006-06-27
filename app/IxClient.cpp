// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2003 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "IxClient.h"

#include <visual/DemeterScene.h>

#include <gui/Gui.h>
#include <gui/Console.h>

#include <Eris/View.h>
#include <Eris/Entity.h>
#include <Eris/Avatar.h>

static const Uint32 maxClickTime = 300; // milliseconds

IxClient::IxClient() : GameClient(* new DemeterScene(*this), "perigee"),
                       m_use_mouse(true)
{
}

IxClient::~IxClient()
{
}

bool IxClient::event(SDL_Event & event)
{
    bool eaten = gui->event(event);

    if (eaten) {
        return true;
    }

    switch(event.type) {
        case SDL_MOUSEMOTION:
            if (clickDown == true) {
                // What?
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT && inGame) {
                clickDown = true;
                // FIXME It may be a bad idea to change these if clickDown was
                // already true
                clickDownX = event.motion.x;
                clickDownY = event.motion.y;
                clickDownTime = SDL_GetTicks();
                return true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                bool ret = false;
                if (clickDown) {
                    Uint16 period = SDL_GetTicks() - clickDownTime;
                    clickDown = false;
                    if (period < maxClickTime) {
                        Eris::Entity * we = m_view->getTopLevel();
                        Eris::Entity * e = renderer.selectWorld(we, event.motion.x,
                                                                    event.motion.y);
                        if (e == we) {
                            const int x = event.motion.x;
                            const int y = renderer.getHeight() - event.motion.y;
                            renderer.origin();
                            const float z = renderer.getZ(x, y);
                            bool run = (SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT));
                            // Check that the point clicked on is not in the far
                            // distance
                            if (z < 0.999) {
                                moveCharacter(renderer.getWorldCoord(x, y, z), run);
                            }
                        } else if (e != 0) {
                            m_avatar->touch(e);
                        }
                    } else if (clickDownX == event.motion.x &&
                               clickDownY == event.motion.y) {
                        // Hold left button
                        // Its not clear whether the users will cope with
                        // anything else here.
                    } else {
                        // Drag left button
                        // Drag thing around or into inventory
                    }
                    ret = true;
                }
                if (!m_use_mouse) {
                    int mx = renderer.getWidth() / 2,
                        my = renderer.getHeight() / 2;
                    SDL_WarpMouse(mx, my);
                }
                return ret;
            }
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    m_use_mouse = m_use_mouse ? false : true;
                    if (!m_use_mouse) {
                        int mx = renderer.getWidth() / 2,
                            my = renderer.getHeight() / 2;
                        SDL_WarpMouse(mx, my);
                    }
                    return true;
                    break;
                case SDLK_RETURN:
                    if ((event.key.keysym.mod & KMOD_LALT) ||
                        (event.key.keysym.mod & KMOD_LALT)) {
                        renderer.toggleFullscreen();
                    }
                    if (event.key.keysym.mod == 0) {
                        consoleWidget->takeFocus();
                    }
                    break;
                default:
                    break;
            }
            break;
    }
    return false;
}

void IxClient::mouse(int x, int y, Uint8 buttons)
{
    if (m_use_mouse) { return; }
    if (buttons & SDL_BUTTON(1)) { return; }
    int mx = renderer.getWidth() / 2,
        my = renderer.getHeight() / 2;
    int dx = x - mx,
        dy = y - my;
    if ((dx == 0) && (dy == 0)) { return; }
    
    float newRot = renderer.getRotation() + (dx / 4.f);
    while (newRot >= 360) { newRot -= 360; };
    renderer.setRotation(newRot);

    float newElv = renderer.getElevation() + (dy / 4.f);
    if (newElv < -90) { newElv = -90; }
    if (newElv > 90) { newElv = 90; }
    renderer.setElevation(newElv);

    SDL_WarpMouse(mx, my);
    return;
}
