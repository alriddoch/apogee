// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_IXCLIENT_H
#define APOGEE_IXCLIENT_H

#include "GameClient.h"

class IxClient : public GameClient {
  private:
    bool m_use_mouse;
  public:
    IxClient(Eris::Connection & con);
    virtual ~IxClient();

    bool event(SDL_Event &);
    bool mouse(int x, int y);
};

#endif // APOGEE_IXCLIENT_H
