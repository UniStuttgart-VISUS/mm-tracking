###############################################################################
# MAIN
###############################################################################

cmake_minimum_required (VERSION 3.8)

project(all)

# Set default install path 
#set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "default install path" FORCE)

# Add directory structure for visual studio
if(WIN32)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD TRUE)
endif()

set(GLOBAL_INSTALL_DIR    "${CMAKE_INSTALL_PREFIX}")
set(GLOBAL_BIN_DIR        "${CMAKE_INSTALL_PREFIX}/bin")
set(GLOBAL_LIB_DIR        "${CMAKE_INSTALL_PREFIX}/lib")
set(GLOBAL_LIB_STATIC_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(GLOBAL_INCLUDE_DIR    "${CMAKE_INSTALL_PREFIX}/include")


set(TRACKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tracking")
set(CHECK_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/check")

###############################
# TRACKING
###############################
add_subdirectory(${TRACKING_DIR})

###############################
# CHECK
###############################
option(ADD_TRACKING_TEST "Creates the tracking test program check.exe" OFF)
if(ADD_TRACKING_TEST)
    add_subdirectory(${CHECK_DIR})
endif()
