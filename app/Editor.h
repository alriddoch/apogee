#ifndef APOGEE_EDITOR_H
#define APOGEE_EDITOR_H

#include <SDL.h>

#include <coal/bladeloader.h>

#include "Application.h"

class Sprite;
class Model;

class Editor: public Application {
  private:
    void grid();
    void doWorld();
    void compass();
    void axis();

    CoalDatabase map_database;
    Sprite * character;
    Model * model;
  public:
    Editor(Renderer & rend, World & wrld) : Application(rend, wrld),
                                            character(NULL) { }
    virtual ~Editor() { }

    bool setup();
    bool update();
    bool event(SDL_Event &);
};

#endif // APOGEE_EDITOR_H
