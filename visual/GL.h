// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_VISUAL_GL_H
#define APOGEE_VISUAL_GL_H

#define GL_GLEXT_PROTOTYPES

#if 0

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef GL_ARB_multitexture

  typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum texture);
  extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

#else // GL_ARB_multitexture

  #ifndef GL_TEXTURE0
    #define GL_TEXTURE0 GL_TEXTURE0_ARB
    #define GL_TEXTURE1 GL_TEXTURE1_ARB
  #endif // GL_TEXTURE0

  #ifndef glActiveTexture
    #define glActiveTexture glActiveTextureARB
  #endif // glActiveTexture

#endif // GL_ARB_multitexture

#else // 0

#include <sage/sage.h>

#endif // 0


#endif // APOGEE_VISUAL_GL_H
