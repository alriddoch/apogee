// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2003 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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
