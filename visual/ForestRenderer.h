// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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
