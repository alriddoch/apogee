// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_TERRAIN_RENDERER_H
#define APOGEE_TERRAIN_RENDERER_H

#include "EntityRenderer.h"

#include "GL.h"

#include <Mercator/Terrain.h>

class TerrainRenderer : public EntityRenderer
{
  protected:
    Mercator::Terrain m_terrain;
    int m_numLineIndeces;
    unsigned int * const m_lineIndeces;
    float * const m_texCoords;
    GLint m_texture, m_texture2;
    bool m_haveTerrain;

    void drawRegion(Mercator::Segment *);
    void drawMap(Mercator::Terrain &);
    void readTerrain();
  public:
    TerrainRenderer(Renderer &, Eris::Entity & e);
    virtual ~TerrainRenderer();

    virtual void render(Renderer &);
    virtual void select(Renderer &);
};

#endif // APOGEE_TERRAIN_RENDERER_H
