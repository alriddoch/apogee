// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_VISUAL_TILEMAP_H
#define APOGEE_VISUAL_TILEMAP_H

class Tile;

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
};

#endif // APOGEE_VISUAL_TILEMAP_H
