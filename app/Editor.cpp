// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Editor.h"

#include <visual/Renderer.h>
#include <visual/Sprite.h>
#include <visual/Model.h>

#include <world/World.h>

#include <iostream>

#include <math.h>

#include <coal/debug.h>

void Editor::grid()
{
    // Draws a 1 metre flat grid at ground level
    const double scale = renderer.getScale();
    const int w = renderer.getWidth(), h = renderer.getHeight();
    const int maxlen = sqrt(w*w + h*h) * scale / (Renderer::meterSize);

    renderer.viewPoint();
    renderer.orient();
    renderer.lightOff();
    for(int i = 0; i < maxlen; i++) {
        glBegin(GL_LINES);
        glColor3f(0.3, 0.3, 0.3);
        glVertex3f(i, maxlen, 0.0);
        glVertex3f(i, -maxlen, 0.0);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(-i, maxlen, 0.0);
        glVertex3f(-i, -maxlen, 0.0);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(maxlen, i, 0.0);
        glVertex3f(-maxlen, i, 0.0);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(maxlen, -i, 0.0);
        glVertex3f(-maxlen, -i, 0.0);
        glEnd();
    }
    renderer.lightOn();
}

void Editor::compass()
{
    // Draw a needle pointing north in the top left corner
    renderer.viewPoint();
    const double scale = renderer.getScale();
    const double x=-((renderer.getWidth())/(2*Renderer::meterSize)-1.2)*scale;
    const double y=((renderer.getHeight())/(2*Renderer::meterSize)-1.2)*scale;

    renderer.lightOff();
    glTranslatef(x, y, 18);
    renderer.orient();
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(0, scale, 0);
    glVertex3f(0, -scale, 0);
    glEnd();
    glBegin(GL_LINES);

    glVertex3f(-0.1*scale, 0.8*scale, 0);
    glVertex3f(-0.1*scale, scale, 0);
    glVertex3f(-0.1*scale, scale, 0);
    glVertex3f(0.1*scale, 0.8*scale, 0);
    glVertex3f(0.1*scale, 0.8*scale, 0);
    glVertex3f(0.1*scale, scale, 0);
    glEnd();
    renderer.lightOn();
}

void Editor::axis()
{
    // draws an axis indicator in the bottom left corner
    renderer.viewPoint();
    const double scale = renderer.getScale();
    const double x=-((renderer.getWidth()) /(2*Renderer::meterSize)-1.2)*scale;
    const double y=-((renderer.getHeight())/(2*Renderer::meterSize)-1.2)*scale;
    renderer.lightOff();
    glTranslatef(x, y, 19);
    renderer.orient();
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, scale);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(scale, 0, 0);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, scale, 0);
    glEnd();
    renderer.lightOn();
}

bool Editor::setup()
{
    CoalBladeLoader loader (map_database);
    loader.LoadMap ("blade2.xml");
    CoalDebug debug;
    debug.Dump (map_database);
    character = new Sprite();
    character->load("swinesherd_female_1_us_E_0.png");

    model = new Model();
    if (!model->onInit(Datapath() + "paladin.dat")) {
        std::cerr << "Loading paladin model failed" << std::endl << std::flush;
    }
    return 0;
}

void Editor::doWorld()
{
    const World::edict & ents = world.getWorld();

    World::edict::const_iterator I;
    for (I = ents.begin(); I != ents.end(); I++) {
        std::cout << ":" << I->first << I->second->getPos() << ":"
                  << I->second->getBbox() << I->second->getBmedian()
                  << std::endl << std::flush;
        renderer.draw3DBox(I->second->getXyz(),
                           I->second->getBbox(),
                           I->second->getBmedian());
    }
}

bool Editor::update()
{
    renderer.clear();
    renderer.drawMap(map_database);
    grid();
    // renderer.draw3Dentity();
    // renderer.drawCharacter(character, 1, 2);
    model->onUpdate(0.1);
    renderer.drawCal3DModel(model);
    doWorld();
    compass();
    axis();
    // renderer.draw2Dtest();
    renderer.flip();
    return false;
}

bool Editor::event(SDL_Event & event)
{
    static int oldx = 0;
    static int oldy = 0;
    static int oldRot = 0;
    static int oldElv = 0;
    static double oldScl = 0;
    switch(event.type) {
        case SDL_MOUSEMOTION:
            if (event.motion.state & SDL_BUTTON(2)) {
                //int w = renderer.getWidth();
                //int h = renderer.getHeight();
                const int x = event.motion.x;
                const int y = event.motion.y;
                int newRot = ((x - oldx) * 360) / renderer.getWidth();
                if (oldy > renderer.getHeight()/2) {
                    newRot = oldRot + newRot;
                } else {
                    newRot = oldRot - newRot;
                }
                while (newRot >= 360) { newRot -= 360; };
                int newElv = ((y - oldy) * 90) / renderer.getHeight();
                newElv = oldElv + newElv;
                if (newElv < 0) { newElv = 0; }
                if (newElv > 90) { newElv = 90; }
                renderer.setRotation(newRot);
                renderer.setElevation(newElv);
                return true;
            } else if (event.motion.state & SDL_BUTTON(3)) {
                const int y = event.motion.y;
                const int h = renderer.getHeight();
                double newScl = ((double)(h + y - oldy) / h) * oldScl;
                renderer.setScale(newScl);
                return true;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if ((event.button.type & SDL_MOUSEBUTTONDOWN) &&
                ((event.button.button & SDL_BUTTON_MIDDLE) ||
                 (event.button.button & SDL_BUTTON_RIGHT)) &&
                (event.button.state & SDL_PRESSED)) {
                oldx = event.button.x;
                oldy = event.button.y;
                oldRot = renderer.getRotation();
                oldElv = renderer.getElevation();
                oldScl = renderer.getScale();
                return false;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_0:
                    renderer.setElevation(30);
                    renderer.setRotation(45);
                    return true;
                    break;
                case SDLK_UP:
                    renderer.setYoffset(renderer.getYoffset() + 1);
                    break;
                case SDLK_DOWN:
                    renderer.setYoffset(renderer.getYoffset() - 1);
                    break;
                case SDLK_LEFT:
                    renderer.setXoffset(renderer.getXoffset() - 1);
                    break;
                case SDLK_RIGHT:
                    renderer.setXoffset(renderer.getXoffset() + 1);
                    break;
                default:
                    return false;
                    break;
            }
            return true;
            break;
    }
    return false;
}
