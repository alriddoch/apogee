// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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
