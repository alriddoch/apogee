// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "Cal3dRenderer.h"

#include "Model.h"
#include "Renderer.h"

#include "common/system.h"

#include <sigc++/object_slot.h>

Cal3dModel * Cal3dRenderer::m_default = 0;

void Cal3dRenderer::drawCal3dModel(Cal3dModel * m)
{
    float scale = 0.008f / m->getRenderScale();
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    // glScalef(0.025f, 0.025f, 0.025f);
    glScalef(scale, scale, scale);
    m->onRender();
    glPopMatrix();
}

void Cal3dRenderer::selectCal3dModel(Cal3dModel * m)
{
    float scale = 0.008f / m->getRenderScale();
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    // glScalef(0.025f, 0.025f, 0.025f);
    glScalef(scale, scale, scale);
    m->onSelect();
    glPopMatrix();
}

void Cal3dRenderer::update(float secs)
{
    m_model->onUpdate(secs);
}

Cal3dRenderer::Cal3dRenderer(Renderer & r, RenderableEntity & e) :
                             EntityRenderer(r, e), m_model(0)
{
    m_drawContents = false;
    if (m_default == 0) {
        m_default = new Cal3dModel();
        if (!m_default->onInit(getMediaPath() + "/media/media_new/3d_skeletons/cally/cally.cfg")) {
            std::cerr << "Loading paladin model failed" << std::endl << std::flush;
        }
        m_default->setLodLevel(1.0f);
        m_default->onUpdate(0);
    }
    m_model = m_default;
    r.Update.connect(SigC::slot(*this, &Cal3dRenderer::update));
}

Cal3dRenderer::~Cal3dRenderer()
{
}

void Cal3dRenderer::load(const std::string & file)
{
    Cal3dModel * model = new Cal3dModel();
    if (!model->onInit(getMediaPath() + "/" + file)) {
        std::cerr << "Loading cal3d model " << file << " failed"
                  << std::endl << std::flush;
        return;
    }
    model->setLodLevel(1.0f);
    model->onUpdate(0);
    m_model = model;
}

void Cal3dRenderer::render(Renderer &, const PosType & camPos)
{
    drawCal3dModel(m_model);
}

void Cal3dRenderer::select(Renderer &, const PosType & camPos)
{
    selectCal3dModel(m_model);
}
