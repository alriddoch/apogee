// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

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
