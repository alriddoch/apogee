// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_IXCLIENT_H
#define APOGEE_IXCLIENT_H

#include <coal/bladeloader.h>

#include <string>

#include "GameClient.h"

class Sprite;
class Model;

class IxClient : public GameClient {
  private:
    void doWorld();

    CoalDatabase map_database;
    Model * model;
  public:
    IxClient(Renderer & rend, Eris::Connection & con) : GameClient(rend, con)
                                                        { }
    virtual ~IxClient() { }

    bool setup();
    bool update();
    bool event(SDL_Event &);
    void mouse(int x, int y);
};

#endif // APOGEE_IXCLIENT_H
