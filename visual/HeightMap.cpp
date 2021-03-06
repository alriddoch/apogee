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

// #define USE_SDL

#include "HeightMap.h"

#include <SDL/SDL_image.h>

#include "png.h"

#include <iostream>
#include <cassert>

void pngErrorFunction(png_structp, const char *c)
{
    std::cerr << "pngErrorFunction" << c << std::endl << std::flush;
}

void pngWarningFunction(png_structp, const char *c)
{
    std::cerr << "pngWarningFunction: " << c << std::endl << std::flush;
}

HeightMap::HeightMap() : m_heightImage(NULL), m_rowPointers(NULL)
{
}

bool HeightMap::load(const std::string & file)
{
#ifdef USE_SDL
    m_heightImage = IMG_Load(file.c_str());
    if (m_heightImage == NULL) {
        std::cerr << "Failed to load heightmap " << file << " "
                  << SDL_GetError() << std::endl << std::flush;
        return false;
    }

    int bpp = m_heightImage->format->BitsPerPixel;

    if (bpp != 8) {
        std::cerr << "Height map is " << bpp << " not greyscale"
                  << std::endl << std::flush;
        return false;
    }
    return true;
#else
    FILE *img_fp = fopen(file.c_str(), "rb");
    if (!img_fp) {
        std::cerr << "Image file " << file << " could not be opened."
                  << std::endl << std::flush;
        perror("fopen");
        return false;
    }

    png_byte header[8];
    fread(header, 1, 8, img_fp);
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
        std::cerr << "Image file " << file << " does not seem to be a png."
                  << std::endl << std::flush;
        return false;
    }
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 (png_voidp)NULL,
                                                 pngErrorFunction,
                                                 pngWarningFunction);
    if (!png_ptr) {
        std::cerr << "libpng failed to create read struct"
                  << std::endl << std::flush;
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        std::cerr << "libpng failed to create info struct"
                  << std::endl << std::flush;
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return false;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        std::cerr << "libpng failed to create end info struct"
                  << std::endl << std::flush;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return false;
    }
    std::cout << "PNG_READ: Succeeded in getting this far"
              << std::endl << std::flush;
    png_init_io(png_ptr, img_fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);

    int depth, colourType;
    png_get_IHDR(png_ptr, info_ptr,
                 (png_uint_32*)&m_width, (png_uint_32*)&m_height, &depth,
                 &colourType, NULL, NULL, NULL);

    if ((depth != 8) || (colourType != PNG_COLOR_TYPE_GRAY)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return false;
    }

    m_rowPointers = png_get_rows(png_ptr, info_ptr);

    // png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return true;
#endif
}

int HeightMap::get(int x, int y) const
{
#ifdef USE_SDL
    assert(m_heightImage != NULL);

    Uint8 * pixelbuf = (Uint8 *)m_heightImage->pixels;
    Uint16 pitch = m_heightImage->pitch;
    int width = m_heightImage->w;
    int height = m_heightImage->h;

    int ix = x;
    int iy = height - y;

    return *(pixelbuf + (iy * pitch + ix)) - 127;
#else
    assert(m_rowPointers != NULL);
    
    return *(m_rowPointers[m_height - y] + x) - 127;
#endif
}
