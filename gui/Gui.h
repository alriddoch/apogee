// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_GUI_GUI_H
#define APOGEE_GUI_GUI_H

#include "visual/GL.h"

#include <sigc++/object.h>

#include <SDL/SDL.h>

#include <map>
#include <list>
#include <string>

class Sprite;
class Renderer;
class Widget;

typedef std::map<int, Widget *> widgmap;
typedef std::list<GLuint> hitlist;

class Gui : public SigC::Object {
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
