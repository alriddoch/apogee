#include "Texture.h"

#include <SDL_image.h>
#include <GL/gl.h>

std::map<string, int> * Texture::texturedb = NULL;

SDL_Surface * Texture::imageLoad(const string & filename)
// This code was created by Jeff Molofee '99
// (ported to SDL by Sam Lantinga '2000)
//
// If you've found this code useful, please let me know.
{
    SDL_Surface *image;

    image = IMG_Load(filename.c_str());
    if ( image == NULL ) {
        cerr << "Unable to load" << filename.c_str() << ":"
             << SDL_GetError() << endl << flush;
        return(NULL);
    }
    return imageTransform(image);
}

SDL_Surface * Texture::imageTransform(SDL_Surface * image)
{
    Uint8 *rowhi, *rowlo;
    Uint8 *tmpbuf, tmpch;
    int i, j;

    /* GL surfaces are upsidedown and RGB, not BGR :-) */
    tmpbuf = (Uint8 *)malloc(image->pitch);
    if ( tmpbuf == NULL ) {
        fprintf(stderr, "Out of memory\n");
        return(NULL);
    }
    rowhi = (Uint8 *)image->pixels;
    rowlo = rowhi + (image->h * image->pitch) - image->pitch;
    for ( i=0; i<image->h/2; ++i ) {
#if 0
        for ( j=0; j<image->w; ++j ) {
            tmpch = rowhi[j*3];
            rowhi[j*3] = rowhi[j*3+2];
            rowhi[j*3+2] = tmpch;
            tmpch = rowlo[j*3];
            rowlo[j*3] = rowlo[j*3+2];
            rowlo[j*3+2] = tmpch;
        }
#endif
        memcpy(tmpbuf, rowhi, image->pitch);
        memcpy(rowhi, rowlo, image->pitch);
        memcpy(rowlo, tmpbuf, image->pitch);
        rowhi += image->pitch;
        rowlo -= image->pitch;
    }
    free(tmpbuf);
    return(image);
}


int Texture::get(const string & filename)
{
    if (textures().find(filename) != textures().end()) {
        return textures()[filename];
    }
    cout << "Loading new texture " << filename << endl << flush;
    SDL_Surface * image = imageLoad(filename);

    if (image == NULL) {
        cerr << "Failed to load texture " << filename << endl << flush;
        return -1;
    }

    int tex_id = loadTexture(image);
    if (tex_id != -1) { textures()[filename] = tex_id; }
    return tex_id;
}
    
int Texture::loadTexture(SDL_Surface * image)
{
    int tex_id;
    int format;
    int x, y;
    int bpp = image->format->BitsPerPixel;

    if(bpp != 24 && bpp != 32) {
        SDL_FreeSurface(image);
        cerr << "Failed to load texture: wrong format " << endl << flush;
        return -1;
    }

    format = (bpp == 24) ? GL_RGB : GL_RGBA;

    /* load the texture into OGL */
    glGenTextures(1, (unsigned int *)&tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0,
                 format, GL_UNSIGNED_BYTE, image->pixels);
    cout << image->w << " " << image->h << endl << flush;
    if (glGetError() != 0) {
        cerr << "BANFG" << endl << flush;
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return tex_id;
}
