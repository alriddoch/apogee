// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include <coal/database.h>
#include <coal/region.h>

#include <Eris/Entity.h>
#include <Eris/TypeInfo.h>

#include <iostream>

#include <SDL_image.h>

#include "Isometric.h"
#include "Texture.h"
#include "Sprite.h"
#include "Model.h"
#include "Tile.h"

#include <app/WorldEntity.h>

#include <common/debug.h>

static const bool debug_flag = false;

Isometric::Isometric(int wdth, int hght) : Renderer(wdth, hght), charType(NULL)
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
    model->setLodLevel(0.01f);
}

inline void Isometric::viewScale(float scale_factor)
{
#if 1
    glScalef(scale_factor, scale_factor, scale_factor);
#else
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float xscale = width * scale * scale_factor / meterSize();
    float yscale = height * scale * scale_factor / meterSize();
    glOrtho(-xscale/2.0f, xscale/2.0f,
            -yscale/2.0f, yscale/2.0f,
            -2000.0f * scale_factor, 2000.0f * scale_factor );
    glMatrixMode(GL_MODELVIEW);
#endif
}

inline void Isometric::viewPoint()
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
    glTranslatef(-x_offset,-y_offset,0.0f);
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

void Isometric::drawCharacter(Sprite * character, float x, float y)
{
    origin();
    glTranslatef(x,y,0);
    reorient();
    character->draw();
}

#if 0
void Isometric::draw2Dtest()
{
    lightOff();
    if (button == NULL) {
        button = IMG_Load("button_base.png");
        // std::cerr << "Could not load test image" << std::endl << std::flush;
    }
    SDL_Rect destRect = { width - button->w - 8, 32, button->w, button->h };
    for(int i=0; i < 8; i++) {
        destRect.x = width - button->w * (1 + i / 4) - 4;
        destRect.y = 4 + (i % 4) * 24;
        SDL_BlitSurface(button, NULL, screen, &destRect);
    }
    destRect.x = width - button->w * 2 - 4;
    destRect.y = 4;
    destRect.w = button->w * 2;
    destRect.h = button->h * 4;
    SDL_UpdateRects(screen, 1, &destRect);
    lightOn();
}

void Isometric::draw3Dtest()
{
    viewPoint();

    glBindTexture(GL_TEXTURE_2D, Texture::get("texture.png"));
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    origin();
        
    for(int i = 0; i < 4; i++) {
        // draw a square (quadrilateral)

        glBegin(GL_QUADS);                 // start drawing a polygon (4 sided)
        glColor3f(1, 0, 0);
        glVertex3f(-0.5, 0.5, 0.0);        // Top Left
        glColor3f(1, 1, 0);
        glVertex3f( 0.5, 0.5, 0.0);        // Top Right
        glColor3f(0, 1, 1);
        glVertex3f( 0.5,-0.5, 0.0);        // Bottom Right
        glColor3f(0, 0, 1);
        glVertex3f(-0.5,-0.5, 0.0);        // Bottom Left  
        glEnd();                              // done with the polygon
      
        glTranslatef(1.0,0.0,0.0);         // Move Right 3 Units
    }
}
#endif

void Isometric::draw3Dentity()
{
    origin();
    glTranslatef(4,4,0.0);
    // Lets draw a building
    glBindTexture(GL_TEXTURE_2D, Texture::get("media/media-3d/collection-pegasus/textures_512x512/buildings/wall_sandstone_red_1_orig.png"));
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(0, 1); glVertex3f(0, 0, 5);
    glTexCoord2f(1, 1); glVertex3f(5, 0, 5);
    glTexCoord2f(1, 0); glVertex3f(5, 0, 0);

    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(0, 5, 0);
    glTexCoord2f(0, 1); glVertex3f(0, 5, 5);
    glTexCoord2f(1, 1); glVertex3f(0, 0, 5);
    glTexCoord2f(1, 0); glVertex3f(0, 0, 0);
 
    glNormal3f(1, 0, 0);
    glTexCoord2f(1, 0); glVertex3f(0, 5, 0);
    glTexCoord2f(1, 1); glVertex3f(0, 5, 5);
    glTexCoord2f(0, 1); glVertex3f(5, 5, 5);
    glTexCoord2f(0, 0); glVertex3f(5, 5, 0);
 
    glNormal3f(0, 1, 0);
    glTexCoord2f(1, 0); glVertex3f(5, 5, 0);
    glTexCoord2f(1, 1); glVertex3f(5, 5, 5);
    glTexCoord2f(0, 1); glVertex3f(5, 0, 5);
    glTexCoord2f(0, 0); glVertex3f(5, 0, 0);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, Texture::get("media/media-3d/collection-pegasus/textures_512x512/floor/wood_massiv_1_orig.png"));
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 4.8);
    glTexCoord2f(0, 1); glVertex3f(0, 5, 4.8);
    glTexCoord2f(1, 1); glVertex3f(5, 5, 4.8);
    glTexCoord2f(1, 0); glVertex3f(5, 0, 4.8);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Isometric::drawCal3DModel(Model * m, const Vector3D & coords,
                               const Eris::Quaternion & orientation)
{
    glPushMatrix();
    glTranslatef(coords.X(), coords.Y(), coords.Z());
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    float orient[4][4];
    orientation.asMatrix(orient);
    float omatrix[16];
    glMultMatrixf(&orient[0][0]);
    viewScale(0.025f);
    m->onRender();
    glPopMatrix();
}

void Isometric::draw3DBox(const Vector3D & coords, const Eris::BBox & bbox)
{
    lightOff();

    glPushMatrix();
    // origin();
    glTranslatef(coords.X(), coords.Y(), coords.Z());

    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);
    //glVertex3f(-box.X(),box.Y(),-box.Z());
    //glVertex3f(box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);
    //glVertex3f(-box.X(),box.Y(),box.Z());
    //glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);
    //glVertex3f(-box.X(),-box.Y(),box.Z());
    //glVertex3f(box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);
    //glVertex3f(-box.X(),-box.Y(),-box.Z());
    //glVertex3f(-box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);
    //glVertex3f(box.X(),-box.Y(),-box.Z());
    //glVertex3f(box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);
    //glVertex3f(-box.X(),box.Y(),-box.Z());
    //glVertex3f(-box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);
    //glVertex3f(box.X(),box.Y(),-box.Z());
    //glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);
    //glVertex3f(-box.X(),-box.Y(),-box.Z());
    //glVertex3f(-box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);
    //glVertex3f(box.X(),-box.Y(),-box.Z());
    //glVertex3f(box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);
    //glVertex3f(-box.X(),-box.Y(),box.Z());
    //glVertex3f(-box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);
    //glVertex3f(box.X(),-box.Y(),box.Z());
    //glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    lightOn();
    glPopMatrix();
}

void Isometric::draw3DArea(const Vector3D & coords, const Vector3D & bbox,
                         const Vector3D & bmedian)
{
}

void Isometric::drawEntity(Eris::Entity * ent)
{
    glPushMatrix();
    const Eris::Coord & pos = ent->getPosition();
    glTranslatef(pos.x, pos.y, pos.z);
    int numEnts = ent->getNumMembers();
    debug(cout << ent->getID() << " " << numEnts << " emts" << endl << flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        Vector3D pos = Vector3D(e->getPosition());
        WorldEntity * we = dynamic_cast<WorldEntity *>(e);
        if (we != NULL) {
            debug( cout << Vector3D(e->getVelocity()) << " " << (worldTime - we->getTime()) << " " << pos; );
            pos = pos + Vector3D(e->getVelocity()) * (double)((worldTime - we->getTime())/1000.0f);
            debug( cout << "=" << pos << endl << flush; );
        } else {
            cout << "Eris::Entity \"" << e->getID() << "\" is not a WorldEntity" << endl << flush;
        }
        debug(std::cout << ":" << e->getID() << e->getPosition() << ":"
                        << e->getBBox().u << e->getBBox().v
                        << std::endl << std::flush;);
        if (!e->isVisible()) { continue; }
        Eris::TypeInfo * type = Eris::TypeInfo::findSafe(*e->getInherits().begin());
        if (type->safeIsA(charType)) {
            drawCal3DModel(model, pos, e->getOrientation());
        } else {
            draw3DBox(pos, e->getBBox());
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
    model->onUpdate(0.1);
    drawEntity(wrld);
}

#if 0
void Isometric::drawMapRegion(CoalRegion & map_region)
{
    int tex_id = -1;
    CoalFill * fill = map_region.GetFill();
    if (fill != NULL) {
        if ((fill->graphic != NULL) && (fill->graphic->filename.size() != 0)) {
            tex_id = Texture::get(fill->graphic->filename);
        }
        if (tex_id == -1) {
            CoalGraphic * fillGraphic = fill->graphic;
            float r = fillGraphic->bkgdcolor.red;
            float g = fillGraphic->bkgdcolor.green;
            float b = fillGraphic->bkgdcolor.blue;
            //std::cout << "[" << r << "," << g << "," << b
                      //<< std::endl << std::flush;
            glBegin(GL_POLYGON);                // start drawing a polygon
            glColor3f(r/100, g/100, b/100);
        } else {
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_POLYGON);                // start drawing a polygon
            glColor3f(1.0, 0.0, 1.0);
        }
    }
    glNormal3f(0, 0, 1);
    CoalShape & shape = map_region;
    for (unsigned int i = 0; i < shape.GetCoordCount (); i++) {
        const CoalCoord & coord = shape.GetCoord(i);
        float x = coord.GetX();
        float y = coord.GetY();
        float z = coord.GetZ();
        //std::cout << "[" << x << "," << y << "z" << z << std::endl
                  //<< std::flush;
        glTexCoord2f(x/16, y/16); glVertex3f(x, y, z);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Isometric::drawMapObject(CoalObject & map_object)
{
    
}

void Isometric::drawMap(CoalDatabase & map_base)
{
    origin();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthMask(GL_FALSE);
    int count = map_base.GetRegionCount();
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region) {
            drawMapRegion(*region);
        }
    }
    // glDepthMask(GL_TRUE);

    count = map_base.GetObjectCount();
    for (int i = 0; i < count; i++) {
        CoalObject * object = (CoalObject*)map_base.GetObject(i);
        if (object) {
            drawMapObject(*object);
        }
    }

}
#else

void Isometric::drawMapRegion(CoalRegion & map_region)
{
    Tile * tile = NULL;
    CoalFill * fill = map_region.GetFill();
    if (fill != NULL) {
        if ((fill->graphic != NULL) && (fill->graphic->filename.size() != 0)) {
            tile = Tile::get(fill->graphic->filename);
        }
        glNormal3f(0, 0, 1);
        CoalShape & shape = map_region;
        if (shape.GetCoordCount() != 2) {
            cout << "This don't look like a tile" << endl << flush;
        } else {
            //for (unsigned int i = 0; i < shape.GetCoordCount (); i++) {
            glPushMatrix();
            const CoalCoord & coord = shape.GetCoord(0);
            float bx = coord.GetX();
            float by = coord.GetY();
            float bz = coord.GetZ();
            glTranslatef(bx, by, bz);
            if (tile != NULL) {
                tile->draw();
            } else {
                CoalGraphic * fillGraphic = fill->graphic;
                float r = fillGraphic->bkgdcolor.red;
                float g = fillGraphic->bkgdcolor.green;
                float b = fillGraphic->bkgdcolor.blue;
                glBegin(GL_QUADS);                // start drawing a polygon
                glColor3f(r/100, g/100, b/100);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(1.0f, 0.0f, 0.0f);
                glVertex3f(1.0f, 1.0f, 0.0f);
                glVertex3f(0.0f, 1.0f, 0.0f);
                glEnd();
            }
            glPopMatrix();
        }
    }
}

void Isometric::drawMapObject(CoalObject & map_object)
{
    
}

void Isometric::drawMap(CoalDatabase & map_base)
{
    origin();

    glTranslatef(-100.0f, -100.0f, 0.0f);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthMask(GL_FALSE);
    // glDisable(GL_DEPTH_TEST);
    int count = map_base.GetRegionCount();
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region) {
            drawMapRegion(*region);
        }
    }
    // glDepthMask(GL_TRUE);
    // glEnable(GL_DEPTH_TEST);

    count = map_base.GetObjectCount();
    for (int i = 0; i < count; i++) {
        CoalObject * object = (CoalObject*)map_base.GetObject(i);
        if (object) {
            drawMapObject(*object);
        }
    }
}
#endif

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
