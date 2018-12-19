###############################################################################
# TRACKING
###############################################################################


# Stop if the user tries to build this plugin with Linux.
if(WIN32)
  
    project(tracking)
    
    string(TOUPPER ${PROJECT_NAME} EXPORT_NAME)
    
    file(GLOB_RECURSE TRACKING_HEADERS RELATIVE ${PROJECT_SOURCE_DIR} "include/*.h")
    file(GLOB_RECURSE TRACKING_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} "src/*.cpp")
    
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

    set(TRACKING_INSTALL_HEADER "include/TrackingUtilizer.h" "include/Tracker.h")
    set(TRACKING_INSTALL_CONF "conf/tracking.conf" )

    
    message(STATUS "[${PROJECT_NAME}] configuring VRPN ...")
        
    # Get the external vrpn project from GitHub.
    include(ExternalProject)
    
    # Set the install location and the version for the vrpn project.
    set(VRPN_INSTALL_LOCATION "${GLOBAL_INSTALL_DIR}" CACHE INTERNAL '')
    set(VRPN_VER "v07.34")
    
    # Set the paths for the needed libraries and include files from the vrpn
    # project.
    set(VRPN_INCLUDE_COPY "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Assert.h"     "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_BaseClass.h" 
                          "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Button.h"     "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Configure.h" 
  						  "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Connection.h" "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_EndpointContainer.h" 
  						  "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Serial.h"     "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Shared.h" 
  						  "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Thread.h"     "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Tracker.h" 
  						  "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_Types.h"      "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn/vrpn_WindowsH.h")
	
    set(VRPN_INCLUDE_DIR ${GLOBAL_INCLUDE_DIR})
    # Directory is required for installation of vrpn header files
    file(MAKE_DIRECTORY ${VRPN_INCLUDE_DIR})
    
    # Add the vrpn project as an external project and set the cmake commands
    # for configuring and building it. For some reason the x64 flag is
    # necessary because if it is not set vrpn is using x86. The BUILD_COMMAND
	# is issued three times, once for Debug, once for Release and once for
	# RelWithDebInfo since cmake is unable to determine what the current 
	# setting in Visual Studio is.
    ExternalProject_Add(
  	  vrpn
      GIT_REPOSITORY https://github.com/vrpn/vrpn.git
      GIT_TAG ${VRPN_VER} # the only version the software was tested with
  	  GIT_SUBMODULES
  	  CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_GENERATOR_PLATFORM=x64 -DVRPN_BUILD_CLIENTS:BOOL=OFF -DVRPN_BUILD_SERVERS:BOOL=OFF ${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn
  	  BUILD_COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn-build --config Debug COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn-build --config Release COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn-build --config RelWithDebInfo
  	  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${VRPN_INCLUDE_COPY} ${VRPN_INCLUDE_DIR}
    )
    #INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${VRPN_INCLUDE_DIR} -E copy ${VRPN_INCLUDE_COPY} ${VRPN_INCLUDE_DIR}
    

    # Set the paths to the libraries.
    set(QUAT_LIBRARY debug "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn-build/quat/Debug/quatd.lib" optimized "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn-build/quat/Release/quat.lib")
    set(VRPN_LIBRARY debug "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn-build/Debug/vrpnd.lib" optimized "${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn-build/Release/vrpn.lib")  
        
        
    message(STATUS "[${PROJECT_NAME}] configuring NATNET ...")
    
    # Find the NatNet library, the location of the NatNet dll and the location of the
    # include files.
    option(USE_NATNET "use NatNet" ON)
    if(USE_NATNET)
      find_package(NATNET REQUIRED)
      message(STATUS ${NATNET_LIBRARIES})
      message(STATUS ${NATNET_DLL_DIR})
      message(STATUS ${NATNET_INCLUDE_DIR})
      if (NOT NATNET_FOUND)
        message(FATAL_ERROR "Cannot find NatNet")
      endif()
    endif(USE_NATNET)

    
    message(STATUS "[${PROJECT_NAME}] configuring ${PROJECT_NAME} ...")
    
    include_directories(${VRPN_INCLUDE_DIR} ${NATNET_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/src")
  
    # TARGET DEFINITION
    add_library(${PROJECT_NAME} SHARED ${TRACKING_HEADERS} ${TRACKING_SOURCES})
    add_dependencies(${PROJECT_NAME} "vrpn")
    set(TRACKING_LIBS ${NATNET_LIBRARIES} ${VRPN_LIBRARY} ${QUAT_LIBRARY})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TRACKING_LIBS})
    target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/natnet/include>)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "${EXPORT_NAME}_EXPORTS")
  
    # INSTALLATION
    install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME} ${PROJECT_NAME} 
        RUNTIME DESTINATION ${GLOBAL_BIN_DIR}
        LIBRARY DESTINATION ${GLOBAL_LIB_DIR}
        ARCHIVE DESTINATION ${GLOBAL_LIB_STATIC_DIR}
    )
  	install(FILES ${NATNET_DLL_DIR} DESTINATION ${GLOBAL_BIN_DIR})
    install(FILES ${TRACKING_INSTALL_CONF} DESTINATION ${GLOBAL_BIN_DIR})
    #install(FILES ${TRACKING_INSTALL_HEADER} DESTINATION ${GLOBAL_INCLUDE_DIR})
    #install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" DESTINATION ${GLOBAL_INCLUDE_DIR})
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/natnet/include/" DESTINATION ${GLOBAL_INCLUDE_DIR})

    # Grouping in Visual Studio
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER plugins)
    source_group("Header Files" FILES ${TRACKING_HEADERS})
    source_group("Source Files" FILES ${TRACKING_SOURCES})
  
    message(STATUS "[${PROJECT_NAME}] DONE.")

endif(WIN32)
