#ifndef APOGEE_APPLICATION_H
#define APOGEE_APPLICATION_H

#include <SDL.h>

class Renderer;

class Application {
  protected:
    Renderer * renderer;

    Application(Renderer * rend) : renderer(rend) { }
    //virtual ~Application() = 0;
  public:

    virtual bool setup() { return 0; };
    virtual int update() = 0;
    virtual bool event(SDL_Event &) = 0;
};

#endif // APOGEE_APPLICATION_H
