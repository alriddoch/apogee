// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Gui.h"

#include <visual/Sprite.h>
#include <visual/Renderer.h>

#include <GL/glu.h>

#include "Alert.h"

Gui::Gui(Renderer & r) : renderer(r), nameCount(0), inMotion(-1)
{

}

bool Gui::setup()
{
    Item * item = new Item(*this, 150, 150);
    if (item->load("bag.png")) { widgets[newName()] = item; }

    item = new Item(*this, 330, 30);
    if (item->load("media/media-2d/ui-collection-pegasus/ui_panels_text/panels_text_christal/pnl_text_christals_grey_1_up_462x90_us.png")) { widgets[newName()] = item; }
    
    item = new Alert(*this, 500, 60, "HELLO");
    if (item->load("media/media-2d/ui-collection-pegasus/ui_panels_text/panels_text_christal/pnl_text_christals_grey_1_up_462x90_us.png")) { widgets[newName()] = item; }

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
    for (; I != widgets.end(); I++) {
        Widget & w = *I->second;
        glTranslated(w.x(),w.y(),0.0f);
        glPushName(I->first);
        w.select();
        glPopName();
        glTranslated(-w.x(),-w.y(),0.1f);
    }

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
    cout << "The closest hit has " << noNames << " names: ";
    for (int i = 0; i < noNames; i++,nameItr++) {
        cout << *nameItr;
        widgmap::const_iterator I = widgets.find(*nameItr);
        if (I != widgets.end()) {
            return *nameItr;
        }
    }
    cout << endl << flush;
}

bool Gui::event(SDL_Event & event)
{
    bool update = false;
    cout << "Gui event" << endl << flush;
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
                if (inMotion != -1) {
                    mx = event.button.x;
                    my = event.button.y;
                }
            }
            break;
    }
    return update;
}

void Gui::print(const char * str)
{
    glColor3f(0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBindTexture(GL_TEXTURE_2D, textTexture);
    glEnable(GL_TEXTURE_2D);
    glListBase(textBase-32);
    glCallLists(strlen(str),GL_BYTE,str);
    glDisable(GL_TEXTURE_2D);
}
