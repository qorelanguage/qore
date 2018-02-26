cmake_minimum_required(VERSION 2.8.12)

project(qore-language)

option(ICONV_TRANSLIT
       "Force //Translit to iconv encoding to do transliteration, if OFF it is tested for"
       OFF)

option(QORE_RUNTIME_THREAD_STACK_TRACE
       "enable runtime thread stack trace (turning off breaks compatibility)"
       ON)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 9)
set(VERSION_SUB 0)

STRING(LENGTH ${VERSION_SUB} SUB_LEN)
if (${SUB_LEN} EQUAL 1)
    set(VERSION_FORMATTED_SUB 0${VERSION_SUB})
else()
    set(VERSION_FORMATTED_SUB ${VERSION_SUB})
endif()

if(DEFINED VERSION_PATCH)
    set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_SUB}.${VERSION_PATCH})
else()
    set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_SUB})
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "release")
endif (NOT CMAKE_BUILD_TYPE)
string(TOLOWER ${CMAKE_BUILD_TYPE} QORE_BUILD_TYPE_LWR)
if (${QORE_BUILD_TYPE_LWR} MATCHES "debug")
    add_definitions(-DDEBUG)
else ()
    add_definitions(-DNDEBUG)
endif ()

#if ((NOT ${QORE_BUILD_TYPE_LWR} MATCHES "debug") AND QORE_RUNTIME_THREAD_STACK_TRACE)
#    message(FATAL_ERROR "QORE_RUNTIME_THREAD_STACK_TRACE only available on debug builds")
#endif()

# simulate QoreConfig
set(QORE_QDX_EXECUTABLE "${CMAKE_SOURCE_DIR}/doxygen/qdx")
# simulate QoreConfig

include("${CMAKE_SOURCE_DIR}/cmake/QoreMacros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/QoreMacrosIntern.cmake")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include_directories( ${CMAKE_BINARY_DIR} )
include_directories( ${CMAKE_BINARY_DIR}/include/ )
include_directories( ${CMAKE_BINARY_DIR}/include/qore )
include_directories( ${CMAKE_BINARY_DIR}/include/qore/intern )
include_directories( ${CMAKE_SOURCE_DIR}/include )
include_directories( ${CMAKE_SOURCE_DIR}/include/qore )

include(CheckIncludeFiles)
include(GNUInstallDirs)

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(myprefix ${CMAKE_INSTALL_PREFIX})
set(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})

set(MODULE_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/qore-modules)
set(MODULE_VER_DIR ${MODULE_DIR}/${VERSION})
set(USER_MODULE_DIR ${CMAKE_INSTALL_FULL_DATADIR}/qore-modules/)
set(USER_MODULE_VER_DIR ${USER_MODULE_DIR}${VERSION})

get_module_api_versions()

set(LIBQORE_QPP_SRC
    lib/QC_AbstractSmartLock.qpp
    lib/QC_AbstractThreadResource.qpp
    lib/QC_AutoGate.qpp
    lib/QC_AutoLock.qpp
    lib/QC_AutoReadLock.qpp
    lib/QC_AutoWriteLock.qpp
    lib/QC_Condition.qpp
    lib/QC_Counter.qpp
    lib/QC_AbstractIterator.qpp
    lib/QC_AbstractQuantifiedIterator.qpp
    lib/QC_AbstractBidirectionalIterator.qpp
    lib/QC_AbstractQuantifiedBidirectionalIterator.qpp
    lib/QC_ListIterator.qpp
    lib/QC_ListReverseIterator.qpp
    lib/QC_HashIterator.qpp
    lib/QC_HashReverseIterator.qpp
    lib/QC_HashKeyIterator.qpp
    lib/QC_HashKeyReverseIterator.qpp
    lib/QC_HashPairIterator.qpp
    lib/QC_HashPairReverseIterator.qpp
    lib/QC_ObjectIterator.qpp
    lib/QC_ObjectReverseIterator.qpp
    lib/QC_ObjectKeyIterator.qpp
    lib/QC_ObjectKeyReverseIterator.qpp
    lib/QC_ObjectPairIterator.qpp
    lib/QC_ObjectPairReverseIterator.qpp
    lib/QC_HashListIterator.qpp
    lib/QC_HashListReverseIterator.qpp
    lib/QC_ListHashIterator.qpp
    lib/QC_ListHashReverseIterator.qpp
    lib/QC_AbstractLineIterator.qpp
    lib/QC_FileLineIterator.qpp
    lib/QC_DataLineIterator.qpp
    lib/QC_InputStreamLineIterator.qpp
    lib/QC_SingleValueIterator.qpp
    lib/QC_AbstractDatasource.qpp
    lib/QC_AbstractSQLStatement.qpp
    lib/QC_RangeIterator.qpp
    lib/QC_Datasource.qpp
    lib/QC_DatasourcePool.qpp
    lib/QC_Dir.qpp
    lib/QC_ReadOnlyFile.qpp
    lib/QC_File.qpp
    lib/QC_FtpClient.qpp
    lib/QC_Gate.qpp
    lib/QC_GetOpt.qpp
    lib/QC_HTTPClient.qpp
    lib/QC_Mutex.qpp
    lib/QC_Program.qpp
    lib/QC_ProgramControl.qpp
    lib/QC_DebugProgram.qpp
    lib/QC_Breakpoint.qpp
    lib/QC_Queue.qpp
    lib/QC_RWLock.qpp
    lib/QC_SQLStatement.qpp
    lib/QC_Sequence.qpp
    lib/QC_Socket.qpp
    lib/QC_TermIOS.qpp
    lib/QC_TimeZone.qpp
    lib/QC_TreeMap.qpp
    lib/QC_SSLCertificate.qpp
    lib/QC_SSLPrivateKey.qpp
    lib/QC_ThreadPool.qpp
    lib/QC_StreamBase.qpp
    lib/QC_InputStream.qpp
    lib/QC_BinaryInputStream.qpp
    lib/QC_StringInputStream.qpp
    lib/QC_FileInputStream.qpp
    lib/QC_EncodingConversionInputStream.qpp
    lib/QC_OutputStream.qpp
    lib/QC_BinaryOutputStream.qpp
    lib/QC_StringOutputStream.qpp
    lib/QC_FileOutputStream.qpp
    lib/QC_EncodingConversionOutputStream.qpp
    lib/QC_StreamPipe.qpp
    lib/QC_PipeInputStream.qpp
    lib/QC_PipeOutputStream.qpp
    lib/QC_StreamWriter.qpp
    lib/QC_StreamReader.qpp
    lib/QC_BufferedStreamReader.qpp
    lib/QC_Transform.qpp
    lib/QC_TransformInputStream.qpp
    lib/QC_TransformOutputStream.qpp
    lib/QC_StdoutOutputStream.qpp
    lib/QC_StderrOutputStream.qpp
    lib/ql_misc.qpp
    lib/ql_compression.qpp
    lib/ql_thread.qpp
    lib/ql_crypto.qpp
    lib/ql_lib.qpp
    lib/ql_file.qpp
    lib/ql_string.qpp
    lib/ql_time.qpp
    lib/ql_math.qpp
    lib/ql_list.qpp
    lib/ql_type.qpp
    lib/ql_pwd.qpp
    lib/ql_object.qpp
    lib/ql_env.qpp
    lib/ql_dbi.qpp
    lib/ql_context.qpp
    lib/qc_option.qpp
    lib/qc_errno.qpp
    lib/qc_qore.qpp
)

set(PSEUDO_QPP_SRC
    lib/Pseudo_QC_All.qpp
    lib/Pseudo_QC_Nothing.qpp
    lib/Pseudo_QC_Date.qpp
    lib/Pseudo_QC_Object.qpp
    lib/Pseudo_QC_Hash.qpp
    lib/Pseudo_QC_String.qpp
    lib/Pseudo_QC_Binary.qpp
    lib/Pseudo_QC_List.qpp
    lib/Pseudo_QC_Bool.qpp
    lib/Pseudo_QC_Int.qpp
    lib/Pseudo_QC_Float.qpp
    lib/Pseudo_QC_Number.qpp
    lib/Pseudo_QC_Closure.qpp
    lib/Pseudo_QC_Callref.qpp
)

set(QORE_BIN_FILES
    doxygen/qdx
    bin/qget
    bin/rest
    bin/schema-reverse
    bin/sfrest
    bin/sqlutil
	bin/qdbg
	bin/qdbg-server
	bin/qdbg-remote
)

set(QORE_DOX_TMPL_SRC
    doxygen/lang/mainpage.dox.tmpl
    doxygen/lang/000_intro.dox.tmpl
    doxygen/lang/105_desc_overview.dox.tmpl
    doxygen/lang/110_environment_variables.dox.tmpl
    doxygen/lang/115_conditional_parsing.dox.tmpl
    doxygen/lang/120_modules.dox.tmpl
    doxygen/lang/125_include_files.dox.tmpl
    doxygen/lang/130_identifiers.dox.tmpl
    doxygen/lang/135_comments.dox.tmpl
    doxygen/lang/140_variables.dox.tmpl
    doxygen/lang/145_basic_data_types.dox.tmpl
    doxygen/lang/150_container_data_types.dox.tmpl
    doxygen/lang/152_code_data_types.dox.tmpl
    doxygen/lang/155_data_type_declarations.dox.tmpl
    doxygen/lang/157_lvalue_references.dox.tmpl
    doxygen/lang/160_overloading.dox.tmpl
    doxygen/lang/165_time_zones.dox.tmpl
    doxygen/lang/170_character_encoding.dox.tmpl
    doxygen/lang/175_expressions.dox.tmpl
    doxygen/lang/180_operators.dox.tmpl
    doxygen/lang/185_qore_regex.dox.tmpl
    doxygen/lang/190_date_time_arithmetic.dox.tmpl
    doxygen/lang/195_statements.dox.tmpl
    doxygen/lang/200_functions.dox.tmpl
    doxygen/lang/202_code_flags.dox.tmpl
    doxygen/lang/205_namespaces.dox.tmpl
    doxygen/lang/210_constants.dox.tmpl
    doxygen/lang/215_classes.dox.tmpl
    doxygen/lang/217_hashdecl.dox.tmpl
    doxygen/lang/220_threading.dox.tmpl
    doxygen/lang/225_exception_handling.dox.tmpl
    doxygen/lang/230_signal_handling.dox.tmpl
    doxygen/lang/235_event_handling.dox.tmpl
    doxygen/lang/240_command_line_processing.dox.tmpl
    doxygen/lang/245_parse_directives.dox.tmpl
    doxygen/lang/250_warnings.dox.tmpl
    doxygen/lang/255_keywords.dox.tmpl
    doxygen/lang/900_release_notes.dox.tmpl
    doxygen/lang/Doxyfile.tmpl
)

# qore interpreter
set(QORE_CPP_SRC command-line.cpp qore-main.cpp)
#set(QR_CPP_SRC command-line.cpp qr-main.cpp)

# find needed libraries
qore_find_pthreads()
find_package(ICONV REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX 2.5.35 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(MPFR REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCRE REQUIRED libpcre)
if (NOT (WIN32 AND MINGW AND MSYS))
    find_package(Backtrace)
    if (Backtrace_FOUND)
        list(APPEND LIBQORE_OPTIONAL_INCLUDES ${Backtrace_INCLUDE_DIRS})
        list(APPEND LIBQORE_OPTIONAL_LIBS ${Backtrace_LIBRARIES})
    endif (Backtrace_FOUND)
endif (NOT (WIN32 AND MINGW AND MSYS))

if (WIN32 AND MINGW AND MSYS)
    SET(CMAKE_DL_LIBS dl)
    SET(MPFR_LIBRARIES mpfr gmp)
    SET(OPENSSL_LIBRARIES ssl crypto)
    SET(WINSOCKET_LIBRARIES ws2_32)
    SET(NO_SIGNAL_HANDLING ON)
endif (WIN32 AND MINGW AND MSYS)

create_git_revision()

# check for C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(HAVE_CXX11 true)
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# check if we need libatomic
check_cxx_atomic()

# other checks
qore_os_checks()
qore_iconv_translit_check()
qore_openssl_checks()
qore_mpfr_checks()

qore_check_headers_cxx(arpa/inet.h cxxabi.h dlfcn.h fcntl.h getopt.h glob.h grp.h iconv.h inttypes.h memory.h netdb.h netinet/in.h netinet/tcp.h poll.h pwd.h stdbool.h stddef.h stdint.h stdlib.h string.h strings.h sys/select.h sys/socket.h sys/socket.h sys/stat.h sys/statvfs.h sys/time.h sys/types.h sys/un.h sys/wait.h termios.h umem.h unistd.h vfork.h winsock2.h ws2tcpip.h)

qore_search_libs(LIBQORE_LIBS setsockopt socket)
qore_search_libs(LIBQORE_LIBS gethostbyname nsl)
qore_search_libs(LIBQORE_LIBS clock_gettime rt)

set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${LIBQORE_LIBS})
qore_check_funcs(access alarm atoll bzero chown clock_gettime doprnt exp2 floor fork fsync getaddrinfo getegid geteuid getgid getgrgid_r getgrnam_r getgroups gethostbyaddr gethostbyname gethostname getifaddrs getnameinfo getppid getpwnam_r getpwuid_r getsockopt gettimeofday getuid glob gmtime_r inet_ntop inet_pton isblank kill lchown localtime_r lstat memmem memmove memset mkfifo mkfifo nanosleep poll pthread_attr_getstacksize putenv random readlink realloc realpath regcomp round select setegid setegid setenv seteuid seteuid setgid setgroups setsid setsockopt setuid setuid sleep socket strcasecmp strcasestr strchr strdup strerror strncasecmp strspn strstr strtoll strtol symlink system tbbmalloc timegm unsetenv usleep vfork vprintf)
qore_func_strerror_r()
qore_gethost_checks()
qore_find_pthread_setname_np()
qore_find_pthread_getattr_np()
unset(CMAKE_REQUIRED_LIBRARIES)

qore_other_checks()
qore_dlcpp_check()
qore_stl_hash_map(include/qore/hash_map_include.h)
qore_stl_slist(include/qore/slist_include.h)
qore_cpu_checks()
qore_assembler_files()

execute_process(COMMAND uname -a OUTPUT_VARIABLE QORE_BUILD_HOST OUTPUT_STRIP_TRAILING_WHITESPACE)
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
   set(TARGET_BITS 64)
else()
   set(TARGET_BITS 32)
endif()

if(NOT DEFINED NO_SIGNAL_HANDLING)
   set(HAVE_SIGNAL_HANDLING true)
endif()

# prepare scanner and parser targets
bison_target(qoreparser lib/parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
flex_target(qorescanner lib/scanner.lpp ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
add_flex_bison_dependency(qorescanner qoreparser)

# qpp
if(NOT HAVE_GETOPT_H)
set(QPP_GETOPT_LONG_SRC lib/getopt_long.cpp)
endif()
add_executable(qpp
    lib/qpp.cpp
    ${QPP_GETOPT_LONG_SRC}
)
target_compile_definitions(qpp PUBLIC HAVE_UNIX_CONFIG_H)
# just fake qpp for macro
set(QORE_QPP_EXECUTABLE ${CMAKE_BINARY_DIR}/qpp)
target_link_libraries(qpp ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES})

# libqore generators
qore_wrap_qpp_value(LIBQORE_QPP_CPP_SRC DOXLIST QPP_DOX_SRC ${LIBQORE_QPP_SRC})
qore_wrap_qpp_value(PSEUDO_QPP_CPP_SRC DOXLIST QPP_DOX_SRC ${PSEUDO_QPP_SRC})

add_custom_target(QPP_GENERATED_FILES DEPENDS ${LIBQORE_QPP_CPP_SRC} ${PSEUDO_QPP_CPP_SRC})
add_custom_target(BISON_GENERATED_FILES DEPENDS ${BISON_qoreparser_OUTPUTS})
add_custom_target(FLEX_GENERATED_FILES DEPENDS ${FLEX_qorescanner_OUTPUTS})

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(LIBQORE_CPP_SRC
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
    lib/BarewordNode.cpp
    lib/SelfVarrefNode.cpp
    lib/StaticClassVarRefNode.cpp
    lib/lvalue_ref.cpp
    lib/ReferenceNode.cpp
    lib/BackquoteNode.cpp
    lib/ContextrefNode.cpp
    lib/ComplexContextrefNode.cpp
    lib/ContextRowNode.cpp
    lib/ClassRefNode.cpp
    lib/ScopedRefNode.cpp
    lib/AbstractQoreNode.cpp
    lib/QoreStringNode.cpp
    lib/DateTimeNode.cpp
    lib/qore_date_private.cpp
    lib/QoreHashNode.cpp
    lib/BinaryNode.cpp
    lib/QoreBigIntNode.cpp
    lib/QoreBoolNode.cpp
    lib/QoreFloatNode.cpp
    lib/QoreNumberNode.cpp
    lib/QoreNullNode.cpp
    lib/QoreNothingNode.cpp
    lib/VarRefNode.cpp
    lib/FunctionCallNode.cpp
    lib/NewComplexTypeNode.cpp
    lib/QoreParseHashNode.cpp
    lib/QoreClosureParseNode.cpp
    lib/QoreClosureNode.cpp
    lib/QoreImplicitArgumentNode.cpp
    lib/QoreImplicitElementNode.cpp
    lib/Function.cpp
    lib/GlobalVariableList.cpp
    lib/FunctionList.cpp
    lib/AbstractStatement.cpp
    lib/OnBlockExitStatement.cpp
    lib/ExpressionStatement.cpp
    lib/ReturnStatement.cpp
    lib/StatementBlock.cpp
    lib/ContextStatement.cpp
    lib/SummarizeStatement.cpp
    lib/IfStatement.cpp
    lib/WhileStatement.cpp
    lib/DoWhileStatement.cpp
    lib/ForStatement.cpp
    lib/ForEachStatement.cpp
    lib/TryStatement.cpp
    lib/ThrowStatement.cpp
    lib/SwitchStatement.cpp
    lib/Variable.cpp
    lib/support.cpp
    lib/QoreType.cpp
    lib/ModuleManager.cpp
    lib/QoreException.cpp
    lib/ExceptionSink.cpp
    lib/QoreStandardException.cpp
    lib/QoreClass.cpp
    lib/TypedHashDecl.cpp
    lib/Context.cpp
    lib/FindNode.cpp
    lib/charset.cpp
    lib/unicode-charmaps.cpp
    lib/ThreadClosureVariableStack.cpp
    lib/ThreadLocalVariableData.cpp
    lib/WeakReferenceNode.cpp
    lib/QoreProgram.cpp
    lib/QoreProgramHelper.cpp
    lib/QoreNamespace.cpp
    lib/QoreNet.cpp
    lib/QoreURL.cpp
    lib/QoreFile.cpp
    lib/QoreDir.cpp
    lib/QoreSocket.cpp
    lib/DateTime.cpp
    lib/QoreLib.cpp
    lib/QoreTimeZoneManager.cpp
    lib/QoreString.cpp
    lib/QoreObject.cpp
    lib/RSet.cpp
    lib/RSection.cpp
    lib/QoreParseListNode.cpp
    lib/QoreListNode.cpp
    lib/QoreValueList.cpp
    lib/qore-main.cpp
    lib/QoreGetOpt.cpp
    lib/QoreFtpClient.cpp
    lib/DBI.cpp
    lib/ConstantList.cpp
    lib/QoreClassList.cpp
    lib/HashDeclList.cpp
    lib/thread.cpp
    lib/AbstractThreadResource.cpp
    lib/ThreadResourceList.cpp
    lib/VRMutex.cpp
    lib/VLock.cpp
    lib/QoreRWLock.cpp
    lib/AbstractSmartLock.cpp
    lib/SmartMutex.cpp
    lib/Datasource.cpp
    lib/DatasourcePool.cpp
    lib/ManagedDatasource.cpp
    lib/SQLStatement.cpp
    lib/QoreSQLStatement.cpp
    lib/ExecArgList.cpp
    lib/CallReferenceNode.cpp
    lib/CallStack.cpp
    lib/NamedScope.cpp
    lib/RWLock.cpp
    lib/QoreSSLBase.cpp
    lib/QoreSSLCertificate.cpp
    lib/QoreSSLPrivateKey.cpp
    lib/QoreSocketObject.cpp
    lib/QoreCondition.cpp
    lib/QoreQueue.cpp
    lib/QoreQueueHelper.cpp
    lib/QoreRegex.cpp
    lib/QoreRegexBase.cpp
    lib/QoreRegexSubst.cpp
    lib/QoreTransliteration.cpp
    lib/Sequence.cpp
    lib/QoreReferenceCounter.cpp
    lib/QoreHTTPClient.cpp
    lib/QoreHttpClientObject.cpp
    lib/ParseOptionMap.cpp
    lib/SystemEnvironment.cpp
    lib/QoreCounter.cpp
    lib/ReferenceArgumentHelper.cpp
    lib/ReferenceHelper.cpp
    lib/QoreTypeInfo.cpp
    lib/QoreDeleteOperatorNode.cpp
    lib/QoreRemoveOperatorNode.cpp
    lib/QoreSpliceOperatorNode.cpp
    lib/QoreExtractOperatorNode.cpp
    lib/QoreCastOperatorNode.cpp
    lib/QoreUnaryMinusOperatorNode.cpp
    lib/QoreUnaryPlusOperatorNode.cpp
    lib/QoreLogicalNotOperatorNode.cpp
    lib/QoreDotEvalOperatorNode.cpp
    lib/QoreLogicalEqualsOperatorNode.cpp
    lib/QoreLogicalAbsoluteEqualsOperatorNode.cpp
    lib/QoreModuloOperatorNode.cpp
    lib/QoreBinaryAndOperatorNode.cpp
    lib/QoreBinaryNotOperatorNode.cpp
    lib/QoreBinaryOrOperatorNode.cpp
    lib/QoreBinaryXorOperatorNode.cpp
    lib/QoreShiftLeftOperatorNode.cpp
    lib/QoreShiftRightOperatorNode.cpp
    lib/QoreExistsOperatorNode.cpp
    lib/QoreElementsOperatorNode.cpp
    lib/QoreInstanceOfOperatorNode.cpp
    lib/QoreHashObjectDereferenceOperatorNode.cpp
    lib/QoreRegexMatchOperatorNode.cpp
    lib/QoreRegexNMatchOperatorNode.cpp
    lib/QoreRegexExtractOperatorNode.cpp
    lib/QoreRegexSubstOperatorNode.cpp
    lib/QoreTransliterationOperatorNode.cpp
    lib/QoreAssignmentOperatorNode.cpp
    lib/QoreListAssignmentOperatorNode.cpp
    lib/QorePlusEqualsOperatorNode.cpp
    lib/QoreIntPlusEqualsOperatorNode.cpp
    lib/QoreMinusEqualsOperatorNode.cpp
    lib/QoreIntMinusEqualsOperatorNode.cpp
    lib/QoreOrEqualsOperatorNode.cpp
    lib/QoreAndEqualsOperatorNode.cpp
    lib/QoreModuloEqualsOperatorNode.cpp
    lib/QoreMultiplyEqualsOperatorNode.cpp
    lib/QoreDivideEqualsOperatorNode.cpp
    lib/QoreXorEqualsOperatorNode.cpp
    lib/QoreShiftLeftEqualsOperatorNode.cpp
    lib/QoreShiftRightEqualsOperatorNode.cpp
    lib/QorePostIncrementOperatorNode.cpp
    lib/QoreIntPostIncrementOperatorNode.cpp
    lib/QorePostDecrementOperatorNode.cpp
    lib/QoreIntPostDecrementOperatorNode.cpp
    lib/QorePreIncrementOperatorNode.cpp
    lib/QoreIntPreIncrementOperatorNode.cpp
    lib/QorePreDecrementOperatorNode.cpp
    lib/QoreIntPreDecrementOperatorNode.cpp
    lib/QoreLogicalLessThanOperatorNode.cpp
    lib/QoreLogicalGreaterThanOperatorNode.cpp
    lib/QoreLogicalLessThanOrEqualsOperatorNode.cpp
    lib/QoreLogicalGreaterThanOrEqualsOperatorNode.cpp
    lib/QoreDivisionOperatorNode.cpp
    lib/QoreMapOperatorNode.cpp
    lib/QoreMapSelectOperatorNode.cpp
    lib/QoreHashMapOperatorNode.cpp
    lib/QoreHashMapSelectOperatorNode.cpp
    lib/QoreFoldlOperatorNode.cpp
    lib/QoreNullCoalescingOperatorNode.cpp
    lib/QoreValueCoalescingOperatorNode.cpp
    lib/QoreChompOperatorNode.cpp
    lib/QoreTrimOperatorNode.cpp
    lib/QoreSquareBracketsOperatorNode.cpp
    lib/QoreShiftOperatorNode.cpp
    lib/QoreUnshiftOperatorNode.cpp
    lib/QorePopOperatorNode.cpp
    lib/QorePushOperatorNode.cpp
    lib/QoreLogicalAndOperatorNode.cpp
    lib/QoreLogicalOrOperatorNode.cpp
    lib/QoreLogicalComparisonOperatorNode.cpp
    lib/QorePlusOperatorNode.cpp
    lib/QoreMinusOperatorNode.cpp
    lib/QoreMultiplicationOperatorNode.cpp
    lib/QoreBackgroundOperatorNode.cpp
    lib/QoreValue.cpp
    lib/StreamPipe.cpp
    lib/CompressionTransforms.cpp
    lib/EncryptionTransforms.cpp
    lib/Transform.cpp
    lib/QorePseudoMethods.cpp
    lib/xxhash.cpp
    lib/minitest.cpp
    lib/FunctionalOperator.cpp
    lib/FunctionalOperatorInterface.cpp
    lib/QoreQuestionMarkOperatorNode.cpp
    lib/QoreKeysOperatorNode.cpp
    lib/QoreSelectOperatorNode.cpp
    lib/QoreRangeOperatorNode.cpp
    lib/QoreSquareBracketsRangeOperatorNode.cpp
)

if (HAVE_SIGNAL_HANDLING)
    set(LIBQORE_CPP_SRC ${LIBQORE_CPP_SRC} lib/QoreSignal.cpp)
endif ()
if (NOT HAVE_GLOB)
    set(LIBQORE_CPP_SRC ${LIBQORE_CPP_SRC} lib/glob.cpp)
endif ()
if (NOT HAVE_INET_NTOP)
    set(LIBQORE_CPP_SRC ${LIBQORE_CPP_SRC} lib/inet_ntop.cpp)
endif ()
if (NOT HAVE_INET_PTON)
    set(LIBQORE_CPP_SRC ${LIBQORE_CPP_SRC} lib/inet_pton.cpp)
endif ()
if (${QORE_BUILD_TYPE_LWR} MATCHES "debug")
    set(LIBQORE_CPP_SRC ${LIBQORE_CPP_SRC} lib/ql_debug.cpp)
endif ()

# prepare libqore target
if (SINGLE_COMPILATION_UNIT)
    add_library(libqore SHARED lib/single-compilation-unit.cpp)
else (SINGLE_COMPILATION_UNIT)
    add_library(libqore SHARED ${LIBQORE_QPP_CPP_SRC} ${LIBQORE_CPP_SRC} ${LIBQORE_ASM_SRC})
endif (SINGLE_COMPILATION_UNIT)

add_dependencies(QPP_GENERATED_FILES qpp)
add_dependencies(libqore QPP_GENERATED_FILES BISON_GENERATED_FILES FLEX_GENERATED_FILES)

target_compile_definitions(libqore PUBLIC _QORE_LIB_INTERN HAVE_UNIX_CONFIG_H)
target_compile_definitions(libqore PRIVATE MODULE_DIR="${MODULE_DIR}")
target_compile_definitions(libqore PRIVATE MODULE_VER_DIR="${MODULE_VER_DIR}")
target_compile_definitions(libqore PRIVATE USER_MODULE_DIR="${USER_MODULE_DIR}")
target_compile_definitions(libqore PRIVATE USER_MODULE_VER_DIR="${USER_MODULE_VER_DIR}")
if (WIN32 AND MINGW AND MSYS)
    target_compile_definitions(libqore PUBLIC BUILDING_DLL)
endif (WIN32 AND MINGW AND MSYS)

target_include_directories(libqore PUBLIC ${BZIP2_INCLUDE_DIR} ${ICONV_INCLUDE_DIR} ${MPFR_INCLUDE_DIRS} ${LIBQORE_OPTIONAL_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR} ${PCRE_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(libqore ${BZIP2_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${ICONV_LIBRARY} ${LIBQORE_LIBS} ${LIBQORE_OPTIONAL_LIBS} ${MPFR_LIBRARIES} ${OPENSSL_LIBRARIES} ${PCRE_LDFLAGS} ${ZLIB_LIBRARIES})
if (WIN32 AND MINGW AND MSYS)
    target_link_libraries(libqore ${WINSOCKET_LIBRARIES})
endif (WIN32 AND MINGW AND MSYS)

set_target_properties(libqore PROPERTIES OUTPUT_NAME qore)
if (APPLE)
    set_target_properties(libqore PROPERTIES VERSION 6)
    set_target_properties(libqore PROPERTIES SOVERSION 6)
    set_target_properties(libqore PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_FULL_LIBDIR})
else (APPLE)
    set_target_properties(libqore PROPERTIES VERSION 6.0.0)
    set_target_properties(libqore PROPERTIES SOVERSION 6)
endif (APPLE)

# prepare qore target
add_executable(qore ${QORE_CPP_SRC})
target_link_libraries(qore libqore)
#add_executable(qr ${QR_CPP_SRC})
#target_link_libraries(qr libqore)

#set_target_properties(qore qr PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR})
set_target_properties(qore PROPERTIES INSTALL_RPATH_USE_LINK_PATH ${CMAKE_INSTALL_FULL_LIBDIR})

# configuration files
message(STATUS "Generating unix-config.h")
configure_file(${CMAKE_SOURCE_DIR}/cmake/unix-config.h.cmake
               ${CMAKE_BINARY_DIR}/include/qore/intern/unix-config.h)
message(STATUS "Generating qore-version.h")
configure_file(${CMAKE_SOURCE_DIR}/cmake/qore-version.h.cmake
               ${CMAKE_BINARY_DIR}/include/qore/qore-version.h)
message(STATUS "Generating QoreConfigVersion.cmake")
configure_file(${CMAKE_SOURCE_DIR}/cmake/QoreConfigVersion.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/QoreConfigVersion.cmake @ONLY)
message(STATUS "Generating QoreConfig.cmake")
configure_file(${CMAKE_SOURCE_DIR}/cmake/QoreConfig.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/QoreConfig.cmake @ONLY)
message(STATUS "Generating qore.pc")
configure_file(${CMAKE_SOURCE_DIR}/qore.pc.in
               ${CMAKE_BINARY_DIR}/qore.pc @ONLY)

# prepare dist target
qore_dist(${VERSION})

# prepare installation targets
install(PROGRAMS ${QORE_BIN_FILES}
        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

install(TARGETS qore qpp libqore
        RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
#install(TARGETS qore qpp qr libqore
#        RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
#        LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${CMAKE_BINARY_DIR}/cmake/QoreConfig.cmake ${CMAKE_BINARY_DIR}/cmake/QoreConfigVersion.cmake cmake/QoreMacros.cmake
        DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/Qore)
install(FILES ${CMAKE_BINARY_DIR}/qore.pc
        DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)
install(DIRECTORY include/qore
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
        PATTERN intern EXCLUDE
        PATTERN minitest.hpp EXCLUDE
        PATTERN macros-none.h EXCLUDE
        PATTERN hash_map_include.h EXCLUDE
        PATTERN slist_include.h EXCLUDE
        PATTERN qore-version.h EXCLUDE)
install(FILES ${CMAKE_BINARY_DIR}/include/qore/qore-version.h
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/qore)
install(FILES qore.1
        DESTINATION ${CMAKE_INSTALL_FULL_MANDIR}/man1)
install(FILES doxygen/Doxyfile.in
        DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/qore)

# prepare documentation targets
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # documentation
    configure_file(${CMAKE_SOURCE_DIR}/doxygen/footer_template.html ${CMAKE_BINARY_DIR}/doxygen/footer_template.html @ONLY)
    add_custom_target(docs COMMENT "Generating API documentation")

    # doc generators
    qore_wrap_dox(QORE_DOX_SRC ${QORE_DOX_TMPL_SRC})
    add_custom_target(QORE_DOX_FILES DEPENDS ${QORE_DOX_SRC})
    add_dependencies(QORE_DOX_FILES qpp)

    # get space-separated string from list of generated files
    string(REPLACE ";" " " QORE_DOX_SRC_STRING "${QORE_DOX_SRC}")

    # get space-separated string from list of generated files
    string(REPLACE ";" " " QPP_DOX_SRC_STRING "${QPP_DOX_SRC}")

    add_dependencies(libqore QORE_DOX_FILES)

    # library
    message(STATUS "Preparing Doxyfile for library")
    configure_file(${CMAKE_SOURCE_DIR}/doxygen/lib/Doxyfile.in ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.lib @ONLY)
    add_custom_target(docs-lib
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs/library/html
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.lib
        COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/library/html/*.html
        COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/library/html/search/*.html
        COMMENT "Generating API documentation with Doxygen for: lib" VERBATIM
    )
    add_dependencies(docs docs-lib)

    # lang
    message(STATUS "Preparing Doxyfile for lang")
    configure_file(${CMAKE_SOURCE_DIR}/doxygen/lang/Doxyfile.in ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.lang @ONLY)
    add_custom_target(docs-lang
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs/lang/html
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.lang
        COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/lang/html/*.html ${CMAKE_BINARY_DIR}/docs/lang/html/*.js
        COMMAND QORE_MODULE_DIR=${CMAKE_SOURCE_DIR}/qlib ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/lang/html/search/*.html ${CMAKE_BINARY_DIR}/docs/lang/html/search/*.js
        DEPENDS QORE_DOX_FILES
        COMMENT "Generating API documentation with Doxygen for: lang" VERBATIM
    )
    add_dependencies(docs docs-lang)
endif (DOXYGEN_FOUND)

add_custom_target(install-docs
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/docs/ ${CMAKE_INSTALL_PREFIX}/share/doc/qore
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/docs
    COMMENT "Installing API documentation to: ${CMAKE_INSTALL_PREFIX}/share/doc/qore" VERBATIM
)
add_dependencies(install-docs docs)
# end of documentation

# astparser module
add_subdirectory(modules/astparser)

# HACK: we need to simulate QORE_USER_MODULES_DIR as the versioned one
#       for qore core qlib modules.
# NOTE: anything below will be installed into versioned dirs!
set(QORE_USER_MODULES_DIR ${USER_MODULE_VER_DIR})
# modules must go after docs target - it's depending on it

# qlib modules + their documentation
# ---
# modules here are divided to groups in the order of dependence
# and then sorted alphabetically inside the groups

# base modules
qore_user_module("qlib/DatasourceProvider.qm" "")
qore_user_module("qlib/DebugUtil.qm" "")
qore_user_module("qlib/Mapper.qm" "")
qore_user_module("qlib/Qdx.qm" "")
qore_user_module("qlib/Qorize.qm" "")
qore_user_module("qlib/Util.qm" "")
qore_user_module("qlib/WebSocketUtil.qm" "")

# base 2 modules
qore_user_module("qlib/ConnectionProvider.qm" "Util")
qore_user_module("qlib/CsvUtil.qm" "Util")
qore_user_module("qlib/Diff.qm" "Util")
qore_user_module("qlib/FilePoller.qm" "Util")
qore_user_module("qlib/FixedLengthUtil.qm" "Util")
qore_user_module("qlib/Mime.qm" "Util")
qore_user_module("qlib/QUnit.qm" "Util")
qore_user_module("qlib/SqlUtil.qm" "Util")
qore_user_module("qlib/TextWrap.qm" "Util")

# base 3 modules
qore_user_module("qlib/DebugCmdLine.qm" "ConnectionProvider;DebugUtil")
qore_user_module("qlib/DebugProgramControl.qm" "DebugUtil")
qore_user_module("qlib/HttpServerUtil.qm" "Mime;Util")
qore_user_module("qlib/MailMessage.qm" "Mime")
qore_user_module("qlib/Schema.qm" "SqlUtil;Util")
qore_user_module("qlib/WebSocketClient.qm" "ConnectionProvider;Util;WebSocketUtil")

# base 4 modules
qore_user_module("qlib/RestSchemaValidator.qm" "HttpServerUtil")
qore_user_module("qlib/DebugLinenoiseCmdLine.qm" "DebugUtil;DebugCmdLine")

# base 5 modules
qore_user_module("qlib/Swagger.qm" "Mime;Util;HttpServerUtil;RestSchemaValidator")

# base 6 modules
qore_user_module("qlib/RestClient.qm" "ConnectionProvider;Mime;RestSchemaValidator;Swagger")

# base 7 modules
qore_user_module("qlib/BulkSqlUtil.qm" "SqlUtil")
qore_user_module("qlib/FreetdsSqlUtil.qm" "SqlUtil")
qore_user_module("qlib/Logger.qm" "Logger")
qore_user_module("qlib/HttpServer.qm" "HttpServerUtil;Mime;Util")
qore_user_module("qlib/MysqlSqlUtil.qm" "SqlUtil")
qore_user_module("qlib/OracleSqlUtil.qm" "SqlUtil;Util")
qore_user_module("qlib/PgsqlSqlUtil.qm" "SqlUtil")
qore_user_module("qlib/Pop3Client.qm" "ConnectionProvider;MailMessage;Mime")
qore_user_module("qlib/RestHandler.qm" "HttpServerUtil;Mime;Util;RestSchemaValidator")
qore_user_module("qlib/SalesforceRestClient.qm" "ConnectionProvider;Mime;RestClient")
qore_user_module("qlib/SewioRestClient.qm" "ConnectionProvider;Mime;RestClient")
qore_user_module("qlib/SewioWebSocketClient.qm" "ConnectionProvider;Util;WebSocketUtil;WebSocketClient")
qore_user_module("qlib/SchemaReverse.qm" "Qorize;Schema;SqlUtil;Util")
qore_user_module("qlib/SmtpClient.qm" "ConnectionProvider;MailMessage;Mime")
qore_user_module("qlib/TableMapper.qm" "Mapper;SqlUtil")
qore_user_module("qlib/TelnetClient.qm" "ConnectionProvider")
qore_user_module("qlib/WebSocketHandler.qm" "HttpServerUtil;WebSocketUtil")
qore_user_module("qlib/WebUtil.qm" "HttpServerUtil;Mime;Util")

# base 7 modules
qore_user_module("qlib/DebugHandler.qm" "DebugProgramControl;DebugUtil;HttpServerUtil;WebSocketHandler;WebSocketUtil")

# print info
message(STATUS "")
message(STATUS "Implicit link libraries and flags detected for C++: ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES}")
qore_config_info()
