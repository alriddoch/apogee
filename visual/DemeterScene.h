// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_DEMETER_H
#define APOGEE_DEMETER_H

#include "Renderer.h"
#include "Camera.h"

#include <Demeter/Terrain.h>

namespace Eris {
  class TypeInfo;
}

class TileMap;
class Model;

class DemeterScene : public Renderer {
  private:
    DemeterScene() { throw RendererException(); }
    DemeterScene(int wdth, int hght);

    Demeter::Terrain * terrain;
    Demeter::Settings * settings;

    TileMap * tilemap;
    Eris::TypeInfo * charType;
    float worldTime;

    float z_offset;

    void init();
    virtual void shapeView();
  public:
    static Renderer * Instance(int width = 640, int height = 480) {
        if (instance == NULL) {
            instance = new DemeterScene(width, height);
        }
        return instance;
    }

    void drawCal3DModel(Model *, const Point3D & coords,
                                const WFMath::Quaternion & orientation);
    void draw3DBox(const Point3D & coords,
                           const WFMath::AxisBox<3> & bbox);
    void draw3DArea(const Point3D & coords,
                            const Vector3D & bbox = Vector3D(),
                            const Vector3D & bmedian = Vector3D()) { }
    void drawEntity(Eris::Entity * ent);
    void drawWorld(Eris::Entity * wrld);
    void drawMap(Coal::Container &, HeightMap &);
    void drawGui();
    void resize(int,int);
    void clear();
    void viewScale(float);
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

#endif // APOGEE_DEMETER_H
