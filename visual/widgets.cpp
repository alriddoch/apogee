// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#error This file is removed from the build

#include "Renderer.h"

#include <GL/gl.h>

namespace widgets {

void grid(Renderer & renderer)
{
    // Draws a 1 metre flat grid at ground level
    const float scale = renderer.getScale();
    const int w = renderer.getWidth(), h = renderer.getHeight();
    const int maxlen = sqrt(w*w + h*h) * scale / (renderer.meterSize());

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

void compass(Renderer & renderer)
{
    // Draw a needle pointing north in the top left corner
    renderer.viewPoint();
    const float scale = renderer.getScale();
    const float x=-((renderer.getWidth())/(2*renderer.meterSize())-1.2)*scale;
    const float y=((renderer.getHeight())/(2*renderer.meterSize())-1.2)*scale;

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

void axis(Renderer & renderer)
{
    // draws an axis indicator in the bottom left corner
    renderer.viewPoint();
    const float scale = renderer.getScale();
    const float x=-((renderer.getWidth()) /(2*renderer.meterSize())-1.2)*scale;
    const float y=-((renderer.getHeight())/(2*renderer.meterSize())-1.2)*scale;
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

}
