// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Isometric.h"
#include "HeightMap.h"
#include "Texture.h"
#include "Sprite.h"
#include "Model.h"
#include "Tile.h"
#include "TileMap.h"

#include <app/WorldEntity.h>

#include <common/debug.h>

#include <Eris/Entity.h>
#include <Eris/TypeInfo.h>

#include <SDL_image.h>

#include <iostream>

static const bool debug_flag = false;

Isometric::Isometric(int wdth, int hght) : Renderer(wdth, hght),
                                           tilemap(NULL), charType(NULL)
{
    init();
}

void Isometric::init()
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
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    this->shapeView();

    model = new Model();
    if (!model->onInit(Datapath() + "paladin.cfg")) {
        std::cerr << "Loading paladin model failed" << std::endl << std::flush;
    }
    model->setLodLevel(1.0f);

    int textureUnits = -23;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &textureUnits);
    std::cout << "TEXTURE UNITS AVAILABLE: " << textureUnits
              << std::endl << std::flush;
}

void Isometric::update(float secs)
{
    model->onUpdate(secs);
}

void Isometric::viewPoint()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // this puts us in perpective projection
    //gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    // this puts us into orthographic perspective
    float xscale = width * scale / meterSize();
    float yscale = height * scale / meterSize();
    glOrtho(-xscale/2, xscale/2, -yscale/2, yscale/2, -2000.0, 2000.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    glTranslatef(0.0f, -1.0f, 0.0f);      // Aim to centre the character
    // GLfloat AmbientColor[] = {1, 0.5, 0.5, 1.0};
    // GLfloat DiffuseColor[] = {0, 1, 0, 1.0};
    // GLfloat LightPos[] = {xscale, yscale, 0.0, 1.0};
    // glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientColor);
    // glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseColor);
    // glLightfv(GL_LIGHT1, GL_POSITION,LightPos);
    // glEnable(GL_LIGHT1);
    // glEnable(GL_LIGHTING);
}

inline void Isometric::reorient()
{
    glRotatef(-rotation, 0.0, 0.0, 1.0);
    glRotatef(90-elevation, 1.0, 0.0, 0.0);
}

inline void Isometric::orient()
{
    glRotatef(elevation-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
}

inline void Isometric::translate()
{
    glTranslatef(-x_offset,-y_offset,-z_offset);
}

// This function moves the render cursor to the origin and rotates the
// axis to be inline with the worldforge axis
inline void Isometric::origin()
{
    viewPoint();
    orient();
    translate();
}

void Isometric::lightOn()
{
    glEnable(GL_LIGHTING);
}

void Isometric::lightOff()
{
    glDisable(GL_LIGHTING);
}

void Isometric::shapeView()
{
    //if (screen != NULL) {
        //SDL_FreeSurface(screen);
    //}
    if ((screen = SDL_SetVideoMode(width, height, 16,
            SDL_OPENGL|SDL_DOUBLEBUF|SDL_RESIZABLE)) == NULL) {
        std::cerr << "Failed to set video mode" << std::endl << std::flush;
        SDL_Quit();
        throw RendererSDLinit();
    }
    SDL_WM_SetCaption("apogee", "isometric");

    glViewport(0, 0, width, height);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // glShadeModel(GL_SMOOTH);
    viewPoint();
}

void Isometric::drawCal3DModel(Model * m, const Point3D & coords,
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

void Isometric::draw3DBox(const Point3D & coords, const WFMath::AxisBox<3> & bbox)
{
    lightOff();

    glPushMatrix();
    // origin();
    glTranslatef(coords.x(), coords.y(), coords.z());

    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    //glVertex3f(-box.X(),box.Y(),-box.Z());
    //glVertex3f(box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    //glVertex3f(-box.X(),box.Y(),box.Z());
    //glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    //glVertex3f(-box.X(),-box.Y(),box.Z());
    //glVertex3f(box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    //glVertex3f(-box.X(),-box.Y(),-box.Z());
    //glVertex3f(-box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    //glVertex3f(box.X(),-box.Y(),-box.Z());
    //glVertex3f(box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    //glVertex3f(-box.X(),box.Y(),-box.Z());
    //glVertex3f(-box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    //glVertex3f(box.X(),box.Y(),-box.Z());
    //glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    //glVertex3f(-box.X(),-box.Y(),-box.Z());
    //glVertex3f(-box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    //glVertex3f(box.X(),-box.Y(),-box.Z());
    //glVertex3f(box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    //glVertex3f(-box.X(),-box.Y(),box.Z());
    //glVertex3f(-box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    //glVertex3f(box.X(),-box.Y(),box.Z());
    //glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    lightOn();
    glPopMatrix();
}

void Isometric::draw3DArea(const Point3D & coords, const Vector3D & bbox,
                           const Vector3D & bmedian)
{
}

void Isometric::drawEntity(Eris::Entity * ent)
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

void Isometric::drawWorld(Eris::Entity * wrld)
{
    worldTime = SDL_GetTicks();
    if (charType == NULL) {
        charType = Eris::TypeInfo::findSafe("character");
    }
    drawEntity(wrld);
}

void Isometric::buildTileMap(Coal::Container & map_base)
{
    tilemap = new TileMap();
    tilemap->build(map_base);
}

void Isometric::drawMap(Coal::Container & map_base, HeightMap & map_height)
{
    if (tilemap == NULL) {
        buildTileMap(map_base);
    }
    origin();

    glTranslatef(-100.0f, -100.0f, 0.0f);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthMask(GL_FALSE);
    // glDisable(GL_DEPTH_TEST);

    tilemap->draw(map_height, x_offset, y_offset);

    // glDepthMask(GL_TRUE);
    // glEnable(GL_DEPTH_TEST);
}

void Isometric::drawGui()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -20.0f, 20.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    //glClear(GL_DEPTH_BUFFER_BIT);

}

void Isometric::resize(int wdth, int hght)
{
    width = wdth;
    height = hght;
    shapeView();
}

void Isometric::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen

}
