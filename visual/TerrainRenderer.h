// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_TERRAIN_RENDERER_H
#define APOGEE_TERRAIN_RENDERER_H

#include "EntityRenderer.h"

#include "GL.h"

#include <Mercator/Terrain.h>

namespace Atlas {
  namespace Message {
    class Element;
  }
}

class TerrainRenderer : public EntityRenderer
{
  public:
    typedef std::map<int, GLuint> DisplayListColumn;
    typedef std::map<int, DisplayListColumn> DisplayListStore;

    Mercator::Terrain m_terrain;

  protected:
    DisplayListStore m_displayLists;
    int m_numLineIndeces;
    int m_numHalfIndeces;
    unsigned short * const m_lineIndeces;
    unsigned short * const m_halfIndeces;
    GLuint m_textures[8];
    GLuint m_alphaTextures[8];
    GLuint m_landscapeList;
    bool m_haveTerrain;

    void enableRendererState();
    void disableRendererState();

    void generateAlphaTextures(Mercator::Segment *);
    void drawRegion(Mercator::Segment *);
    void drawMap(Mercator::Terrain &, const PosType & camPos);
    void drawSea(Renderer &, Mercator::Terrain &);
    void drawShadow(const WFMath::Point<2> & pos, float radius = 1.f);

    void removeDisplayList(int x, int y);
    void readTerrainFrom(const Atlas::Message::Element &);
    bool readTerrain();
  public:
    TerrainRenderer(Renderer &, RenderableEntity & e);
    virtual ~TerrainRenderer();

    void load(const std::string &) { }

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_TERRAIN_RENDERER_H
