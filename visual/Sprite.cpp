#include "Sprite.h"
#include "Renderer.h"

#include <GL/gl.h>
#include <SDL.h>
#include <SDL_image.h>

#include <math.h>

unsigned int Sprite::twoN(unsigned int size)
{
    for(int i = 0; i < 31; i++) {
        const unsigned int num = pow(2,i) - 1;
        cout << i << " " << num << " " << endl << flush;
        if ((size & ~num) == 0) {
            cout << "woot" << endl << flush;
            return num + 1;
        }
    }
    return pow(2,31);
}

bool Sprite::load(const string & filename)
{
    SDL_Surface * image = IMG_Load(filename.c_str());
    if (image == NULL) {
        return false;
    }

    unsigned int w = twoN(image->w);
    unsigned int h = twoN(image->h);
    SDL_Surface * texImage = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
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

    SDL_SetAlpha(image, SDL_SRCALPHA, 0);
    SDL_Rect dest = { 0, h - image->h, image->w, image->h };
    SDL_BlitSurface(image, NULL, texImage, &dest);
    SDL_FreeSurface(image);
    image = Texture::imageTransform(texImage);
    if (image == NULL) {
        SDL_FreeSurface(texImage);
        return false;
    }
    tex_id = Texture::loadTexture(texImage);
    SDL_FreeSurface(texImage);
    if (tex_id == -1) { return false; }
    m_w = (double)image->w / Renderer::meterSize;
    m_h = (double)image->h / Renderer::meterSize;
    m_pw = (double)image->w / w;
    m_ph = (double)image->h / h;
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
