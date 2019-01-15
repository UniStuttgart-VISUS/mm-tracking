/**
 * stdafx.h
 *
 * Copyright (C) 2014-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#ifndef TRACKING_STDAFX_INCLUDED
#define TRACKING_STDAFX_INCLUDED

#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /** (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#ifdef _WIN32 
/** Windows includes */
 
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define TRACKING_AVAILABLE

#else 
 /** other OS */
 
#error Platform not supported

#endif /** _WIN32 */

/// HEADERS ///////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <array>

/// DEFINES ///////////////////////////////////////////////////////////////////

// If defined, additional ouput for debugging ist written.
//#define TRACKING_DEBUG_OUTPUT

// If defined, playback logs of vrpn device are written (see VrpnDevice.h line 222).
//#define TRACKING_VRPN_DEVICE_WRITE_PLAYBACKLOG 

/// TYPES /////////////////////////////////////////////////////////////////////

#include "TrackingMath.h"

namespace tracking
{
    typedef unsigned int ButtonMask;
    
    typedef struct {
        Point2D left_top;
        Point2D left_bottom;
        Point2D right_top;
        Point2D right_bottom;
    } Rectangle;

}

#endif /** TRACKING_STDAFX_INCLUDED */
