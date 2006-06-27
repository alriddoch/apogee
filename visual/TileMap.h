// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2001 Alistair Riddoch
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

#error This file has been removed from the build

#ifndef APOGEE_VISUAL_TILEMAP_H
#define APOGEE_VISUAL_TILEMAP_H

class Tile;
class HeightMap;

namespace Coal {
  class Container;
}

namespace Mercator {
  class Terrain;
}

#include <map>

typedef std::pair<int, int> coord;

class TileMap {
  private:
    std::map<coord, Tile *> m_tiles;
  public:
    TileMap();

    void add(int x, int y, Tile * tile) {
        m_tiles[coord(x,y)] = tile;
    }

    Tile * get(int x, int y) {
        std::map<coord, Tile *>::const_iterator I=m_tiles.find(coord(x,y));
        if (I != m_tiles.end()) { return I->second; }
        return NULL;
    }

    void build(Coal::Container & map_base);
    void draw(Mercator::Terrain & map_height, int, int);
};

#endif // APOGEE_VISUAL_TILEMAP_H
