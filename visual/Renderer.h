// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_RENDERER_H
#define APOGEE_RENDERER_H

#include <common/Vector3D.h>
#include <lib3ds/file.h>

class Sprite;
class Model;
class HeightMap;
class Application;

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
  class TypeInfo;
}

namespace Mercator {
  class Terrain;
  class Segment;
}

class Renderer {
  protected:
    Renderer(Application & app, int wdth, int hght);

    static const int segSize = 64;

    virtual void shapeView() = 0;

    void init();
    void draw3dsFile(Lib3dsFile * node);

    struct SDL_Surface * screen;
    int width, height;
    float elevation, rotation;
    float scale, x_offset, y_offset, z_offset;
    Eris::Entity * focus;
    int m_numLineIndeces;
    unsigned int * const m_lineIndeces;
    float * const m_texCoords;
    Lib3dsFile * treemodel;
    unsigned long treemodel_list;
    Eris::TypeInfo * charType;
    float worldTime;

  public:
    Application & application;
    Model * model;

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

    void drawCal3DModel(Model *, const Point3D & coords,
                                const WFMath::Quaternion &);
    void draw3DBox(const Point3D & coords,
                           const WFMath::AxisBox<3> & bbox);
    void drawEntity(Eris::Entity * ent);
    void drawWorld(Eris::Entity * wrld);
    // virtual void drawMap(Coal::Container &, HeightMap &) = 0;
    void drawRegion(Mercator::Segment *);
    void drawMap(Mercator::Terrain &);
    void drawGui();
    void resize(int,int);
    void clear();
    virtual void viewPoint() = 0;
    virtual void reorient() = 0;
    virtual void orient() = 0;
    virtual void translate() = 0;
    virtual void origin();
    void lightOn();
    void lightOff();
    virtual void update(float) = 0;

    virtual const float meterSize() const = 0;
};

#endif // APOGEE_RENDERER_H
