#include "Sprite.h"
#include "Renderer.h"

#include <GL/gl.h>
#include <SDL.h>
#include <SDL_image.h>

#include <math.h>
#include <unistd.h>

// This function has an arbitrary size limit of 4096x4096 for textures

unsigned int Sprite::twoN(unsigned int size)
{
    for(int i = 0; i < 12; i++) {
        const unsigned int num = pow(2,i) - 1;
        std::cout << i << " " << num << " " << std::endl << std::flush;
        if ((size & ~num) == 0) {
            std::cout << "woot" << std::endl << std::flush;
            return num + 1;
        }
    }
    return pow(2,12);
}

bool Sprite::load(const std::string & filename)
{
    SDL_Surface * image = IMG_Load(filename.c_str());
    if (image == NULL) {
        return false;
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
    tex_id = Texture::loadTexture(image);
    m_w = (double)sprite_w / Renderer::meterSize;
    m_h = (double)sprite_h / Renderer::meterSize;
    m_pw = (double)sprite_w / textur_w;
    m_ph = (double)sprite_h / textur_h;
    SDL_FreeSurface(image);
    if (tex_id == -1) { return false; }
    return true;
}


void Sprite::draw()
{
    if (tex_id == -1) { return; }
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-m_w/2, 0, 0);
    glTexCoord2f(m_pw, 0); glVertex3f(m_w/2, 0, 0);
    glTexCoord2f(m_pw, m_ph); glVertex3f(m_w/2, m_h, 0);
    glTexCoord2f(0, m_ph); glVertex3f(-m_w/2, m_h, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
