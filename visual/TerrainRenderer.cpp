// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "TerrainRenderer.h"

#include "Texture.h"

#include <Eris/Entity.h>

#include <Mercator/Segment.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/Surface.h>

#include <iostream>

typedef Atlas::Message::Object Element;

static const int segSize = 64;

void TerrainRenderer::drawRegion(Mercator::Segment * map)
{
    glNormal3f(0.f, 0.f, 1.f);
    static float * harray = 0;
    float * narray = map->getNormals();
    static int allocated_segSize = 0;
    if (narray == 0) {
        map->populateNormals();
        narray = map->getNormals();
    }
    // Only re-allocate the vertex arrays if we are dealing with a different
    // segment size.
    if (segSize != allocated_segSize) {
        if (harray != 0) {
            delete [] harray;
            harray = 0;
        }
        harray = new float[(segSize + 1) * (segSize + 1) * 3];
        allocated_segSize = segSize;
        int idx = -1;
        // Fill in the invarient vertices and colors, so we only do it once
        for(int j = 0; j < (segSize + 1); ++j) {
            for(int i = 0; i < (segSize + 1); ++i) {
                // float h = map->get(i,j);
                harray[++idx] = i;
                harray[++idx] = j;
                harray[++idx] = 0.f;
            }
        }
    }
    // Fill in the vertex Z coord, and alpha value, which vary
    int idx = -1;
    for(int j = 0; j < (segSize + 1); ++j) {
        for(int i = 0; i < (segSize + 1); ++i) {
            float h = map->get(i,j);
            idx += 2;
            harray[++idx] = h;
        }
    }
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);
    glNormalPointer(GL_FLOAT, 0, narray);
    glVertexPointer(3, GL_FLOAT, 0, harray);
    // if (have_GL_EXT_compiled_vertex_array) {
        // glLockArraysEXT(0, (segSize + 1) * (segSize + 1));
    // }
    glEnable(GL_BLEND);

    const Mercator::Segment::Surfacestore & surfaces = map->getSurfaces();
    Mercator::Segment::Surfacestore::const_iterator I = surfaces.begin();
    for (int texNo = 0; I != surfaces.end(); ++I, ++texNo) {
        glColorPointer(4, GL_FLOAT, 0, (*I)->getData());
        glBindTexture(GL_TEXTURE_2D, m_textures[texNo]);
        glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_INT, m_lineIndeces);

        // glBindTexture(GL_TEXTURE_2D, m_textures[2]);
        // glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                       // GL_UNSIGNED_INT, m_lineIndeces);
    }

    glDisable(GL_BLEND);

    // if (have_GL_EXT_compiled_vertex_array) {
        // glUnlockArraysEXT();
    // }
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
}

void TerrainRenderer::drawMap(Mercator::Terrain & t)
{
    const Mercator::Terrain::Segmentstore & segs = t.getTerrain();

    Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            Mercator::Segment * s = J->second;
            if (!s->isValid()) {
                s->populate();
                s->populateSurfaces();
            }
            drawRegion(s);
            glPopMatrix();
        }
    }
}

void TerrainRenderer::drawSea(Mercator::Terrain & t)
{
    const Mercator::Terrain::Segmentstore & segs = t.getTerrain();

    Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            GLfloat vertices[] = { 0.f, 0.f, 0.f,
                                   segSize, 0, 0.f,
                                   segSize, segSize, 0.f,
                                   0, segSize, 0.f };
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glColor4f(0.8f, 0.8f, 1.f, 0.6f);
            glDrawArrays(GL_QUADS, 0, 4);
            glPopMatrix();
        }
    }
    glDisable(GL_BLEND);
    glEnable(GL_FOG);
    glEnable(GL_LIGHTING);
}

void TerrainRenderer::readTerrain()
{
    if (!m_ent.hasProperty("terrain")) {
        std::cerr << "World entity has no terrain" << std::endl << std::flush;
        std::cerr << "World entity id " << m_ent.getID() << std::endl
                  << std::flush;
        return;
    }
    const Element & terrain = m_ent.getProperty("terrain");
    if (!terrain.IsMap()) {
        std::cerr << "Terrain is not a map" << std::endl << std::flush;
    }
    const Element::MapType & tmap = terrain.AsMap();
    Element::MapType::const_iterator I = tmap.find("points");
    int xmin = 0, xmax = 0, ymin = 0, ymax = 0;
    if ((I == tmap.end()) || !I->second.IsList()) {
        std::cerr << "No terrain points" << std::endl << std::flush;
    }
    const Element::ListType & plist = I->second.AsList();
    Element::ListType::const_iterator J = plist.begin();
    for(; J != plist.end(); ++J) {
        if (!J->IsList()) {
            std::cout << "Non list in points" << std::endl << std::flush;
            continue;
        }
        const Element::ListType & point = J->AsList();
        if (point.size() != 3) {
            std::cout << "point without 3 nums" << std::endl << std::flush;
            continue;
        }
        int x = (int)point[0].AsNum();
        int y = (int)point[1].AsNum();
        xmin = std::min(xmin, x);
        xmax = std::max(xmax, x);
        ymin = std::min(ymin, y);
        ymax = std::max(ymax, y);
        m_terrain.setBasePoint(x, y, point[2].AsNum());
    }
}

TerrainRenderer::TerrainRenderer(Renderer & r, Eris::Entity & e) :
    EntityRenderer(r, e), m_terrain(Mercator::Terrain::SHADED),
    m_numLineIndeces(0),
    m_lineIndeces(new unsigned int[(segSize + 1) * (segSize + 1) * 2]),
    m_texCoords(new float[(segSize + 1) * (segSize + 1) * 3]),
    m_haveTerrain(false)

{
    m_textures[0] = Texture::get("granite.png");
    // m_textures[1] = Texture::get("media/media_new/scratchpad/pato/textures/finished/ground_sand_256x256_00.png");
    m_textures[1] = Texture::get("sand.png");
    m_textures[2] = Texture::get("rabbithill_grass_hh.png");
    m_textures[3] = Texture::get("dark.png");
    m_textures[4] = Texture::get("snow.png");

    int idx = -1;
    for (int i = 0; i < (segSize + 1) - 1; ++i) {
        for (int j = 0; j < (segSize + 1); ++j) {
            m_lineIndeces[++idx] = j * (segSize + 1) + i;
            m_lineIndeces[++idx] = j * (segSize + 1) + i + 1;
        }
        if (++i >= (segSize + 1) - 1) { break; }
        for (int j = (segSize + 1) - 1; j >= 0; --j) {
            m_lineIndeces[++idx] = j * (segSize + 1) + i + 1;
            m_lineIndeces[++idx] = j * (segSize + 1) + i;
        }
    }
    m_numLineIndeces = ++idx;

    int tidx = -1;
    for(int j = 0; j < (segSize + 1); ++j) {
        for(int i = 0; i < (segSize + 1); ++i) {
            m_texCoords[++tidx] = ((float)i)/8;
            m_texCoords[++tidx] = ((float)j)/8;
        }
    }

    m_terrain.addShader(new Mercator::FillShader());
    m_terrain.addShader(new Mercator::BandShader(-2.f, 1.5f)); // Sandy beach
    m_terrain.addShader(new Mercator::GrassShader(1.f, 27.f, .5f, 1.f)); // Grass
    m_terrain.addShader(new Mercator::DepthShader(0.f, -10.f)); // Underwater
    m_terrain.addShader(new Mercator::HighShader(35.f)); // Snow
}

TerrainRenderer::~TerrainRenderer()
{
}

void TerrainRenderer::render(Renderer &)
{
    if (!m_haveTerrain) {
        readTerrain();
        m_haveTerrain = true;
    }
    drawMap(m_terrain);
    drawSea(m_terrain);
}

void TerrainRenderer::select(Renderer &)
{
    drawMap(m_terrain);
    // selectTerrain(m_terrain);
}
