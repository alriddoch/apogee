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

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <Demeter/Terrain.h>

// Stripped down matrix taken mostly from David Eberly's excellent matrix class. See his book
// "3D Game Engine Design" for a complete implementation of this class.
// This class is embedded here in the sample to avoid making the sample dependent on external math libraries.
////////////////////////////////////////////////
class Matrix
{
public:
                                Matrix() {     SetIdentity(); }
                                ~Matrix() {}
    float&                      operator() ( int i,int j );
    const float&                operator() ( int i,int j ) const;
    Matrix                      operator * ( const Matrix& matOther );
    Demeter::Vector             operator * ( const Demeter::Vector& vector );
    void                        SetRotateZ( float theta );
    void                        SetRotateX( float theta );
    void                        SetRotateY( float theta );
    void                        SetIdentity();
private:
    float                        m_Elements[4][4];
};

#endif
