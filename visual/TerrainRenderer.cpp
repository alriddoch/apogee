// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "TerrainRenderer.h"

#include "Texture.h"
#include "GL.h"

#include <Mercator/Segment.h>

static const int segSize = 64;

void TerrainRenderer::drawRegion(Mercator::Segment * map)
{
    GLint texture = -1, texture2 = -1;
    texture = Texture::get("granite.png");
    texture2 = Texture::get("rabbithill_grass_hh.png");
    static float * harray = 0;
    static float * carray = 0;
    static int allocated_segSize = 0;
    // Only re-allocate the vertex arrays if we are dealing with a different
    // segment size.
    if (segSize != allocated_segSize) {
        if (harray != 0) {
            delete [] harray;
            delete [] carray;
            harray = 0;
            carray = 0;
        }
        harray = new float[(segSize + 1) * (segSize + 1) * 3];
        carray = new float[(segSize + 1) * (segSize + 1) * 4];
        allocated_segSize = segSize;
        int idx = -1, cdx = -1;
        // Fill in the invarient vertices and colors, so we only do it once
        for(int j = 0; j < (segSize + 1); ++j) {
            for(int i = 0; i < (segSize + 1); ++i) {
                // float h = map->get(i,j);
                harray[++idx] = i;
                harray[++idx] = j;
                harray[++idx] = 0.f;
                carray[++cdx] = 1.f;
                carray[++cdx] = 1.f;
                carray[++cdx] = 1.f;
                carray[++cdx] = 0.f;
            }
        }
    }
    // Fill in the vertex Z coord, and alpha value, which vary
    int idx = -1, cdx = -1;
    for(int j = 0; j < (segSize + 1); ++j) {
        for(int i = 0; i < (segSize + 1); ++i) {
            float h = map->get(i,j);
            idx += 2;
            harray[++idx] = h;
            cdx += 3;
            carray[++cdx] = (h > 0.4f) ? 1.f : 0.f;
        }
    }
    if (texture != -1) {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);
        glColorPointer(4, GL_FLOAT, 0, carray);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    glVertexPointer(3, GL_FLOAT, 0, harray);
    if (have_GL_EXT_compiled_vertex_array) {
        glLockArraysEXT(0, (segSize + 1) * (segSize + 1));
    }
    glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                   GL_UNSIGNED_INT, m_lineIndeces);
    if ((texture != -1) && (texture2 != -1)) {
        glBindTexture(GL_TEXTURE_2D, texture2);
        glEnable(GL_BLEND);
        glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_INT, m_lineIndeces);
        glDisable(GL_BLEND);
    }
    if (have_GL_EXT_compiled_vertex_array) {
        glUnlockArraysEXT();
    }
    if (texture != -1) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
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
            drawRegion(J->second);
            glPopMatrix();
        }
    }
}

TerrainRenderer::TerrainRenderer(Eris::Entity & e) :
    EntityRenderer(e), m_numLineIndeces(0),
    m_lineIndeces(new unsigned int[(segSize + 1) * (segSize + 1) * 2]),
    m_texCoords(new float[(segSize + 1) * (segSize + 1) * 3])

{
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
}

TerrainRenderer::~TerrainRenderer()
{
}

void TerrainRenderer::render(Renderer &)
{
}

void TerrainRenderer::select(Renderer &)
{
}
