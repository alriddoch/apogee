// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "GL.h"

#include "DemeterScene.h"

#include "Texture.h"

#include "common/system.h"

DemeterScene::DemeterScene(Application & app, int wdth, int hght) :
                                           Renderer(app, wdth, hght)
{
    elevation = 10;
    rotation = 45;

    m_windowName = "perigee";
    m_iconName = "perspective";
}

static const float maxViewDistance = 250.0f;

void DemeterScene::projection()
{
    // Not sure how to use this one to get a decent projection no matter
    // what the aspect ratio. Keep it here for reference.
    //gluPerspective(45.0f, (float)width/(float)height,0.65f, maxViewDistance);

    // Sort out perspective projection, with scale tied to height of the window
    // this allow a very wide window to give a nice widescreen view, and
    // allows a narrow window to be usable.
    float s = ((float)width / (float)height) * 3.0f / 8.0f;
    glFrustum(-s,s,-0.375f,0.375f,0.65f,maxViewDistance);
}

void DemeterScene::viewPoint()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                     // Reset The View

    glTranslatef(0.0f, 0.0f, -5.0f);

    glRotatef(elevation-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    glTranslatef(0.0f, 0.0f, -2.5f);

    glTranslatef(-x_offset,-y_offset,-z_offset);

    static GLfloat AmbientColor[] = {0.3f, 0.3f, 0.3f, 1.f};
    static GLfloat DiffuseColor[] = {1.f, 1.f, 0.95f, 1.f};
    static GLfloat LightPos[] = {0.f, 1.f, 1.f, 0.f};
    static GLfloat lmodel_ambient[] = {0.f, 0.f, 0.f, 1.f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientColor);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseColor);
    glLightfv(GL_LIGHT1, GL_POSITION,LightPos);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

void DemeterScene::drawSky()
{
    // static GLuint skyList = 0;
    // if (!skyList) {
        // skyList = glGenLists(1);
        // glNewList(skyList, GL_COMPILE);
    
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
            t_front = Texture::get(getMediaPath() + "/media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_front.png", false);
            t_back = Texture::get(getMediaPath() + "/media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_back.png", false);
            t_left = Texture::get(getMediaPath() + "/media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_left.png", false);
            t_right = Texture::get(getMediaPath() + "/media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_right.png", false);
            t_up = Texture::get(getMediaPath() + "/media/media-3d/collection-gfire/textures/envs/sunsky01/skybox_256_up.png", false);
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

        // glEndList();
    // }
    // glCallList(skyList);
}
