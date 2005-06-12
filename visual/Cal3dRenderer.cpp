// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "Cal3dRenderer.h"

#include "Model.h"
#include "Renderer.h"

#include "common/system.h"

#include <wfmath/quaternion.h>

#include <sigc++/object_slot.h>

#include <sys/types.h>
#include <sys/stat.h>

Cal3dModel * Cal3dRenderer::m_default = 0;

void Cal3dRenderer::drawCal3dModel(Cal3dModel * m)
{
    float scale = 0.008f / m->getRenderScale();
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    // glScalef(0.025f, 0.025f, 0.025f);
    glScalef(scale, scale, scale);
    glEnable(GL_NORMALIZE);
    m->onRender();
    glDisable(GL_NORMALIZE);
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
    if (m_model == 0) {
        return;
    }
    m_model->onUpdate(secs);
}

Cal3dRenderer::Cal3dRenderer(Renderer & r, RenderableEntity & e) :
                             EntityRenderer(r, e), m_model(0)
{
    m_drawContents = false;
    if (m_default == 0) {
        m_default = new Cal3dModel();
        if (!m_default->onInit(getMediaPath() + "/media/media_new/3d_skeletons/cally/cally.cfg")) {
            std::cerr << "Loading default model failed" << std::endl << std::flush;
            delete m_default;
            m_default = 0;
        } else {
            m_default->setLodLevel(1.0f);
            m_default->onUpdate(0);
        }
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
    std::string path(getMediaPath() + "/" + file);
    struct stat sbuf;
    if (stat(path.c_str(), &sbuf)) {
        std::cerr << "Loading cal3d model " << file << " does not exist"
                  << std::endl << std::flush;
        return;
    }
    if (!model->onInit(path)) {
        std::cerr << "Loading cal3d model " << file << " failed"
                  << std::endl << std::flush;
        return;
    }
    model->setLodLevel(1.0f);
    model->onUpdate(0);
    m_model = model;
#if 0

    model->getCalModel().getMixer()->blendCycle(0, 1, 0);
    cb1 = 0;

    CalSkeleton * cs = model->getCalModel().getSkeleton();
    if (cs == 0) {
        return;
    }
    std::cout << "Got skeleton" << std::endl << std::flush;
    CalCoreSkeleton * ccs = cs->getCoreSkeleton();
    if (ccs == 0) {
        return;
    }
    int boneId = ccs->getCoreBoneId("Bip01 R Hand");
    if (boneId == -1) {
        return;
    }
    cb1 = cs->getBone(boneId);
    if (cb1 == 0) {
        return;
    }
#endif
}

void Cal3dRenderer::render(Renderer & r, const PosType & camPos)
{
    if (m_model == 0) {
        return;
    }
    drawCal3dModel(m_model);
#if 0

    if (cb1 == 0) {
        return;
    }
    const CalQuaternion & cq2 = cb1->getRotationAbsolute();
    const CalVector & cv1 = cb1->getTranslationAbsolute();
    std::cout << "[" << cq2.x << "," << cq2.y << "," << cq2.z << "," << cq2.w << "]" << std::endl << std::flush;
    std::cout << "[" << cv1.x << "," << cv1.y << "," << cv1.z << "]" << std::endl << std::flush;

    
    static float v[] = { 0,0,0, 50,0,0, 0,0,0, 0,50,0, 0,0,0, 0,0,50  };
    static float c[] = { 1,1,1, 1,0,0, 1,1,1, 0,1,0, 1,1,1, 0,0,1 };
    glVertexPointer(3, GL_FLOAT, 0, v);
    glColorPointer(3, GL_FLOAT, 0, c);
    float scale = 0.08f / m_model->getRenderScale();

    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_COLOR_MATERIAL);

    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, scale);
    glTranslatef(cv1.x, cv1.y, cv1.z);
    r.orient(WFMath::Quaternion(cq2.w, -cq2.x, -cq2.y, -cq2.z));
    glDrawArrays(GL_LINES, 0, 6);

    glDisable(GL_COLOR_MATERIAL);
    glDisableClientState(GL_COLOR_ARRAY);
    glPopMatrix();
#endif
}

void Cal3dRenderer::select(Renderer &, const PosType & camPos)
{
    if (m_model == 0) {
        return;
    }
    selectCal3dModel(m_model);
}
