// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "GL.h"

#include "ForestRenderer.h"

ForestRenderer::ForestRenderer(Renderer & r, Eris::Entity & e):
      EntityRenderer(r, e) //, m_forest(),
{
}

ForestRenderer::~ForestRenderer()
{
}

void ForestRenderer::render(Renderer & r, const PosType & camPos)
{
}

void ForestRenderer::select(Renderer & r, const PosType & camPos)
{
}
