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
    std::string m_textures[8];
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
    void flush();

    virtual void render(Renderer &, const PosType & camPos);
    virtual void select(Renderer &, const PosType & camPos);
};

#endif // APOGEE_TERRAIN_RENDERER_H
