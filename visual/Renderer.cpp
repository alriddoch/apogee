// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "Renderer.h"
#include "Texture.h"
#include "Sprite.h"
#include "Model.h"

#include <app/WorldEntity.h>
#include <app/Application.h>

#include <common/debug.h>

#include <Eris/Entity.h>
#include <Eris/TypeInfo.h>
#include <Eris/Connection.h>

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>

#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include <SDL_image.h>

#include <GL/glu.h>

#include <iostream>

static const bool debug_flag = false;

#ifndef GL_EXT_compiled_vertex_array
PFNGLLOCKARRAYSEXTPROC glLockArraysExt = 0;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysExt = 0;
#endif // GL_EXT_compiled_vertex_array

Renderer::Renderer(Application & app, int wdth, int hght) : 
                                         screen(NULL),
                                         width(wdth), height(hght),
                                         elevation(30), rotation(45),
                                         scale(1), x_offset(0), y_offset(0),
                                         m_numLineIndeces(0),
                       m_lineIndeces(new unsigned int[(segSize + 1) * (segSize + 1) * 2]),
                       m_texCoords(new float[(segSize + 1) * (segSize + 1) * 3]),
                                         treemodel(0), treemodel_list(0), charType(0),
                                         application(app)



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

    // These should be turned on when running in production mode, but they
    // make using a debugger really hard and are only required in the perspective
    // client
    // SDL_WM_GrabInput(SDL_GRAB_ON);
    // SDL_ShowCursor(0);


    this->shapeView();

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

    treemodel = lib3ds_file_load("oak.3ds");
    if (!treemodel) {
        std::cerr << "Unable to load oak.3ds model file"
                  << std::endl << std::flush;
    } else {
        lib3ds_file_eval(treemodel,0);
    }

    int idx = -1;
    for (int i = 0; i < (segSize + 1) - 1; ++i) {
        for (int j = 0; j < (segSize + 1); ++j) {
            m_lineIndeces[++idx] = j * (segSize + 1) + i;
            m_lineIndeces[++idx] = j * (segSize + 1) + i + 1;
        }
        if (++i >= (segSize + 1) - 1) { break; }
        for (int j = (segSize + 1) - 1; j >= 0; --j) {
            m_lineIndeces[++idx] = j * (segSize + 1) + i + 1;
            m_lineIndeces[++idx] = j * (segSize + 1) + i;
        }
    }
    m_numLineIndeces = ++idx;

    int tidx = -1;
    for(int j = 0; j < (segSize + 1); ++j) {
        for(int i = 0; i < (segSize + 1); ++i) {
            m_texCoords[++tidx] = ((float)i)/8;
            std::cout << m_texCoords[tidx] << ":";
            m_texCoords[++tidx] = ((float)j)/8;
            std::cout << m_texCoords[tidx] << ":";
        }
    }
    std::cout << std::endl << std::flush;
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

    GLint realy;  /*  OpenGL y coordinate position  */

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
    glEnable(GL_LIGHTING);
}

void Renderer::lightOff()
{
    glDisable(GL_LIGHTING);
}

void Renderer::draw3dsFile(Lib3dsFile * node)
{
  int num_meshes = 0;
  if (!treemodel_list) {
    Lib3dsMesh *mesh;
    for (mesh=node->meshes; mesh!=0; mesh=mesh->next) {
      std::cout << "Mesh no. " << ++num_meshes << std::endl << std::flush;

      treemodel_list=glGenLists(1);
      glNewList(treemodel_list, GL_COMPILE);

      {
        unsigned p;
        Lib3dsVector *normalL = (Lib3dsVector *)malloc(3*sizeof(Lib3dsVector)*mesh->faces);

        {
          Lib3dsMatrix M;
          lib3ds_matrix_copy(M, mesh->matrix);
          lib3ds_matrix_inv(M);
          glMultMatrixf(&M[0][0]);
        }
        lib3ds_mesh_calculate_normals(mesh, normalL);

        std::cout << "COMPILING MODEL WITH " << mesh->faces << " faces and " << mesh->texels << " texels" << std::endl << std::flush;
        for (p = 0; p < mesh->faces; ++p) {
          Lib3dsFace *f=&mesh->faceL[p];
          Lib3dsMaterial *mat=0;
          GLint texture = -1;
          if (f->material[0]) {
            std::cout << "MATERIAL NAME: " << f->material
                      << std::endl << std::flush;
            mat=lib3ds_file_material_by_name(treemodel, f->material);
          }else{
            mat=node->materials;
          }
          if (mat) {
            static GLfloat a[4]={0,0,0,1};
            float s;
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
            s = pow(2, 10.0*mat->shininess);
            if (s>128.0) {
              s=128.0;
            }
            glMaterialf(GL_FRONT, GL_SHININESS, s);
            if (mat->texture1_map.name[0]) {
                std::cout << "TEXTURE NAME: " << mat->texture1_map.name
                          << std::endl << std::flush;
                texture = Texture::get(mat->texture1_map.name);
            }
          } else {
            Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
            Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
            Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
            glMaterialfv(GL_FRONT, GL_SPECULAR, s);
          }
          {
            if (texture != -1) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture);
            }
            int i;
            glBegin(GL_TRIANGLES);
              glNormal3fv(f->normal);
              for (i=0; i<3; ++i) {
                glNormal3fv(normalL[3*p+i]);
                if (texture != -1) {
                    glTexCoord2fv(mesh->texelL[f->points[i]]);
                }
                glVertex3fv(mesh->pointL[f->points[i]].pos);
              }
            glEnd();
            if (texture != -1) {
                glDisable(GL_TEXTURE_2D);
            }
          }
        }

        free(normalL);
      }

      glEndList();
    }
  }

    if (treemodel_list) {
      Lib3dsObjectData *d;

      glPushMatrix();
      // d=&node->data.object;
      // glMultMatrixf(&node->matrix[0][0]);
      // glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
      glScalef(0.1f, 0.1f, 0.1f);
      glCallList(treemodel_list);
      /*glutSolidSphere(50.0, 20,20);*/
      glPopMatrix();
    }
}

void Renderer::origin()
{
    viewPoint();
    orient();
    translate();
}


void Renderer::drawCal3DModel(Model * m, const Point3D & coords,
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


void Renderer::draw3DBox(const Point3D & coords, const WFMath::AxisBox<3> & bbox)
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

void Renderer::drawEntity(Eris::Entity * ent)
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
        Eris::TypeInfo * type = application.connection.getTypeInfoEngine()->findSafe(*e->getInherits().begin());
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

void Renderer::drawWorld(Eris::Entity * wrld)
{
    draw3dsFile(treemodel);


    worldTime = SDL_GetTicks();
    if (charType == NULL) {
        charType = application.connection.getTypeInfoEngine()->findSafe("character");
    }
    drawEntity(wrld);
}

void Renderer::drawRegion(Mercator::Segment * map)
{
    GLint texture = -1;
    texture = Texture::get("rabbithill_grass_hh.png");
    float * harray = new float[(segSize + 1) * (segSize + 1) * 3];
    int idx = -1, cdx = -1;
    for(int j = 0; j < (segSize + 1); ++j) {
        for(int i = 0; i < (segSize + 1); ++i) {
            float h = map->get(i,j);
            harray[++idx] = i;
            harray[++idx] = j;
            harray[++idx] = h;
        }
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    if (texture != -1) {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    glVertexPointer(3, GL_FLOAT, 0, harray);
    // if (have_GL_EXT_compiled_vertex_array) {
        // glLockArraysEXT(0, (segSize + 1) * (segSize + 1));
    // }
    glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                   GL_UNSIGNED_INT, m_lineIndeces);
    // if (have_GL_EXT_compiled_vertex_array) {
        // glUnlockArraysEXT();
    // }
    if (texture != -1) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    delete harray;
}

void Renderer::drawMap(Mercator::Terrain & t)
{
    origin();

    const Mercator::Terrain::Segmentstore & segs = t.getTerrain();

    Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            drawRegion(J->second);
            glPopMatrix();
        }
    }
}

void Renderer::drawGui()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -800.0f, 800.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View
    // glClear(GL_DEPTH_BUFFER_BIT);
    // glDisable(GL_CULL_FACE);
}

void Renderer::resize(int wdth, int hght)
{
    width = wdth;
    height = hght;
    shapeView();
}

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen

}



