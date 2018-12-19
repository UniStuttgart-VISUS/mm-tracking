###############################################################################
# CHECK
###############################################################################


# Stop if the user tries to build this plugin with Linux.
if(WIN32)

    project(check)
    message(STATUS "[${PROJECT_NAME}] configuring ...")

    set(TRACKING_NAME "tracking")

    file(GLOB_RECURSE CHECK_HEADERS RELATIVE ${PROJECT_SOURCE_DIR} "include/*.h")
    file(GLOB_RECURSE CHECK_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} "src/*.cpp")
    
    include_directories(${GLOBAL_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include")

    # TARGET DEFINITION
    add_executable(${PROJECT_NAME} 
        ${CHECK_HEADERS} 
        ${CHECK_SOURCES}
    )
    add_dependencies(${PROJECT_NAME} ${TRACKING_NAME})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TRACKING_NAME})
    target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

    # INSTALL TARGET
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${GLOBAL_BIN_DIR})

    # Grouping in Visual Studio
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER plugins)
    source_group("Header Files" FILES ${CHECK_HEADERS})
    source_group("Source Files" FILES ${CHECK_SOURCES})

    message(STATUS "[${PROJECT_NAME}] DONE.")
        
endif(WIN32)