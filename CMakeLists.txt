cmake_minimum_required(VERSION 2.8.3)

project(qore-language)


include("${CMAKE_SOURCE_DIR}/cmake/QoreMacros.cmake")

include_directories( ${CMAKE_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/include )
include_directories( ${CMAKE_SOURCE_DIR}/include/qore )

# libqore - unused for now
set(CPP_SRC_LIB lib/single-compilation-unit.cpp )

set(QPP_SRC_LIB
	lib/QC_AbstractSmartLock.qpp 
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

# qore interpreter
set(CPP_SRC single-compilation-unit.cpp)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(qoreparser lib/parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
flex_target(qorescanner lib/scanner.lpp ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
add_flex_bison_dependency(qorescanner qoreparser)

# qpp
add_executable(qpp lib/qpp.cpp)
# just fake qpp for macro
set(QORE_QPP_EXECUTABLE ${CMAKE_BINARY_DIR}/qpp)

# libqore generators
qore_wrap_qpp(QPP_CPP_SRC_LIB ${QPP_SRC_LIB})


add_library(libqore SHARED ${QPP_CPP_SRC_LIB}
						   ${CPP_SRC_LIB}
						   ${BISON_qoreparser_OUTPUTS}
						   ${FLEX_qorescanner_OUTPUTS}
		    )
add_dependencies(libqore qpp)

add_executable(qore ${CPP_SRC})
target_link_libraries(qore libqore)


