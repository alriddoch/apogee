#ifndef APOGEE_EDITOR_H
#define APOGEE_EDITOR_H

#include <SDL.h>

#include <coal/bladeloader.h>

#include "Application.h"

class Sprite;

class Editor: public Application {
  private:
    void grid();
    void compass();
    void axis();

    double grid_size;
    bool m_grid;
    CoalDatabase map_database;
    Sprite * character;
  public:
    Editor(Renderer * rend) : Application(rend) { }
    //virtual ~Editor() { }

    bool setup();
    int update();
    bool event(SDL_Event &);
};

#endif // APOGEE_EDITOR_H
