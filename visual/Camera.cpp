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

#include "Camera.h"

using namespace Demeter;

Camera::Camera()
{
	m_ViewplaneDistance = 1.0f;
	m_Position.x = m_Position.y = m_Position.z = 0.0f;
	m_LookAt.x = 0.0f;
	m_LookAt.y = 1.0f;
	m_LookAt.z = 0.0f;
	m_LookUp.x = 0.0f;
	m_LookUp.y = 0.0f;
	m_LookUp.z = 1.0f;
}

Camera::~Camera()
{
}

void Camera::SetPosition( float x,float y,float z )
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void Camera::SetLookAt( float x,float y,float z )
{
	m_LookAt.x = x;
	m_LookAt.y = y;
	m_LookAt.z = z;
}

void Camera::SetLookUp( float x,float y,float z )
{
	m_LookUp.x = x;
	m_LookUp.y = y;
	m_LookUp.z = z;
}

void Camera::GetPosition( Vector& position )
{
	position = m_Position;
}

void Camera::GetLookAt( Vector& lookAt )
{
	lookAt = m_LookAt;
}

void Camera::GetLookUp( Vector& up )
{
	up = m_LookUp;
}

void Camera::SetViewplaneDistance( float d )
{
	m_ViewplaneDistance = d;
}

float Camera::GetViewplaneDistance()
{
	return m_ViewplaneDistance;
}

void Camera::UpdateViewTransform()
{
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( m_Position.x,m_Position.y,m_Position.z,m_LookAt.x,m_LookAt.y,m_LookAt.z,m_LookUp.x,m_LookUp.y,m_LookUp.z );
}
