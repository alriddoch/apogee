// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_IXCLIENT_H
#define APOGEE_IXCLIENT_H

#include <coal/bladeloader.h>

#include <visual/HeightMap.h>

#include <string>

#include "GameClient.h"

class Sprite;
class Model;
class Compass;

class IxClient : public GameClient {
  private:
    void doWorld();

    CoalDatabase map_database;
    HeightMap map_height;
    Compass * compassWidget;

    bool terrain_detail;
    bool terrain_over;
  public:
    IxClient(Renderer & rend, Eris::Connection & con);
    virtual ~IxClient();

    bool setup();
    bool update(float);
    bool event(SDL_Event &);
    void mouse(int x, int y);
};

#endif // APOGEE_IXCLIENT_H
