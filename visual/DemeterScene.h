// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_DEMETER_H
#define APOGEE_DEMETER_H

#include "Renderer.h"
#include "Camera.h"

#include <Demeter/Terrain.h>

class TileMap;

class DemeterScene : public Renderer {
  private:
    DemeterScene() { throw RendererException(); }
    DemeterScene(int wdth, int hght);

    Demeter::Terrain * terrain;
    Demeter::Settings * settings;
    Demeter::Vector cameraPosition;
    Demeter::Vector cameraAngle;
    Demeter::Vector lookAt;
    Demeter::Vector lookUp;

    Camera camera;
    
    float z_offset;

    TileMap * tilemap;

    void init();
    virtual void shapeView();
  public:
    static Renderer * Instance(int width = 640, int height = 480) {
        if (instance == NULL) {
            instance = new DemeterScene(width, height);
        }
        return instance;
    }

    virtual void draw3Dentity();
    virtual void drawCal3DModel(Model *, const Point3D & coords,
                                const WFMath::Quaternion & orientation);
    virtual void draw3DBox(const Point3D & coords,
                           const WFMath::AxisBox<3> & bbox);
    virtual void draw3DArea(const Point3D & coords,
                            const Vector3D & bbox = Vector3D(),
                            const Vector3D & bmedian = Vector3D()) { }
    virtual void drawEntity(Eris::Entity * ent) { }
    virtual void drawWorld(Eris::Entity * wrld) { }
    virtual void drawCharacter(Sprite *, float, float);
    virtual void drawMap(CoalDatabase &, HeightMap &);
    virtual void drawGui();
    virtual void resize(int,int);
    virtual void clear();
    virtual void viewScale(float);
    virtual void viewPoint();
    virtual void reorient();
    virtual void orient();
    virtual void translate();
    virtual void origin();
    virtual void lightOn();
    virtual void lightOff();
    virtual void update(float);

    virtual const float meterSize() const { return 40.2492f; }
};

#endif // APOGEE_DEMETER_H
