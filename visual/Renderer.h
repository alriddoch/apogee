#ifndef APOGEE_RENDERER_H
#define APOGEE_RENDERER_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <coal/database.h>

class Sprite;

class RendererException { };

class RendererSDLinit : public RendererException { };

class Renderer {
  private:
    Renderer() { throw RendererException(); }
    Renderer(int wdth, int hght) : screen(NULL), character(NULL), button(NULL),
                                   elevation(30), rotation(45), scale(1)
                 { init(wdth, hght); }

    void init(int, int);
    void shapeView();

    static Renderer * instance;

    SDL_Surface * screen;
    SDL_Surface * character;
    SDL_Surface * button;
    int width, height;
    int elevation, rotation;
    double scale;
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

    void draw3Dtest();
    void draw2Dtest();
    void draw3Dentity();
    void drawCharacter(Sprite *, double, double);
    void drawMapRegion(CoalRegion &);
    void drawMapObject(CoalObject &);
    void drawMap(CoalDatabase &);
    void resize(int,int);
    void viewPoint();
    void reorient();
    void orient();
    void origin();

    static const double meterSize = 40.2492;
};

#endif // APOGEE_RENDERER_H
