###############################################################################
# TEST
###############################################################################


# Stop if the user tries to build this plugin with Linux.
if(WIN32)

    project(test)
    message(STATUS "[${PROJECT_NAME}] configuring ...")

    set(TRACKING_NAME "tracking")

    file(GLOB_RECURSE TEST_HEADERS RELATIVE ${PROJECT_SOURCE_DIR} "include/*.h")
    file(GLOB_RECURSE TEST_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} "src/*.cpp")
    
    include_directories("${CMAKE_INSTALL_PREFIX}/include" "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include")

    # TARGET DEFINITION
    add_executable(${PROJECT_NAME} 
        ${TEST_HEADERS} 
        ${TEST_SOURCES}
    )
    add_dependencies(${PROJECT_NAME} ${TRACKING_NAME})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TRACKING_NAME})

    # INSTALL TARGET
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "bin")

    # Grouping in Visual Studio
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER plugins)
    source_group("Header Files" FILES ${TEST_HEADERS})
    source_group("Source Files" FILES ${TEST_SOURCES})

    message(STATUS "[${PROJECT_NAME}] DONE.")
        
endif(WIN32)
