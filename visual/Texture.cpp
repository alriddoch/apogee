// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "Texture.h"

#include "GL.h"
#include "GLU.h"
#include "default_texture.h"

#include <SDL/SDL_image.h>

#include <iostream>

Texture::TextureStore * Texture::textureStore = NULL;
GLuint Texture::defaultTexture;
bool Texture::defaultTextureLoaded;
unsigned int Texture::defaultTextureWidth;
unsigned int Texture::defaultTextureHeight;

Texture::Texture()
{
}

SDL_Surface * Texture::imageLoad(const std::string & filename)
// This code was created by Jeff Molofee '99
// (ported to SDL by Sam Lantinga '2000)
//
// If you've found this code useful, please let me know.
{
    SDL_Surface *image;

    image = IMG_Load(filename.c_str());
    if ( image == NULL ) {
        std::cerr << "Unable to load " << filename.c_str() << " : "
                  << SDL_GetError() << std::endl << std::flush;
        return(NULL);
    }
    return imageTransform(image);
}

SDL_Surface * Texture::imageTransform(SDL_Surface * image)
{
    Uint8 *rowhi, *rowlo;
    Uint8 *tmpbuf;
    int i;

    // GL surfaces are upsidedown
    tmpbuf = (Uint8 *)malloc(image->pitch);
    if ( tmpbuf == NULL ) {
        fprintf(stderr, "Out of memory\n");
        // Can't get buffer, so we just return the image untransformed
        return image;
    }
    rowhi = (Uint8 *)image->pixels;
    rowlo = rowhi + (image->h * image->pitch) - image->pitch;
    for ( i=0; i<image->h/2; ++i ) {
        // Code to convert reverse order of colour triples removed.
        memcpy(tmpbuf, rowhi, image->pitch);
        memcpy(rowhi, rowlo, image->pitch);
        memcpy(rowlo, tmpbuf, image->pitch);
        rowhi += image->pitch;
        rowlo -= image->pitch;
    }
    free(tmpbuf);
    return(image);
}


unsigned int Texture::get(const std::string & filename, bool wrap, GLint filter)
{
    TextureStore::const_iterator I = textures().find(filename);
    if (I != textures().end()) {
        GLuint h = I->second.glHandle;
        if (!glIsTexture(h)) {
            std::cout << "Texture for \"" << filename
                      << "\" in store is no longer a texture in this context"
                      << std::endl << std::flush;
        }
        return h;
    }
    std::cout << "Loading new texture " << filename << std::endl << std::flush;
    SDL_Surface * image = imageLoad(filename);

    if (image == NULL) {
        std::cerr << "Failed to load texture " << filename
                  << std::endl << std::flush;
        return getDefault();
    }

    unsigned int tex_id = loadTexture(image, wrap, filter);
    SDL_FreeSurface(image);

    textures()[filename].glHandle = tex_id;
    return tex_id;
}

unsigned int Texture::getDefault()
{
    if (defaultTextureLoaded) {
        return defaultTexture;
    }
    glGenTextures(1, &defaultTexture);
    glBindTexture(GL_TEXTURE_2D, defaultTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, texture_default_texture_internalFormat,
                 texture_default_texture_width, texture_default_texture_height, 0,
                 texture_default_texture_format, GL_UNSIGNED_BYTE,
                 texture_default_texture_pixels);
    if (glGetError() != 0) {
        std::cerr << "ERROR: Failed to load font texture" << std::endl << std::flush;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    defaultTextureLoaded = true;
    defaultTextureWidth = texture_default_texture_width;
    defaultTextureHeight = texture_default_texture_height;
    return defaultTexture;
}
    
unsigned int Texture::loadTexture(SDL_Surface * image, bool wrap, GLint filter)
{
    unsigned int tex_id;
    int format, fmt;
    int bpp = image->format->BitsPerPixel;

    if(bpp != 24 && bpp != 32) {
        SDL_FreeSurface(image);
        std::cerr << "Failed to load texture: wrong format "
                  << std::endl << std::flush;
        return getDefault();
    }

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    if (image->format->Rshift > image->format->Bshift) {
#else // SDL_BYTEORDER == SDL_LIL_ENDIAN
    if (image->format->Rshift < image->format->Bshift) {
#endif // SDL_BYTEORDER == SDL_LIL_ENDIAN
        format = (bpp == 24) ? GL_BGR : GL_BGRA;
    } else {
        format = (bpp == 24) ? GL_RGB : GL_RGBA;
    }
    // FIXME The internal format seems to be very performance critical
    fmt = (bpp == 24) ? GL_RGB4 : GL_RGBA4;

    /* load the texture into OGL */
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if ((filter == GL_LINEAR_MIPMAP_NEAREST) ||
        (filter == GL_LINEAR_MIPMAP_LINEAR)) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, fmt, image->w, image->h, format,
                          GL_UNSIGNED_BYTE, image->pixels);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, image->w, image->h, 0,
                     format, GL_UNSIGNED_BYTE, image->pixels);
    }

    std::cout << image->w << " " << image->h << std::endl << std::flush;
    GLenum er = glGetError();
    if (er != 0) {
        std::cerr << "BAD TEXTURE: \"" << gluErrorString(er) << "\""
                  << std::endl << std::flush;
    }

    // Mag filter is linear unless filter is nearest
    GLint magFilter = (filter == GL_NEAREST) ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

    if (wrap) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }


    return tex_id;
}
