// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_ISOCLIENT_H
#define APOGEE_ISOCLIENT_H

#include <coal/bladeloader.h>

#include <string>

#include "GameClient.h"

class Sprite;
class Model;

class IsoClient : public GameClient {
  private:
    void grid();
    void doWorld();
    void compass();
    void axis();

    CoalDatabase map_database;
    Sprite * character;
    Model * model;
  public:
    IsoClient(Renderer & rend, Eris::Connection & con) : GameClient(rend, con),
                                                      character(NULL) { }
    virtual ~IsoClient() { }

    bool setup();
    bool update();
    bool event(SDL_Event &);
    void mouse(int,int) { }
};

#endif // APOGEE_ISOCLIENT_H
