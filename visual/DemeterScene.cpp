// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "DemeterScene.h"
#include "Texture.h"
#include "Sprite.h"
#include "Model.h"
#include "Matrix.h"
#include "TileMap.h"

#include <common/debug.h>

#include <app/WorldEntity.h>

#include <Eris/Entity.h>
#include <Eris/TypeInfo.h>

#include <SDL_image.h>

#include <iostream>

// #define USE_DEMETER 1

static const bool debug_flag = false;

const float PI = 3.14159f;
const float FOG_RED = 0.5f;
const float FOG_GREEN = 0.75f;
const float FOG_BLUE = 1.0f;
const float FOG_ALPHA = 0.0f;

DemeterScene::DemeterScene(int wdth, int hght) : Renderer(wdth, hght),
                                                 tilemap(NULL), charType(NULL)
{
    init();
}

void DemeterScene::init()
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) != 0) { 
        std::cerr << "Failed to initialise video subsytem"
                  << std::endl << std::flush;
        throw RendererSDLinit();
    }

    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_EnableUNICODE(1);
    SDL_WM_SetCaption("perigee", "demeter");

    // These should be turned on when running in production mode, but they
    // make using a debugger really hard.
    // SDL_WM_GrabInput(SDL_GRAB_ON);
    // SDL_ShowCursor(0);

    settings = Demeter::Settings::GetInstance();
    settings->SetMediaPath("maps/");
    settings->SetScreenWidth(width);
    settings->SetScreenWidth(height);

    this->shapeView();

    model = new Model();
    if (!model->onInit(Datapath() + "paladin.cfg")) {
        std::cerr << "Loading paladin model failed" << std::endl << std::flush;
    }
    model->setLodLevel(1.0f);

    const int maxNumVisibleTriangles = 40000;

#if USE_DEMETER
    terrain = new Demeter::Terrain("moraf_hm.jpg", "moraf.jpg", "grass.png", 0.78125, 0.03125, maxNumVisibleTriangles);
    terrain->SetMaximumVisibleBlockSize(64);
    // terrain->SetCommonTextureRepeats(50.0f);

    cout << "Loaded Terrain " << terrain->GetWidth() << " : "
         << terrain->GetHeight()
         << endl << flush;
#endif

    // x_offset = -4.0f;
    // y_offset = -4.0f;
    // z_offset = 2.0f;

    elevation = 10;
    rotation = 45;

    // GLfloat ambientColor[] = {1, 1, 1, 1.0};
    // GLfloat diffuseColor[] = {0, 1, 0, 1.0};
    // GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
// 
    // glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
    // glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    // glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

}

void DemeterScene::update(float secs)
{
    model->onUpdate(secs);
}

void DemeterScene::shapeView()
{
    const float maxViewDistance = 4500.0f;

    if ((screen = SDL_SetVideoMode(width, height, 16,
            SDL_OPENGL|SDL_DOUBLEBUF|SDL_RESIZABLE)) == NULL) {
        std::cerr << "Failed to set video mode" << std::endl << std::flush;
        SDL_Quit();
        throw RendererSDLinit();
    }

    settings->SetScreenWidth(width);
    settings->SetScreenWidth(height);

    glViewport(0, 0, width, height);
    glClearColor(0.5f, 0.75f, 1.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    //glShadeModel(GL_SMOOTH);
    float fogColor[4];
    fogColor[0] = FOG_RED;
    fogColor[1] = FOG_GREEN;
    fogColor[2] = FOG_BLUE;
    fogColor[3] = FOG_ALPHA;
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE,GL_LINEAR);
    glFogfv(GL_FOG_COLOR,fogColor);
    glFogf(GL_FOG_START,100.0f);
    glFogf(GL_FOG_END,maxViewDistance - 100.0f);
    glHint(GL_FOG_HINT,GL_FASTEST);

    viewPoint();
}

inline void DemeterScene::viewScale(float scale_factor)
{
    // const float maxViewDistance = 4500.0f;
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // float xscale = 0.5f / scale_factor;
    // float yscale = 0.5f / scale_factor;
    // This is complete arse
    // glFrustum(-xscale, xscale, -yscale, yscale, 0.65f / scale_factor, maxViewDistance * scale_factor );
    // gluPerspective(45.0f, (float)width/(float)height,0.65f, maxViewDistance);
    // glMatrixMode(GL_MODELVIEW);
    glScalef(scale_factor, scale_factor, scale_factor);
}

inline void DemeterScene::viewPoint()
{
    const float maxViewDistance = 4500.0f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Not sure how to use this one to get a decent projection no matter
    // what the aspect ratio. Keep it here for reference.
    //gluPerspective(45.0f, (float)width/(float)height,0.65f, maxViewDistance);

    // Sort out perspective projection, with scale tied to height of the window
    // this allow a very wide window to give a nice widescreen view, and
    // allows a narrow window to be usable.
    float s = ((float)width / (float)height) * 3.0f / 8.0f;
    glViewport(0,0,width,height);
    glFrustum(-s,s,-0.375f,0.375f,0.65f,maxViewDistance);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    glEnable(GL_DEPTH_TEST);
    glTranslatef(0.0f, 0.0f, -5.0f);
}

inline void DemeterScene::reorient()
{
    glRotatef(-rotation, 0.0, 0.0, 1.0);
    glRotatef(90-elevation, 1.0, 0.0, 0.0);
}

inline void DemeterScene::orient()
{
    glRotatef(elevation-90, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, -2.5f);
}

inline void DemeterScene::translate()
{
    glTranslatef(-x_offset,-y_offset,-z_offset);
}

// This function moves the render cursor to the origin and rotates the
// axis to be inline with the worldforge axis
inline void DemeterScene::origin()
{
    viewPoint();
    orient();
    translate();
}

void DemeterScene::lightOn()
{
    glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
}

void DemeterScene::lightOff()
{
    glDisable(GL_LIGHTING);
    // glDisable(GL_LIGHT0);
}

void DemeterScene::drawCal3DModel(Model * m, const Point3D & coords,
                                  const WFMath::Quaternion & orientation)
{
    glPushMatrix();
    glTranslatef(coords.x(), coords.y(), coords.z());
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
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
    glScalef(0.025f, 0.025f, 0.025f);
    m->onRender();
    glPopMatrix();
}

void DemeterScene::draw3DBox(const Point3D & coords,
                             const WFMath::AxisBox<3> & bbox)
{

    glPushMatrix();
    // origin();
    glTranslatef(coords.x(), coords.y(), coords.z());

    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glEnd();

    glPopMatrix();
}

void DemeterScene::drawEntity(Eris::Entity * ent)
{
    glPushMatrix();
    const Point3D & pos = ent->getPosition();
    glTranslatef(pos.x(), pos.y(), pos.z());
    int numEnts = ent->getNumMembers();
    debug(std::cout << ent->getID() << " " << numEnts << " emts"
                    << std::endl << std::flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        Point3D pos = e->getPosition();
        WorldEntity * we = dynamic_cast<WorldEntity *>(e);
        if (we != NULL) {
            debug( std::cout << e->getVelocity() << " " << (worldTime - we->getTime()) << " " << pos; );
            pos = pos + e->getVelocity() * (double)((worldTime - we->getTime())/1000.0f);
            debug( std::cout << "=" << pos << std::endl << std::flush; );
        } else {
            std::cout << "Eris::Entity \"" << e->getID() << "\" is not a WorldEntity" << std::endl << std::flush;
        }
        // debug(std::cout << ":" << e->getID() << e->getPosition() << ":"
                        // << e->getBBox().u << e->getBBox().v
                        // << std::endl << std::flush;);
        if (!e->isVisible()) { continue; }
        Eris::TypeInfo * type = Eris::TypeInfo::findSafe(*e->getInherits().begin());
        if (type->safeIsA(charType)) {
            drawCal3DModel(model, pos, e->getOrientation());
        } else {
            if (e->hasBBox()) {
                draw3DBox(pos, e->getBBox());
            }
            drawEntity(e);
        }
    }
    glPopMatrix();
}

void DemeterScene::drawWorld(Eris::Entity * wrld)
{
    worldTime = SDL_GetTicks();
    if (charType == NULL) {
        charType = Eris::TypeInfo::findSafe("character");
    }
    drawEntity(wrld);
}

void DemeterScene::drawMap(Coal::Container & map_base, HeightMap & map_height)
{
    if (tilemap == NULL) {
        tilemap = new TileMap();
        tilemap->build(map_base);
    }
    origin();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTranslatef(-100.0f, -100.0f, 0.0f);

    tilemap->draw(map_height, x_offset, y_offset);

#if USE_DEMETER
    glTranslatef(0.0f, 0.0f, -4.0f);

    const float threshold = 12.0f;
    terrain->SetDetailThreshold(threshold);
    terrain->ModelViewMatrixChanged();
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrain->Render();

#endif // 0
}

void DemeterScene::drawGui()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -800.0f, 800.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    // glClear(GL_DEPTH_BUFFER_BIT);
    // glDisable(GL_CULL_FACE);
}

void DemeterScene::resize(int wdth, int hght)
{
    width = wdth;
    height = hght;
    shapeView();
}

void DemeterScene::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen

}
