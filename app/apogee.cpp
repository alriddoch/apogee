#include <iostream>

#include <unistd.h>

#include <visual/Renderer.h>
#include <net/Connection.h>
#include "Editor.h"
//#include "Client.h"

#include <world/World.h>

#define MIN_WIDTH	100
#define MIN_HEIGHT	100

int main(int argc, char ** argv)
{
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " <host> <username>"
                  << std::endl << std::flush;
        return 1;
    }

    World world;

    Connection con(world);

    std::string host(argv[1]), user(argv[2]), password;

    con.connect(host);

    std::cout << "Password: ";
    std::cin >> password;

    con.login(user, password);
    con.create_char("Al", "farmer");
    con.look();

    Renderer * renderer = Renderer::Instance();
    Application * app = new Editor(*renderer, world);
    //Application * app = new Client(renderer);

    app->setup();
    app->update();

    bool done = false;
    SDL_Event event;
    int newWidth;
    int newHeight;

    while (!done) {
        bool updated;
        while (SDL_PollEvent(&event) && !done) {
            updated = app->event(event);
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
        updated = con.poll() || updated;
        if (updated) {
            app->update();
        }
        usleep(10000L);
    }
    delete app;
    Renderer::Shutdown();
}
