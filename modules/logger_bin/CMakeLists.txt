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
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR/build/include})
include_directories(${CMAKE_SOURCE_DIR/include})

# source files
set(LOGGER_QPP_SRC
    src/QC_LoggerLevel.qpp
    src/QC_LoggerPattern.qpp
    src/QC_LoggerEvent.qpp
    src/QC_LoggerFilter.qpp
    src/QC_LoggerLayout.qpp
    src/QC_LoggerLayoutPattern.qpp
    src/QC_LoggerAppender.qpp
    src/QC_LoggerAppenderQueue.qpp
    src/QC_LoggerAppenderWithLayout.qpp
    src/QC_LoggerAppenderFile.qpp
)

set(LOGGER_CPP_SRC
    src/logger-module.cpp
    src/QoreLoggerLevel.cpp
    src/QoreLoggerPattern.cpp
    src/QoreLoggerLayoutPattern.cpp
    src/QoreLoggerEvent.cpp
    src/QoreLoggerAppender.cpp
    src/QoreLoggerAppenderQueue.cpp
    src/QoreLoggerAppenderWithLayout.cpp
    src/QoreLoggerAppenderFile.cpp
)

# we need copy header files to avoid doxygen warning about duplicate files *.cpp
# when src/ dir is in included_directory
set(LOGGER_H_SRC
    src/qore_logger.h
    src/QoreLoggerLevel.h
    src/QoreLoggerPattern.h
    src/QoreLoggerAppender.h
    src/QoreLoggerAppenderQueue.h
    src/QoreLoggerAppenderWithLayout.h
    src/QoreLoggerAppenderFile.h
    src/QC_LoggerLevel.h
    src/QC_LoggerPattern.h
    src/QC_LoggerEvent.h
    src/QC_LoggerFilter.h
    src/QC_LoggerLayout.h
    src/QC_LoggerLayoutPattern.h
    src/QC_LoggerAppender.h
    src/QC_LoggerAppenderQueue.h
    src/QC_LoggerAppenderWithLayout.h
    src/QC_LoggerAppenderFile.h
)

set(QORE_DOX_LOGGER_TMPL_SRC
    docs/mainpage.dox.tmpl
)

foreach (it ${LOGGER_H_SRC})
    get_filename_component(_outfile ${it} NAME)
    get_filename_component(_infile ${it} ABSOLUTE)
    configure_file(${_infile} ${_outfile} COPYONLY)
    set(LOGGER_H_SOURCES, ${LOGGER_H_SOURCES} ${_outfile})
endforeach(it)

qore_wrap_qpp_value(LOGGER_QPP_SOURCES DOXLIST _dox_src ${LOGGER_QPP_SRC})
add_custom_target(LOGGER_QPP_GENERATED_FILES DEPENDS qpp ${LOGGER_QPP_SOURCES} ${LOGGER_H_SOURCES})

# prepare logger module target
set(logger_module_name "logger_bin")

if (SINGLE_COMPILATION_UNIT)
    add_library(${logger_module_name} MODULE src/logger_scu.cpp)
else (SINGLE_COMPILATION_UNIT)
    add_library(${logger_module_name} MODULE ${LOGGER_CPP_SRC} ${LOGGER_QPP_SOURCES})
endif (SINGLE_COMPILATION_UNIT)

add_dependencies(${logger_module_name} libqore LOGGER_QPP_GENERATED_FILES)
set(MODULE_DOX_INPUT ${CMAKE_BINARY_DIR}/modules/logger_bin/mainpage.dox ${_dox_src})
qore_binary_module_qore(${logger_module_name} "${VERSION}" "/${VERSION}")

if (DOXYGEN_FOUND)
    qore_wrap_dox(QORE_DOX_LOGGER_SRC ${QORE_DOX_LOGGER_TMPL_SRC})
    add_custom_target(QORE_LOGGER_MOD_DOX_FILES DEPENDS ${QORE_DOX_LOGGER_SRC})
    add_dependencies(docs-logger_bin QORE_LOGGER_MOD_DOX_FILES)
endif()
