// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "GL.h"

#include "EntityRenderer.h"

#include <Eris/Entity.h>

#include <wfmath/axisbox.h>

EntityRenderer::EntityRenderer(Renderer & r, Eris::Entity & e) : m_ent(e),
                                             m_drawContents(true)
{
}

EntityRenderer::~EntityRenderer()
{
}

/*
 * This is the vertex layout used by the 2 3Dbox functions.
 *
 *
 *                                   6
 *
 *
 *                         7                    5
 *
 *
 *                                   4
 *
 *
 *
 *                                   2
 *
 *
 *                         3                    1
 *
 *
 *                                   0
 */

void BBoxRenderer::draw3DBox(const WFMath::AxisBox<3> & bbox)
{
    GLfloat vertices[] = {
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z() 
    };
    static const GLuint indices[] = { 0, 1, 3, 2, 7, 6, 4, 5, 0, 4, 1, 5,
                                      3, 7, 2, 6, 0, 3, 1, 2, 4, 7, 5, 6 };

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, indices);
}

void BBoxRenderer::select3DBox(const WFMath::AxisBox<3> & bbox)
{
    GLfloat vertices[] = {
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z() 
    };
    static const GLuint indices[] = { 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6,
                                      3, 0, 4, 7, 4, 5, 6, 7, 0, 3, 2, 1 };

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, indices);
}

BBoxRenderer::BBoxRenderer(Renderer & r, Eris::Entity & e) : EntityRenderer(r, e)
{
}

BBoxRenderer::~BBoxRenderer()
{
}

void BBoxRenderer::render(Renderer &, const PosType & camPos)
{
    if (m_ent.hasBBox()) {
        draw3DBox(m_ent.getBBox());
    }
}

void BBoxRenderer::select(Renderer &, const PosType & camPos)
{
    if (m_ent.hasBBox()) {
        select3DBox(m_ent.getBBox());
    }
}
