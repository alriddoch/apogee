// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Gui.h"

#include <visual/Sprite.h>
#include <visual/Renderer.h>

#include <GL/glu.h>

#include "Item.h"

Gui::Gui(Renderer & r) : renderer(r), nameCount(0)
{

}

bool Gui::setup()
{
    Item * bag = new Item(50, 50);
    if (!bag->load("bag.png")) { return false; }
    widgets[newName()] = bag;
    return true;
}

void Gui::draw()
{
    widgmap::const_iterator I = widgets.begin();
    for (; I != widgets.end(); I++) {
        I->second->draw();
    }
}

void Gui::select(int x, int y)
{

    glSelectBuffer(512,selectBuf);
    glRenderMode(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    gluPickMatrix(x, renderer.getHeight() - y, 5, 5, viewport);
    glOrtho(0, renderer.getWidth(), 0, renderer.getHeight(), -20.0f, 20.0f);

    glMatrixMode(GL_MODELVIEW);
    glInitNames();

    widgmap::const_iterator I = widgets.begin();
    for (; I != widgets.end(); I++) {
        glPushName(I->first);
        I->second->select();
        glPopName();
    }

    int hits = glRenderMode(GL_RENDER);

    cout << "Got " << hits << " hits" << endl << flush;
    if (hits < 1) { return; }

    GLuint * ptr = &selectBuf[0];
    GLuint minDepth = UINT_MAX, noNames = 0;
    GLuint * namePtr;
    for (int i = 0; i < hits; i++) {
        int names = *(ptr++);
        cout << "{" << *ptr << "}";
        if (*ptr < minDepth) {
            noNames = names;
            minDepth = *ptr;
            namePtr = ptr + 2;
        }
        ptr += (names + 2);
    }

    cout << "The closest hit has " << noNames << " names: ";
    for (int i = 0; i < noNames; i++,namePtr++) {
        cout << *namePtr;
    }
    cout << endl << flush;
}

bool Gui::event(SDL_Event & event)
{
    cout << "Gui event" << endl << flush;
    switch(event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if ((event.button.type & SDL_MOUSEBUTTONDOWN) &&
                (event.button.button & SDL_BUTTON_LEFT) &&
                (event.button.state & SDL_PRESSED)) {
                cout << "Gui button pressed" << endl << flush;
                select(event.button.x, event.button.y);
            }
            break;
    }
    return false;
}
