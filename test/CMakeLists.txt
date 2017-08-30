
add_custom_target(
        Test
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build.sh "$<TARGET_FILE:FleetLang>"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
        )