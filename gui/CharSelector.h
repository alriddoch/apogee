// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_CHAR_SELECTOR_H
#define APOGEE_GUI_CHAR_SELECTOR_H

#include "Button.h"

#include "visual/GL.h"

#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

#include <map>
#include <set>
#include <string>

class CharSelector : public Widget {
  private:
    GLuint selectName;
    GLuint createName;
    std::map <GLuint, std::pair<std::string, std::string> > charNames;
    std::string selectedChar;
    Button selectb;
    Button create;

  public:
    SigC::Signal1<void, const std::string &> selectSignal;
    SigC::Signal0<void> createSignal;

    CharSelector(Gui & g, int x, int y) : Widget(g, x, y),
                                          selectb(g, x, y, "select"),
                                          create(g, x, y, "create") { }
    virtual ~CharSelector();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual bool key(int,int);

    void addCharacters(const std::set<std::pair<std::string, std::string> > &);
};

#endif // APOGEE_GUI_CHAR_SELECTOR_H
