// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef APOGEE_GUI_PIE_H
#define APOGEE_GUI_PIE_H

#include "Widget.h"

#include "visual/Sprite.h"

#include <GL/gl.h>
#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

#include <string>
#include <vector>

class PieSlice {
  public:
    PieSlice() { }
};

class Pie : public Widget {
  private:
    std::vector<PieSlice> m_slices;

  protected:
    static Sprite m_sliceGraphic;

  public:
    SigC::Signal1<void, const std::string &> SliceSelected;

    Pie(Gui & g, int x, int y) : Widget(g, x, y) { }

    virtual ~Pie();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual void key(int, int);

    void addSlice(const std::string & name, const std::string & graphic);
};

#endif // APOGEE_GUI_PIE_H
