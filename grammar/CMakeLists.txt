
if (WIN32)
    add_custom_target(
        Build_Grammar
        ${CMAKE_CURRENT_SOURCE_DIR}/build.bat
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif (WIN32)

if (UNIX)
	add_custom_target(
		Build_Grammar
		${CMAKE_CURRENT_SOURCE_DIR}/build.sh
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif (UNIX)