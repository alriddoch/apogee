// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_GUI_H
#define APOGEE_GUI_GUI_H

#include <SDL.h>
#include <GL/gl.h>
#include <map>

class Sprite;
class Renderer;
class Widget;

typedef std::map<int, Widget *> widgmap;

class Gui {
  private:
    GLuint selectBuf[512];
    Renderer & renderer;
    int nameCount;

    widgmap widgets;

    int newName() { return ++nameCount; }
  public:
    Gui(Renderer & r);

    bool setup();
    void draw();
    void select(int, int);
    bool event(SDL_Event &);

};

#endif // APOGEE_GUI_GUI_H
