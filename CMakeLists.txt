cmake_minimum_required(VERSION 3.1.3)
project(FleetLang)

# set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

add_subdirectory(test)
add_subdirectory(grammar)
add_subdirectory(antlr4)
include_directories(SYSTEM ${CMAKE_BINARY_DIR}/antlr4/include)

if (UNIX)
        #   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-fuse-ld=gold -rdynamic")
        #   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fvisibility-inlines-hidden -ffunction-sections -fdata-sections")
        #   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=date-time -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wno-missing-field-initializers -Wno-long-long -Wno-uninitialized -Wdelete-non-virtual-dtor -Wno-comment")
        #   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
    find_package(LLVM 6 REQUIRED CONFIG)
    if (${LLVM_FOUND})
        message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
        message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

        include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "LLVM Not found!")
    endif()
endif(UNIX)

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

if (MSVC)
        message(FATAL_ERROR "Windows target is not ready to use yet")
        if (${CMAKE_SIZEOF_VOID_P} EQUAL "8")
                set (FLEETLANG_ARCHITECTURE "x64")
        else ()
                set (FLEETLANG_ARCHITECTURE "x32")
        endif ()

        include_directories(SYSTEM "win-libs/llvm/$(Configuration)/${FLEETLANG_ARCHITECTURE}/include")
        link_directories("win-libs/llvm/$(Configuration)/${FLEETLANG_ARCHITECTURE}/lib")
        link_directories("win-libs/antlr4/$(Configuration)/${FLEETLANG_ARCHITECTURE}/")

        set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Visual Studio condiguration files" FORCE)

endif(MSVC)


set(SOURCE_FILES
        src/main.cpp
        src/Helpers.h
        src/Helpers.cpp

        src/AST/ExprAST.cpp
        src/AST/ExprAST.h
        src/AST/StmtAST.cpp
        src/AST/StmtAST.h
        src/NewVisitor.cpp
        src/NewVisitor.h
        src/AST/Program.cpp
        src/AST/Program.h
        src/AST/Function.cpp
        src/AST/Function.h
        src/AST/CodegenHelper.cpp
        src/AST/CodegenHelper.h

        src/Validator.cpp
        src/Validator.h
        src/SymbolTable.cpp
        src/SymbolTable.h
        src/AST/Type.h)

#add -pedantic only to out sources, not to GRAMMAR_SOURCES
set_property(SOURCE ${SOURCE_FILES} PROPERTY COMPILE_FLAGS -pedantic)

set(GEN_DIR "${PROJECT_BINARY_DIR}/generated" CACHE INTERNAL "" FORCE)

antlr_grammar(${CMAKE_CURRENT_SOURCE_DIR}/grammar FleetLang ${GEN_DIR})

include_directories(SYSTEM ${GEN_DIR})

add_executable(FleetLang ${SOURCE_FILES} ${FleetLang_GRAMMAR_SOURCES})

target_link_libraries(FleetLang antlr4_static LLVM)

add_dependencies(Test FleetLang)
add_dependencies(FleetLang antlr4_static)
