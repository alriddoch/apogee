// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_VISUAL_HEIGHTMAP_H
#define APOGEE_VISUAL_HEIGHTMAP_H

struct SDL_Surface;

#include <string>

class HeightMap {
  private:
    struct SDL_Surface * m_heightImage;

  public:
    HeightMap();

    bool load(const std::string & file);
    int get(int x, int y) const;
};

#endif // APOGEE_VISUAL_HEIGHTMAP_H
