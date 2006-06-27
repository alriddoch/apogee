// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2003 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "Console.h"
#include "Gui.h"

#include <sigc++/functors/mem_fun.h>

static const int lines = 10;
static const int columns = 40;

static const int charSize = 16;

Console::Console(Gui & g, int x, int y) : Widget(g, x, y), numLines(lines)
{
    // lineContents.push_back("Test line one");
    // lineContents.push_back("Test line two");
    // lineContents.push_back("Test line three");
}

Console::~Console()
{
}

void Console::setup()
{
}

void Console::draw()
{
    glEnable(GL_BLEND);
    glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
    glScalef(0.75f, 0.75f, 0.75f);
    float width = columns * 16 + 8;
    float height = numLines * 16 + 8;
    static const float cvertices[] = { 0.f, 0.f,
                                       width, 0.f,
                                       width, 16.f,
                                       0.f, 16.f };
    float vertices[] = { 0.f, 20.f,
                         width, 20.f,
                         width, 20.0f + height,
                         0.f, 20.0f + height };
    glVertexPointer(2, GL_FLOAT, 0, cvertices);
    glDrawArrays(GL_QUADS, 0, 4);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 4);
    m_g.print(cmdLine.c_str());
    if (hasFocus()) {
        glPushMatrix();
        glTranslatef(10.f * cmdLine.size(), 0.f, 0.f);
        m_g.print("_");
        glPopMatrix();
    }
    std::deque<std::string>::const_iterator I = lineContents.begin();
    glTranslatef(0.f, 4.f, 0.f);
    for(; I != lineContents.end(); ++I) {
        glTranslatef(0.f, 16.f, 0.f);
        m_g.print(I->c_str());
    }
}

void Console::select()
{
    glScalef(0.75f, 0.75f, 0.75f);
    float width = columns * 16 + 8;
    float height = numLines * 16 + 8;
    float vertices[] = { 0.f, 0.f, width, 0.f, width, height, 0.f, height };
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 4);
}

void Console::click()
{
}

void Console::release()
{
}

bool Console::key(int sym,int mod)
{
    int k = Gui::keyToAscii(sym, mod);
    if (k != -1) {
        cmdLine.push_back(k);
        return true;
    }
    switch (sym) {
        case SDLK_BACKSPACE:
            if (cmdLine.size() != 0) {
                cmdLine.erase(cmdLine.end() - 1);
            }
            return true;
            break;
        case SDLK_RETURN:
            if (cmdLine.empty()) {
                releaseFocus();
            } else {
                lineEntered.emit(cmdLine);
            }
            cmdLine.clear();
            return true;
        default:
            break;
    };
    return false;
}

void Console::addChannel(const std::string & name)
{
}

void Console::pushLine(const std::string & line)
{
    lineContents.push_front(line);
    while (lineContents.size() > numLines) {
        lineContents.pop_back();
    }
}
