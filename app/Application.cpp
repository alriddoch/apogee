// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Application.h"

#include <Eris/Connection.h>

void Application::connect(const std::string & host, const std::string & port)
{
    connection.connect(host,6767);
}
