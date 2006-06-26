// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_APPLICATION_H
#define APOGEE_APPLICATION_H

#include <sigc++/trackable.h>
#include <SDL/SDL.h>
#include <string>

class Renderer;
class World;
class Gui;

namespace Eris {
 class Connection;
}

class Application : public sigc::trackable {
  protected:
    Gui * gui;

    Application(Renderer & rend, const std::string & name) :
                gui(0), renderer(rend), connection(0) { }
    virtual ~Application();
  public:
    Renderer & renderer;
    Eris::Connection * connection;
    std::string appName;

    virtual bool setup() = 0;
    virtual bool update(float) = 0;
    virtual bool event(SDL_Event &) = 0;
    virtual void mouse(int, int, Uint8) = 0;
};

#endif // APOGEE_APPLICATION_H
