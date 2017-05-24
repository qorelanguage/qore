find_package(BISON REQUIRED)
find_package(FLEX 2.5.35 REQUIRED)

# start of hacks to simulate installed QoreConfig.cmake
# which cannot be available yet - the QORE_LIBRARY is missing
# as find_library does not work yet
#include(${CMAKE_BINARY_DIR}/cmake/QoreConfig.cmake)
if (NOT QORE_MODULES_DIR)
    set(QORE_MODULES_DIR ${MODULE_DIR})
endif (NOT QORE_MODULES_DIR)

if (NOT QORE_LIBRARY)
    set(QORE_LIBRARY libqore)
endif (NOT QORE_LIBRARY)

set(QORE_API_VERSION ${MODULE_API_MAJOR}.${MODULE_API_MINOR})

include(${CMAKE_SOURCE_DIR}/cmake/QoreMacros.cmake)
### end of hacks


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

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
add_custom_target(ASTPARSER_QPP_GENERATED_FILES DEPENDS qpp ${ASTPARSER_QPP_SOURCES})
add_custom_target(ASTPARSER_BISON_GENERATED_FILES DEPENDS ${BISON_astparser_OUTPUTS})
add_custom_target(ASTPARSER_FLEX_GENERATED_FILES DEPENDS ${FLEX_astscanner_OUTPUTS})
set(astparser_module_name "astparser")

set(QORE_DOX_TMPL_SRC
  docs/mainpage.dox.tmpl
)

add_library(${astparser_module_name} SHARED ${ASTPARSER_CPP_SRC} ${ASTPARSER_QPP_SOURCES})
add_dependencies(${astparser_module_name} libqore ASTPARSER_QPP_GENERATED_FILES ASTPARSER_BISON_GENERATED_FILES ASTPARSER_FLEX_GENERATED_FILES)
qore_binary_module(${astparser_module_name} "${VERSION}")

#FIND_PACKAGE(Doxygen)
#if (DOXYGEN_FOUND)
#  qore_wrap_dox(QORE_DOX_SRC ${QORE_DOX_TMPL_SRC})
#  add_custom_target(QORE_MOD_DOX_FILES DEPENDS ${QORE_DOX_SRC})
#  add_dependencies(docs QORE_MOD_DOX_FILES)
#endif()
