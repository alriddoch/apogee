// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_OPTION_H
#define APOGEE_GUI_OPTION_H

#include "Button.h"

#include <GL/gl.h>
#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

class Option : public Widget {
  private:
    GLuint buttonOneName, buttonTwoName;
    Button buttonOne;
    Button buttonTwo;
    std::string question;

  protected:
    static Sprite background;

  public:
    SigC::Signal0<void> buttonOneSignal;
    SigC::Signal0<void> buttonTwoSignal;

    Option(Gui & g, int x, int y,
           const std::string & b1,
           const std::string & b2) : Widget(g, x, y),
                                     buttonOne(g, x, y, b1),
                                     buttonTwo(g, x, y, b2) { }
    virtual ~Option();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual bool key(int,int);

    void setQuestion(const std::string & q) {
        question = q;
    }
};

#endif // APOGEE_GUI_OPTION_H
