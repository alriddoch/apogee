// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "GL.h"

#include "ForestRenderer.h"

#include "Renderer.h"

#include <Mercator/Plant.h>

#include <Eris/Entity.h>

#include <iostream>

ForestRenderer::ForestRenderer(Renderer & r, Eris::Entity & e):
      BBoxRenderer(r, e)
{
}

ForestRenderer::~ForestRenderer()
{
}

void ForestRenderer::drawTree(float height)
{
    float width = height / 10.f;
    draw3DBox(WFMath::AxisBox<3>(WFMath::Point<3>(0,0,0),
                                 WFMath::Point<3>(width, width, height)));
}

void ForestRenderer::render(Renderer & r, const PosType & camPos)
{
    if (!m_forest.getArea().isValid()) {
        m_forest.setVolume(m_ent.getBBox());
    }

    const Mercator::Forest::PlantStore & ps = m_forest.getPlants();

    if (ps.empty()) {
        m_forest.populate();
    }

    Mercator::Forest::PlantStore::const_iterator I = ps.begin();
    for(; I != ps.end(); ++I) {
        Mercator::Forest::PlantColumn::const_iterator J = I->second.begin();
        for(; J != I->second.end(); ++J) {
            const Mercator::Plant & p = J->second;
            const WFMath::Point<2> & d = p.getDisplacement();
            glPushMatrix();
            glTranslatef(d.x() + I->first, d.y() + J->first, 0.f);
            r.orient(p.getOrientation());
            drawTree(p.getHeight());
            glPopMatrix();
        }
    }
}

void ForestRenderer::select(Renderer & r, const PosType & camPos)
{
}
