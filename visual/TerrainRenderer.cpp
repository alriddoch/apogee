// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "TerrainRenderer.h"

#include "Texture.h"
#include "GLU.h"

#include "common/debug.h"

#include <Eris/Entity.h>

#include <Mercator/Segment.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/Surface.h>

#include <iostream>

using Atlas::Message::Element;

static const bool debug_flag = false;
static const int segSize = 64;

static GLfloat sx0[] = {0.125f, 0.f, 0.f, 0.f};
static GLfloat ty0[] = {0.f, 0.125f, 0.f, 0.f};

static GLfloat sx1[] = {0.015625f, 0.f, 0.f, 0.f};
static GLfloat ty1[] = {0.f, 0.015625f, 0.f, 0.f};

void TerrainRenderer::enableRendererState()
{
    glEnable(GL_NORMALIZE);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sx0);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, ty0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sx1);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, ty1);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void TerrainRenderer::disableRendererState()
{
    // Can we do this using the state stack

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glDisable(GL_BLEND);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_NORMALIZE);
}

void TerrainRenderer::generateAlphaTextures(Mercator::Segment * map)
{
    const Mercator::Segment::Surfacestore & surfaces = map->getSurfaces();
    Mercator::Segment::Surfacestore::const_iterator I = surfaces.begin();

    glGenTextures(surfaces.size(), m_alphaTextures);
    for (int texNo = 0; I != surfaces.end(); ++I, ++texNo) {
        glBindTexture(GL_TEXTURE_2D, m_alphaTextures[texNo]);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, 65, 65, GL_ALPHA,
                          GL_UNSIGNED_BYTE, (*I)->getData());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        GLenum er;
        if ((er = glGetError()) != 0) {
            std::cerr << "Booya " << gluErrorString(er)
                      << std::endl << std::flush;
        }
    }
}

void TerrainRenderer::drawRegion(Mercator::Segment * map)
{
    float harray[(segSize+1)*(segSize+1)*3];
    float * narray = map->getNormals();
    if (narray == 0) {
        std::cout << "Populating normals" << std::endl << std::flush;
        map->populateNormals();
        narray = map->getNormals();
    }
    // Fill in the vertex Z coord, which varies
    std::cout << "Populating vertex cache" << std::endl << std::flush;
    int idx = -1;
    for(int j = 0; j < (segSize + 1); ++j) {
        for(int i = 0; i < (segSize + 1); ++i) {
            float h = map->get(i,j);
            harray[++idx] = i;
            harray[++idx] = j;
            harray[++idx] = h;
        }
    }
    glNormalPointer(GL_FLOAT, 0, narray);
    glVertexPointer(3, GL_FLOAT, 0, harray);

    const Mercator::Segment::Surfacestore & surfaces = map->getSurfaces();
    Mercator::Segment::Surfacestore::const_iterator I = surfaces.begin();

    for (int texNo = 0; I != surfaces.end(); ++I, ++texNo) {
        if (!(*I)->m_shader.checkIntersect(**I)) {
            continue;
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textures[texNo]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_alphaTextures[texNo]);
                     
        glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_SHORT, m_lineIndeces);

        if (texNo == 0) {
            glDepthMask(GL_FALSE);
        }
    }

    glDepthMask(GL_TRUE);

}

using Mercator::Terrain;

void TerrainRenderer::drawMap(Mercator::Terrain & t,
                              const WFMath::Point<3> & camPos)
{
    long lowXBound = lrintf(camPos[0] / segSize) - 2,
         upXBound = lrintf(camPos[0] / segSize) + 2,
         lowYBound = lrintf(camPos[1] / segSize) - 2,
         upYBound = lrintf(camPos[1] / segSize) + 2;

    enableRendererState();

    const Terrain::Segmentstore & segs = t.getTerrain();

    Terrain::Segmentstore::const_iterator I = segs.lower_bound(lowXBound);
    Terrain::Segmentstore::const_iterator K = segs.upper_bound(upXBound);
    for (; I != K; ++I) {
        const Terrain::Segmentcolumn & col = I->second;
        TerrainRenderer::DisplayListStore::iterator M = m_displayLists.find(I->first);

        Terrain::Segmentcolumn::const_iterator J = col.lower_bound(lowYBound);
        Terrain::Segmentcolumn::const_iterator L = col.upper_bound(upYBound);
        for (; J != L; ++J) {
            DisplayListColumn & dcol = (M == m_displayLists.end()) ? 
                                           m_displayLists[I->first] :
                                           M->second;
            DisplayListColumn::const_iterator N = dcol.find(J->first);
            GLuint display_list;
            if (N != dcol.end()) {
                debug(std::cout << "Using display list for "
                                << I->first << ", " << J->first
                                << std::endl << std::flush;);
                display_list = N->second;
            } else {
                debug(std::cout << "Building display list for "
                                << I->first << ", " << J->first
                                << std::endl << std::flush;);


                Mercator::Segment * s = J->second;
                if (!s->isValid()) {
                    s->populate();
                    s->populateSurfaces();
                }

                generateAlphaTextures(s);

                display_list = glGenLists(1);
                glNewList(display_list, GL_COMPILE);

                glPushMatrix();
                glTranslatef(I->first * segSize, J->first * segSize, 0.0f);

                drawRegion(s);
                glPopMatrix();
    
                glEndList();
                dcol[J->first] = display_list;
            }

            glCallList(display_list);
        }
    }
    disableRendererState();

}

void TerrainRenderer::drawSea(Mercator::Terrain & t)
{
    const Terrain::Segmentstore & segs = t.getTerrain();

    Terrain::Segmentstore::const_iterator I = segs.begin();
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    for (; I != segs.end(); ++I) {
        const Terrain::Segmentcolumn & col = I->second;
        Terrain::Segmentcolumn::const_iterator J = col.begin();
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
    if (!terrain.isMap()) {
        std::cerr << "Terrain is not a map" << std::endl << std::flush;
    }
    const Element::MapType & tmap = terrain.asMap();
    Element::MapType::const_iterator I = tmap.find("points");
    int xmin = 0, xmax = 0, ymin = 0, ymax = 0;
    if ((I == tmap.end()) || !I->second.isList()) {
        std::cerr << "No terrain points" << std::endl << std::flush;
    }
    const Element::ListType & plist = I->second.asList();
    Element::ListType::const_iterator J = plist.begin();
    for(; J != plist.end(); ++J) {
        if (!J->isList()) {
            std::cout << "Non list in points" << std::endl << std::flush;
            continue;
        }
        const Element::ListType & point = J->asList();
        if (point.size() != 3) {
            std::cout << "point without 3 nums" << std::endl << std::flush;
            continue;
        }
        int x = (int)point[0].asNum();
        int y = (int)point[1].asNum();
        xmin = std::min(xmin, x);
        xmax = std::max(xmax, x);
        ymin = std::min(ymin, y);
        ymax = std::max(ymax, y);
        m_terrain.setBasePoint(x, y, point[2].asNum());
    }
}

TerrainRenderer::TerrainRenderer(Renderer & r, Eris::Entity & e) :
    EntityRenderer(r, e), m_terrain(Terrain::SHADED),
    m_numLineIndeces(0),
    m_lineIndeces(new unsigned short[(segSize + 1) * (segSize + 1) * 2]),
    m_landscapeList(0), m_haveTerrain(false)

{
    m_textures[0] = Texture::get("granite.png", true, GL_LINEAR_MIPMAP_NEAREST);
    m_textures[1] = Texture::get("sand.png", true, GL_LINEAR_MIPMAP_NEAREST);
    m_textures[2] = Texture::get("rabbithill_grass_hh.png", true, GL_LINEAR_MIPMAP_NEAREST);
    m_textures[3] = Texture::get("dark.png", true, GL_LINEAR_MIPMAP_NEAREST);
    m_textures[4] = Texture::get("snow.png", true, GL_LINEAR_MIPMAP_NEAREST);

    int idx = -1;
    for (unsigned int i = 0; i < (segSize + 1) - 1; ++i) {
        for (unsigned int j = 0; j < (segSize + 1); ++j) {
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

    m_terrain.addShader(new Mercator::FillShader());
    m_terrain.addShader(new Mercator::BandShader(-2.f, 1.5f)); // Sandy beach
    m_terrain.addShader(new Mercator::GrassShader(1.f, 27.f, .5f, 1.f)); // Grass
    m_terrain.addShader(new Mercator::DepthShader(0.f, -10.f)); // Underwater
    m_terrain.addShader(new Mercator::HighShader(35.f)); // Snow
}

TerrainRenderer::~TerrainRenderer()
{
}

void TerrainRenderer::render(Renderer &, const WFMath::Point<3> & camPos)
{
    if (!m_haveTerrain) {
        readTerrain();
        m_haveTerrain = true;
    }
    drawMap(m_terrain, camPos);
    drawSea(m_terrain);
}

void TerrainRenderer::select(Renderer &, const WFMath::Point<3> & camPos)
{
    drawMap(m_terrain, camPos);
    // selectTerrain(m_terrain);
}
