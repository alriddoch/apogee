// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_ISOCLIENT_H
#define APOGEE_ISOCLIENT_H

#include "GameClient.h"

#include <visual/Isometric.h>
#include <visual/HeightMap.h>

#include <coal/container.h>

#include <Mercator/Terrain.h>

#include <string>

class Sprite;
class Compass;

class IsoClient : public GameClient {
  private:
    void doWorld();

    Coal::Container map_database;
    HeightMap map_height;
    Mercator::Terrain mterrain;
    Compass * compassWidget;
  public:
    IsoClient(Eris::Connection & con) : GameClient(*new Isometric(*this), con)
                                                       { }
    virtual ~IsoClient() { }

    bool setup();
    bool update(float);
    bool event(SDL_Event &);
    void mouse(int,int) { }
};

#endif // APOGEE_ISOCLIENT_H
