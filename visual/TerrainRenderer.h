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
    typedef std::map<int, GLuint> DisplayListColumn;
    typedef std::map<int, DisplayListColumn> DisplayListStore;

    Mercator::Terrain m_terrain;
    DisplayListStore m_displayLists;
    int m_numLineIndeces;
    unsigned short * const m_lineIndeces;
    float * const m_texCoords;
    GLuint m_textures[8]; //, m_texture2;
    GLuint m_landscapeList;
    bool m_haveTerrain;

    void enableRendererState();
    void disableRendererState();

    void drawRegion(Mercator::Segment *);
    void drawMap(Mercator::Terrain &, const WFMath::Point<3> & camPos);
    void drawSea(Mercator::Terrain &);
    void readTerrain();
  public:
    TerrainRenderer(Renderer &, Eris::Entity & e);
    virtual ~TerrainRenderer();

    virtual void render(Renderer &, const WFMath::Point<3> & camPos);
    virtual void select(Renderer &, const WFMath::Point<3> & camPos);
};

#endif // APOGEE_TERRAIN_RENDERER_H
