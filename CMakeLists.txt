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
# TEST
###############################
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")

option(CREATE_TRACKING_TEST_PROGRAM "Configure the tracking test program." OFF)
if(CREATE_TRACKING_TEST_PROGRAM)
    add_subdirectory(${TEST_DIR})
endif()
