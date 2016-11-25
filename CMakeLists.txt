cmake_minimum_required(VERSION 3.0.0)

project(qore-language)

option(ICONV_TRANSLIT
       "Force //Translit to iconv encoding to do transliteration, if OFF it is tested for"
       OFF)

option(QORE_RUNTIME_THREAD_STACK_TRACE
       "enable runtime thread stack trace (performance penalty), only for debug builds"
       OFF)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 8)
set(VERSION_SUB 12)

# If changing the module API numbers, make sure to change the appropriate
# defines in include/qore/ModuleManager.h too.
set(MODULE_API_MAJOR 0)
set(MODULE_API_MINOR 19)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "release")
endif (NOT CMAKE_BUILD_TYPE)
string(TOLOWER ${CMAKE_BUILD_TYPE} QORE_BUILD_TYPE_LWR)
if (${QORE_BUILD_TYPE_LWR} MATCHES "debug")
    add_definitions(-DDEBUG)
else ()
    add_definitions(-DNDEBUG)
endif ()

if ((NOT ${QORE_BUILD_TYPE_LWR} MATCHES "debug") AND QORE_RUNTIME_THREAD_STACK_TRACE)
    message(FATAL_ERROR "QORE_RUNTIME_THREAD_STACK_TRACE only available on debug builds")
endif()

# simulate QoreConfig
set(QORE_QDX_EXECUTABLE "${CMAKE_SOURCE_DIR}/doxygen/qdx")
# simulate QoreConfig

include("${CMAKE_SOURCE_DIR}/cmake/QoreMacros.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/QoreMacrosIntern.cmake")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include_directories( ${CMAKE_BINARY_DIR} )
include_directories( ${CMAKE_BINARY_DIR}/include/ )
include_directories( ${CMAKE_SOURCE_DIR}/include )
include_directories( ${CMAKE_SOURCE_DIR}/include/qore )

include(CheckIncludeFiles)
include(GNUInstallDirs)
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
#check_include_files(getopt.h HAVE_GETOPT_H)
#check_include_files(malloc.h HAVE_MALLOC_H)
#check_include_files("sys/param.h;sys/mount.h" HAVE_SYS_MOUNT_H)
#configure_file(${CMAKE_SOURCE_DIR}/include/qore/intern/unix-config.h.in
#               ${CMAKE_BINARY_DIR}/include/qore/intern/unix-config.h)
#add_definitions(-DHAVE_UNIX_CONFIG_H)
#add_definitions(-DHAVE_GETOPT_H)
#add_definitions(-DHAVE_STRCASESTR)

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(myprefix ${CMAKE_INSTALL_PREFIX})
set(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_SUB})

set(MODULE_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/qore-modules)
set(MODULE_VER_DIR ${MODULE_DIR}/${VERSION})
set(USER_MODULE_DIR ${CMAKE_INSTALL_FULL_DATADIR}/qore-modules/)
set(USER_MODULE_VER_DIR ${USER_MODULE_DIR}${VERSION})


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
	lib/QC_FileLineIterator.qpp
	lib/QC_SingleValueIterator.qpp
	lib/QC_AbstractDatasource.qpp
	lib/QC_RangeIterator.qpp
	lib/QC_DataLineIterator.qpp
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

set(QMOD
    qlib/BulkSqlUtil.qm
    qlib/CsvUtil.qm
    qlib/Diff.qm
    qlib/FixedLengthUtil.qm
    qlib/FreetdsSqlUtil.qm
    qlib/HttpServer.qm
    qlib/HttpServerUtil.qm
    qlib/MailMessage.qm
    qlib/Mapper.qm
    qlib/Mime.qm
    qlib/MysqlSqlUtil.qm
    qlib/OracleSqlUtil.qm
    qlib/PgsqlSqlUtil.qm
    qlib/Pop3Client.qm
    qlib/QUnit.qm
    qlib/Qorize.qm
    qlib/RestClient.qm
    qlib/RestHandler.qm
    qlib/Schema.qm
    qlib/SmtpClient.qm
    qlib/SqlUtil.qm
    qlib/TableMapper.qm
    qlib/TelnetClient.qm
    qlib/Util.qm
    qlib/WebSocketClient.qm
    qlib/WebSocketHandler.qm
    qlib/WebSocketUtil.qm
    qlib/WebUtil.qm
)

# qore interpreter
set(QORE_CPP_SRC command-line.cpp qore-main.cpp)
set(QR_CPP_SRC command-line.cpp qr-main.cpp)

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
endif (NOT (WIN32 AND MINGW AND MSYS))

if (WIN32 AND MINGW AND MSYS)
    SET(CMAKE_DL_LIBS dl)
    SET(MPFR_LIBRARIES mpfr gmp)
    SET(OPENSSL_LIBRARIES ssl crypto)
    SET(WINSOCKET_LIBRARIES ws2_32)
    SET(NO_SIGNAL_HANDLING ON)
endif (WIN32 AND MINGW AND MSYS)

create_git_revision()

qore_os_checks()
qore_iconv_translit_check()
qore_openssl_checks()
qore_mpfr_checks()

qore_check_headers_cxx(fcntl.h inttypes.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h cxxabi.h arpa/inet.h sys/socket.h sys/statvfs.h winsock2.h ws2tcpip.h glob.h sys/un.h termios.h netinet/tcp.h pwd.h sys/wait.h getopt.h stdint.h sys/select.h poll.h grp.h)

qore_search_libs(LIBQORE_LIBS setsockopt socket)
qore_search_libs(LIBQORE_LIBS gethostbyname nsl)
qore_search_libs(LIBQORE_LIBS clock_gettime rt)

set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${LIBQORE_LIBS})
qore_check_funcs(bzero floor gethostbyaddr gethostbyname gethostname gettimeofday memmove memset mkfifo putenv regcomp select socket setsockopt getsockopt strcasecmp strchr strdup strerror strspn strstr atoll strtol strtoll isblank localtime_r gmtime_r exp2 clock_gettime realloc timegm seteuid setegid setenv unsetenv round pthread_attr_getstacksize getpwuid_r getpwnam_r getgrgid_r getgrnam_r glob system inet_ntop inet_pton lstat fsync lchown chown setsid setuid mkfifo random kill getppid getgid getegid getuid geteuid setuid seteuid setgid setegid sleep usleep nanosleep readlink symlink access strcasestr strncasecmp setgroups getgroups poll realpath memmem)
qore_func_strerror_r()
qore_gethost_checks()
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
qore_wrap_qpp_value(LIBQORE_QPP_CPP_SRC ${LIBQORE_QPP_SRC})

add_custom_target(QPP_GENERATED_FILES DEPENDS ${LIBQORE_QPP_CPP_SRC})
add_custom_target(BISON_GENERATED_FILES DEPENDS ${BISON_qoreparser_OUTPUTS})
add_custom_target(FLEX_GENERATED_FILES DEPENDS ${FLEX_qorescanner_OUTPUTS})

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(LIBQORE_CPP_SRC
        ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
        lib/Operator.cpp
        lib/QoreTreeNode.cpp
        lib/BarewordNode.cpp
        lib/SelfVarrefNode.cpp
        lib/StaticClassVarRefNode.cpp
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
        lib/QoreClosureParseNode.cpp
        lib/QoreClosureNode.cpp
        lib/QoreImplicitArgumentNode.cpp
        lib/QoreImplicitElementNode.cpp
        lib/Function.cpp
        lib/BuiltinFunctionList.cpp
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
        lib/QoreClass.cpp
        lib/Context.cpp
        lib/FindNode.cpp
        lib/charset.cpp
        lib/unicode-charmaps.cpp
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
        lib/QoreListNode.cpp
        lib/QoreValueList.cpp
        lib/qore-main.cpp
        lib/QoreGetOpt.cpp
        lib/QoreFtpClient.cpp
        lib/DBI.cpp
        lib/ConstantList.cpp
        lib/QoreClassList.cpp
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
        lib/NamedScope.cpp
        lib/RWLock.cpp
        lib/QoreSSLBase.cpp
        lib/QoreSSLCertificate.cpp
        lib/QoreSSLPrivateKey.cpp
        lib/QoreSocketObject.cpp
        lib/QoreCondition.cpp
        lib/QoreQueue.cpp
        lib/QoreQueueHelper.cpp
        lib/QoreRegexNode.cpp
        lib/QoreRegexBase.cpp
        lib/RegexSubstNode.cpp
        lib/RegexTransNode.cpp
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
        lib/QoreLogicalNotOperatorNode.cpp
        lib/QoreDotEvalOperatorNode.cpp
        lib/QoreLogicalEqualsOperatorNode.cpp
        lib/QoreModuloOperatorNode.cpp
        lib/QoreAssignmentOperatorNode.cpp
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
        lib/QoreNullCoalescingOperatorNode.cpp
        lib/QoreValueCoalescingOperatorNode.cpp
        lib/QoreChompOperatorNode.cpp
        lib/QoreTrimOperatorNode.cpp
        lib/QoreSquareBracketsOperatorNode.cpp
        lib/QoreValue.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_thread.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_time.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_lib.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_math.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_type.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_env.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_string.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_pwd.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_misc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_list.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_crypto.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_object.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_file.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_compression.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_dbi.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ql_context.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/qc_option.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/qc_errno.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/qc_qore.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Socket.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Program.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ReadOnlyFile.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_File.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Dir.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_GetOpt.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_FtpClient.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AbstractIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AbstractQuantifiedIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AbstractBidirectionalIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AbstractQuantifiedBidirectionalIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ListIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ListReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HashIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HashReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HashKeyIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HashKeyReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HashPairIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HashPairReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HashListIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HashListReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ListHashIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ListHashReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ObjectIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ObjectReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ObjectKeyIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ObjectKeyReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ObjectPairIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ObjectPairReverseIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_FileLineIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_DataLineIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_SingleValueIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_RangeIterator.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_ThreadPool.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AbstractDatasource.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Datasource.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_DatasourcePool.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_SQLStatement.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Queue.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Mutex.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Condition.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_RWLock.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Gate.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Sequence.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_Counter.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_SSLCertificate.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_SSLPrivateKey.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_HTTPClient.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AutoLock.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AutoGate.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AutoReadLock.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AutoWriteLock.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_TermIOS.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AbstractSmartLock.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_TimeZone.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_TreeMap.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/QC_AbstractThreadResource.cpp
        lib/QorePseudoMethods.cpp
        lib/xxhash.cpp
        lib/minitest.cpp
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

add_library(libqore SHARED ${LIBQORE_CPP_SRC} ${LIBQORE_ASM_SRC})
add_dependencies(QPP_GENERATED_FILES qpp)
add_dependencies(libqore QPP_GENERATED_FILES BISON_GENERATED_FILES FLEX_GENERATED_FILES)

target_compile_definitions(libqore PUBLIC _QORE_LIB_INTERN HAVE_UNIX_CONFIG_H)
if (WIN32 AND MINGW AND MSYS)
    target_compile_definitions(libqore PUBLIC BUILDING_DLL)
endif (WIN32 AND MINGW AND MSYS)

target_include_directories(libqore PUBLIC ${Backtrace_INCLUDE_DIRS} ${BZIP2_INCLUDE_DIR} ${ICONV_INCLUDE_DIR} ${MPFR_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${PCRE_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(libqore ${Backtrace_LIBRARIES} ${BZIP2_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${ICONV_LIBRARY} ${LIBQORE_LIBS} ${MPFR_LIBRARIES} ${OPENSSL_LIBRARIES} ${PCRE_LDFLAGS} ${ZLIB_LIBRARIES})
if (WIN32 AND MINGW AND MSYS)
    target_link_libraries(libqore ${WINSOCKET_LIBRARIES})
endif (WIN32 AND MINGW AND MSYS)

set_target_properties(libqore PROPERTIES OUTPUT_NAME qore)
if (APPLE)
    set_target_properties(libqore PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_FULL_LIBDIR})
endif (APPLE)

add_executable(qore ${QORE_CPP_SRC})
target_link_libraries(qore libqore)
add_executable(qr ${QR_CPP_SRC})
target_link_libraries(qr libqore)

set_target_properties(qore qr PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR})

configure_file(${CMAKE_SOURCE_DIR}/cmake/unix-config.h.cmake
               ${CMAKE_BINARY_DIR}/include/qore/intern/unix-config.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/qore-version.h.cmake
               ${CMAKE_BINARY_DIR}/include/qore/qore-version.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/QoreConfigVersion.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/QoreConfigVersion.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/cmake/QoreConfig.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/QoreConfig.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/qore.pc.in
               ${CMAKE_BINARY_DIR}/qore.pc @ONLY)

install(PROGRAMS doxygen/qdx
        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
install(PROGRAMS bin/qget
        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
install(PROGRAMS bin/rest
        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
install(PROGRAMS bin/schema
        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
install(PROGRAMS bin/sfrest
        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
install(PROGRAMS bin/sqlutil
        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
install(TARGETS qore qpp qr libqore
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
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

find_package(Doxygen)
if (DOXYGEN_FOUND)
    # documentation
    configure_file(${CMAKE_SOURCE_DIR}/doxygen/footer_template.html ${CMAKE_BINARY_DIR}/doxygen/footer_template.html @ONLY)
    add_custom_target(docs COMMENT "Generating API documentation")
# TODO/FIXME    add_dependencies(libqore docs)
    # library
    message(STATUS "Doxyfile for library")
    configure_file(${CMAKE_SOURCE_DIR}/doxygen/lib/Doxyfile.in ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.lib @ONLY)
    add_custom_target(docs-lib
        ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.lib
        COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/library/html/*.html
        COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/library/html/search/*.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/docs/library/html
        COMMENT "Generating API documentation with Doxygen for: lib" VERBATIM
    )
# TODO/FIXME    add_dependencies(docs docs-lib)
    # lang
    message(STATUS "Doxyfile for lang")
    configure_file(${CMAKE_SOURCE_DIR}/doxygen/lang/Doxyfile.in ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.lang @ONLY)
    add_custom_target(docs-lang
        ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.lang
        COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/lang/html/*.html
        COMMAND ${QORE_QDX_EXECUTABLE} --post ${CMAKE_BINARY_DIR}/docs/lang/html/search/*.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/docs/lang/html
        COMMENT "Generating API documentation with Doxygen for: lang" VERBATIM
    )
# TODO/FIXME    add_dependencies(docs docs-lang)
endif (DOXYGEN_FOUND)
# end of documentation

# HACK: we need to simulate QORE_USER_MODULES_DIR as the versioned one
#       for qore core qlib modules.
# NOTE: anything below will be installed into versioned dirs!
set(QORE_USER_MODULES_DIR ${USER_MODULE_VER_DIR})
# modules must go after docs target - it’s depending on it
qore_user_modules("${QMOD}")


message(STATUS "CXX ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES}")
message(WARNING "Please do not use cmake for production/real builds. Cmake support is experimental and it does not contant all required setup")
