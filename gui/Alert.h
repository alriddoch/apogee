// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_GUI_ALERT_H
#define APOGEE_GUI_ALERT_H

#include "Item.h"
#include <GL/gl.h>

class Alert : public Item {
  private:
    std::string m_message;
    GLuint messageName;

  public:
    Alert(Gui & g, int x, int y, const std::string & message) : Item(g, x, y),
                                           m_message(message) { }

    virtual void setup();
    virtual void draw();
    virtual void select();
};

#endif // APOGEE_GUI_ALERT_H
