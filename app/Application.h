// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_APPLICATION_H
#define APOGEE_APPLICATION_H

#include <sigc++/object.h>
#include <SDL/SDL.h>
#include <string>

class Renderer;
class World;
class Gui;

namespace Eris {
 class Connection;
}

class Application : public SigC::Object {
  protected:
    Gui * gui;

    Application(Renderer & rend, Eris::Connection & con) :
                gui(0), renderer(rend), connection(con) { }
    virtual ~Application();
  public:
    Renderer & renderer;
    Eris::Connection & connection;

    virtual bool setup() = 0;
    virtual bool update(float) = 0;
    virtual bool event(SDL_Event &) = 0;
    virtual void mouse(int, int, Uint8) = 0;

    void connect(const std::string & host, const std::string & port);
};

#endif // APOGEE_APPLICATION_H
