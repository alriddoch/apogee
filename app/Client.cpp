// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#error This file has been removed from the build

#include "Client.h"

#include <visual/Renderer.h>
#include <visual/Sprite.h>

#include <iostream>

#include <coal/debug.h>

bool Client::setup()
{
    CoalIsoLoader loader (map_database);
    loader.LoadMap ("agrilan.map");
    CoalDebug debug;
    debug.Dump (map_database);
    character = new Sprite();
    character->load("swinesherd_female_1_us_E_0.png",renderer);

    return 0;
}

bool Client::update()
{
    renderer.clear();
    //renderer.drawCharacter();
    //renderer.draw2Dtest();
    //renderer.draw3Dtest();
    renderer.flip();
    return false;
}

bool Client::event(SDL_Event & event)
{
    return false;
}
