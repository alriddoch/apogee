// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include <coal/database.h>

#include <iostream>

#include <SDL_image.h>

#include "DemeterScene.h"
#include "Texture.h"
#include "Sprite.h"
#include "Model.h"
#include "Matrix.h"

const float PI = 3.1459f;
const float FOG_RED = 0.5f;
const float FOG_GREEN = 0.75f;
const float FOG_BLUE = 1.0f;
const float FOG_ALPHA = 0.0f;

DemeterScene::DemeterScene(int wdth, int hght) : Renderer(wdth, hght)
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
    SDL_WM_SetCaption("apogee", "demeter");

    settings = Demeter::Settings::GetInstance();
    settings->SetMediaPath("maps/");
    settings->SetScreenWidth(width);
    settings->SetScreenWidth(height);

    this->shapeView();

    const int maxNumVisibleTriangles = 40000;

    terrain = new Demeter::Terrain("Test.map", maxNumVisibleTriangles, false);
    terrain->SetMaximumVisibleBlockSize(64);
    terrain->SetCommonTextureRepeats(50.0f);

    cout << "Loaded Terrain " << terrain->GetWidth() << " : "
         << terrain->GetHeight()
         << endl << flush;
    cameraPosition.x = terrain->GetWidth() / 2.0f ;//- 400.0f;
    cameraPosition.y = terrain->GetHeight() / 2.0f ;//- 351.0f;
    cameraPosition.z = 200.0f;
    camera.SetPosition(cameraPosition.x,cameraPosition.y,cameraPosition.z); 
    cameraAngle.x = 0.0f;
    cameraAngle.y = 0.0f;
    cameraAngle.z = 0.0f;


}

void DemeterScene::shapeView()
{
    const float maxViewDistance = 4500.0f;
    //if (screen != NULL) {
        //SDL_FreeSurface(screen);
    //}
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
    glDepthFunc(GL_LESS);
    glDisable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
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
    const float maxViewDistance = 4500.0f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float xscale = 0.5f / scale_factor;
    float yscale = 0.5f / scale_factor;
    // This is complete arse
    // glFrustum(-xscale, xscale, -yscale, yscale, 0.65f / scale_factor, maxViewDistance * scale_factor );
    gluPerspective(45.0f, (float)width/(float)height,0.65f, maxViewDistance);
    glMatrixMode(GL_MODELVIEW);
}

inline void DemeterScene::viewPoint()
{
    const float maxViewDistance = 4500.0f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width/(float)height,0.65f, maxViewDistance);
    // This is copied from the demeter demo app
    // glFrustum(-0.5f,0.5f,-0.5f,0.5f,0.65f,maxViewDistance);
    // this puts us into orthographic perspective
    // float xscale = width * scale / meterSize();
    // float yscale = height * scale / meterSize();
    // glOrtho(-xscale/2, xscale/2, -yscale/2, yscale/2, -20.0, 20.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
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
}

inline void DemeterScene::translate()
{
    glTranslatef(-x_offset,-y_offset,0.0f);
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
}

void DemeterScene::lightOff()
{
    glDisable(GL_LIGHTING);
}

void DemeterScene::drawCharacter(Sprite * character, float x, float y)
{
    origin();
    glTranslatef(x,y,0.0f);
    reorient();
    character->draw();
}

void DemeterScene::draw3Dentity()
{
    origin();
    glTranslatef(4,4,0.0);
    // Lets draw a building
    glBindTexture(GL_TEXTURE_2D, Texture::get("media-3d/collection-pegasus/textures_512x512/buildings/wall_sandstone_red_1_orig.png"));
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glNormal3f(0, -1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);
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
    glBindTexture(GL_TEXTURE_2D, Texture::get("media-3d/collection-pegasus/textures_512x512/floor/wood_massiv_1_orig.png"));
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 4.8);
    glTexCoord2f(0, 1); glVertex3f(0, 5, 4.8);
    glTexCoord2f(1, 1); glVertex3f(5, 5, 4.8);
    glTexCoord2f(1, 0); glVertex3f(5, 0, 4.8);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void DemeterScene::drawCal3DModel(Model * m)
{
    viewScale(40);
    m->onRender();
    viewScale(1);
}

void DemeterScene::draw3DBox(const Vector3D & coords, const Vector3D & bbox,
                         const Vector3D & bmedian)
{
    Vector3D def(0.2,0.2,0.2);
    const Vector3D & box = bbox ? bbox : def;
    const Vector3D & median = bmedian ? bmedian : box;
    lightOff();

    origin();
    glTranslatef(coords.X()+median.X(),
                 coords.Y()+median.Y(),
                 coords.Z()+median.Z());
    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-box.X(),-box.Y(),-box.Z());
    glVertex3f(box.X(),-box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-box.X(),box.Y(),-box.Z());
    glVertex3f(box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-box.X(),box.Y(),box.Z());
    glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-box.X(),-box.Y(),box.Z());
    glVertex3f(box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-box.X(),-box.Y(),-box.Z());
    glVertex3f(-box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(box.X(),-box.Y(),-box.Z());
    glVertex3f(box.X(),-box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-box.X(),box.Y(),-box.Z());
    glVertex3f(-box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(box.X(),box.Y(),-box.Z());
    glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-box.X(),-box.Y(),-box.Z());
    glVertex3f(-box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(box.X(),-box.Y(),-box.Z());
    glVertex3f(box.X(),box.Y(),-box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-box.X(),-box.Y(),box.Z());
    glVertex3f(-box.X(),box.Y(),box.Z());
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(box.X(),-box.Y(),box.Z());
    glVertex3f(box.X(),box.Y(),box.Z());
    glEnd();

    lightOn();
}

void DemeterScene::drawMapRegion(CoalRegion & map_region)
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

void DemeterScene::drawMapObject(CoalObject & map_object)
{
    
}

void DemeterScene::drawMap(CoalDatabase & map_base)
{
    origin();

#if 0
    glDepthMask(GL_FALSE);
    int count = map_base.GetRegionCount();
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region) {
            drawMapRegion(*region);
        }
    }
    glDepthMask(GL_TRUE);

    count = map_base.GetObjectCount();
    for (int i = 0; i < count; i++) {
        CoalObject * object = (CoalObject*)map_base.GetObject(i);
        if (object) {
            drawMapObject(*object);
        }
    }
#endif

    cameraAngle.z = -rotation * PI / 180;
    cameraAngle.x = -elevation * PI / 180;
    cout << elevation << "}{" << cameraAngle.x << endl << flush;

    Matrix rotateX,rotateZ,cameraTransform;
    rotateX.SetRotateX(cameraAngle.x);
    rotateZ.SetRotateZ(cameraAngle.z);
    cameraTransform = rotateX * rotateZ;
    lookAt.x = 0.0f;
    lookAt.y = 100000.0f;
    lookAt.z = 0.0f;
    lookUp.x = 0.0f;
    lookUp.y = 0.5f;
    lookUp.z = 1.0f;
    lookUp.Normalize();
    Demeter::Vector currentLookAt = cameraTransform * lookAt;
    Demeter::Vector currentLookUp = cameraTransform * lookUp;
    camera.SetLookAt(cameraPosition.x + currentLookAt.x,cameraPosition.y + currentLookAt.y,cameraPosition.z + currentLookAt.z);
    camera.SetLookUp(currentLookUp.x,currentLookUp.y,currentLookUp.z);
    float currentCameraElevation = terrain->GetElevation(cameraPosition.x,cameraPosition.y) + cameraPosition.z;
    camera.SetPosition(cameraPosition.x,cameraPosition.y,currentCameraElevation);
    camera.UpdateViewTransform();

    const float threshold = 12.0f;
    terrain->SetDetailThreshold(threshold);
    terrain->ModelViewMatrixChanged();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrain->Render();

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
