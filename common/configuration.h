// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_GLOBALS_H
#define COMMON_GLOBALS_H

#include <string>

namespace varconf {
  class Config;
}

extern varconf::Config * global_conf;
extern std::string skin;

bool loadConfig(int argc, char ** argv);

#endif // COMMON_GLOBALS_H
