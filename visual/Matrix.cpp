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

#include "Matrix.h"

using namespace Demeter;

inline float& Matrix::operator() ( int i,int j )
{
    return m_Elements[i][j];
}

inline const float& Matrix::operator() ( int i,int j ) const
{
    return m_Elements[i][j];
}

void Matrix::SetIdentity()
{
    m_Elements[0][0] = 1.0f;
    m_Elements[0][1] = 0.0f;
    m_Elements[0][2] = 0.0f;
    m_Elements[0][3] = 0.0f;

    m_Elements[1][0] = 0.0f;
    m_Elements[1][1] = 1.0f;
    m_Elements[1][2] = 0.0f;
    m_Elements[1][3] = 0.0f;

    m_Elements[2][0] = 0.0f;
    m_Elements[2][1] = 0.0f;
    m_Elements[2][2] = 1.0f;
    m_Elements[2][3] = 0.0f;

    m_Elements[3][0] = 0.0f;
    m_Elements[3][1] = 0.0f;
    m_Elements[3][2] = 0.0f;
    m_Elements[3][3] = 1.0f;
}

void Matrix::SetRotateZ( float theta )
{
    SetIdentity();
    m_Elements[0][0] = (float)cos( theta );
    m_Elements[0][1] = (float)sin( theta );
    m_Elements[1][0] = -(float)sin( theta );
    m_Elements[1][1] = (float)cos( theta );
}

void Matrix::SetRotateX( float theta )
{
    SetIdentity();
    m_Elements[1][1] = (float)cos( theta );
    m_Elements[1][2] = (float)sin( theta );
    m_Elements[2][1] = -(float)sin( theta );
    m_Elements[2][2] = (float)cos( theta );
}

void Matrix::SetRotateY( float theta )
{
    SetIdentity();
    m_Elements[0][0] = (float)cos( theta );
    m_Elements[0][2] = -(float)sin( theta );
    m_Elements[2][0] = (float)sin( theta );
    m_Elements[2][2] = (float)cos( theta );
}

Matrix Matrix::operator * ( const Matrix& matOther )
{
    long int        i,j;
    Matrix        matResult;
    Matrix&        matThis = *this;

    for (i = 0; i <= 3; i++)
    {
        for (j = 0; j <= 3; j++)
        {
            matResult( i,j ) = matThis( i,0 ) * matOther( 0,j ) +
                matThis( i,1 ) * matOther( 1,j ) + 
                matThis( i,2 ) * matOther( 2,j ) + 
                matThis( i,3 ) * matOther( 3,j );
        }
    }

    return matResult;
}

Vector Matrix::operator * ( const Vector& vector )
{
    Vector        vResult;
    Matrix&        matThis = *this;

    vResult.x = vector.x * matThis( 0,0 ) + vector.y * matThis( 1,0 ) +
    vector.z * matThis( 2,0 );
    vResult.y = vector.x * matThis( 0,1 ) + vector.y * matThis( 1,1 ) +
    vector.z * matThis( 2,1 );
    vResult.z = vector.z * matThis( 0,2 ) + vector.y * matThis( 1,2 ) +
    vector.z * matThis( 2,2 );

    return vResult;
}
