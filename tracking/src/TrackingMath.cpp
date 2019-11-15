/**
* TrackingMath.cpp
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#include "TrackingMath.h" 


mm_tracking::Vector3D mm_tracking::operator *(const float lhs, const mm_tracking::Vector3D& rhs) {
    return (rhs * lhs);
}
