// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef APOGEE_GUI_PIE_H
#define APOGEE_GUI_PIE_H

#include "Widget.h"

#include "Gui.h"

#include "visual/Sprite.h"

#include <sigc++/signal.h>

#include <vector>

class PieSlice {
  private:
    GLuint m_name;
    bool m_pressed;
    std::string m_id;
  public:
    PieSlice(Gui & g, const std::string & id) : m_name(g.newName()),
                                                m_pressed(false),
                                                m_id(id) { }

    GLuint getName() const {
        return m_name;
    }

    const std::string & getId() const {
        return m_id;
    }

    void press() {
        m_pressed = true;
    }

    void unpress() {
        m_pressed = false;
    }

    bool isPressed() const {
        return m_pressed;
    }
};

class Pie : public Widget {
  private:
    std::vector<PieSlice> m_slices;

  protected:
    static Sprite m_sliceGraphic;
    static Sprite m_slicePressedGraphic;

  public:
    SigC::Signal1<void, const std::string &> SliceSelected;

    Pie(Gui & g, int x, int y) : Widget(g, x, y) { }

    virtual ~Pie();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual bool key(int, int);

    void addSlice(const std::string & name, const std::string & graphic);
};

#endif // APOGEE_GUI_PIE_H
