// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_GUI_H
#define APOGEE_GUI_GUI_H

#include <SDL.h>
#include <GL/gl.h>

class Sprite;
class Renderer;

class Gui {
  private:
    Sprite * bag;
    GLuint selectBuf[512];
    Renderer & renderer;
    // Sprite * belt;
  public:
    Gui(Renderer & r);

    bool setup();
    void draw();
    void select(int, int);
    bool event(SDL_Event &);

};

#endif // APOGEE_GUI_GUI_H
