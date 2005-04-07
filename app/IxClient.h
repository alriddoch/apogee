// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_IXCLIENT_H
#define APOGEE_IXCLIENT_H

#include "GameClient.h"

class IxClient : public GameClient {
  private:
    bool m_use_mouse;
  public:
    IxClient();
    virtual ~IxClient();

    bool event(SDL_Event &);
    void mouse(int x, int y, Uint8);
};

#endif // APOGEE_IXCLIENT_H
