// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "HeightMap.h"

#include <SDL_image.h>

HeightMap::HeightMap() : m_heightImage(NULL)
{
}

bool HeightMap::load(const std::string & file)
{
    m_heightImage = IMG_Load(file.c_str());
    if (m_heightImage == NULL) {
        std::cerr << "Failed to load heightmap " << file << " "
                  << SDL_GetError() << std::endl << std::flush;
        return false;
    }

    int bpp = m_heightImage->format->BitsPerPixel;

    if (bpp != 24) {
        std::cerr << "Height map is " << bpp << " not true colour"
                  << std::endl << std::flush;
        return false;
    }
    return true;
}

int HeightMap::get(int x, int y) const
{
    assert(m_heightImage != NULL);

    Uint8 * pixelbuf = (Uint8 *)m_heightImage->pixels;
    Uint16 pitch = m_heightImage->pitch;
    int width = m_heightImage->w;
    int height = m_heightImage->h;

    int ix = x;
    int iy = height - y;

    return *(pixelbuf + (iy * pitch + ix * 3)) - 127;
}
