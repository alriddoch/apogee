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
