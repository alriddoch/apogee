#ifndef APOGEE_APPLICATION_H
#define APOGEE_APPLICATION_H

#include <sigc++/object.h>
#include <SDL.h>

class Renderer;
class World;
class Gui;

class Application : public SigC::Object {
  protected:
    Renderer & renderer;
    Gui * gui;

    Application(Renderer & rend) : renderer(rend), gui(NULL) { }
    //virtual ~Application() = 0;
  public:

    virtual bool setup() { return 0; };
    virtual bool update() = 0;
    virtual bool event(SDL_Event &) = 0;
    virtual void mouse(int,int) = 0;
};

#endif // APOGEE_APPLICATION_H
