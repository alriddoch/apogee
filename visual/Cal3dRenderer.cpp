// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Cal3dRenderer.h"

#include "Model.h"

Model * Cal3dRenderer::m_default = 0;

void Cal3dRenderer::drawCal3dModel(Model * m)
{
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.025f, 0.025f, 0.025f);
    m->onRender();
    glPopMatrix();
}

void Cal3dRenderer::selectCal3dModel(Model * m)
{
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.025f, 0.025f, 0.025f);
    m->onSelect();
    glPopMatrix();
}

Cal3dRenderer::Cal3dRenderer(Renderer & r, Eris::Entity & e) : EntityRenderer(r, e)
{
    if (m_default == 0) {
        m_default = new Model();
        if (!m_default->onInit(Datapath() + "paladin.cfg")) {
            std::cerr << "Loading paladin model failed" << std::endl << std::flush;
        }
        m_default->setLodLevel(1.0f);
        m_default->onUpdate(0);
    }
}

Cal3dRenderer::~Cal3dRenderer()
{
}

void Cal3dRenderer::render(Renderer &)
{
    drawCal3dModel(m_default);
}

void Cal3dRenderer::select(Renderer &)
{
    selectCal3dModel(m_default);
}
