#ifndef APOGEE_APPLICATION_H
#define APOGEE_APPLICATION_H

#include <SDL.h>

class Renderer;
class World;

class Application {
  protected:
    Renderer & renderer;
    World & world;

    Application(Renderer & rend, World & wrld) : renderer(rend), world(wrld) { }
    //virtual ~Application() = 0;
  public:

    virtual bool setup() { return 0; };
    virtual bool update() = 0;
    virtual bool event(SDL_Event &) = 0;
};

#endif // APOGEE_APPLICATION_H
