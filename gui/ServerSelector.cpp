// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "ServerSelector.h"
#include "Gui.h"

#include "visual/Model.h"
#include "visual/Renderer.h"

#include <Eris/Metaserver.h>
#include <Eris/ServerInfo.h>

#include <wfmath/quaternion.h>
#include <wfmath/point.h>

static int xoffset[] = {0, 80, -80, 160, -160, 240, -240 };
static int yoffset[] = {0, -15, -15, -35, -35, -60, -60 };

ServerSelector::ServerSelector(Gui & g, int x, int y) : Widget(g, x, y),
    metaServer(* new Eris::Meta("", "metaserver.worldforge.org", 7) ),
    connect(g, x, y, "connect"),
    refresh(g, x, y, "refresh"),
    specify(g, x, y, "specify"),
    filter(g, x, y, "create") 
{
    metaServer.refresh();
}

ServerSelector::~ServerSelector()
{
    // delete &metaServer; FIXME
}

void ServerSelector::setup()
{
    refreshName = m_g.newName();
    connectName = m_g.newName();
    specifyName = m_g.newName();
    filterName = m_g.newName();
    connect.setup();
    refresh.setup();
    specify.setup();
    filter.setup();
}

void ServerSelector::draw()
{
    glPushMatrix();
    glTranslatef(0.0f, -100.0f, 0.01f);
    connect.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    refresh.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    specify.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    filter.draw();
    glPopMatrix();

    std::map<GLuint, const Eris::ServerInfo*>::const_iterator I;
    WFMath::Quaternion rot(1,0,0,0);
    for(unsigned i = 0; i < metaServer.numServers(); ++i) {
        const Eris::ServerInfo & svr = metaServer.getInfoForServer(i);
        glPushMatrix();
        glTranslatef(xoffset[i], yoffset[i], 0.0f);
        m_g.print(svr.getServername().c_str());
        glTranslatef(0.0f, 20.0f, 0.0f);
        if (selectedServer == svr.getHostname()) {
            glScalef(1.2f, 1.2f, 1.2f);
        }
        glRotatef(-90,1,0,0);
        glPushAttrib(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        m_g.renderer.model->onRender();
        glPopAttrib();
        glPopMatrix();
    }
}

void ServerSelector::select()
{
    glPushMatrix();
    glTranslatef(0.0f, -100.0f, 0.01f);
    glPushName(connectName);
    connect.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    glLoadName(refreshName);
    refresh.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    glLoadName(specifyName);
    specify.draw();
    glTranslatef(0.0f, -25.0f, 0.01f);
    glLoadName(filterName);
    filter.draw();
    glPopMatrix();

    names.clear();
    for(unsigned i = 0; i < metaServer.numServers(); ++i) {
        glPushMatrix();
        glTranslatef(xoffset[i], yoffset[i], 0.0f);
        GLuint name = m_g.newName();
        glLoadName(name);
        const Eris::ServerInfo & svr = metaServer.getInfoForServer(i);
        names[name] = &svr;
        // m_g.print(I->second.second.c_str());
        glTranslatef(0.0f, 20.0f, 0.0f);
        if (selectedServer == svr.getHostname()) {
            glScalef(1.2f, 1.2f, 1.2f);
        }
        glRotatef(-90,1,0,0);
        m_g.renderer.model->onRender();
        glPopMatrix();
    }
    glPopName();
}

void ServerSelector::click()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (connectName == *I) {
            connect.press();
        }
        if (refreshName == *I) {
            refresh.press();
        }
        if (specifyName == *I) {
            specify.press();
        }
        if (filterName == *I) {
            filter.press();
        }
        std::map<GLuint, const Eris::ServerInfo * >::const_iterator J;
        J = names.find(*I);
        if (J != names.end()) {
            selectedServer = J->second->getHostname();
            std::cout << "SELECTED: " << selectedServer;
        }
    }
}

void ServerSelector::release()
{
    const hitlist & h = m_g.getHits();

    hitlist::const_iterator I = h.begin();
    for (;I != h.end(); I++) {
        if (connectName == *I) {
            m_obs = true;
            connect.release();
            if (!selectedServer.empty()) {
                connectSignal.emit(selectedServer);
            }
        }
        if (refreshName == *I) {
            refresh.release();
            refreshSignal.emit();
        }
        if (specifyName == *I) {
            m_obs = true;
            specify.release();
            specifySignal.emit();
        }
        if (filterName == *I) {
            filter.release();
            filterSignal.emit();
        }
    }
}

bool ServerSelector::key(int sym, int mod)
{
    return false;
}

#if 0
void ServerSelector::addServers(const std::set<std::pair<std::string, std::string> > & cl)
{
    std::set<std::pair<std::string, std::string> >::const_iterator I;
    for (I = cl.begin(); I != cl.end(); ++I) {
        serverNames[m_g.newName()] = *I;
    }
}
void ServerSelector::addServer(const Eris::ServerInfo & si)
{
    serverInfo[m_g.newName()] = &si;
}
#endif
