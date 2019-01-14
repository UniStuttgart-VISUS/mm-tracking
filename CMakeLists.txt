###############################################################################
# MAIN
###############################################################################

cmake_minimum_required (VERSION 3.8)

project(all)

# Set default install path 
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}
    CACHE PATH "default install path" FORCE)
endif()

# Add directory structure for visual studio
if(WIN32)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD TRUE)
endif()

###############################
# TRACKING
###############################
set(TRACKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tracking")

add_subdirectory(${TRACKING_DIR})

###############################
# CHECK
###############################
set(CHECK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/check")

option(CREATE_TRACKING_TEST_PROGRAM "Creates the tracking test program check.exe" OFF)
if(CREATE_TRACKING_TEST_PROGRAM)
    add_subdirectory(${CHECK_DIR})
endif()
