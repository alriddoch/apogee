// Demeter Terrain Visualization Library by Clay Fowler
// Copyright (C) 2001 Clay Fowler

/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.
*/

#ifndef _CAMERA_DEMETER_VIEWER_H_
#define _CAMERA_DEMETER_VIEWER_H_

#include <Demeter/Terrain.h>

// This is a quick & dirty camera that is functional enough for viewing terrains - real applications
// will usually need something more industrial...
class Camera
{
public:
                    Camera();
                    ~Camera();
    void            SetPosition( float x,float y,float z );
    void            SetLookAt( float x,float y,float z );
    void            SetLookUp( float x,float y,float z );
    void            GetPosition( Demeter::Vector& position );
    void            GetLookAt( Demeter::Vector& lookAt );
    void            GetLookUp( Demeter::Vector& up );
    void            SetViewplaneDistance( float d );
    float            GetViewplaneDistance();
    void            UpdateViewTransform();
private:
    float            m_ViewplaneDistance;
    Demeter::Vector    m_Position,m_LookAt,m_LookUp;
};

#endif
