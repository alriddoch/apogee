#ifndef APOGEE_CLIENT_H
#define APOGEE_CLIENT_H

#include <SDL.h>

#include "Application.h"

class Client: public Application {
  public:
    Client(Renderer * rend) : Application(rend) { }
    //virtual ~Client() { }

    int update();
    bool event(SDL_Event &);
};

#endif // APOGEE_CLIENT_H
