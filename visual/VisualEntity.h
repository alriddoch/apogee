// Apogee Online RPG Test 3D Client
// Copyright (C) 2000-2001 Alistair Riddoch
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

#error This file has been removed from the build

#ifndef APOGEE_VISUAL_VISUALENTITY_H
#define APOGEE_VISUAL_VISUALENTITY_H

#include <common/Vector3D.h>

class VisualEntity
{
  private:
    bool m_topLevel;
    VisualEntity * m_ref;
    Vector3D m_pos, m_velocity, m_bbox, m_bmedian;

  public:
    VisualEntity() : m_topLevel(false), m_ref(NULL) { }
    
    bool isTopLevel() const { return m_topLevel; }

    const Vector3D & pos() const { return m_pos; }
    Vector3D & pos() { return m_pos; }
    const Vector3D & velocity() const { return m_velocity; }
    Vector3D & velocity() { return m_velocity; }
    const Vector3D & bBox() const { return m_bbox; }
    Vector3D & bBox() { return m_bbox; }
    const Vector3D & bMedian() const { return m_bmedian; }
    Vector3D & bMedian() { return m_bmedian; }

    VisualEntity * ref() const { return m_ref; }
    VisualEntity * & ref() { return m_ref; }

    // void update(float usecs) {
        // m_pos += m_velocity * usecs / 1000000;
    // }
};

#endif // APOGEE_VISUAL_VISUALENTITY_H
