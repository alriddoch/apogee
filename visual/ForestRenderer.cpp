// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "GL.h"

#include "ForestRenderer.h"

#include "Renderer.h"

#include "app/WorldEntity.h"

#include <Mercator/Plant.h>
#include <Mercator/Area.h>

#include <Eris/Entity.h>

#include <iostream>
#include <cassert>

ForestRenderer::ForestRenderer(Renderer & r, RenderableEntity & e):
      m3dsRenderer(r, e)
{
}

ForestRenderer::~ForestRenderer()
{
}

void ForestRenderer::drawTree(Renderer & r, float height)
{
    float scale = height / 5;
    glScalef(scale, scale, scale);
    draw3dsFile(r);
    // float width = height / 10.f;
    // draw3DBox(WFMath::AxisBox<3>(WFMath::Point<3>(0,0,0),
                                 // WFMath::Point<3>(width, width, height)));
}

void ForestRenderer::render(Renderer & r, const PosType & camPos)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_NORMALIZE);
    glAlphaFunc(GL_GREATER, 0.2f);

    if (m_model == 0) {
        return;
    }

    Mercator::Area * area = m_forest.getArea();
    if (area == 0) {
        // FIXME
        // m_forest.setVolume(m_ent.getBBox());
    } else {
        const WFMath::Polygon<2> & shape = area->shape();
        if (!shape.isValid() || shape.numCorners() == 0) {
            // FIXME
            // m_forest.setVolume(m_ent.getBBox());
        }
    }

    // FIXME Assumes direct parent is terrain
    Eris::Entity * pe = m_ent.getLocation();
    RenderableEntity * re = dynamic_cast<RenderableEntity *>(pe);
    assert(re != 0);

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
            const WFMath::Point<3> & forestPos = m_ent.getPosition();
            WFMath::Point<3> pos(I->first + d.x() + forestPos.x(),
                                 J->first + d.y() + forestPos.y(), forestPos.z());
            re->constrainChild(m_ent, pos);
            glPushMatrix();
            glTranslatef(pos.x() - forestPos.x(), pos.y() - forestPos.y(), pos.z() - forestPos.z());
            r.orient(p.getOrientation());
            drawTree(r, p.getHeight());
            glPopMatrix();
        }
    }

    glDisable(GL_NORMALIZE);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}

void ForestRenderer::select(Renderer & r, const PosType & camPos)
{
}
