// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef VECTOR_3D_H
#define VECTOR_3D_H

#include <Eris/Types.h>

typedef WFMath::Point<3> Point3D;
typedef WFMath::Vector<3> Vector3D;

Point3D & operator+=(Point3D & lhs, const Point3D & rhs);
Point3D & operator-=(Point3D & lhs, const Point3D & rhs);

const Point3D operator+(const Point3D & lhs, const Point3D & rhs);
// const Point3D operator-(const Point3D & lhs, const Point3D & rhs);

#endif // VECTOR_3D_H
