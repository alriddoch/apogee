// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IxClient.h"

#include <visual/Renderer.h>

#include <Eris/Connection.h>
#include <Eris/PollDefault.h>

#include <sigc++/object_slot.h>

#include <iostream>

#include <unistd.h>

#define MIN_WIDTH	100
#define MIN_HEIGHT	100

using Atlas::Message::Element;

int main(int argc, char ** argv)
{
    Eris::Connection & con = * new Eris::Connection("perigee", true);

    IxClient * app = new IxClient(con);

    if (!app->setup()) {
        std::cerr << "Couldn't open display" << std::endl << std::flush;
        return 1;
    }

    app->update(0);

    bool done = false;
    SDL_Event event;
    int newWidth;
    int newHeight;
    int elapsed_time = SDL_GetTicks();
    int mx = app->renderer.getWidth() / 2,
        my = app->renderer.getHeight() / 2;

    while (!done) {
        while (SDL_PollEvent(&event) && !done) {
            if (app->event(event)) {
                // Event has been eaten
                continue;
            }
            switch (event.type) {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_KEYDOWN:
                    if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                        done = true;
                    }
                    break;
                case SDL_VIDEORESIZE:
                    newWidth = event.resize.w;
                    newHeight = event.resize.h;
                    if (newWidth < MIN_WIDTH) {
                        newWidth = MIN_WIDTH;
                    }
                    if (newHeight < MIN_HEIGHT) {
                        newHeight = MIN_HEIGHT;
                    }
                    app->renderer.resize(newWidth, newHeight);
                    break;
                default:
                    break;
            }
        }
        int dx, dy;
        SDL_GetMouseState(&dx, &dy);
        dx -= mx; dy -= my;
        if ((dx != 0) || (dy != 0)) {
            if (app->mouse(dx, dy)) {
                mx = app->renderer.getWidth() / 2;
                my = app->renderer.getHeight() / 2;
                SDL_WarpMouse(mx, my);
            }
        }

        try {
            Eris::PollDefault::poll();
        } catch (Eris::BaseException b) {
            std::cout << "EXCEPTION: " << b._msg << std::endl << std::flush;
        } catch (Atlas::Objects::NoSuchAttrException n) {
            std::cout << "ATLAS EXCEPTION: " << n.name << std::endl << std::flush;
        } catch (Atlas::Message::WrongTypeException w) {
            std::cout << "ATLAS MESSAGE EXCEPTION" << std::endl << std::flush;
        } catch (...) {
            std::cout << "UNKNOWN EXCEPTION" << std::endl << std::flush;
        }
        int ticks = SDL_GetTicks();
        float delta = (ticks - elapsed_time) / 1000.0f;
        app->update(delta);
        elapsed_time = ticks;
    }
    std::cout << "Quitting" << std::endl << std::flush;
    delete app;
}
