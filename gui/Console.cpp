// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Console.h"
#include "Gui.h"

static const int lines = 10;
static const int columns = 20;

static const int charSize = 16;

Console::~Console()
{
}

void Console::setup()
{
}

void Console::draw()
{
    glEnable(GL_BLEND);
    glColor4f(0.f, 0.f, 0.f, 0.5f);
    float width = columns * 16 + 8;
    float height = lines * 16 + 8;
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
}

void Console::select()
{
    float width = columns * 16 + 8;
    float height = lines * 16 + 8;
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
        default:
            break;
    };
    return false;
}

void Console::addChannel(const std::string & name)
{
}
