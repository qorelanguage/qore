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
        SET(_unitfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.qtest)

        ADD_CUSTOM_COMMAND(OUTPUT ${_cppfile} ${_doxfile}
                           COMMAND ${QORE_QPP_EXECUTABLE}
                           ARGS --output=${_cppfile} --dox-output=${_doxfile} --unit=${_unitfile} ${_infile}
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
    set(oneValueArgs DOXLIST)
    set(multiValueArgs OPTIONS)

    cmake_parse_arguments(_WRAP_QPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    FOREACH (it ${_WRAP_QPP_UNPARSED_ARGUMENTS})
        GET_FILENAME_COMPONENT(_outfile ${it} NAME_WE)
        GET_FILENAME_COMPONENT(_infile ${it} ABSOLUTE)
        SET(_cppfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.cpp)
        SET(_doxfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.dox.h)
        SET(_unitfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.qtest)

        ADD_CUSTOM_COMMAND(OUTPUT ${_cppfile} ${_doxfile}
                           COMMAND ${QORE_QPP_EXECUTABLE}
                           ARGS -V --output=${_cppfile} --dox-output=${_doxfile} --unit=${_unitfile} ${_infile}
                           MAIN_DEPENDENCY ${_infile}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           VERBATIM
                        )
        SET(${_cpp_files} ${${_cpp_files}} ${_cppfile})
        IF(_WRAP_QPP_DOXLIST)
           SET(${_WRAP_QPP_DOXLIST} ${${_WRAP_QPP_DOXLIST}} ${_doxfile})
        ENDIF(_WRAP_QPP_DOXLIST)
        #MESSAGE(STATUS "DEBUG D: " _WRAP_QPP_DOXLIST " ${D}:" ${_WRAP_QPP_DOXLIST} " ${${D}}:" ${${_WRAP_QPP_DOXLIST}})
    ENDFOREACH (it)

ENDMACRO (QORE_WRAP_QPP_VALUE)

#
# Create dox code from dox.tmpl files
#
#  _dox_files : output dox filenames created in CMAKE_CURRENT_BINARY_DIR
#
# usage:
# set(MY_DOX_TMPL foo.dox.tmpl bar.dox.tmpl)
# qore_wrap_dox(MY_DOX ${MY_DOX_TMPL})
#
MACRO (QORE_WRAP_DOX _dox_files)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs OPTIONS)

    cmake_parse_arguments(_WRAP_QPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    FOREACH (it ${_WRAP_QPP_UNPARSED_ARGUMENTS})

        GET_FILENAME_COMPONENT(_outfile ${it} NAME_WE)
        GET_FILENAME_COMPONENT(_infile ${it} ABSOLUTE)
        SET(_doxfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.dox)

        ADD_CUSTOM_COMMAND(OUTPUT ${_doxfile}
                           COMMAND ${QORE_QPP_EXECUTABLE}
                           ARGS --table=${_infile} --output=${_doxfile}
                           MAIN_DEPENDENCY ${_infile}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           VERBATIM
                        )
        SET(${_dox_files} ${${_dox_files}} ${_doxfile})
    ENDFOREACH (it)

ENDMACRO (QORE_WRAP_DOX)

MACRO (QORE_BINARY_MODULE _module_name _version)
   QORE_BINARY_MODULE_INTERN(${_module_name} ${_version} "" ${ARGN})
ENDMACRO (QORE_BINARY_MODULE)

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
MACRO (QORE_BINARY_MODULE_INTERN _module_name _version _install_suffix)

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

    # set install target dir
    set(_mod_target_dir ${QORE_MODULES_DIR}${_install_suffix})

    set_target_properties(${_module_name} PROPERTIES PREFIX "" SUFFIX "-api-${QORE_API_VERSION}.qmod")
    target_link_libraries(${_module_name} ${QORE_LIBRARY} ${_libs})
    # this line breaks jhbuild "chroot": install( TARGETS ${_module_name} DESTINATION ${QORE_MODULES_DIR})
    #install( TARGETS ${_module_name} DESTINATION ${QORE_MODULES_DIR})
    install( TARGETS ${_module_name} DESTINATION ${_mod_target_dir})

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
        message(STATUS "Module ${_module_name} uninstall target: make uninstall")
        message(STATUS "")
    else()
        message(WARNING "Module ${_module_name} uninstall script: no file: ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in")
    endif()

    # docs
    FIND_PACKAGE(Doxygen)
    if (DOXYGEN_FOUND)
        if (EXISTS "${QORE_USERMODULE_DOXYGEN_TEMPLATE}")
            configure_file(${QORE_USERMODULE_DOXYGEN_TEMPLATE} ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)

            add_custom_target(docs
                ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/html ${CMAKE_BINARY_DIR}/html/search
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
            add_dependencies(docs ${_module_name})

            message(STATUS "")
            message(STATUS "Module ${_module_name} documentation target: make docs")
            message(STATUS "")
        else()
            message(WARNING "User module doxygen template file does not exist: ${QORE_USERMODULE_DOXYGEN_TEMPLATE}")
        endif()

    else (DOXYGEN_FOUND)
        message(WARNING "Doxygen not found. Documentation won't be built.")
    endif (DOXYGEN_FOUND)
ENDMACRO (QORE_BINARY_MODULE_INTERN)

# Install qore native/user module (.qm file) into proper location.
#
# Param #1: path to the module; e.g. "qlib/RestHandler.qm"
# Param #2: list of module dependencies separated by semicolon; e.g. "HttpServerUtil;Mime;Util"
#
# Example:
#     qore_user_module("qlib/RestHandler.qm" "HttpServerUtil;Mime;Util")
#
# The module will be installed automatically in 'make install' target.
MACRO (QORE_USER_MODULE _module_file _mod_deps)
    if (DOXYGEN_FOUND)
        # get module name
        get_filename_component(f ${_module_file} NAME_WE)
        message(STATUS "Preparing generation of documentation for module: ${f}")

        # prepare directories for the documentation
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doxygen/qlib/)

        # prepare needed vars
        set(MOD_DOXYFILE "${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f}")
        unset(MOD_DEPS)
        foreach(i ${_mod_deps})
            SET(MOD_DEPS ${MOD_DEPS} -t${i}.tag=${CMAKE_BINARY_DIR}/docs/modules/${i}/html)
        endforeach(i)

        # prepare QDX arguments
        set(QDX_DOXYFILE_ARGS -M=${CMAKE_SOURCE_DIR}/${_module_file}:${CMAKE_BINARY_DIR}/doxygen/qlib/${f}.qm.dox.h ${MOD_DEPS} ${CMAKE_SOURCE_DIR}/doxygen/qlib/Doxyfile.tmpl ${MOD_DOXYFILE})
        set(QDX_QMDOXH_ARGS ${CMAKE_SOURCE_DIR}/${_module_file} ${CMAKE_BINARY_DIR}/doxygen/qlib/${f}.qm.dox.h)

        # add CMake target for the documentation
        if (WIN32 AND (NOT MINGW) AND (NOT MSYS))
            add_custom_target(docs-${f}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs/modules/${f}
                COMMAND set QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib $${QORE_QDX_EXECUTABLE} ${QDX_DOXYFILE_ARGS}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_QMDOXH_ARGS}
                COMMAND ${DOXYGEN_EXECUTABLE} ${MOD_DOXYFILE}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/modules/${f}/html/*.html
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/modules/${f}/html/search/*.html
                COMMENT "Generating API documentation with Doxygen for module: ${f}" VERBATIM
            )
            add_custom_target(docs-fast-${f}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs/modules/${f}
                COMMAND set QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_DOXYFILE_ARGS}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_QMDOXH_ARGS}
                COMMAND ${DOXYGEN_EXECUTABLE} ${MOD_DOXYFILE}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/modules/${f}/html/*.html
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/modules/${f}/html/search/*.html
                COMMENT "Generating API documentation with Doxygen for module: ${f}" VERBATIM
            )
        else (WIN32 AND (NOT MINGW) AND (NOT MSYS))
            add_custom_target(docs-${f}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs/modules/${f}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_DOXYFILE_ARGS}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_QMDOXH_ARGS}
                COMMAND ${DOXYGEN_EXECUTABLE} ${MOD_DOXYFILE}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/modules/${f}/html/*.html
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/modules/${f}/html/search/*.html
                COMMENT "Generating API documentation with Doxygen for module: ${f}" VERBATIM
            )
            add_custom_target(docs-fast-${f}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs/modules/${f}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_DOXYFILE_ARGS}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_QMDOXH_ARGS}
                COMMAND ${DOXYGEN_EXECUTABLE} ${MOD_DOXYFILE}
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/modules/${f}/html/*.html
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/modules/${f}/html/search/*.html
                COMMENT "Generating API documentation with Doxygen for module: ${f}" VERBATIM
            )
        endif (WIN32 AND (NOT MINGW) AND (NOT MSYS))

        # make 'docs' target dependent on this module documentation
        add_dependencies(docs docs-${f})

        # make this dependent on Qore lang and lib documentation targets
        add_dependencies(docs-${f} docs-lang)
        add_dependencies(docs-${f} docs-lib)

        # make this dependent on the other module targets
        foreach(i ${_mod_deps})
            add_dependencies(docs-${f} docs-${i})
        endforeach(i)
    endif (DOXYGEN_FOUND)

    # install qm file
    install(FILES ${_module_file} DESTINATION ${QORE_USER_MODULES_DIR})
ENDMACRO (QORE_USER_MODULE)

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
            configure_file(${QORE_USERMODULE_DOXYGEN_TEMPLATE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f} @ONLY)
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/${f}/)
            add_custom_target(docs-${f}
                ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f}
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/html/*.html
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/html/search/*.html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen for: ${f}" VERBATIM
            )
            add_dependencies(docs docs-${f})
        endforeach(i)
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
    message(STATUS "Tarball creation target: make dist")
    message(STATUS "")

ENDMACRO (QORE_DIST)

# prints a summary of configuration
MACRO (QORE_CONFIG_INFO)
    MESSAGE(STATUS "")
    MESSAGE(STATUS "-----------------------------")
    MESSAGE(STATUS "System: ${CMAKE_SYSTEM}")
    MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    MESSAGE(STATUS "Prefix: ${CMAKE_INSTALL_PREFIX}")
    IF (APPLE)
        MESSAGE(STATUS "Archs: ${CMAKE_OSX_ARCHITECTURES}")
    ENDIF (APPLE)
    MESSAGE(STATUS "C Compiler: ${CMAKE_C_COMPILER} - ${CMAKE_C_COMPILER_ID} - ${CMAKE_C_COMPILER_VERSION}")
    MESSAGE(STATUS "C Flags: ${CMAKE_C_FLAGS}")
    MESSAGE(STATUS "C Flags debug: ${CMAKE_C_FLAGS_DEBUG}")
    MESSAGE(STATUS "C Flags release: ${CMAKE_C_FLAGS_RELEASE}")
    MESSAGE(STATUS "C Flags reldeb: ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    MESSAGE(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER} - ${CMAKE_CXX_COMPILER_ID} - ${CMAKE_CXX_COMPILER_VERSION}")
    MESSAGE(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
    MESSAGE(STATUS "CXX Flags debug: ${CMAKE_CXX_FLAGS_DEBUG}")
    MESSAGE(STATUS "CXX Flags release: ${CMAKE_CXX_FLAGS_RELEASE}")
    MESSAGE(STATUS "CXX Flags reldeb: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    MESSAGE(STATUS "-----------------------------")
    MESSAGE(STATUS "")
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
