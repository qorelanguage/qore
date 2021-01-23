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
    QORE_BINARY_MODULE_INTERN2(${_module_name} ${_version} "${_install_suffix}" "" ${ARGN})
ENDMACRO (QORE_BINARY_MODULE_INTERN)

MACRO (QORE_BINARY_MODULE_QORE _module_name _version _install_suffix)
    QORE_BINARY_MODULE_INTERN2(${_module_name} ${_version} "${_install_suffix}" 1 ${ARGN})
ENDMACRO (QORE_BINARY_MODULE_QORE)

# macro for external binary modules; supports complex documentation generation with QORE_EXTERNAL_USER_MODULE as well
MACRO (QORE_EXTERNAL_BINARY_MODULE _module_name _version)
    QORE_BINARY_MODULE_INTERN2(${_module_name} ${_version} "" 2 ${ARGN})
    set(_external_module_name ${_module_name})
ENDMACRO (QORE_EXTERNAL_BINARY_MODULE)

MACRO (QORE_BINARY_MODULE_INTERN2 _module_name _version _install_suffix _mod_suffix)
    if ("${_mod_suffix}" STREQUAL "")
        set(_docs_targ docs)
        set(_uninstall_targ uninstall)
        set(_working_dir ${CMAKE_BINARY_DIR})
        set(_dox_src ${CMAKE_SOURCE_DIR})
        set(_dox_output ${CMAKE_BINARY_DIR})
    elseif ("${_mod_suffix}" STREQUAL "1")
        set(_docs_targ docs-${_module_name})
        set(_uninstall_targ uninstall-${_module_name})
        set(_working_dir ${CMAKE_BINARY_DIR}/modules/${_module_name})
        set(_dox_src ${CMAKE_SOURCE_DIR}/modules/${_module_name}/src)
        set(_dox_output ${CMAKE_BINARY_DIR}/docs/modules/${_module_name})
        set(QORE_MOD_NAME ${_module_name})

        if ("${QORE_USERMODULE_DOXYGEN_TEMPLATE}" STREQUAL "")
            set(QORE_USERMODULE_DOXYGEN_TEMPLATE ${CMAKE_SOURCE_DIR}/doxygen/modules/Doxyfile.in)
        endif ()
    else()
        # this configurtion ensures that the primary binary module docs are build first as target "docs-module"
        # the "docs" target them will depend on "docs-module", and all user modules will also depend on "docs-module"
        # so that they can use symbols from the primary binary module
        set(_docs_targ docs-module)
        set(_extra_docs_targ docs)
        set(_uninstall_targ uninstall)
        set(_working_dir ${CMAKE_BINARY_DIR})
        set(_dox_src ${CMAKE_SOURCE_DIR})
        set(_dox_output "${CMAKE_BINARY_DIR}/docs/${_module_name}")
    endif()

    if (DEFINED MODULE_DOX_INPUT)
        string(REPLACE ";" " " MODULE_DOX_INPUT_STR "${MODULE_DOX_INPUT}")
        set(_dox_input ${MODULE_DOX_INPUT_STR})
    else ()
        set(_dox_input ${CMAKE_BINARY_DIR})
    endif ()

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

    # issue #3802: do not link with the qore library
    target_link_libraries(${_module_name} ${_libs})

    # ensure that modules use dynamic lookups; works with g++ & clang++
    set(CMAKE_MODULE_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-Wl,-undefined -Wl,dynamic_lookup")

    install( TARGETS ${_module_name} DESTINATION ${_mod_target_dir})

    if (APPLE)
        # It should allow to use full path in the module reference itself. otool -L /path/to/module.qmod, 1st line.
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
        ADD_CUSTOM_TARGET(${_uninstall_targ}
            "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

        if (NOT "${_uninstall_suffix}" STREQUAL "")
            add_dependencies(uninstall ${_uninstall_targ})
        endif ()

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
            set(CURRENT_MODULE_NAME ${_module_name})
            configure_file(${QORE_USERMODULE_DOXYGEN_TEMPLATE} ${_working_dir}/Doxyfile @ONLY)

            add_custom_target(${_docs_targ}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${_dox_output}
                COMMAND ${DOXYGEN_EXECUTABLE} ${_working_dir}/Doxyfile
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${_dox_output}/html ${_dox_output}/html/search
                WORKING_DIRECTORY ${_working_dir}
                COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
            add_dependencies(${_docs_targ} ${_module_name})

            if (NOT "${_extra_docs_targ}" STREQUAL "")
                add_custom_target(${_extra_docs_targ})
                add_dependencies(${_extra_docs_targ} ${_docs_targ})
            endif ()

            if (NOT "${_mod_suffix}" STREQUAL "")
                add_dependencies(docs ${_docs_targ})
            endif ()

            message(STATUS "")
            message(STATUS "Module ${_module_name} documentation target: make docs")
            message(STATUS "")
        else()
            message(WARNING "User module doxygen template file does not exist: ${QORE_USERMODULE_DOXYGEN_TEMPLATE}")
        endif()

    else (DOXYGEN_FOUND)
        message(WARNING "Doxygen not found. Documentation won't be built.")
    endif (DOXYGEN_FOUND)
ENDMACRO (QORE_BINARY_MODULE_INTERN2)

# Install qore native/user module (.qm file) into the proper location.
#
# NOTE: this macro is for the library only
#
# Param #1: path to the module; e.g. "qlib/RestHandler.qm"
# Param #2: list of module dependencies separated by semicolon; e.g. "HttpServerUtil;Mime;Util"
#
# Example:
#     qore_user_module("qlib/RestHandler.qm" "HttpServerUtil;Mime;Util")
#
# The module will be installed automatically in 'make install' target.
MACRO (QORE_USER_MODULE _module_file _mod_deps)
    get_filename_component(f ${_module_file} NAME_WE)
    if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/qlib/${f})
        file(GLOB _mod_targets "${CMAKE_SOURCE_DIR}/qlib/${f}/*.qm" "${CMAKE_SOURCE_DIR}/qlib/${f}/*.qc")
        set(qm_install_subdir "${f}") # install files into a subdir
        #message(STATUS "_mod_targets ${_mod_targets}")
    else()
        set(_mod_targets ${_module_file})
        set(qm_install_subdir "") # common qm file
    endif()

    if (DOXYGEN_FOUND)
        # get module name
        #message(STATUS "Preparing generation of documentation for module: ${f}")

        # prepare directories for the documentation
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doxygen/qlib/)

        # prepare needed vars
        set(MOD_DOXYFILE "${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f}")
        unset(MOD_DEPS)
        foreach(i ${_mod_deps})
            # we must use relative directories for tags; using absolute paths for tags will break the documentation
            # when used on any system except the one where it's generated
            SET(MOD_DEPS ${MOD_DEPS} -t${i}.tag=../../${i}/html)
        endforeach(i)

        # prepare QDX arguments
        set(QDX_DOXYFILE_ARGS -T${CMAKE_SOURCE_DIR} -M=${CMAKE_SOURCE_DIR}/${_module_file}:${CMAKE_BINARY_DIR}/doxygen/qlib/${f}.qm.dox.h ${MOD_DEPS} ${CMAKE_SOURCE_DIR}/doxygen/qlib/Doxyfile.tmpl ${MOD_DOXYFILE})
        set(QDX_QMDOXH_ARGS ${CMAKE_SOURCE_DIR}/${_module_file} ${CMAKE_BINARY_DIR}/doxygen/qlib/${f}.qm.dox.h)

        # add CMake target for the documentation
        if (WIN32 AND (NOT MINGW) AND (NOT MSYS))
            add_custom_target(docs-${f}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs/modules/${f}
                COMMAND set QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib
                COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_DOXYFILE_ARGS}
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
    install(FILES ${_mod_targets} DESTINATION ${QORE_USER_MODULES_DIR}/${qm_install_subdir})
ENDMACRO (QORE_USER_MODULE)

# Install qore native/user module (.qm file) into proper location.
#
# NOTE: must be called afer QORE_EXTERNAL_BINARY_MODULE
#
# Param #1: path to the module; e.g. "qlib/RestHandler.qm"
# Param #2: list of module dependencies separated by semicolon; e.g. "HttpServerUtil;Mime;Util"
#
# Example:
#     qore_module_user_module("qlib/RestHandler.qm" "HttpServerUtil;Mime;Util")
#
# The module will be installed automatically in 'make install' target.
MACRO (QORE_EXTERNAL_USER_MODULE _module_file _mod_deps)
    get_filename_component(f ${_module_file} NAME_WE)
    if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/qlib/${f})
        file(GLOB _mod_targets "${CMAKE_SOURCE_DIR}/qlib/${f}/*.qm" "${CMAKE_SOURCE_DIR}/qlib/${f}/*.qc")
        set(qm_install_subdir "${f}") # install files into a subdir
        #message(STATUS "_mod_targets ${_mod_targets}")
    else()
        set(_mod_targets ${_module_file})
        set(qm_install_subdir "") # common qm file
    endif()

    if (DOXYGEN_FOUND)
        # get module name
        #message(STATUS "Preparing generation of documentation for module: ${f}")

        # prepare directories for the documentation
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doxygen/qlib/)

        # prepare needed vars
        set(MOD_DOXYFILE "${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f}")
        #set(MOD_DEPS -t${_external_module_name}.tag=../../${_external_module_name}/html)
        set(CURRENT_MODULE_NAME ${f})
        set(TAGFILES ${CMAKE_BINARY_DIR}/${_external_module_name}.tag=../../${_external_module_name}/html)
        foreach(i ${_mod_deps})
            # we must use relative directories for tags; using absolute paths for tags will break the documentation
            # when used on any system except the one where it's generated
            #SET(MOD_DEPS ${MOD_DEPS} -t${i}.tag=../../${i}/html)
            SET(TAGFILES ${TAGFILES} ${CMAKE_BINARY_DIR}/${i}.tag=../../${i}/html)
        endforeach(i)
        string (REPLACE ";" " " TAGFILES "${TAGFILES}")

        set(_dox_output ${CMAKE_BINARY_DIR}/docs/${f})
        set(_dox_input ${CMAKE_BINARY_DIR}/doxygen/qlib/${f}.qm.dox.h)

        # prepare QDX arguments
        configure_file(${QORE_USERMODULE_DOXYGEN_TEMPLATE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f} @ONLY)
        #set(QDX_DOXYFILE_ARGS -T${CMAKE_SOURCE_DIR} -M=${CMAKE_SOURCE_DIR}/${_module_file}:${CMAKE_BINARY_DIR}/doxygen/qlib/${f}.qm.dox.h ${MOD_DEPS} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${f}.tmpl ${MOD_DOXYFILE})
        set(QDX_QMDOXH_ARGS ${CMAKE_SOURCE_DIR}/${_module_file} ${CMAKE_BINARY_DIR}/doxygen/qlib/${f}.qm.dox.h)

        # add CMake target for the documentation
        #message(STATUS "Doxyfile for ${f}")
        set(QORE_QMOD_FNAME ${f}) # used for configure_file line below

        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/${f}/${qm_install_subdir}/)
        add_custom_target(docs-${f}
            #COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_DOXYFILE_ARGS}
            COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} ${QDX_QMDOXH_ARGS}
            COMMAND ${DOXYGEN_EXECUTABLE} ${MOD_DOXYFILE}
            COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/${f}/html/*.html
            COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/${f}/html/search/*.html

            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen for: ${f}" VERBATIM
        )

        # make 'docs' target dependent on this module documentation
        add_dependencies(docs docs-${f})

        # make this dependent on the other module targets
        add_dependencies(docs-${f} docs-module)
        foreach(i ${_mod_deps})
            add_dependencies(docs-${f} docs-${i})
        endforeach(i)
    endif (DOXYGEN_FOUND)

    # install qm file
    install(FILES ${_mod_targets} DESTINATION ${QORE_USER_MODULES_DIR}/${qm_install_subdir})
ENDMACRO (QORE_EXTERNAL_USER_MODULE)

# Install qore native/user modules (qm files) into the proper location.
# Example:
#   set(QM_FILES foo.qm bar.qm)
#   qore_user_modules(${QM_FILES})
# Files will be installed automatically in 'make install' target
MACRO (QORE_USER_MODULES _inputs)
    # first - handle qlib documentation
    find_package(Doxygen)
    foreach(f ${_inputs})
        unset(_mod_targets)
        if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${f})
            if (${f} MATCHES "^qlib/")
                string(LENGTH "${f}" f_len)
                MATH(EXPR f_len "${f_len}-5")
                string(SUBSTRING ${f} 5 ${f_len} new_f)

                file(GLOB _mod_targets "${CMAKE_SOURCE_DIR}/${f}/*.qm" "${CMAKE_SOURCE_DIR}/${f}/*.qc")
                set(qm_install_subdir "${new_f}") # install files into a subdir
                #message(STATUS "_mod_targets ${_mod_targets}")
            else()
                file(GLOB _mod_targets "${CMAKE_SOURCE_DIR}/${f}/*.qm" "${CMAKE_SOURCE_DIR}/${f}/*.qc")
                set(qm_install_subdir "${f}") # install files into a subdir
                #message(STATUS "_mod_targets ${_mod_targets}")
            endif()
        else()
            set(_mod_targets ${f})
            set(qm_install_subdir "") # common qm file
        endif()

        if (xDOXYGEN_FOUND)
            get_filename_component(file ${f} NAME_WE)
            #message(STATUS "Doxyfile for ${file}")
            set(CURRENT_MODULE_NAME ${_file}) # used for configure_file line below
            set(_dox_input ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${MODULE_DOX_INPUT})
            configure_file(${QORE_USERMODULE_DOXYGEN_TEMPLATE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${file} @ONLY)
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/${file}/${qm_install_subdir}/)
            add_custom_target(docs-${file}
                ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.${file}
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/html/*.html
                COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/html/search/*.html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen for: ${file}" VERBATIM
            )
            add_dependencies(docs docs-${file})
        endif (xDOXYGEN_FOUND)
        # install qm files
        install(FILES ${_mod_targets} DESTINATION ${QORE_USER_MODULES_DIR}/${qm_install_subdir})
    endforeach(f)
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

# find pthread_setname
macro(QORE_FIND_PTHREAD_SETNAME_NP)
  message(STATUS "Looking for pthread_setname_np()")

  check_cxx_source_compiles("
#include <pthread.h>
int main(int argc, char* argv []) {
    pthread_setname_np(\"foo\");
    return 0;
}
" QORE_HAVE_PTHREAD_SETNAME_NP_1)

  if (NOT QORE_HAVE_PTHREAD_SETNAME_NP_1)
    check_cxx_source_compiles("
#include <pthread.h>
int main(int argc, char* argv []) {
    pthread_setname_np(pthread_self(), \"foo\");
    return 0;
}
" QORE_HAVE_PTHREAD_SETNAME_NP_2)

    if (NOT QORE_HAVE_PTHREAD_SETNAME_NP_2)
      check_cxx_source_compiles("
#include <pthread.h>
int main(int argc, char* argv []) {
    pthread_setname_np(pthread_self(), \"foo\", (void*)0);
    return 0;
}
" QORE_HAVE_PTHREAD_SETNAME_NP_3)

      if (NOT QORE_HAVE_PTHREAD_SETNAME_NP_3)
        check_cxx_source_compiles("
#include <pthread.h>
int main(int argc, char* argv []) {
    pthread_set_name_np(pthread_self(), \"foo\");
    return 0;
}
" QORE_HAVE_PTHREAD_SET_NAME_NP)

      endif()

    endif()

  endif()

endmacro(QORE_FIND_PTHREAD_SETNAME_NP)

# find pthread_getattr_np
macro(QORE_FIND_PTHREAD_GETATTR_NP)
  check_cxx_source_compiles("
#include <pthread.h>
int main(int argc, char* argv []) {
    pthread_attr_t attr;
    pthread_getattr_np(pthread_self(), &attr);
    return 0;
}
" QORE_HAVE_PTHREAD_GETATTR_NP)
endmacro()
