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

class Isometric : public Renderer {
  private:
    TileMap * tilemap;
    Eris::TypeInfo * charType;
    float worldTime;

    Isometric() { throw RendererException(); }
    Isometric(int wdth, int hght);

    void init();
    void shapeView();
    void draw3dsNode(Lib3dsNode * node);
    void draw3dsFile(Lib3dsFile * node);

    void Isometric::buildTileMap(Coal::Container &);
  public:
    Lib3dsFile * treemodel;
    unsigned long treemodel_list;

    static Renderer * Instance(int width = 640, int height = 480) {
        if (instance == NULL) {
            instance = new Isometric(width, height);
        }
        return instance;
    }

    void drawCal3DModel(Model *, const Point3D & coords,
                                const WFMath::Quaternion & orientation);
    void draw3DBox(const Point3D & coords,
                   const WFMath::AxisBox<3> & bbox);
    void draw3DArea(const Point3D & coords,
                    const Vector3D & bbox = Vector3D(),
                    const Vector3D & bmedian = Vector3D());
    void drawEntity(Eris::Entity * ent);
    void drawWorld(Eris::Entity * wrld);
    void drawMap(Coal::Container &, HeightMap &);
    void drawGui();
    void resize(int,int);
    void clear();
    void viewPoint();
    void reorient();
    void orient();
    void translate();
    void origin();
    void lightOn();
    void lightOff();
    void update(float);

    const float meterSize() const { return 40.2492f; }
};

#endif // APOGEE_ISOMETRIC_H
