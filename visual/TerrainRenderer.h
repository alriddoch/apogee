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
  public:
    typedef std::map<int, GLuint> DisplayListColumn;
    typedef std::map<int, DisplayListColumn> DisplayListStore;

    Mercator::Terrain m_terrain;

  protected:
    DisplayListStore m_displayLists;
    int m_numLineIndeces;
    unsigned short * const m_lineIndeces;
    GLuint m_textures[8];
    GLuint m_alphaTextures[8];
    GLuint m_landscapeList;
    bool m_haveTerrain;

    void enableRendererState();
    void disableRendererState();

    void generateAlphaTextures(Mercator::Segment *);
    void drawRegion(Mercator::Segment *);
    void drawMap(Mercator::Terrain &, const PosType & camPos);
    void drawSea(Mercator::Terrain &);
    void drawShadow(const WFMath::Point<2> & pos, float radius = 1.f);
    void readTerrain();
  public:
    TerrainRenderer(Renderer &, Eris::Entity & e);
    virtual ~TerrainRenderer();

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_TERRAIN_RENDERER_H
