// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"
#include "GLU.h"

#include "Renderer.h"
#include "EntityRenderer.h"
#include "Texture.h"
#include "Model.h"

#include <app/WorldEntity.h>
#include <app/Application.h>

#include <common/debug.h>

#include <Eris/Entity.h>
#include <Eris/TypeInfo.h>
#include <Eris/Connection.h>

#include <SDL_image.h>

#include <iostream>

static const bool debug_flag = false;

bool have_GL_EXT_compiled_vertex_array = false;

#ifndef GL_EXT_compiled_vertex_array
PFNGLLOCKARRAYSEXTPROC glLockArraysExt = 0;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysExt = 0;
#endif // GL_EXT_compiled_vertex_array

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
                                         application(app)



{
}

Renderer::~Renderer()
{
}

void Renderer::init()
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) != 0) { 
        std::cerr << "Failed to initialise video subsytem"
                  << std::endl << std::flush;
        throw RendererSDLinit();
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

    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glEnableClientState(GL_VERTEX_ARRAY);

    std::string extensions = (char *)glGetString(GL_EXTENSIONS);

    std::cout << "EXTENSIONS: " << extensions << std::endl << std::flush;

    if (extensions.find("GL_EXT_compiled_vertex_array") != std::string::npos) {
        std::cout << "Found GL_EXT_compiled_vertex_array extension"
                  << std::endl << std::flush;
        have_GL_EXT_compiled_vertex_array = true;

#ifndef GL_EXT_compiled_vertex_array
        glLockArraysExt = (PFNGLLOCKARRAYSEXTPROC)SDL_GL_GetProcAddress("glLockArraysExt");
        glUnlockArraysExt = (PFNGLUNLOCKARRAYSEXTPROC)SDL_GL_GetProcAddress("glUnlockArraysExt");
#endif // GL_EXT_compiled_vertex_array
    }

    model = new Model();
    if (!model->onInit(Datapath() + "paladin.cfg")) {
        std::cerr << "Loading paladin model failed" << std::endl << std::flush;
    }
    model->setLodLevel(1.0f);

    int textureUnits = -23;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &textureUnits);
    std::cout << "TEXTURE UNITS AVAILABLE: " << textureUnits
              << std::endl << std::flush;
    int depthbits = -1;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthbits);
    std::cout << "DEPTH BITS AVAILABLE: " << depthbits
              << std::endl << std::flush;

}



float Renderer::getZ(int x, int y)
{
    float z = 0;
    glReadPixels (x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    return z;
}

const Point3D Renderer::getWorldCoord(int x, int y, float z)
{
    GLint viewport[4];

    GLdouble mvmatrix[16], projmatrix[16];

    GLdouble wx, wy, wz;  /*  returned world x, y, z coords  */

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

    gluUnProject (x, y, z, mvmatrix, projmatrix, viewport, &wx, &wy, &wz);

    std::cout << "{" << wx << ";" << wy << ";" << wz << std::endl << std::flush;
    return Point3D(wx, wy, wz);
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
    glEnable(GL_CULL_FACE);
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

void Renderer::drawEntity(Eris::Entity * ent)
{
    Point3D pos = ent->getPosition();
    MovableEntity * me = dynamic_cast<MovableEntity *>(ent);
    if (me != NULL) {
        debug( std::cout << ent->getVelocity() << " "
                         << (worldTime - me->getTime()) << " " << pos; );
        pos = pos + ent->getVelocity() * (double)((worldTime - me->getTime())/1000.0f);
        debug( std::cout << "=" << pos << std::endl << std::flush; );
    } else {
        debug(std::cout << "Eris::Entity \"" << ent->getID() << "\" is not a MovableEntity" << std::endl << std::flush;);
    }
    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    orient(ent->getOrientation());
    RenderableEntity * re = dynamic_cast<RenderableEntity *>(ent);
    if (re != 0) {
        re->m_drawer->render(*this, Vector3D());
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
        drawEntity(e);
    }
    glPopMatrix();
}

void Renderer::drawWorld(Eris::Entity * wrld)
{
    worldTime = SDL_GetTicks();

    drawEntity(wrld);
}

void Renderer::drawSky()
{
    static GLint t_front = -1,
                 t_back = -1,
                 t_left = -1,
                 t_right = -1,
                 t_up = -1;
    static float vertices[] = { -1, -1, -1,
                                 1, -1, -1,
                                 1,  1, -1,
                                -1,  1, -1,
                                -1, -1,  1,
                                 1, -1,  1,
                                 1,  1,  1,
                                -1,  1,  1 };
    static float fb_tcoords[] = { 1, 0,
                                  0, 0,
                                  1, 0,
                                  0, 0,
                                  1, 1,
                                  0, 1,
                                  1, 1,
                                  0, 1 };
    static float lr_tcoords[] = { 0, 0,
                                  1, 0,
                                  0, 0,
                                  1, 0,
                                  0, 1,
                                  1, 1,
                                  0, 1,
                                  1, 1 };
    static float ud_tcoords[] = { 0, 0,
                                  1, 0,
                                  1, 1,
                                  0, 1,
                                  0, 1,
                                  1, 1,
                                  1, 0,
                                  0, 0 };
    static GLubyte front[] = { 3, 2, 6, 7 };
    static GLubyte back[] = { 1, 0, 4, 5 };
    static GLubyte left[] = { 0, 3, 7, 4 };
    static GLubyte right[] = { 2, 1, 5, 6 };
    static GLubyte up[] = { 7, 6, 5, 4 };
    // static GLubyte down[] = { 0, 1, 2, 3 };
    if (t_front == -1) {
        t_front = Texture::get("media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_front.png", false);
        t_back = Texture::get("media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_back.png", false);
        t_left = Texture::get("media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_left.png", false);
        t_right = Texture::get("media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_right.png", false);
        t_up = Texture::get("media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_up.png", false);
    }


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(elevation-90, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    glDepthMask(GL_FALSE);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_COORD_ARRAY);

    glTexCoordPointer(2, GL_FLOAT, 0, fb_tcoords);
    glBindTexture(GL_TEXTURE_2D, t_front);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, front);
    glBindTexture(GL_TEXTURE_2D, t_back);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, back);

    glTexCoordPointer(2, GL_FLOAT, 0, lr_tcoords);
    glBindTexture(GL_TEXTURE_2D, t_left);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, left);
    glBindTexture(GL_TEXTURE_2D, t_right);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, right);

    glTexCoordPointer(2, GL_FLOAT, 0, ud_tcoords);
    glBindTexture(GL_TEXTURE_2D, t_up);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, up);

    glDisable(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    
    glDepthMask(GL_TRUE);
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

void Renderer::selectEntity(Eris::Entity * ent, SelectMap & name, GLuint & next)
{
    Point3D pos = ent->getPosition();
    MovableEntity * me = dynamic_cast<MovableEntity *>(ent);
    if (me != NULL) {
        debug( std::cout << ent->getVelocity() << " "
                         << (worldTime - me->getTime()) << " " << pos; );
        pos = pos + ent->getVelocity() * (double)((worldTime - me->getTime())/1000.0f);
        debug( std::cout << "=" << pos << std::endl << std::flush; );
    } else {
        debug(std::cout << "Eris::Entity \"" << ent->getID() << "\" is not a MovableEntity" << std::endl << std::flush;);
    }
    glLoadName(++next);
    name[next] = ent;
    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    orient(ent->getOrientation());
    RenderableEntity * re = dynamic_cast<RenderableEntity *>(ent);
    if (re != 0) {
        re->m_drawer->select(*this);
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
        selectEntity(e, name, next);
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

    selectEntity(wrld, nameMap, nextName);

    debug(std::cout << "DONE ENITTIES" << std::endl << std::flush;);
    glPopName();

    int hits = glRenderMode(GL_RENDER);

    debug(std::cout << "DONE POST " << hits << std::endl << std::flush;);
    if (hits < 1) {
        return 0;
    }

    GLuint * ptr = &selectBuf[0];
    GLuint minDepth = UINT_MAX, noNames = 0;
    GLuint * namePtr;
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

    int new_width = 0, new_height;
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
