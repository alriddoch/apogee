#ifndef APOGEE_CLIENT_H
#define APOGEE_CLIENT_H

#include <SDL.h>

#include <coal/isoloader.h>

#include "Application.h"

class Sprite;

class Client: public Application {
  private:
    CoalDatabase map_database;
    Sprite * character;
  public:
    Client(Renderer * rend) : Application(rend) { }
    //virtual ~Client() { }

    bool setup();
    int update();
    bool event(SDL_Event &);
};

#endif // APOGEE_CLIENT_H
