// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_ISOMETRIC_H
#define APOGEE_ISOMETRIC_H

#include "Renderer.h"

class TileMap;

class Isometric : public Renderer {
  private:
    virtual void shapeView();
  public:
    explicit Isometric(Application & app, int wdth = 640, int hght = 480);

    void viewPoint();
    void reorient();
    void orient();
    void translate();

    const float meterSize() const { return 40.2492f; }
};

#endif // APOGEE_ISOMETRIC_H
