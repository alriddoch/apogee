#ifndef APOGEE_RENDERER_H
#define APOGEE_RENDERER_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <coal/database.h>

#include <common/Vector3D.h>

class Sprite;
class Model;

class RendererException { };

class RendererSDLinit : public RendererException { };

class Renderer {
  private:
    Renderer() { throw RendererException(); }
    Renderer(int wdth, int hght);

    void init();
    void shapeView();

    static Renderer * instance;

    SDL_Surface * screen;
    SDL_Surface * character;
    SDL_Surface * button;
    int width, height;
    int elevation, rotation;
    double scale, x_offset, y_offset;
  public:
    static Renderer * Instance(int width = 640, int height = 480) {
        if (instance == NULL) {
            instance = new Renderer(width, height);
        }
        return instance;
    }

    static void Shutdown() {
        if (instance == NULL) {
            return;
        }
        SDL_Quit();
    }

    void flip() {
        SDL_GL_SwapBuffers();
    }

    void clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen
    }

    int getWidth() { return width; }

    int getHeight() { return height; }

    int setRotation(int rot) { int tmp=rotation; rotation = rot; return tmp; }
    int getRotation() { return rotation; }

    int setElevation(int elv) {int tmp=elevation; elevation = elv; return tmp; }
    int getElevation() { return elevation; }

    double setScale(double scl) {double tmp=scale; scale = scl; return tmp; }
    double getScale() { return scale; }

    double setXoffset(double xoff) {double tmp=x_offset; x_offset = xoff; return tmp; }
    double getXoffset() { return x_offset; }

    double setYoffset(double yoff) {double tmp=y_offset; y_offset = yoff; return tmp; }
    double getYoffset() { return y_offset; }

    void draw3Dtest();
    void draw2Dtest();
    void draw3Dentity();
    void drawCal3DModel(Model *);
    void draw3DBox(const Vector3D & coords,
                   const Vector3D & bbox = Vector3D(),
                   const Vector3D & bmedian = Vector3D());
    void drawCharacter(Sprite *, double, double);
    void drawMapRegion(CoalRegion &);
    void drawMapObject(CoalObject &);
    void drawMap(CoalDatabase &);
    void resize(int,int);
    void viewScale(double);
    void viewPoint();
    void reorient();
    void orient();
    void translate();
    void origin();
    void lightOn();
    void lightOff();

    static const double meterSize = 40.2492;
};

#endif // APOGEE_RENDERER_H
