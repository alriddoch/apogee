// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "3dsRenderer.h"

#include "GL.h"
#include "Texture.h"

#include "common/system.h"

#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include <iostream>

void m3dsRenderer::draw3dsFile()
{
    Lib3dsNode *p;
    for (p = m_model->nodes; p != 0; p = p->next) {
        draw3dsNode(p);
    }
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
        Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(m_model, node->name);
        ASSERT(mesh);
        if (!mesh) {
          return;
        }
        std::cout << "Node" << std::endl << std::flush;

        Lib3dsVector * normalL= new Lib3dsVector[3 * mesh->faces];
        lib3ds_mesh_calculate_normals(mesh, normalL);

        VertexBuffer * vb = new VertexBuffer();
        node->user.p = (void*)vb;

        vb->bobject = glGenLists(1);
        glNewList(vb->bobject, GL_COMPILE);

        Lib3dsMatrix M;
        lib3ds_matrix_copy(M, mesh->matrix);
        lib3ds_matrix_inv(M);
        glMultMatrixf(&M[0][0]);

        glBegin(GL_TRIANGLES);

        Lib3dsMaterial *mat=0;
        GLuint texture = 0;
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
                glEnd();
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
                  texture = Texture::get(mat->texture1_map.name);
                  glEnable(GL_TEXTURE_2D);
                  glBindTexture(GL_TEXTURE_2D, texture);
                } else {
                  glDisable(GL_TEXTURE_2D);
                }
                glBegin(GL_TRIANGLES);
              }
            } else {
              mat = 0;
              static const Lib3dsRgba a = {0.2f, 0.2f, 0.2f, 1.0f};
              static const Lib3dsRgba d = {0.8f, 0.8f, 0.8f, 1.0f};
              static const Lib3dsRgba s = {0.0f, 0.0f, 0.0f, 1.0f};
              glEnd();
              glMaterialfv(GL_FRONT, GL_AMBIENT, a);
              glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
              glMaterialfv(GL_FRONT, GL_SPECULAR, s);
              glDisable(GL_TEXTURE_2D);
              glBegin(GL_TRIANGLES);
            }
          }

          for (unsigned i = 0; i < 3; ++i) {
            glNormal3fv(normalL[3*p+i]);
            // How to flag this?
            glTexCoord2fv(mesh->texelL[f->points[i]]);
            glVertex3fv(mesh->pointL[f->points[i]].pos);
          }
        }
        glEnd();

        free(normalL);

        glEndList();
    }

    if (node->user.p) {
      Lib3dsObjectData *d;
      VertexBuffer * vb = (VertexBuffer*)node->user.p;

      glPushMatrix();
      d=&node->data.object;
      glMultMatrixf(&node->matrix[0][0]);
      glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
      glCallList(vb->bobject);
      /*glutSolidSphere(50.0, 20,20);*/
      glPopMatrix();
    }
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
    std::cout << "LOADING " << fullPath << std::endl << std::flush;
    m_model = lib3ds_file_load(fullPath.c_str());
    if (m_model != 0) {
        lib3ds_file_eval(m_model, 0);
    }
}

void m3dsRenderer::render(Renderer &, const PosType &)
{
    if (m_model != 0) {
        glPushMatrix();
        glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_NORMALIZE);
        glAlphaFunc(GL_GREATER, 0.2f);
        glScalef(5.f, 5.f, 5.f);
        // draw3dsModel();
        draw3dsFile();
        glDisable(GL_NORMALIZE);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        glPopMatrix();
    }
}

void m3dsRenderer::select(Renderer & r, const PosType & p)
{
    render(r, p);
}
