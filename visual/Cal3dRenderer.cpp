// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Cal3dRenderer.h"

#include "Model.h"

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

Cal3dRenderer::Cal3dRenderer(Eris::Entity & e) : EntityRenderer(e)
{
}

Cal3dRenderer::~Cal3dRenderer()
{
}

void Cal3dRenderer::render(Renderer &)
{
}

void Cal3dRenderer::select(Renderer &)
{
}
