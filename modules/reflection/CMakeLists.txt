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

# source files
set(REFLECTION_QPP_SRC
    src/QC_AbstractVariant.qpp
    src/QC_FunctionVariant.qpp
    src/QC_AbstractMethodVariant.qpp
    src/QC_PseudoMethodVariant.qpp
    src/QC_NormalMethodVariant.qpp
    src/QC_StaticMethodVariant.qpp
    src/QC_ConstructorMethodVariant.qpp
    src/QC_DestructorMethodVariant.qpp
    src/QC_CopyMethodVariant.qpp
    src/QC_AbstractReflectionFunction.qpp
    src/QC_Function.qpp
    src/QC_AbstractMethod.qpp
    src/QC_PseudoMethod.qpp
    src/QC_NormalMethod.qpp
    src/QC_StaticMethod.qpp
    src/QC_ConstructorMethod.qpp
    src/QC_DestructorMethod.qpp
    src/QC_CopyMethod.qpp
    src/QC_AbstractMember.qpp
    src/QC_AbstractClassMember.qpp
    src/QC_NormalMember.qpp
    src/QC_StaticMember.qpp
    src/QC_AbstractClass.qpp
    src/QC_Class.qpp
    src/QC_PseudoClass.qpp
    src/QC_AbstractConstant.qpp
    src/QC_ClassConstant.qpp
    src/QC_Type.qpp
    src/QC_Namespace.qpp
    src/QC_Constant.qpp
    src/QC_GlobalVar.qpp
    src/QC_TypedHash.qpp
    src/QC_TypedHashMember.qpp
)

set(REFLECTION_CPP_SRC
    src/reflection-module.cpp
    src/AbstractReflectionObject.cpp
)

set(QORE_DOX_TMPL_SRC
    docs/mainpage.dox.tmpl
)

qore_wrap_qpp_value(REFLECTION_QPP_SOURCES ${REFLECTION_QPP_SRC})
add_custom_target(REFLECTION_QPP_GENERATED_FILES DEPENDS qpp ${REFLECTION_QPP_SOURCES})

# prepare reflection module target
set(reflection_module_name "reflection")

if (SINGLE_COMPILATION_UNIT)
    add_library(${reflection_module_name} SHARED src/reflection_scu.cpp)
else (SINGLE_COMPILATION_UNIT)
    add_library(${reflection_module_name} SHARED ${REFLECTION_CPP_SRC} ${REFLECTION_QPP_SOURCES})
endif (SINGLE_COMPILATION_UNIT)

add_dependencies(${reflection_module_name} libqore REFLECTION_QPP_GENERATED_FILES)
qore_binary_module_qore(${reflection_module_name} "${VERSION}" "/${VERSION}")

if (DOXYGEN_FOUND)
    qore_wrap_dox(QORE_DOX_SRC ${QORE_DOX_TMPL_SRC})
    add_custom_target(QORE_REFLECTION_MOD_DOX_FILES DEPENDS ${QORE_DOX_SRC})
    add_dependencies(docs-reflection QORE_REFLECTION_MOD_DOX_FILES)
endif()
