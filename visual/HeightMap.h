// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#error This file has been removed from the build

#ifndef APOGEE_VISUAL_HEIGHTMAP_H
#define APOGEE_VISUAL_HEIGHTMAP_H

struct SDL_Surface;

#include <string>
#include <stdint.h>

class HeightMap {
  private:
    struct SDL_Surface * m_heightImage;
    uint8_t ** m_rowPointers;
    uint32_t m_width;
    uint32_t m_height;
    

  public:
    HeightMap();

    bool load(const std::string & file);
    int get(int x, int y) const;
};

#endif // APOGEE_VISUAL_HEIGHTMAP_H
