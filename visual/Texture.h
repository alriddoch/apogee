#ifndef APOGEE_VISUAL_TEXTURE_H
#define APOGEE_VISUAL_TEXTURE_H

#include <SDL.h>

#include <string>
#include <map>

class Texture {
  private:
    Texture() { }

    static std::map<string, int> & textures() {
        if (texturedb == NULL) {
            texturedb = new std::map<string, int>;
        }
        return *texturedb;
    }
    static SDL_Surface * imageLoad(const string & filename);

    static std::map<string, int> * texturedb;
  public:
    static int loadTexture(SDL_Surface * image);
    static SDL_Surface * imageTransform(SDL_Surface * image);
    static int get(const string & filename);
};

#endif // APOGEE_VISUAL_TEXTURE_H
