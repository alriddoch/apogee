#ifndef APOGEE_DEMETER_H
#define APOGEE_DEMETER_H

#include "Renderer.h"
#include "Camera.h"

#include <Terrain.h>

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
    virtual void drawCal3DModel(Model *, float x, float y);
    virtual void draw3DBox(const Vector3D & coords,
                           const Vector3D & bbox = Vector3D(),
                           const Vector3D & bmedian = Vector3D());
    virtual void drawCharacter(Sprite *, float, float);
    virtual void drawMapRegion(CoalRegion &);
    virtual void drawMapObject(CoalObject &);
    virtual void drawMap(CoalDatabase &);
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

#endif // APOGEE_DEMETER_H