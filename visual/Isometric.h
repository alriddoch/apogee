// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_ISOMETRIC_H
#define APOGEE_ISOMETRIC_H

#include "Renderer.h"

class Isometric : public Renderer {
  public:
    explicit Isometric(Application & app, int wdth = 640, int hght = 480);

    void drawSky();
    void projection();
    void viewPoint();

    const float meterSize() const { return 40.2492f; }
};

#endif // APOGEE_ISOMETRIC_H
