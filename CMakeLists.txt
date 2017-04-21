cmake_minimum_required(VERSION 2.8.3)

project(astparser)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 0)

set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "release")
endif (NOT CMAKE_BUILD_TYPE)
string(TOLOWER ${CMAKE_BUILD_TYPE} QORE_BUILD_TYPE_LWR)
if (${QORE_BUILD_TYPE_LWR} MATCHES "debug")
    add_definitions(-DDEBUG)
else ()
    add_definitions(-DNDEBUG)
endif ()

find_package(BISON REQUIRED)
find_package(FLEX 2.5.35 REQUIRED)
find_package(Qore 0.8.13 REQUIRED)

# Check for C++11.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(QPP_SRC src/QC_AstParser.qpp)

set(CPP_SRC
    src/astparser-module.cpp
    src/AstParser.cpp
    src/AstParserHolder.cpp
    src/AstTreePrinter.cpp
    src/AstTreeSearcher.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/ast_parser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/ast_scanner.cpp
)

bison_target(qoreastparser src/ast_parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/ast_parser.cpp)
flex_target(qoreastscanner src/ast_scanner.lpp ${CMAKE_CURRENT_BINARY_DIR}/ast_scanner.cpp)
add_flex_bison_dependency(qoreastscanner qoreastparser)

qore_wrap_qpp(QPP_SOURCES ${QPP_SRC})
add_custom_target(QPP_GENERATED_FILES DEPENDS ${QPP_SOURCES})
add_custom_target(BISON_GENERATED_FILES DEPENDS ${BISON_qoreastparser_OUTPUTS})
add_custom_target(FLEX_GENERATED_FILES DEPENDS ${FLEX_qoreastscanner_OUTPUTS})

set(module_name "astparser")

set(QORE_DOX_TMPL_SRC
  docs/mainpage.dox.tmpl
)

add_library(${module_name} SHARED ${CPP_SRC} ${QPP_SOURCES})
add_dependencies(${module_name} QPP_GENERATED_FILES BISON_GENERATED_FILES FLEX_GENERATED_FILES)
target_include_directories(${module_name} PUBLIC ${CMAKE_SOURCE_DIR}/src)

if (WIN32 AND MINGW AND MSYS)
    target_compile_definitions(${module_name} PUBLIC BUILDING_DLL)
endif (WIN32 AND MINGW AND MSYS)

qore_binary_module(${module_name} ${PROJECT_VERSION})

qore_dist(${PROJECT_VERSION})

qore_config_info()

FIND_PACKAGE(Doxygen)
if (DOXYGEN_FOUND)
  qore_wrap_dox(QORE_DOX_SRC ${QORE_DOX_TMPL_SRC})
  add_custom_target(QORE_MOD_DOX_FILES DEPENDS ${QORE_DOX_SRC})
  add_dependencies(docs QORE_MOD_DOX_FILES)
endif()
