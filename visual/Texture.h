// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

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
