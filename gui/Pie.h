// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef APOGEE_GUI_PIE_H
#define APOGEE_GUI_PIE_H

#include "PieSlice.h"

class Pie : public Widget {
  private:
    std::set<PieSlice> m_slices;

  protected:

  public:
    SigC::Signal<void, const std::string &> SliceSelected;

    Pie(Gui & g, int x, int y) : Widget(g, x. y) { }

    virtual ~Pie();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual void key(int, int);

    void addSlice(const std::string & name, const std::string & graphic);
};
