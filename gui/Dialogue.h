// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_DIALOGUE_H
#define APOGEE_GUI_DIALOGUE_H

#include "Button.h"

#include "TextField.h"
#include "visual/GL.h"

#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

class Dialogue : public Widget {
  private:
    GLuint oButtonName, cButtonName, field1Name, field2Name;
    Button oButton;
    Button cButton;
    TextField field1;
    TextField field2;
    std::string field1Tag, field2Tag;
    int fieldCount;
    int focus;

  protected:
    static Sprite background;

  public:
    SigC::Signal2<void, const std::string &, const std::string &> oButtonSignal;
    SigC::Signal0<void> cButtonSignal;

    Dialogue(Gui & g, int x, int y) : Widget(g, x, y),
                                      oButton(g, x, y, "OK"),
                                      cButton(g, x, y, "Cancel"),
                                      field1(g, x, y),
                                      field2(g, x, y),
                                      fieldCount(0), focus(-1) { }
    virtual ~Dialogue();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual bool key(int,int);

    void addField(const std::string & name, const std::string & contents);
};

#endif // APOGEE_GUI_DIALOGUE_H
