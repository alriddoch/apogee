// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_RENDERER_H
#define APOGEE_RENDERER_H

#include "GL.h"

#include "common/Vector3D.h"

#include <sigc++/signal.h>

#include <SDL/SDL.h>

#include <map>

class Cal3dModel;
class Application;
class RenderableEntity;

namespace Eris {
  class Entity;
  class Quaternion;
  class TypeInfo;
}

class Renderer {
  public:
    typedef std::map<GLuint, Eris::Entity *> SelectMap;
  protected:
    Renderer(int wdth, int hght);

    static const float maxViewDistance = 250.0f;

    void shapeView();

    struct SDL_Surface * screen;
    SDL_Rect ** videoModes;
    int width, height;
    bool fullscreen;
    int window_width, window_height;
    float elevation, rotation;
    float scale, x_offset, y_offset, z_offset;
    Eris::Entity * focus;
    float worldTime;
    int frameCount;
    float time;
    float lastCount;
    char * m_windowName;
    char * m_iconName;

  public:
    Cal3dModel * model;

    virtual ~Renderer();

    bool init();

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
    const PosType getWorldCoord(int, int, float);

    void drawEntity(Eris::Entity * ent, RenderableEntity * parent,
                    const PosType & camPos);
    void drawWorld(Eris::Entity * wrld);
    virtual void drawSky() = 0;
    void drawGui();

    void selectEntity(Eris::Entity * ent, RenderableEntity * parent,
                      const PosType & camPos,
                      SelectMap & names, GLuint & next);
    Eris::Entity * selectWorld(Eris::Entity *, int, int);

    void resize(int,int);
    void clear();
    virtual void projection() = 0;
    virtual void viewPoint() = 0;
    void origin();
    void orient(const WFMath::Quaternion &);
    void fogOn();
    void fogOff();
    void lightOn();
    void lightOff();
    void update(float);
    void toggleFullscreen();
    void cleanUp();

    virtual const float meterSize() const = 0;

    SigC::Signal0<void> Restart;
    SigC::Signal1<void, float> Update;
};

#endif // APOGEE_RENDERER_H
