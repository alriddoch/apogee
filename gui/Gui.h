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

#ifndef APOGEE_GUI_GUI_H
#define APOGEE_GUI_GUI_H

#include "visual/GL.h"

#include <sigc++/trackable.h>

#include <SDL/SDL.h>

#include <map>
#include <list>
#include <string>

class Sprite;
class Renderer;
class Widget;

typedef std::map<int, Widget *> widgmap;
typedef std::list<GLuint> hitlist;

class Gui : public sigc::trackable {
  public:
    typedef enum { SELECT_MISS, SELECT_CLICK, SELECT_DRAG } select_t;

    Renderer & renderer;
  private:
    GLuint selectBuf[512];
    GLuint textBase;
    GLuint textTexture;
    int nameCount;
    GLint inMotion;
    int mx,my;

    widgmap widgets;
    hitlist hitNames;
    int focus;
    std::string skinName;

    void initFont();
  public:
    Gui(Renderer & r);

    int newName() { return ++nameCount; }
    const hitlist & getHits() { return hitNames; }
    const std::string & getSkinName() const { return skinName; }
    int getFocus() const { return focus; }

    bool setup();
    void checkSetup();
    void draw();
    GLint select(int, int);
    bool event(SDL_Event &);
    void print(const char *);
    void printColour(const char *);
    void addWidget(Widget *);
    void clearFocus();
    void takeFocus(const Widget &);

    static int keyToAscii(int key, int mod);
};

#endif // APOGEE_GUI_GUI_H
