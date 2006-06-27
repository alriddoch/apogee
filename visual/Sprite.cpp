// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2003 Alistair Riddoch
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

#include "GL.h"

#include "Sprite.h"

#include "Renderer.h"

#include <SDL/SDL_image.h>

#include <iostream>
#include <cmath>

// This function has an arbitrary size limit of 4096x4096 for textures

unsigned int Sprite::twoN(unsigned int size)
{
    for(int i = 0; i < 12; i++) {
        const unsigned int num = (1 << i);
        if (size <= num) {
            return num;
        }
    }
    return (unsigned int)pow(2,12);
}

bool Sprite::load(const std::string & filename)
{
    m_filename = filename;
    return loadData();
}

bool Sprite::loadData()
{
    std::cout << "Sprite " << m_filename << std::endl << std::flush;
    SDL_Surface * image = IMG_Load(m_filename.c_str());
    if (image == NULL) {
        tex_id = Texture::getDefault();
        m_w = Texture::getDefaultWidth();
        m_h = Texture::getDefaultHeight();
        m_pw = (float)m_w / (float)twoN((unsigned int)m_w);
        m_ph = (float)m_h / (float)twoN((unsigned int)m_h);
        loadedp = true;
        return true;
    }

    const unsigned int textur_w = twoN(image->w);
    const unsigned int textur_h = twoN(image->h);
    const unsigned int sprite_w = image->w;
    const unsigned int sprite_h = image->h;
    SDL_Surface * texImage = SDL_CreateRGBSurface(SDL_SWSURFACE, textur_w,
                                                  textur_h, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                    0x000000FF, 
                    0x0000FF00, 
                    0x00FF0000, 
                    0xFF000000
#else
                    0xFF000000,
                    0x00FF0000, 
                    0x0000FF00, 
                    0x000000FF
#endif
                    );

    SDL_SetAlpha(image, 0, 0);
    SDL_Rect dest = { 0, textur_h - sprite_h, sprite_w, sprite_h };
    SDL_BlitSurface(image, NULL, texImage, &dest);
    SDL_FreeSurface(image);
    image = Texture::imageTransform(texImage);
    if (image == NULL) {
        SDL_FreeSurface(texImage);
        return false;
    }
    tex_id = Texture::loadTexture(image, false);
    m_w = sprite_w;
    m_h = sprite_h;
    m_pw = (float)sprite_w / textur_w;
    m_ph = (float)sprite_h / textur_h;
    SDL_FreeSurface(image);
    loadedp = true;
    return true;
}

void Sprite::draw()
{
    if (!loadedp) { return; }
    if (!glIsTexture(tex_id)) {
        loadData();
    }
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.f,1.f,1.f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float texcoords[] = { 0, 0,
                          m_pw, 0,
                          m_pw, m_ph,
                          0, m_ph };
    float vertices[] = { -m_w/2.f, 0.f, 0.f,
                          m_w/2.f, 0.f, 0.f,
                          m_w/2.f, m_h, 0.f,
                         -m_w/2.f, m_h, 0.f };
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}
