
message(STATUS "Configuring tests")

file(GLOB test_files "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.txt")
foreach(filepath ${test_files})
    get_filename_component(filename ${filepath} NAME_WE)
    add_test(NAME "${filename}"
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/run_test.py" "$<TARGET_FILE:FleetLang>" "${filepath}")
endforeach()