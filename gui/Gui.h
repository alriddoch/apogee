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
    GLuint textBase;
    GLuint textTexture;
    Renderer & renderer;
    int nameCount;
    GLint inMotion;
    int mx,my;

    widgmap widgets;

    int newName() { return ++nameCount; }
  public:
    Gui(Renderer & r);

    bool setup();
    void draw();
    GLint select(int, int);
    bool event(SDL_Event &);

    void print(const char *);
};

#endif // APOGEE_GUI_GUI_H
