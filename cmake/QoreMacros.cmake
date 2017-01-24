#
# Qore Programming Languages cmake macros
#

include(CMakeParseArguments)

#
# Create C++ code from the QPP files
#
#  _cpp_files : output list of filenames created in CMAKE_CURRENT_BINARY_DIR.
#
# usage:
# set(MY_QPP foo.qpp bar.qpp)
# qore_wrap_qpp(MY_CPP ${MY_QPP})
#
MACRO (QORE_WRAP_QPP _cpp_files)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs OPTIONS)

    cmake_parse_arguments(_WRAP_QPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    FOREACH (it ${_WRAP_QPP_UNPARSED_ARGUMENTS})

        GET_FILENAME_COMPONENT(_outfile ${it} NAME_WE)
        GET_FILENAME_COMPONENT(_infile ${it} ABSOLUTE)
        SET(_cppfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.cpp)
        SET(_doxfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.dox.h)

        ADD_CUSTOM_COMMAND(OUTPUT ${_cppfile} ${_doxfile}
                           COMMAND ${QORE_QPP_EXECUTABLE}
                           ARGS --output=${_cppfile} --dox-output=${_doxfile} ${_infile}
                           MAIN_DEPENDENCY ${_infile}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           VERBATIM
                        )
        SET(${_cpp_files} ${${_cpp_files}} ${_cppfile})
    ENDFOREACH (it)

ENDMACRO (QORE_WRAP_QPP)

#
# Create C++ code using the new value API from the QPP files
#
#  _cpp_files : output list of filenames created in CMAKE_CURRENT_BINARY_DIR.
#
# usage:
# set(MY_QPP foo.qpp bar.qpp)
# qore_wrap_qpp_value(MY_CPP ${MY_QPP})
#
MACRO (QORE_WRAP_QPP_VALUE _cpp_files)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs OPTIONS)

    cmake_parse_arguments(_WRAP_QPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    FOREACH (it ${_WRAP_QPP_UNPARSED_ARGUMENTS})

        GET_FILENAME_COMPONENT(_outfile ${it} NAME_WE)
        GET_FILENAME_COMPONENT(_infile ${it} ABSOLUTE)
        SET(_cppfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.cpp)
        SET(_doxfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.dox.h)

        ADD_CUSTOM_COMMAND(OUTPUT ${_cppfile} ${_doxfile}
                           COMMAND ${QORE_QPP_EXECUTABLE}
                           ARGS -V --output=${_cppfile} --dox-output=${_doxfile} ${_infile}
                           MAIN_DEPENDENCY ${_infile}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           VERBATIM
                        )
        SET(${_cpp_files} ${${_cpp_files}} ${_cppfile})
    ENDFOREACH (it)

ENDMACRO (QORE_WRAP_QPP_VALUE)

# Create qore binary module.
# Arguments:
#  _module_name - string name of the module
#  _version - a version. Used in -DPACKAGE_VERSION=...
# [additional libraries] - optional list of libs
#
# The initial shared library is renamed to for Qore modules naming convention.
# Linking with [additional libraries] is enabled.
# libqore is linked automatically.
#
# additional targets created:
#  'make docs' - if there is Doxygen found
#  'make uninstall' - if exists CMAKE_CURRENT_SOURCE_DIR/cmake/cmake_uninstall.cmake.in
MACRO (QORE_BINARY_MODULE _module_name _version)

    # standard repeating stuff for modules
    add_definitions("-DPACKAGE_VERSION=\"${_version}\"")
    if(DEFINED QORE_PRE_INCLUDES)
       include_directories( ${QORE_PRE_INCLUDES} )
    endif()
    include_directories( ${QORE_INCLUDE_DIR} )
    include_directories( ${CMAKE_BINARY_DIR} )

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

    # add additional libraries
    if(DEFINED QORE_POST_LIBS)
        foreach (value ${QORE_POST_LIBS})
            set(_libs "${_libs};${value}")
        endforeach (value)
    endif()

    set_target_properties(${_module_name} PROPERTIES PREFIX "" SUFFIX "-api-${QORE_API_VERSION}.qmod")
    target_link_libraries(${_module_name} ${QORE_LIBRARY} ${_libs})
    # this line breaks jhbuild "chroot": install( TARGETS ${_module_name} DESTINATION ${QORE_MODULES_DIR})
    install( TARGETS ${_module_name} DESTINATION ${QORE_MODULES_DIR})

    if (APPLE)
        # TODO/FIXME: @Niclas: please verify if it's correct
        # It should allow to use full path in the module refernce itself. otool -L /path/to/module.qmod, 1st line.
        set_target_properties(${_module_name} PROPERTIES INSTALL_NAME_DIR ${QORE_MODULES_DIR})
    endif (APPLE)
    set_target_properties(${_module_name} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

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

    # docs
    FIND_PACKAGE(Doxygen)
    if (DOXYGEN_FOUND)
        if (EXISTS "${CMAKE_SOURCE_DIR}/docs/Doxyfile.in")
            configure_file(${CMAKE_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)

            add_custom_target(docs
                ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/html ${CMAKE_BINARY_DIR}/html/search
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
            add_dependencies(docs ${_module_name})

            message(STATUS "")
            message(STATUS "documentation target: make docs")
            message(STATUS "")
        else()
            message(WARNING "file does not exits: ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in")
        endif()

    else (DOXYGEN_FOUND)
        message(WARNING "Doxygen not found. Documentation won't be built")
    endif (DOXYGEN_FOUND)
ENDMACRO (QORE_BINARY_MODULE)

# Install qore native/user modules (qm files) into proper location.
# Example:
#   set(QM_FILES foo.qm bar.qm)
#   qore_user_modules(${QM_FILES})
# Files will be installed automatically in 'make install' target
MACRO (QORE_USER_MODULES _inputs)
    # first - handle qlib documentation
    find_package(Doxygen)
    if (xDOXYGEN_FOUND)
        foreach(i ${_inputs})
            get_filename_component(f ${i} NAME_WE)
            message(STATUS "Doxyfile for ${f}")
            set(QORE_QMOD_FNAME ${f}) # used for configure_file line below
            configure_file(${CMAKE_SOURCE_DIR}/doxygen/qlib/Doxyfile.in ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f} @ONLY)
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/${f}/)
            add_custom_target(docs-${f}
                ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f}
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/html/*.html
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/html/search/*.html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen for: ${f}" VERBATIM
            )
            add_dependencies(docs docs-${f})
        endforeach()
    else (xDOXYGEN_FOUND)
        message(WARNING "Doxygen support for user modules is still TODO")
    endif (xDOXYGEN_FOUND)
    # install qm files
    install(FILES ${_inputs} DESTINATION ${QORE_USER_MODULES_DIR})
ENDMACRO (QORE_USER_MODULES)


# Make distributable (source code) tarball. Target 'make dist'
MACRO (QORE_DIST _version)
    # packaging related stuff
    string(TOLOWER ${CMAKE_PROJECT_NAME} CPACK_PACKAGE_NAME)
    SET(CPACK_PACKAGE_VERSION "${_version}")
    SET(CPACK_SOURCE_GENERATOR "TBZ2")
    SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;\\\\.~$;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;refresh-copyright-and-license.pl;\\\\.spec$;/\\\\.git/;/\\\\.hg/;\\\\.fslckout")
    SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})
    INCLUDE(CPack)
    # simulate autotools' "make dist"
    add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

    message(STATUS "")
    message(STATUS "tarbal creation target: make dist")
    message(STATUS "")

ENDMACRO (QORE_DIST)

# prints a sumamry of configuration
MACRO (QORE_CONFIG_INFO)
    MESSAGE(STATUS "")
    MESSAGE(STATUS "-----------------------------")
    MESSAGE(STATUS "System: ${CMAKE_SYSTEM}")
    MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    MESSAGE(STATUS "Prefix: ${CMAKE_INSTALL_PREFIX}")
    IF (APPLE)
        MESSAGE(STATUS "Archs: ${CMAKE_OSX_ARCHITECTURES}")
    ENDIF (APPLE)
    MESSAGE(STATUS "Compiler: ${CMAKE_CXX_COMPILER} - ${CMAKE_CXX_COMPILER_ID}")
    MESSAGE(STATUS "FLags deb: ${CMAKE_CXX_FLAGS_DEBUG}")
    MESSAGE(STATUS "FLags rel: ${CMAKE_CXX_FLAGS}")
    MESSAGE(STATUS "FLags reldeb: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    MESSAGE(STATUS "-----------------------------")
ENDMACRO (QORE_CONFIG_INFO)

#
# Find Pthreads.
# Uses FindThreads with pthreads preference to look for pthreads.
# Cmake generation will break if pthreads are not found.
# For variables and targets defined by this, see docs for FindThreads.
#
macro(QORE_FIND_PTHREADS)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(CMAKE_USE_PTHREADS_INIT)
    message(STATUS "Found POSIX Threads: TRUE")
else(CMAKE_USE_PTHREADS_INIT)
    message(STATUS "Found POSIX Threads: FALSE")
    message(FATAL_ERROR "POSIX threads do not seem to be supported on this platform, aborting")
endif()
endmacro(QORE_FIND_PTHREADS)
