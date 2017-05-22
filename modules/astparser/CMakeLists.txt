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

set(ASTPARSER_QPP_SRC
    src/QC_AstParser.qpp
    src/QC_AstTree.qpp
    src/QC_AstTreeSearcher.qpp
)

set(ASTPARSER_CPP_SRC
    src/astparser-module.cpp
    src/AstParser.cpp
    src/AstParserHolder.cpp
    src/AstPrinter.cpp
    src/AstTreeHolder.cpp
    src/AstTreePrinter.cpp
    src/AstTreeSearcher.cpp
    src/queries/FindMatchingSymbolsQuery.cpp
    src/queries/FindNodeQuery.cpp
    src/queries/FindNodeAndParentsQuery.cpp
    src/queries/FindReferencesQuery.cpp
    src/queries/FindSymbolInfoQuery.cpp
    src/queries/FindSymbolsQuery.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/ast_parser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/ast_scanner.cpp
)

bison_target(astparser src/ast_parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/ast_parser.cpp)
flex_target(astscanner src/ast_scanner.lpp ${CMAKE_CURRENT_BINARY_DIR}/ast_scanner.cpp)
add_flex_bison_dependency(astscanner astparser)

qore_wrap_qpp(ASTPARSER_QPP_SOURCES ${ASTPARSER_QPP_SRC})
add_custom_target(ASTPARSER_QPP_GENERATED_FILES DEPENDS ${ASTPARSER_QPP_SOURCES})
add_custom_target(ASTPARSER_BISON_GENERATED_FILES DEPENDS ${BISON_astparser_OUTPUTS})
add_custom_target(ASTPARSER_FLEX_GENERATED_FILES DEPENDS ${FLEX_astscanner_OUTPUTS})

set(astparser_module_name "astparser")

set(QORE_DOX_TMPL_SRC
  docs/mainpage.dox.tmpl
)

add_library(${astparser_module_name} SHARED ${ASTPARSER_CPP_SRC} ${ASTPARSER_QPP_SOURCES})
add_dependencies(${astparser_module_name} ASTPARSER_QPP_GENERATED_FILES ASTPARSER_BISON_GENERATED_FILES ASTPARSER_FLEX_GENERATED_FILES)
target_include_directories(${astparser_module_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (WIN32 AND MINGW AND MSYS)
    target_compile_definitions(${astparser_module_name} PUBLIC BUILDING_DLL)
endif (WIN32 AND MINGW AND MSYS)

#qore_binary_module(${astparser_module_name} ${PROJECT_VERSION})

add_definitions("-DPACKAGE_VERSION=\"${PROJECT_VERSION}\"")
if(DEFINED QORE_PRE_INCLUDES)
    include_directories( ${QORE_PRE_INCLUDES} )
endif()
include_directories( ${QORE_INCLUDE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# compiler stuff
if (CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
endif (CMAKE_COMPILER_IS_GNUCXX)

# add BUILDING_DLL=1 define to modules' CXXFLAGS
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_DLL=1")

# setup the target
set (_libs "")
foreach (value ${ARGN})
    set(_libs "${_libs};${value}")
endforeach (value)

# add pthread library on Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(_libs "${_libs};pthread")
endif()

# add additional libraries
if(DEFINED QORE_POST_LIBS)
    foreach (value ${QORE_POST_LIBS})
        set(_libs "${_libs};${value}")
    endforeach (value)
endif()

set_target_properties(${astparser_module_name} PROPERTIES PREFIX "" SUFFIX "-api-${QORE_API_VERSION}.qmod")
target_link_libraries(${astparser_module_name} ${QORE_LIBRARY} ${_libs})
# this line breaks jhbuild "chroot": install( TARGETS ${astparser_module_name} DESTINATION ${QORE_MODULES_DIR})
install( TARGETS ${astparser_module_name} DESTINATION ${QORE_MODULES_DIR})

if (APPLE)
    # TODO/FIXME: @Niclas: please verify if it's correct
    # It should allow to use full path in the module refernce itself. otool -L /path/to/module.qmod, 1st line.
    set_target_properties(${astparser_module_name} PROPERTIES INSTALL_NAME_DIR ${QORE_MODULES_DIR})
endif (APPLE)
set_target_properties(${astparser_module_name} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# uninstall
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in")
    # make uninstall
    CONFIGURE_FILE(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )
    ADD_CUSTOM_TARGET(uninstall
        "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

    message(STATUS "")
    message(STATUS "uninstall target: make uninstall")
    message(STATUS "")
else()
    message(WARNING "Uninstall script: no file: ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in")
endif()


qore_config_info()

#FIND_PACKAGE(Doxygen)
#if (DOXYGEN_FOUND)
#  qore_wrap_dox(QORE_DOX_SRC ${QORE_DOX_TMPL_SRC})
#  add_custom_target(QORE_MOD_DOX_FILES DEPENDS ${QORE_DOX_SRC})
#  add_dependencies(docs QORE_MOD_DOX_FILES)
#endif()
