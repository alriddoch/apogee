// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "TileMap.h"
#include "Tile.h"

#include <coal/container.h>
#include <coal/maptile.h>

#include <GL/gl.h>

TileMap::TileMap()
{

}

void TileMap::build(Coal::Container & map_base)
{
#if 1
    const std::vector<Coal::Component*> & contents = map_base.getChildren();
    std::vector<Coal::Component*>::const_iterator I = contents.begin();
    for (; I != contents.end(); ++I) {
        Coal::MapTile * tile = dynamic_cast<Coal::MapTile*>(*I);
        if (tile == NULL) { continue; }
        Coal::Graphic * fill = tile->getGraphic();
        if ((fill == NULL) || fill->filename.empty()) {
            continue;
        }
        Tile * tileImage = Tile::get(fill->filename);
        if (tileImage == NULL) {
            continue;
        }
        Coal::Coord coord = tile->getAnchor();
        float bx = coord.x;
        float by = coord.y;
        add((int)bx, (int)by, tileImage);
    }

#else
    int count = map_base.GetRegionCount();
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region) {
            CoalFill * fill = region->GetFill();
            if ((fill == NULL) || (fill->graphic == NULL) ||
                (fill->graphic->filename.size() == 0)) {
                continue;
            }
            Tile * tile = Tile::get(fill->graphic->filename);
            if (tile == NULL) {
                continue;
            }
            CoalShape & shape = *region;
            const CoalCoord & coord = shape.GetCoord(0);
            float bx = coord.GetX();
            float by = coord.GetY();
            add((int)bx, (int)by, tile);
        }
    }
#endif
}

void TileMap::draw(HeightMap & map_height, int x_offset, int y_offset)
{
    for(int i = -10; i < 10; i++) {
        for(int j = -10; j < 10; j++) {
            int x = x_offset + 100 + i;
            int y = y_offset + 100 + j;
            Tile * t = get(x, y);
            if (t == NULL) { continue; }
            glPushMatrix();
            glTranslatef(x, y, 0);
            t->draw(map_height, x, y);
            // t->draw();
            glPopMatrix();
        }
    }
}
