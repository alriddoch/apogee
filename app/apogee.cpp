// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "IsoClient.h"

#include "common/configuration.h"

#include <visual/Renderer.h>

#include <Eris/Connection.h>
#include <Eris/PollDefault.h>
#include <Eris/Exceptions.h>

#include <iostream>

#define MIN_WIDTH	100
#define MIN_HEIGHT	100

int main(int argc, char ** argv)
{
    loadConfig(argc, argv);

    IsoClient * app = new IsoClient();

    Eris::setLogLevel(Eris::LOG_DEBUG);

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
        Uint8 buttons = SDL_GetMouseState(&dx, &dy);
        app->mouse(dx, dy, buttons);

        try {
            Eris::PollDefault::poll();
        } catch (Eris::BaseException b) {
            std::cout << "EXCEPTION: " << b._msg << std::endl << std::flush;
        } catch (Atlas::Objects::NoSuchAttrException n) {
            std::cout << "ATLAS ATTRIBUTE EXCEPTION: " << n.getName()
                      << std::endl << std::flush;
        } catch (Atlas::Exception n) {
            std::cout << "ATLAS EXCEPTION: " << n.getDescription()
                      << std::endl << std::flush;
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

    return 0;
}
