// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Gui.h"

#include <visual/Sprite.h>
#include <visual/Renderer.h>

#include <GL/glu.h>

#include "Alert.h"
#include "Dialogue.h"
#include "Item.h"
#include "Compass.h"

Gui::Gui(Renderer & r) : renderer(r), nameCount(0), inMotion(-1), focus(-1)
{

}

bool Gui::setup()
{
    Item * item = new Item(*this, 150, 150);
    item->setup();
    if (item->load("bag.png")) { widgets[newName()] = item; }

    // Widget * w = new Alert(*this, 500, 60, "This program is currently under test");
    // w->setup();
    // widgets[newName()] = w;

    textTexture = Texture::get("font.png");
    if (textTexture == -1) {
        cerr << "Failed to load font texture" << endl << flush;
        return false;
    }
    textBase = glGenLists(256);
    glBindTexture(GL_TEXTURE_2D, textTexture);
    for(int loop=0; loop<256; loop++) {
        float cx=(float)(loop%16)/16.0f;             // X Position Of Current Character
        float cy=(float)(loop/16)/16.0f;             // Y Position Of Current Character

        glNewList(textBase+loop,GL_COMPILE);       // Start Building A List
        glBegin(GL_QUADS);                     // Use A Quad For Each Character
        glTexCoord2f(cx,1-cy-0.0625f);         // Texture Coord (Bottom Left)
        glVertex2i(0,0);                       // Vertex Coord (Bottom Left)
        glTexCoord2f(cx+0.0625f,1-cy-0.0625f); // Texture Coord (Bottom Right)
        glVertex2i(16,0);                      // Vertex Coord (Bottom Right)
        glTexCoord2f(cx+0.0625f,1-cy);         // Texture Coord (Top Right)
        glVertex2i(16,16);                     // Vertex Coord (Top Right)
        glTexCoord2f(cx,1-cy);                 // Texture Coord (Top Left)
        glVertex2i(0,16);                      // Vertex Coord (Top Left)
        glEnd();                               // Done Building Our Quad (Character)
        glTranslated(10,0,0);                  // Move To The Right Of The Character
        glEndList();                           // Done Building The Display List
    }                                        
    

    return true;
}

void Gui::draw()
{
    widgmap::const_iterator I = widgets.begin();
    for (; I != widgets.end(); I++) {
        Widget & w = *I->second;
        if (w.obsolete()) {
            widgets.erase(I->first);
            delete &w;
            break;
        }
    }
    I = widgets.begin();
    for (; I != widgets.end(); I++) {
        Widget & w = *I->second;
        if (!w.visible()) { continue; }
        glTranslated(w.x(),w.y(),0.0f);
        w.draw();
        glTranslated(-w.x(),-w.y(),0.1f);
    }
}

GLint Gui::select(int x, int y)
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
    glPushName(I->first);
    for (; I != widgets.end(); I++) {
        Widget & w = *I->second;
        if (!w.visible()) { continue; }
        glTranslated(w.x(),w.y(),0);
        glLoadName(I->first);
        w.select();
        glTranslated(-w.x(),-w.y(),0.1);
    }
    glPopName();

    int hits = glRenderMode(GL_RENDER);

    cout << "Got " << hits << " hits" << endl << flush;
    if (hits < 1) { return -1; }

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
    GLuint * nameItr = namePtr;
    int closest = -1;
    cout << "The closest hit has " << noNames << " names: ";
    hitNames.clear();
    for (int i = 0; i < noNames; i++,nameItr++) {
        cout << *nameItr;
        widgmap::const_iterator I = widgets.find(*nameItr);
        if (I != widgets.end()) {
            cout << endl << flush;
            closest = *nameItr;
        } else {
            hitNames.push_back(*nameItr);
        }
    }
    cout << endl << flush;
    return closest;
}

bool Gui::event(SDL_Event & event)
{
    bool update = false;
    switch(event.type) {
        case SDL_MOUSEMOTION:
            if ((event.motion.state & SDL_BUTTON(1)) && (inMotion != -1)) {
                cout << "MOVE IT" << endl << flush;
                int dx = event.motion.x - mx;
                int dy = my - event.motion.y;
                mx = event.motion.x;
                my = event.motion.y;
                widgmap::const_iterator I = widgets.find(inMotion);
                if (I != widgets.end()) {
                    cout << "moving " << dx << " " << dy << endl << flush;
                    I->second->move(dx,dy);
                    update = true;
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if ((event.button.type & SDL_MOUSEBUTTONDOWN) &&
                (event.button.button & SDL_BUTTON_LEFT) &&
                (event.button.state & SDL_PRESSED)) {
                cout << "Gui button pressed" << endl << flush;
                inMotion = select(event.button.x, event.button.y);
                focus = inMotion;
                if (inMotion != -1) {
                    mx = event.button.x;
                    my = event.button.y;
                    widgmap::const_iterator I = widgets.find(inMotion);
                    if (I != widgets.end()) {
                        I->second->click();
                        update = true;
                    }
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if ((event.button.type & SDL_MOUSEBUTTONUP) &&
                (event.button.button & SDL_BUTTON_LEFT)) {
                // (event.button.state & SDL_RELEASED)) {
                if (inMotion != -1) {
                    widgmap::const_iterator I = widgets.find(inMotion);
                    if (I != widgets.end()) {
                        I->second->release();
                        update = true;
                    }
                }
                inMotion = -1;
            }
            break;
        case SDL_KEYDOWN:
            int widg = focus;
            if (widg != -1) {
                widgmap::const_iterator I = widgets.find(widg);
                if (I != widgets.end()) {
                    I->second->key(event.key.keysym.sym, event.key.keysym.mod);
                    update = true;
                }
            }
            break;
    }
    return update;
}

void Gui::print(const char * str)
{
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBindTexture(GL_TEXTURE_2D, textTexture);
    glEnable(GL_TEXTURE_2D);
    glListBase(textBase-32);
    glCallLists(strlen(str),GL_BYTE,str);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Gui::addWidget(Widget * w)
{
    w->setup();
    widgets[newName()] = w;
}

int Gui::keyToAscii(int key, int mod)
{
    if ((mod & (KMOD_LCTRL | KMOD_RCTRL | KMOD_LALT | KMOD_RALT)) != 0) {
        cout << "ctrl or alt pressed" << endl << flush;
        return -1;
    }
    int val = -1;
    bool cap = (mod & (KMOD_LSHIFT | KMOD_RSHIFT | KMOD_CAPS)) ? true : false;
    if ((key >= SDLK_SPACE) && (key <= SDLK_AT) ||
        (key >= SDLK_LEFTBRACKET) && (key <= SDLK_z)) {
        val = key;
    }
    if (cap && (val > SDLK_BACKQUOTE) && (val < SDLK_DELETE)) {
        val -= 32;
    }
    return val;
}
