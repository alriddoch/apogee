// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_ISOMETRIC_H
#define APOGEE_ISOMETRIC_H

#include "Renderer.h"

namespace Eris {
  class TypeInfo;
}

class Isometric : public Renderer {
  private:
    Model * model;
    Eris::TypeInfo * charType;
    float worldTime;

    Isometric() { throw RendererException(); }
    Isometric(int wdth, int hght);

    void init();
    virtual void shapeView();
  public:
    static Renderer * Instance(int width = 640, int height = 480) {
        if (instance == NULL) {
            instance = new Isometric(width, height);
        }
        return instance;
    }

    virtual void draw3Dentity();
    virtual void drawCal3DModel(Model *, const Vector3D & coords);
    virtual void draw3DBox(const Vector3D & coords,
                           const Eris::BBox & bbox);
    virtual void draw3DArea(const Vector3D & coords,
                            const Vector3D & bbox = Vector3D(),
                            const Vector3D & bmedian = Vector3D());
    virtual void drawEntity(Eris::Entity * ent);
    virtual void drawWorld(Eris::Entity * wrld);
    virtual void drawCharacter(Sprite *, float, float);
    virtual void drawMapRegion(CoalRegion &);
    virtual void drawMapObject(CoalObject &);
    virtual void drawMap(CoalDatabase &);
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

    virtual const float meterSize() const { return 40.2492f; }
};

#endif // APOGEE_ISOMETRIC_H
