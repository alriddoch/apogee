// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_RENDERER_H
#define APOGEE_RENDERER_H

#include <common/Vector3D.h>

class Sprite;
class Model;
class HeightMap;

namespace Coal {
  class Container;
}

extern "C" void SDL_GL_SwapBuffers();
extern "C" void SDL_Quit();

class RendererException { };

class RendererSDLinit : public RendererException { };

namespace Eris {
  class Entity;
  class Quaternion;
}

class Renderer {
  protected:
    Renderer() { throw RendererException(); }
    Renderer(int wdth, int hght);

    virtual void shapeView() = 0;

    static Renderer * instance;

    struct SDL_Surface * screen;
    int width, height;
    float elevation, rotation;
    float scale, x_offset, y_offset, z_offset;
    Eris::Entity * focus;
  public:
    //static Renderer * Instance(int width = 640, int height = 480) {
        //if (instance == NULL) {
            //instance = new Renderer(width, height);
        //}
        //return instance;
    //}

    static void Shutdown() {
        if (instance == NULL) {
            return;
        }
        instance = NULL;
        SDL_Quit();
    }

    void flip() {
        SDL_GL_SwapBuffers();
    }


    int getWidth() { return width; }

    int getHeight() { return height; }

    float setRotation(float rot) { float tmp=rotation; rotation = rot; return tmp; }
    float getRotation() { return rotation; }

    float setElevation(float elv) {float tmp=elevation; elevation = elv; return tmp; }
    float getElevation() { return elevation; }

    float setScale(float scl) {float tmp=scale; scale = scl; return tmp; }
    float getScale() { return scale; }

    float setXoffset(float xoff) {float tmp=x_offset; x_offset = xoff; return tmp; }
    float getXoffset() { return x_offset; }

    float setYoffset(float yoff) {float tmp=y_offset; y_offset = yoff; return tmp; }
    float getYoffset() { return y_offset; }

    float setZoffset(float zoff) {float tmp=z_offset; z_offset = zoff; return tmp; }
    float getZoffset() { return z_offset; }

    float getZ(int, int);
    const Point3D getWorldCoord(int, int, float);

    virtual void drawCal3DModel(Model *, const Point3D & coords,
                                const WFMath::Quaternion &) = 0;
    virtual void draw3DBox(const Point3D & coords,
                           const WFMath::AxisBox<3> & bbox) = 0;
    virtual void drawEntity(Eris::Entity * ent) = 0;
    virtual void drawWorld(Eris::Entity * wrld) = 0;
    virtual void drawMap(Coal::Container &, HeightMap &) = 0;
    virtual void drawGui() = 0;
    virtual void resize(int,int) = 0;
    virtual void clear() = 0;
    virtual void viewPoint() = 0;
    virtual void reorient() = 0;
    virtual void orient() = 0;
    virtual void translate() = 0;
    virtual void origin() = 0;
    virtual void lightOn() = 0;
    virtual void lightOff() = 0;
    virtual void update(float) = 0;

    virtual const float meterSize() const = 0;
};

#endif // APOGEE_RENDERER_H
