// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_IXCLIENT_H
#define APOGEE_IXCLIENT_H

#include "GameClient.h"

#include <visual/HeightMap.h>

#include <coal/container.h>

#include <string>

class Sprite;
class Model;
class Compass;

class IxClient : public GameClient {
  private:
    void doWorld();

    Coal::Container map_database;
    HeightMap map_height;
    Compass * compassWidget;

    bool m_use_mouse;
    bool terrain_detail;
    bool terrain_over;
  public:
    IxClient(Eris::Connection & con);
    virtual ~IxClient();

    bool setup();
    bool update(float);
    bool event(SDL_Event &);
    void mouse(int x, int y);
};

#endif // APOGEE_IXCLIENT_H
