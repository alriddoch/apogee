// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_ISOMETRIC_H
#define APOGEE_ISOMETRIC_H

#include "Renderer.h"
#include <lib3ds/file.h>

class TileMap;

namespace Eris {
  class TypeInfo;
}

namespace Coal {
  class Container;
}

namespace Mercator {
  class Segment;
}

class Isometric : public Renderer {
  private:
    int frameCount;
    float time;
    float lastCount;

    virtual void shapeView();
  public:
    explicit Isometric(Application & app, int wdth = 640, int hght = 480);

    // void draw3DArea(const Point3D & coords,
                    // const Vector3D & bbox = Vector3D(),
                    // const Vector3D & bmedian = Vector3D());
    void viewPoint();
    void reorient();
    void orient();
    void translate();

    void update(float);

    const float meterSize() const { return 40.2492f; }
};

#endif // APOGEE_ISOMETRIC_H
