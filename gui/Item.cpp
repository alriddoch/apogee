// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Item.h"

Item::~Item()
{
}

bool Item::load(const char * filename)
{
    return graphic.load(filename);
}

void Item::setup()
{
    // This is where we would enumerate 
}

void Item::draw()
{
    graphic.draw();
}

void Item::select()
{
    graphic.draw();
}

void Item::click()
{
}

void Item::release()
{
}

bool Item::key(int,int)
{
    return false;
}
