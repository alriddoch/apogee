// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "GL.h"
#include "GLU.h"

#include "Renderer.h"
#include "EntityRenderer.h"
#include "Texture.h"
#include "Model.h"

#include <app/WorldEntity.h>
#include <app/Application.h>

#include <common/debug.h>
#include <common/system.h>

#include <Eris/Entity.h>
#include <Eris/TypeInfo.h>
#include <Eris/Connection.h>

#include <iostream>
#include <cassert>

static const bool debug_flag = false;

static const float PI = 3.14159f;
static const float FOG_RED = 0.5f;
static const float FOG_GREEN = 0.5f;
static const float FOG_BLUE = 0.5f;
static const float FOG_ALPHA = 0.0f;

Renderer::Renderer(Application & app, int wdth, int hght) : 
                                         screen(NULL),
                                         width(wdth), height(hght),
                                         fullscreen(false),
                                         window_width(wdth),
                                         window_height(hght),
                                         elevation(30), rotation(45),
                                         scale(1), x_offset(0), y_offset(0),
                                         z_offset(0),
                                         frameCount(0), time(0), lastCount(0),
                                         m_windowName("<default>"),
                                         m_iconName("<default>"),
                                         application(app)
{
}

Renderer::~Renderer()
{
}

bool Renderer::init()
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) != 0) { 
        std::cerr << "Failed to initialise video subsytem"
                  << std::endl << std::flush;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    videoModes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

    /* Check is there are any modes available */
    if(videoModes == 0){
        printf("No modes available!\n");
    } else if (videoModes == (SDL_Rect **)-1) {
        printf("All resolutions available.\n");
    } else{
        /* Print valid modes */
        printf("Available Modes\n");
        for(int i=0; videoModes[i]; ++i) {
            printf("  %d x %d\n", videoModes[i]->w, videoModes[i]->h);
        }
    }
    // These should be turned on when running in production mode, but they
    // make using a debugger really hard and are only required in the perspective
    // client
    // SDL_WM_GrabInput(SDL_GRAB_ON);
    // SDL_ShowCursor(0);

    shapeView();

    std::string extensions = (char *)glGetString(GL_EXTENSIONS);

    std::cout << "EXTENSIONS: " << extensions << std::endl << std::flush;

    sage_init();

    if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
        std::cout << "GL_ARB_VERTEX_BUFFER_OBJECT supported" << std::endl << std::flush;
    } else {
        std::cout << "GL_ARB_VERTEX_BUFFER_OBJECT NOT supported" << std::endl << std::flush;
    }

    int textureUnits = -23;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &textureUnits);
    std::cout << "TEXTURE UNITS AVAILABLE: " << textureUnits
              << std::endl << std::flush;
    int depthbits = -1;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthbits);
    std::cout << "DEPTH BITS AVAILABLE: " << depthbits
              << std::endl << std::flush;

    model = new Model();
    if (!model->onInit(getMediaPath() + "/media/media_new/3d_skeletons/paladin/paladin.cfg")) {
        std::cerr << "Loading paladin model failed" << std::endl << std::flush;
    }
    model->setLodLevel(1.0f);

    return true;
}

void Renderer::shapeView()
{
    Uint32 flags = SDL_OPENGL|SDL_DOUBLEBUF|SDL_ANYFORMAT;
    if (fullscreen) {
        flags |= SDL_FULLSCREEN;
    } else {
        flags |= SDL_RESIZABLE;
    }
    if ((screen = SDL_SetVideoMode(width, height, 0, flags)) == NULL) {
        std::cerr << "Failed to set video mode" << std::endl << std::flush;
        SDL_Quit();
    }
    SDL_WM_SetCaption(m_windowName, m_iconName);

    glViewport(0, 0, width, height);

    glDepthFunc(GL_LEQUAL);
    glEnableClientState(GL_VERTEX_ARRAY);

    static const float fogColor[] = { FOG_RED, FOG_GREEN, FOG_BLUE, FOG_ALPHA };
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE,GL_LINEAR);
    glFogfv(GL_FOG_COLOR,fogColor);
    glFogf(GL_FOG_START,15.0f);
    glFogf(GL_FOG_END,maxViewDistance - 15.0f);
    glHint(GL_FOG_HINT,GL_FASTEST);

    if (glIsTexture(17)) {
        std::cout << "17 is a texture" << std::endl << std::flush;
    } else {
        std::cout << "17 is NOT a texture" << std::endl << std::flush;
    }

    if (glIsTexture(23)) {
        std::cout << "23 is a texture" << std::endl << std::flush;
    } else {
        std::cout << "23 is NOT a texture" << std::endl << std::flush;
    }

}

float Renderer::getZ(int x, int y)
{
    float z = 0;
    glReadPixels (x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    return z;
}

const PosType Renderer::getWorldCoord(int x, int y, float z)
{
    GLint viewport[4];

    GLdouble mvmatrix[16], projmatrix[16];

    GLdouble wx, wy, wz;  /*  returned world x, y, z coords  */

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

    gluUnProject (x, y, z, mvmatrix, projmatrix, viewport, &wx, &wy, &wz);

    std::cout << "{" << wx << ";" << wy << ";" << wz << std::endl << std::flush;
    return PosType(wx, wy, wz);
}

void Renderer::lightOn()
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_LIGHTING);
}

void Renderer::lightOff()
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glDisable(GL_LIGHTING);
}

void Renderer::update(float secs)
{
    ++frameCount;
    time += secs;
    // std::cout << "SECS " << secs << ":" << lastCount << std::endl << std::flush;
    if ((time - lastCount) > 1.f) {
        std::cout << frameCount << " frames per second" << std::endl << std::flush;
        lastCount = time;
        frameCount = 0;
    }

    model->onUpdate(secs);
    Update.emit(secs);
}

void Renderer::origin()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projection();
    viewPoint();
    // glEnable(GL_CULL_FACE);
}

void Renderer::orient(const WFMath::Quaternion & orientation)
{
    if (!orientation.isValid()) {
        return;
    }
    float orient[4][4];
    WFMath::RotMatrix<3> omatrix(orientation); // .asMatrix(orient);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            orient[i][j] = omatrix.elem(i,j);
        }
    }
    orient[3][0] = orient[3][1] = orient[3][2] = orient[0][3] = orient[1][3] = orient[2][3] = 0.0f;
    orient[3][3] = 1.0f;
    glMultMatrixf(&orient[0][0]);
}

void Renderer::drawEntity(Eris::Entity * ent, RenderableEntity * pe,
                          const PosType & cp)
{
    assert(ent != 0);

    PosType pos = ent->getPosition();

    MovableEntity * me = dynamic_cast<MovableEntity *>(ent);
    if (me != NULL) {
        debug( std::cout << ent->getVelocity() << " "
                         << (worldTime - me->getTime()) << " " << pos; );
        pos = pos + ent->getVelocity() * (double)((worldTime - me->getTime())/1000.0f);
        debug( std::cout << "=" << pos << std::endl << std::flush; );
        if (pe != 0) {
            pe->constrainChild(pos);;
        }
    } else {
        debug(std::cout << "Eris::Entity \"" << ent->getID()
                        << "\" is not a MovableEntity"
                        << std::endl << std::flush;);
    }
    PosType camPos = cp;
    camPos.toLocalCoords(pos, WFMath::Quaternion().identity());

    RenderableEntity * re = dynamic_cast<RenderableEntity *>(ent);
    if (re == 0) {
        return;
    }

    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    orient(ent->getOrientation());

    re->m_drawer->render(*this, camPos);

    if (!re->m_drawer->drawContents()) {
        glPopMatrix();
        return;
    }

    int numEnts = ent->getNumMembers();
    debug(std::cout << ent->getID() << " " << numEnts << " emts"
                    << std::endl << std::flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        if (!e->isVisible()) { continue; }
        // debug(std::cout << ":" << e->getID() << e->getPosition() << ":"
                        // << e->getBBox().u << e->getBBox().v
                        // << std::endl << std::flush;);
        drawEntity(e, re, camPos);
    }
    glPopMatrix();
}

void Renderer::drawWorld(Eris::Entity * wrld)
{
    worldTime = SDL_GetTicks();
    PosType camPos(x_offset, y_offset, z_offset);

    drawEntity(wrld, 0, camPos);
}

void Renderer::drawGui()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -800.0f, 800.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    // glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
}

void Renderer::selectEntity(Eris::Entity * ent, RenderableEntity * pe,
                            const PosType & cp,
                            SelectMap & name, GLuint & next)
{
    PosType pos = ent->getPosition();
    MovableEntity * me = dynamic_cast<MovableEntity *>(ent);
    if (me != NULL) {
        debug( std::cout << ent->getVelocity() << " "
                         << (worldTime - me->getTime()) << " " << pos; );
        pos = pos + ent->getVelocity() * (double)((worldTime - me->getTime())/1000.0f);
        debug( std::cout << "=" << pos << std::endl << std::flush; );
        if (pe != 0) {
            pe->constrainChild(pos);;
        }
    } else {
        debug(std::cout << "Eris::Entity \"" << ent->getID()
                        << "\" is not a MovableEntity"
                        << std::endl << std::flush;);
    }
    PosType camPos = cp;
    camPos.toLocalCoords(pos, WFMath::Quaternion().identity());
    glLoadName(++next);
    name[next] = ent;

    RenderableEntity * re = dynamic_cast<RenderableEntity *>(ent);
    if (re == 0) {
        return;
    }

    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    orient(ent->getOrientation());

    re->m_drawer->select(*this, camPos);

    if (!re->m_drawer->drawContents()) {
        glPopMatrix();
        return;
    }

    int numEnts = ent->getNumMembers();
    debug(std::cout << ent->getID() << " " << numEnts << " emts"
                    << std::endl << std::flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        debug(std::cout << "DOING " << e->getID() << std::endl << std::flush;);
        if (!e->isVisible()) {
            debug(std::cout << "SKIPPING " << e->getID() << std::endl << std::flush;);
            continue;
        }
        selectEntity(e, re, camPos, name, next);
    }
    glPopMatrix();
}

Eris::Entity * Renderer::selectWorld(Eris::Entity * wrld, int x, int y)
{
    GLuint selectBuf[512];
    GLuint nextName = 0;
    SelectMap nameMap;

    glSelectBuffer(512,selectBuf);
    glRenderMode(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    gluPickMatrix(x, getHeight() - y, 1, 1, viewport);

    projection();
    viewPoint();

    glInitNames();
    
    glPushName(nextName);
    debug(std::cout << "SELECTING" << std::endl << std::flush;);

    PosType camPos(x_offset, y_offset, z_offset);

    selectEntity(wrld, 0, camPos, nameMap, nextName);

    debug(std::cout << "DONE ENITTIES" << std::endl << std::flush;);
    glPopName();

    int hits = glRenderMode(GL_RENDER);

    debug(std::cout << "DONE POST " << hits << std::endl << std::flush;);
    if (hits < 1) {
        return 0;
    }

    GLuint * ptr = &selectBuf[0];
    GLuint minDepth = UINT_MAX, noNames = 0;
    GLuint * namePtr = 0;
    for (int i = 0; i < hits; ++i) {
        int names = *(ptr++);
        debug(std::cout << "{" << *ptr << "}";);
        if (*ptr < minDepth) {
            noNames = names;
            minDepth = *ptr;
            namePtr = ptr + 2;
        }
        ptr += (names + 2);
    }
    std::cout << "The closest hit has " << noNames << " names: " << std::endl << std::flush;
    if (noNames > 1) {
        std::cerr << "WARNING: Got world hit with more than 1 name"
                  << std::endl << std::flush;
    }
    SelectMap::const_iterator I = nameMap.find(*namePtr);
    if (I == nameMap.end()) {
        std::cerr << "ERROR: Got invalid hit" << std::endl << std::flush;
        return 0;
    }
    std::cout << "CLICKED on " << I->second->getID() << std::endl << std::flush;
    return I->second;
}

void Renderer::resize(int wdth, int hght)
{
    width = wdth;
    height = hght;
    shapeView();
}

void Renderer::clear()
{
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen
    glClear(GL_DEPTH_BUFFER_BIT); // Clear The Screen
}

void Renderer::toggleFullscreen()
{
    assert(width > 0);

    int new_width = 0, new_height = 0;
    fullscreen = !fullscreen;

    if (fullscreen) {
        if (videoModes == 0) {
            std::cout << "No modes" << std::endl << std::flush;
            fullscreen = false;
            return;
        }
        window_width = width;
        window_height = height;
        for (int i = 0; videoModes[i]; ++i) {
            if (videoModes[i]->w >= width) {
                if ((new_width == 0) || (videoModes[i]->w < new_width)) {
                    new_width = videoModes[i]->w;
                    new_height = videoModes[i]->h;
                }
            }
        }
        if (new_width == 0) {
            std::cout << "No mode matched" << std::endl << std::flush;
            fullscreen = false;
            return;
        }
    } else {
        new_width = window_width;
        new_height = window_height;
    }
    resize(new_width, new_height);
}

void Renderer::cleanUp()
{
    if (fullscreen) {
        toggleFullscreen();
    }
}
