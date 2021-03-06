cmake_minimum_required(VERSION 3.5.1)
project(FleetLang)

#set(CMAKE_VERBOSE_MAKEFILE TRUE)

add_subdirectory(test)
add_subdirectory(grammar)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

if (UNIX)
#	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-fuse-ld=gold -rdynamic")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fvisibility-inlines-hidden -ffunction-sections -fdata-sections")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=date-time -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wno-missing-field-initializers -Wno-long-long -Wno-uninitialized -Wdelete-non-virtual-dtor -Wno-comment")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

	find_package(LLVM REQUIRED CONFIG)
	include_directories(SYSTEM ${LLVM_INCLUDE_DIR})

	link_directories(antlr4/lib)
endif(UNIX)
if (MSVC)
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

include_directories(SYSTEM gen)
include_directories(SYSTEM antlr4/include)


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

set(GRAMMAR_SOURCES
		gen/FleetLangBaseVisitor.cpp
		gen/FleetLangBaseVisitor.h
		gen/FleetLangLexer.cpp
		gen/FleetLangLexer.h
		gen/FleetLangParser.cpp
		gen/FleetLangParser.h
		gen/FleetLangVisitor.cpp
		gen/FleetLangVisitor.h)

#add -pedantic only to out sources, not to GRAMMAR_SOURCES
set_property(SOURCE ${SOURCE_FILES} PROPERTY COMPILE_FLAGS -pedantic)

add_executable(FleetLang ${SOURCE_FILES} ${GRAMMAR_SOURCES})

target_link_libraries(FleetLang antlr4-runtime)
target_link_libraries(FleetLang LLVM)

add_dependencies(FleetLang Build_Grammar)
add_dependencies(Test FleetLang)