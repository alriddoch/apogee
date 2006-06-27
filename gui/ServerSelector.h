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

#ifndef APOGEE_GUI_SERVER_SELECTOR_H
#define APOGEE_GUI_SERVER_SELECTOR_H

#include "Button.h"

#include <sigc++/signal.h>

#include <map>
#include <set>

namespace Eris {
  class Meta;
  class ServerInfo;
};

class ServerSelector : public Widget {
  private:
    GLuint connectName;
    GLuint refreshName;
    GLuint specifyName;
    GLuint filterName;
    Eris::Meta & metaServer;
    std::map<GLuint, const Eris::ServerInfo*> names;
    std::string selectedServer;
    Button connect;
    Button refresh;
    Button specify;
    Button filter;

  public:
    sigc::signal<void, const std::string &> connectSignal;
    sigc::signal<void> refreshSignal;
    sigc::signal<void> specifySignal;
    sigc::signal<void> filterSignal;

    ServerSelector(Gui & g, int x, int y);
    virtual ~ServerSelector();

    virtual void setup();
    virtual void draw();
    virtual void select();
    virtual void click();
    virtual void release();
    virtual bool key(int,int);
};

#endif // APOGEE_GUI_SERVER_SELECTOR_H
