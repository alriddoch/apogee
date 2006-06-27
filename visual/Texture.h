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

#ifndef APOGEE_VISUAL_TEXTURE_H
#define APOGEE_VISUAL_TEXTURE_H

struct SDL_Surface;

#include "GL.h"

#include <string>
#include <map>

class Texture {
  public:
    typedef std::map<std::string, Texture> TextureStore;

  protected:

    GLuint glHandle;

    static TextureStore & textures() {
        if (textureStore == 0) {
            textureStore = new TextureStore;
        }
        return *textureStore;
    }
    static struct SDL_Surface * imageLoad(const std::string & filename);

    static TextureStore * textureStore;
    static GLuint defaultTexture;
    static bool defaultTextureLoaded;
    static GLuint defaultTextureWidth;
    static GLuint defaultTextureHeight;
  public:
    Texture();

    static GLuint loadTexture(struct SDL_Surface * image,
                              bool wrap = true,
                              GLint filter = GL_LINEAR);
    static struct SDL_Surface * imageTransform(struct SDL_Surface * image);
    static GLuint get(const std::string & filename, bool wrap = true,
                      GLint filter = GL_LINEAR);
    static GLuint getDefault();
    static bool checkReload();

    static unsigned int getDefaultWidth() { return defaultTextureWidth; }
    static unsigned int getDefaultHeight() { return defaultTextureHeight; }
};

#endif // APOGEE_VISUAL_TEXTURE_H
