// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef APOGEE_FOREST_RENDERER_H
#define APOGEE_FOREST_RENDERER_H

#include "EntityRenderer.h"

#include <Mercator/Forest.h>

#include <string>

class ForestRenderer : public BBoxRenderer
{
  public:
    Mercator::Forest m_forest;
  protected:

    void drawTree(float height);
  public:
    ForestRenderer(Renderer &, Eris::Entity & e);
    virtual ~ForestRenderer();

    void load(const std::string &) { }

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_FOREST_RENDERER_H
