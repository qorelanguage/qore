
MACRO (QORE_WRAP_QPP _cpp_files _inputs )
  message(STATUS "QPP: ${QORE_QPP_EXECUTABLE}")
  #message(STATUS "QPP2: ${_inputs}")

  FOREACH (it ${_inputs})
    #message(STATUS "QPP1: ${it}")
    GET_FILENAME_COMPONENT(_outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT(_infile ${it} ABSOLUTE)
    SET(_cppfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.cpp)
    SET(_doxfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.dox.h)
    message(STATUS "QPP processing: ${_infile} -> ${_cppfile} ${_doxfile}")
    ADD_CUSTOM_COMMAND(OUTPUT ${_cppfile} ${_doxfile}
      COMMAND ${QORE_QPP_EXECUTABLE}
      ARGS --output=${_cppfile} --dox-output=${_doxfile} ${_infile}
      MAIN_DEPENDENCY ${_infile}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
    SET(${_cpp_files} ${${_cpp_files}} ${_cppfile})
  ENDFOREACH (it)

  message(STATUS "QPP files: ${${_cpp_files}}")

ENDMACRO (QORE_WRAP_QPP)


