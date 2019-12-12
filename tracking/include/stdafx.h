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
#include <windows.h>
#include <winsock2.h> 

#else 
 /** other OS */
 
#error Platform not supported!

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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


/// DEFINES ///////////////////////////////////////////////////////////////////

// If defined, additional ouput for debugging ist written.
///  #define TRACKING_DEBUG_OUTPUT

// If defined, playback logs of vrpn device are written (see VrpnDevice.h line 222).
///  #define TRACKING_VRPN_DEVICE_WRITE_PLAYBACKLOG 

/// TYPES /////////////////////////////////////////////////////////////////////

namespace tracking
{
    typedef unsigned int Button;

    typedef std::array<glm::vec2, 4> Rectangle;
    /// [0] = left_top
    /// [1] = left_bottom 
    /// [2] = right_top
    /// [3] = right_bottom
}

#endif /** TRACKING_STDAFX_INCLUDED */
