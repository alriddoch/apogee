// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include <coal/database.h>

#include <iostream>

#include <SDL_image.h>

#include "Renderer.h"
#include "Texture.h"
#include "Sprite.h"
#include "Model.h"

Renderer * Renderer::instance = NULL;

Renderer::Renderer(int wdth, int hght) : screen(NULL), character(NULL),
                                         button(NULL),
                                         width(wdth), height(hght),
                                         elevation(30), rotation(45),
                                         scale(1), x_offset(0), y_offset(0)
{
}
