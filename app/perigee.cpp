// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include <iostream>

#include <unistd.h>

#include <visual/DemeterScene.h>
#include "IxClient.h"

#include <Eris/Connection.h>

#include <sigc++/object_slot.h>

#define MIN_WIDTH	100
#define MIN_HEIGHT	100

using Atlas::Message::Object;

int main(int argc, char ** argv)
{
    Eris::Connection & con = * new Eris::Connection("perigee", true);

    Renderer * renderer = DemeterScene::Instance();
    IxClient * app = new IxClient(*renderer, con);

    con.Failure.connect(SigC::slot(app, &IxClient::netFailure));
    con.Connected.connect(SigC::slot(app, &IxClient::netConnected));
    con.Disconnected.connect(SigC::slot(app, &IxClient::netDisconnected));

    app->setup();
    app->update(0);

    bool done = false;
    SDL_Event event;
    int newWidth;
    int newHeight;
    bool updated;
    int elapsed_time = SDL_GetTicks();

    while (!done) {
        updated = false;
        while (SDL_PollEvent(&event) && !done) {
            updated = app->event(event) || updated;
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
                    renderer->resize(newWidth, newHeight);
                    updated = true;
                    break;
                default:
                    break;
            }
        }
        int dx, dy;
        SDL_GetRelativeMouseState(&dx, &dy);
        if ((dx != 0) && (dy != 0)) {
            app->mouse(dx, dy);
            updated = true;
        }

        // updated = con.poll() || updated;
        try {
            con.poll();
        } catch (Eris::BaseException b) {
            cout << "EXCEPTION: " << b._msg << std::endl << std::flush;
        } catch (Atlas::Objects::NoSuchAttrException n) {
            cout << "ATLAS EXCEPTION: " << n.name << std::endl << std::flush;
        } catch (Atlas::Message::WrongTypeException w) {
            cout << "ATLAS MESSAGE EXCEPTION" << std::endl << std::flush;
        } catch (...) {
            cout << "UNKNOWN EXCEPTION" << std::endl << std::flush;
        }
        int ticks = SDL_GetTicks();
        float delta = (ticks - elapsed_time) / 1000.0f;
        app->update(delta);
        elapsed_time = ticks;
    }
    std::cout << "Quitting" << std::endl << std::flush;
    delete app;
    Renderer::Shutdown();
}
