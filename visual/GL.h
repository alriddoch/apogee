// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_VISUAL_GL_H
#define APOGEE_VISUAL_GL_H

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#ifndef GL_EXT_compiled_vertex_array
typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

extern PFNGLLOCKARRAYSEXTPROC glLockArraysExt;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysExt;
#endif // GL_EXT_compiled_vertex_array

extern bool have_GL_EXT_compiled_vertex_array;

#endif // APOGEE_VISUAL_GL_H
