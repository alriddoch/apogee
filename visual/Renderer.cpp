// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "Renderer.h"
#include "EntityRenderer.h"
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
                                         m_numLineIndeces(0),
                       m_lineIndeces(new unsigned int[(segSize + 1) * (segSize + 1) * 2]),
                       m_texCoords(new float[(segSize + 1) * (segSize + 1) * 3]),
                                         treemodel(0), treemodel_list(0), charType(0),
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

    // treemodel = lib3ds_file_load("oak.3ds");
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
            // std::cout << m_texCoords[tidx] << ":";
            m_texCoords[++tidx] = ((float)j)/8;
            // std::cout << m_texCoords[tidx] << ":";
        }
    }
    // std::cout << std::endl << std::flush;
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
      // Lib3dsObjectData *d;

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

#if 0
void Renderer::selectCal3DModel(Model * m, const WFMath::Quaternion & orientation)
{
    glPushMatrix();
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
    m->onSelect();
    glPopMatrix();
}

void Renderer::drawCal3DModel(Model * m, const WFMath::Quaternion & orientation)
{
    glPushMatrix();
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

/*
 * This is the vertex layout used by the 2 3Dbox functions.
 *
 *
 *                                   6
 *
 *
 *                         7                    5
 *
 *
 *                                   4
 *
 *
 *
 *                                   2
 *
 *
 *                         3                    1
 *
 *
 *                                   0
 */

void Renderer::select3DBox(const WFMath::AxisBox<3> & bbox)
{
    GLfloat vertices[] = {
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z() 
    };
    static const GLuint indices[] = { 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6,
                                      3, 0, 4, 7, 4, 5, 6, 7, 0, 3, 2, 1 };
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    if (have_GL_EXT_compiled_vertex_array) {
        glLockArraysEXT(0, 8);
    }
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, indices);
    if (have_GL_EXT_compiled_vertex_array) {
        glUnlockArraysEXT();
    }
}

void Renderer::draw3DBox(const WFMath::AxisBox<3> & bbox)
{
    GLfloat vertices[] = {
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z() 
    };
    static const GLuint indices[] = { 0, 1, 3, 2, 7, 6, 4, 5, 0, 4, 1, 5,
                                      3, 7, 2, 6, 0, 3, 1, 2, 4, 7, 5, 6 };

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    if (have_GL_EXT_compiled_vertex_array) {
        glLockArraysEXT(0, 8);
    }
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, indices);
    if (have_GL_EXT_compiled_vertex_array) {
        glUnlockArraysEXT();
    }
}
#endif

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
        re->m_drawer->render(*this);
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
    // draw3dsFile(treemodel);

    worldTime = SDL_GetTicks();
    if (charType == NULL) {
        charType = application.connection.getTypeInfoEngine()->findSafe("character");
    }
    drawEntity(wrld);
}

#if 0
void Renderer::drawRegion(Mercator::Segment * map)
{
    GLint texture = -1, texture2 = -1;
    texture = Texture::get("granite.png");
    texture2 = Texture::get("rabbithill_grass_hh.png");
    static float * harray = 0;
    static float * carray = 0;
    static int allocated_segSize = 0;
    // Only re-allocate the vertex arrays if we are dealing with a different
    // segment size.
    if (segSize != allocated_segSize) {
        if (harray != 0) {
            delete [] harray;
            delete [] carray;
            harray = 0;
            carray = 0;
        }
        harray = new float[(segSize + 1) * (segSize + 1) * 3];
        carray = new float[(segSize + 1) * (segSize + 1) * 4];
        allocated_segSize = segSize;
        int idx = -1, cdx = -1;
        // Fill in the invarient vertices and colors, so we only do it once
        for(int j = 0; j < (segSize + 1); ++j) {
            for(int i = 0; i < (segSize + 1); ++i) {
                // float h = map->get(i,j);
                harray[++idx] = i;
                harray[++idx] = j;
                harray[++idx] = 0.f;
                carray[++cdx] = 1.f;
                carray[++cdx] = 1.f;
                carray[++cdx] = 1.f;
                carray[++cdx] = 0.f;
            }
        }
    }
    // Fill in the vertex Z coord, and alpha value, which vary
    int idx = -1, cdx = -1;
    for(int j = 0; j < (segSize + 1); ++j) {
        for(int i = 0; i < (segSize + 1); ++i) {
            float h = map->get(i,j);
            idx += 2;
            harray[++idx] = h;
            cdx += 3;
            carray[++cdx] = (h > 0.4f) ? 1.f : 0.f;
        }
    }
    if (texture != -1) {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);
        glColorPointer(4, GL_FLOAT, 0, carray);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    glVertexPointer(3, GL_FLOAT, 0, harray);
    if (have_GL_EXT_compiled_vertex_array) {
        glLockArraysEXT(0, (segSize + 1) * (segSize + 1));
    }
    glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                   GL_UNSIGNED_INT, m_lineIndeces);
    if ((texture != -1) && (texture2 != -1)) {
        glBindTexture(GL_TEXTURE_2D, texture2);
        glEnable(GL_BLEND);
        glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_INT, m_lineIndeces);
        glDisable(GL_BLEND);
    }
    if (have_GL_EXT_compiled_vertex_array) {
        glUnlockArraysEXT();
    }
    if (texture != -1) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
}

void Renderer::drawMap(Mercator::Terrain & t)
{
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
#endif

void Renderer::drawSea(Mercator::Terrain & t)
{
    const Mercator::Terrain::Segmentstore & segs = t.getTerrain();

    Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
    glEnable(GL_BLEND);
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            GLfloat vertices[] = { 0.f, 0.f, 0.f,
                                   segSize, 0, 0.f,
                                   segSize, segSize, 0.f,
                                   0, segSize, 0.f };
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glColor4f(0.8f, 0.8f, 1.f, 0.6f);
            glDrawArrays(GL_QUADS, 0, 4);
            glPopMatrix();
        }
    }
    glDisable(GL_BLEND);
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
