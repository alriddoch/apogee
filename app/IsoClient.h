// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_ISOCLIENT_H
#define APOGEE_ISOCLIENT_H

#include "GameClient.h"

class IsoClient : public GameClient {
  public:
    IsoClient(Eris::Connection & con);
    virtual ~IsoClient();

    bool event(SDL_Event &);
    void mouse(int, int, Uint8) { return; }
};

#endif // APOGEE_ISOCLIENT_H
