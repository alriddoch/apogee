// Apogee Online RPG Test 3D Client
// Copyright (C) 2004 Alistair Riddoch
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

#ifndef APOGEE_FOREST_RENDERER_H
#define APOGEE_FOREST_RENDERER_H

#include "3dsRenderer.h"

#include <Mercator/Forest.h>

#include <string>

class ForestRenderer : public m3dsRenderer
{
  public:
    Mercator::Forest m_forest;
  protected:

    void drawTree(Renderer &, float height);
  public:
    ForestRenderer(Renderer &, RenderableEntity & e);
    virtual ~ForestRenderer();

    // void load(const std::string &) { }

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_FOREST_RENDERER_H
