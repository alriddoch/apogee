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

#ifndef APOGEE_VISUAL_TILE_H
#define APOGEE_VISUAL_TILE_H

struct SDL_Surface;

namespace Mercator {
    class Terrain;
}

#include <string>
#include <map>

class Tile {
  private:
    static unsigned int twoN(unsigned int);

    static std::map<std::string, Tile *> & tiles() {
        if (tiledb == NULL) {
            tiledb = new std::map<std::string, Tile *>;
        }
        return *tiledb;
    }

    static std::map<std::string, Tile *> * tiledb;

    int tex_id;
    int tileSize;
    float m_pw, m_ph;
  public:
    std::string m_name;

    static Tile * get(const std::string & filename);

    Tile() : tex_id(-1), m_pw(0), m_ph(0) { }

    bool load(const std::string & filename);
    void draw();
    void draw(const Mercator::Terrain &, int, int);
    void select();
    void outline(float);

    bool loaded() const { return (tex_id != -1); }
    float pw() { return m_pw; }
    float ph() { return m_ph; }
};

#endif // APOGEE_VISUAL_TILE_H
