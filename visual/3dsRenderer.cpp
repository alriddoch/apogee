// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "3dsRenderer.h"

#include "GL.h"
#include "Texture.h"

#include "common/debug.h"
#include "common/system.h"

#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include <iostream>
#include <cassert>

static const bool debug_flag = true;

void m3dsRenderer::compileVertexBuffer(Lib3dsMesh *mesh, VertexBuffer * vb)
{
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // This must be true, or we might crash
    assert(mesh->texels >= mesh->points);
           
    float * points = new float[3 * mesh->points];
    float * normals = new float[3 * mesh->points];
    float * texcoords = new float[2 * mesh->points];
    unsigned * indices = new unsigned[3 * mesh->faces];
    unsigned * iptr = indices - 1;
    // lib3ds_mesh_calculate_normals(mesh, normalL);

    for (unsigned p = 0; p < mesh->points; ++p) {
      memcpy(&points[p*3], &mesh->pointL[p].pos[0], 3 * sizeof(float));
      memcpy(&texcoords[p*2], &mesh->texelL[p][0], 2 * sizeof(float));
    }
    for (unsigned p = 0; p < mesh->faces; ++p) {
      Lib3dsFace * f = &mesh->faceL[p];
      for (unsigned i = 0; i < 3; ++i) {
        unsigned idx = f->points[i];
        memcpy(&normals[idx * 3], f->normal, sizeof(Lib3dsVector));
        *++iptr = idx;
      }
    }

    vb->bobject = glGenLists(1);
    glNewList(vb->bobject, GL_COMPILE);

    Lib3dsMatrix M;
    lib3ds_matrix_copy(M, mesh->matrix);
    lib3ds_matrix_inv(M);
    glMultMatrixf(&M[0][0]);

    glVertexPointer(3, GL_FLOAT, 0, points);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
    glNormalPointer(GL_FLOAT, 0, normals);

    Lib3dsMaterial *mat=0;
    GLuint texture = 0;
    unsigned begin = 0;

    for (unsigned p = 0; p < mesh->faces; ++p) {
      Lib3dsFace *f=&mesh->faceL[p];
      if (f->material[0]) {
        Lib3dsMaterial * m = lib3ds_file_material_by_name(m_model,
                                                          f->material);
        if (m != 0) {
          if (m != mat) {
            std::cout << "Material " << m << " : \"" << f->material << "\""
                      << std::endl << std::flush;
            mat = m;
            std::cout << "DRAW " << p << ": " << begin << std::endl << std::flush;
            glDrawElements(GL_TRIANGLES, (p - begin) * 3, GL_UNSIGNED_INT,
                           &indices[begin * 3]);
            begin = p;
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat->ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
            float s = pow(2, 10.0*mat->shininess);
            if (s>128.0) {
              s=128.0;
            }
            glMaterialf(GL_FRONT, GL_SHININESS, s);
            if (mat->texture1_map.name[0]) {
              std::cout << "TEXTURE NAME: " << mat->texture1_map.name
                        << std::endl << std::flush;
              texture = Texture::get(mat->texture1_map.name, false, GL_LINEAR_MIPMAP_NEAREST);
              glEnable(GL_TEXTURE_2D);
              glBindTexture(GL_TEXTURE_2D, texture);
            } else {
              glDisable(GL_TEXTURE_2D);
            }
          }
        } else {
          mat = 0;
          std::cout << "NDRAW " << p << ": " << begin << std::endl << std::flush;
          glDrawElements(GL_TRIANGLES, (p - begin) * 3, GL_UNSIGNED_INT,
                         &indices[begin * 3]);
          begin = p;
          static const Lib3dsRgba a = {0.2f, 0.2f, 0.2f, 1.0f};
          static const Lib3dsRgba d = {0.8f, 0.8f, 0.8f, 1.0f};
          static const Lib3dsRgba s = {0.0f, 0.0f, 0.0f, 1.0f};
          glMaterialfv(GL_FRONT, GL_AMBIENT, a);
          glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
          glMaterialfv(GL_FRONT, GL_SPECULAR, s);
          glDisable(GL_TEXTURE_2D);
        }
      }

    }
    glDrawElements(GL_TRIANGLES, (mesh->faces - begin) * 3, GL_UNSIGNED_INT,
                         &indices[begin * 3]);
    std::cout << "FDRAW " << mesh->faces << ": " << begin << std::endl << std::flush;

    delete [] points;
    delete [] normals;
    delete [] texcoords;

    glEndList();
}

void m3dsRenderer::draw3dsFile()
{
    Lib3dsNode * p = m_model->nodes;
    if (p != 0) {
        for (; p != 0; p = p->next) {
            draw3dsNode(p);
        }
    } else {
        for (Lib3dsMesh * mesh = m_model->meshes; mesh!=0; mesh=mesh->next) {
            draw3dsMesh(mesh);
        }
    }
}

void m3dsRenderer::draw3dsMesh(Lib3dsMesh * mesh)
{
    if (!mesh->user.p) {
        debug(std::cout << "Drawing as mesh" << std::endl << std::flush;);
        VertexBuffer * vb = new VertexBuffer();
        mesh->user.p = (void*)vb;

        compileVertexBuffer(mesh, vb);
    }
   
    VertexBuffer * vb = (VertexBuffer*)mesh->user.p;

    if (!glIsList(vb->bobject)) {
        std::cout << "display list for 3ds model is no longer a display list in this context" << std::endl << std::flush;
    }
    glCallList(vb->bobject);
}

void m3dsRenderer::draw3dsNode(Lib3dsNode * node)
{
    for (Lib3dsNode * p=node->childs; p!=0; p=p->next) {
      draw3dsNode(p);
    }

    if (node->type != LIB3DS_OBJECT_NODE) {
      return;
    }

    if (strcmp(node->name,"$$$DUMMY")==0) {
      return;
    }

    if (!node->user.p) {
        debug(std::cout << "Drawing as node" << std::endl << std::flush;);
        Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(m_model, node->name);
        ASSERT(mesh);

        if (!mesh) {
          return;
        }

        std::cout << "Node" << std::endl << std::flush;

        VertexBuffer * vb = new VertexBuffer();
        node->user.p = (void*)vb;

        compileVertexBuffer(mesh, vb);
    }

    Lib3dsObjectData *d;
    VertexBuffer * vb = (VertexBuffer*)node->user.p;

    glPushMatrix();
    d=&node->data.object;
    glMultMatrixf(&node->matrix[0][0]);
    glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
    if (!glIsList(vb->bobject)) {
        std::cout << "display list for 3ds model is no longer a display list in this context" << std::endl << std::flush;
    }
    glCallList(vb->bobject);
    glPopMatrix();
}

m3dsRenderer::m3dsRenderer(Renderer & r, Eris::Entity & e) : EntityRenderer(r, e)
{
}

m3dsRenderer::~m3dsRenderer()
{
}

void m3dsRenderer::load(const std::string & filename)
{
    std::string fullPath = getMediaPath() + "/" + filename;
    debug(std::cout << "LOADING " << fullPath << std::endl << std::flush;);
    m_model = lib3ds_file_load(fullPath.c_str());
    if (m_model == 0) {
        std::cerr << "Failed to load 3ds model \"" << fullPath << "\""
                  << std::endl << std::flush;
    } else {
        lib3ds_file_eval(m_model, 0);
    }
}

void m3dsRenderer::render(Renderer &, const PosType &)
{
    if (m_model != 0) {
        glPushMatrix();
        // Nasty artefacts. To make ALPHA look okay, need to do 2 passes,
        // second with alpha enabled, depth write disabled, and GL_LESS
        // Alpha test turned on.
        // glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_NORMALIZE);
        glAlphaFunc(GL_GREATER, 0.2f);
        glScalef(5.f, 5.f, 5.f);
        draw3dsFile();
        glDisable(GL_NORMALIZE);
        glDisable(GL_ALPHA_TEST);
        // glDisable(GL_BLEND);
        glPopMatrix();
    }
}

void m3dsRenderer::select(Renderer & r, const PosType & p)
{
    render(r, p);
}
