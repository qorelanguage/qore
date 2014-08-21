#
# Qore Programming Languages cmake macros
#

#
# Create C++ code from qthe QPP files
#
#  _ccp_files : output list of filenames created in CMAKE_CURRENT_BINARY_DIR.
#  _inputs : input list of *.qpp files
#
MACRO (QORE_WRAP_QPP _cpp_files _inputs )

    FOREACH (it ${_inputs})

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
    include_directories( ${QORE_INCLUDE_DIR} )
    include_directories( ${CMAKE_BINARY_DIR} )
    
    # compiler stuff
    if (CMAKE_COMPILER_IS_GNUCXX)
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
    endif (CMAKE_COMPILER_IS_GNUCXX)

    # setup the target
    set (_libs "")
    foreach (value ${ARGN})
        set(_libs "${_libs};${value}")
    endforeach (value)

    set_target_properties(${_module_name} PROPERTIES PREFIX "" SUFFIX "-api-${QORE_API_VERSION}.qmod")
    target_link_libraries(${_module_name} ${QORE_LIBRARY} ${_libs})
    # this line breaks jhbuild "chroot": install( TARGETS ${_module_name} DESTINATION ${QORE_MODULES_DIR})
    install( TARGETS ${_module_name} DESTINATION ${QORE_MODULES_DIR})


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
    install(FILES ${_inputs} DESTINATION ${QORE_USER_MODULES_DIR})
ENDMACRO (QORE_USER_MODULES)


# Make distributable (source code) tarball. Target 'make dist'
MACRO (QORE_DIST _version)
    # packaging related stuff
    string(TOLOWER ${CMAKE_PROJECT_NAME} CPACK_PACKAGE_NAME)
    SET(CPACK_PACKAGE_VERSION "${_version}")
    SET(CPACK_SOURCE_GENERATOR "TBZ2")
    SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;\\\\.~$;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;refresh-copyright-and-license.pl;\\\\.spec$")
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
