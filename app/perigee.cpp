#include <iostream>

#include <unistd.h>

#include <visual/DemeterScene.h>
// #include <net/Connection.h>
#include "Editor.h"
//#include "Client.h"

// #include <world/World.h>

#include <Atlas/Objects/Entity/GameEntity.h>

#include <Eris/Connection.h>
#include <Eris/Player.h>
#include <Eris/Lobby.h>

#include <sigc++/object_slot.h>

#define MIN_WIDTH	100
#define MIN_HEIGHT	100

using Atlas::Message::Object;
using Atlas::Objects::Entity::GameEntity;

int main(int argc, char ** argv)
{
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " <host> <username>"
                  << std::endl << std::flush;
        return 1;
    }

    std::string host(argv[1]), user(argv[2]), password;

    Eris::Connection & con = * new Eris::Connection("perigee");

    Renderer * renderer = DemeterScene::Instance();
    Editor * app = new Editor(*renderer, con);

    con.connect(host, 6767);

    con.Failure.connect(SigC::slot(app, &Editor::netFailure));
    con.Connected.connect(SigC::slot(app, &Editor::netConnected));
    con.Disconnected.connect(SigC::slot(app, &Editor::netDisconnected));

    app->setup();
    app->update();

    bool done = false;
    SDL_Event event;
    int newWidth;
    int newHeight;
    bool updated;

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
        if (updated) {
            app->update();
        }
        usleep(10000L);
    }
    std::cout << "Quitting" << std::endl << std::flush;
    delete app;
    Renderer::Shutdown();
}