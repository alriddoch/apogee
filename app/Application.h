#ifndef APOGEE_APPLICATION_H
#define APOGEE_APPLICATION_H

#include <sigc++/object.h>
#include <SDL.h>
#include <string>

class Renderer;
class World;
class Gui;

namespace Eris {
 class Connection;
}

class Application : public SigC::Object {
  protected:
    Renderer & renderer;
    Eris::Connection & connection;
    Gui * gui;

    Application(Renderer & rend, Eris::Connection & con) :
                renderer(rend), connection(con), gui(NULL) { }
    //virtual ~Application() = 0;
  public:

    virtual bool setup() { return 0; };
    virtual bool update() = 0;
    virtual bool event(SDL_Event &) = 0;
    virtual void mouse(int,int) = 0;

    void connect(const std::string & host, const std::string & port);
};

#endif // APOGEE_APPLICATION_H
