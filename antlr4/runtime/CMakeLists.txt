
include_directories(
        ${PROJECT_SOURCE_DIR}/runtime/src
        ${PROJECT_SOURCE_DIR}/runtime/src/atn
        ${PROJECT_SOURCE_DIR}/runtime/src/dfa
        ${PROJECT_SOURCE_DIR}/runtime/src/misc
        ${PROJECT_SOURCE_DIR}/runtime/src/support
        ${PROJECT_SOURCE_DIR}/runtime/src/tree
        ${PROJECT_SOURCE_DIR}/runtime/src/tree/pattern
        ${PROJECT_SOURCE_DIR}/runtime/src/tree/xpath
)


file(GLOB libantlrcpp_SRC
        "${PROJECT_SOURCE_DIR}/runtime/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/runtime/src/atn/*.cpp"
        "${PROJECT_SOURCE_DIR}/runtime/src/dfa/*.cpp"
        "${PROJECT_SOURCE_DIR}/runtime/src/misc/*.cpp"
        "${PROJECT_SOURCE_DIR}/runtime/src/support/*.cpp"
        "${PROJECT_SOURCE_DIR}/runtime/src/tree/*.cpp"
        "${PROJECT_SOURCE_DIR}/runtime/src/tree/pattern/*.cpp"
        "${PROJECT_SOURCE_DIR}/runtime/src/tree/xpath/*.cpp"
        )

if (ANTLR4_BUILD_SHARED)
    add_library(antlr4_shared SHARED ${libantlrcpp_SRC})
endif ()
add_library(antlr4_static STATIC ${libantlrcpp_SRC})

set(LIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/dist") # put generated libraries here.
message(STATUS "Output libraries to ${LIB_OUTPUT_DIR}")

# make sure 'make' works fine even if ${LIB_OUTPUT_DIR} is deleted.
add_custom_target(make_lib_output_dir ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_OUTPUT_DIR}
        )

if (ANTLR4_BUILD_SHARED)
    add_dependencies(antlr4_shared make_lib_output_dir)
endif ()
add_dependencies(antlr4_static make_lib_output_dir)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    if (ANTLR4_BUILD_SHARED)
        target_link_libraries(antlr4_shared ${UUID_LIBRARIES})
    endif ()
    target_link_libraries(antlr4_static ${UUID_LIBRARIES})
elseif (APPLE)
    if (ANTLR4_BUILD_SHARED)
        target_link_libraries(antlr4_shared ${COREFOUNDATION_LIBRARY})
    endif()
    target_link_libraries(antlr4_static ${COREFOUNDATION_LIBRARY})
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(disabled_compile_warnings "/wd4251")
else ()
    set(disabled_compile_warnings "-Wno-overloaded-virtual")
endif ()


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(disabled_compile_warnings "${disabled_compile_warnings} -Wno-dollar-in-identifier-extension -Wno-four-char-constants")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(disabled_compile_warnings "${disabled_compile_warnings} -Wno-multichar")
endif ()

if (ANTLR4_BUILD_SHARED)
    set(extra_share_compile_flags "")
    set(extra_static_compile_flags "")
endif ()
if (WIN32)
    if (ANTLR4_BUILD_SHARED)
        set(extra_share_compile_flags "-DANTLR4CPP_EXPORTS")
    endif ()
    set(extra_static_compile_flags "-DANTLR4CPP_STATIC")
endif (WIN32)
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (ANTLR4_BUILD_SHARED)
        target_compile_options(antlr4_shared PRIVATE "/MD$<$<CONFIG:Debug>:d>")
    endif ()
    target_compile_options(antlr4_static PRIVATE "/MT$<$<CONFIG:Debug>:d>")
endif ()

set(static_lib_suffix "")
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(static_lib_suffix "-static")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (ANTLR4_BUILD_SHARED)
        set(extra_share_compile_flags "-DANTLR4CPP_EXPORTS -MP /wd4251")
    endif ()
    set(extra_static_compile_flags "-DANTLR4CPP_STATIC -MP")
endif ()

if (ANTLR4_BUILD_SHARED)
    set_target_properties(antlr4_shared
            PROPERTIES VERSION ${ANTLR_VERSION}
            SOVERSION ${ANTLR_VERSION}
            OUTPUT_NAME antlr4-runtime
            LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
            # TODO: test in windows. DLL is treated as runtime.
            # see https://cmake.org/cmake/help/v3.0/prop_tgt/LIBRARY_OUTPUT_DIRECTORY.html
            RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
            ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
            COMPILE_FLAGS "${disabled_compile_warnings} ${extra_share_compile_flags}")
endif ()

set_target_properties(antlr4_static
        PROPERTIES VERSION ${ANTLR_VERSION}
        SOVERSION ${ANTLR_VERSION}
        OUTPUT_NAME "antlr4-runtime${static_lib_suffix}"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        COMPILE_FLAGS "${disabled_compile_warnings} ${extra_static_compile_flags}")

if (ANTLR4_BUILD_SHARED)
    install(TARGETS antlr4_shared
            DESTINATION lib)
endif ()

install(TARGETS antlr4_static
        ARCHIVE DESTINATION lib)

set(ANTLR4_INCLUDE_DIR "${CMAKE_BINARY_DIR}/antlr4/include" CACHE PATH "Path to the Antlr4 include files" FORCE)

file(COPY "${PROJECT_SOURCE_DIR}/runtime/src/" DESTINATION "${ANTLR4_INCLUDE_DIR}" FILES_MATCHING PATTERN "*.h")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/runtime/src/"
        DESTINATION "include/antlr4-runtime"
        COMPONENT dev
        FILES_MATCHING PATTERN "*.h"
        )



