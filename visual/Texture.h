// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_VISUAL_TEXTURE_H
#define APOGEE_VISUAL_TEXTURE_H

struct SDL_Surface;

#include "GL.h"

#include <string>
#include <map>

class Texture {
  private:
    Texture() { }

    static std::map<std::string, unsigned int> & textures() {
        if (texturedb == NULL) {
            texturedb = new std::map<std::string, unsigned int>;
        }
        return *texturedb;
    }
    static struct SDL_Surface * imageLoad(const std::string & filename);

    static std::map<std::string, unsigned int> * texturedb;
    static unsigned int defaultTexture;
    static bool defaultTextureLoaded;
    static unsigned int defaultTextureWidth;
    static unsigned int defaultTextureHeight;
  public:
    static unsigned int loadTexture(struct SDL_Surface * image,
                                    bool wrap = true,
                                    GLint filter = GL_LINEAR);
    static struct SDL_Surface * imageTransform(struct SDL_Surface * image);
    static unsigned int get(const std::string & filename, bool wrap = true,
                            GLint filter = GL_LINEAR);
    static unsigned int getDefault();

    static unsigned int getDefaultWidth() { return defaultTextureWidth; }
    static unsigned int getDefaultHeight() { return defaultTextureHeight; }
};

#endif // APOGEE_VISUAL_TEXTURE_H
