/**
* TrackingMath.cpp
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/


#include "TrackingMath.h" 


tracking::Vector3D tracking::operator *(const float lhs, const tracking::Vector3D& rhs) {
    return (rhs * lhs);
}
