libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/$(mypkglibdir)
#modverdir = $(libdir)/$(mymodverdir)
modverdir = $(datarootdir)/$(mymodverdir)
includedir = $(prefix)/include/qore

## use automake to create the Makefile.in for configure

AUTOMAKE_OPTIONS = foreign

SUBDIRS = lib

AM_CPPFLAGS = -I$(top_srcdir)/include

# user modules that have dox referenced from other user modules (docs for these must be built first)
DOX_SRC_MODULES = qlib/Mime.qm \
	qlib/MailMessage.qm \
	qlib/Util.qm \
	qlib/HttpServerUtil.qm \
	qlib/HttpServer.qm \
	qlib/WebSocketUtil.qm \
	qlib/SqlUtil.qm \
	qlib/Mapper.qm

DOX_MODULES = qlib/SmtpClient.qm \
	qlib/TelnetClient.qm \
	qlib/Pop3Client.qm \
	qlib/CsvUtil.qm \
	qlib/FixedLengthUtil.qm \
	qlib/OracleSqlUtil.qm \
	qlib/PgsqlSqlUtil.qm \
        qlib/Qorize.qm \
	qlib/MysqlSqlUtil.qm \
	qlib/RestHandler.qm \
	qlib/RestClient.qm \
	qlib/WebSocketHandler.qm \
	qlib/WebSocketClient.qm \
	qlib/WebUtil.qm \
	qlib/TableMapper.qm \
	qlib/Schema.qm \
	qlib/UnitTest.qm \
	qlib/QUnit.qm \
	qlib/Diff.qm \
	qlib/BulkSqlUtil.qm

# TODO: this is ugly... make should look for all qtest files by itself
TESTSCRIPTS= \
	examples/test/qlib/CsvUtil/csvutil.qtest \
	examples/test/qlib/CsvUtil/test.qtest \
	examples/test/qlib/Diff/Diff.qtest \
	examples/test/qlib/FixedLengthUtil/FixedLengthDataIterator.qtest \
	examples/test/qlib/FixedLengthUtil/FixedLengthDataWriter.qtest \
	examples/test/qlib/FixedLengthUtil/FixedLengthFileIterator.qtest \
	examples/test/qlib/FixedLengthUtil/FixedLengthFileWriter.qtest \
	examples/test/qlib/FixedLengthUtil/FixedLengthUtil.qtest \
	examples/test/qlib/FixedLengthUtil/transition_check_iterator.qtest \
	examples/test/qlib/FixedLengthUtil/transition_check_writer.qtest \
	examples/test/qlib/Mapper/mapper.qtest \
	examples/test/qlib/Mime/mime.qtest \
	examples/test/qlib/Qorize/qorizeNamed.qtest \
	examples/test/qlib/Qorize/qorize.qtest \
	examples/test/qlib/Qorize/realWorldCase.qtest \
	examples/test/qlib/SqlUtil/smoke.qtest \
	examples/test/qlib/SSL/ssl.qtest \
	examples/test/qlib/QUnit/inject.qtest \
	examples/test/qlib/QUnit/results.qtest \
	examples/test/qlib/QUnit/tests.qtest \
	examples/test/qlib/Util/same.qtest \
	examples/test/qlib/Util/slice.qtest \
	examples/test/qlib/Util/tmp_location.qtest \
	examples/test/qore/classes/DataLineIterator/DataLineIterator.qtest \
	examples/test/qore/classes/FtpClient/FtpClient.qtest \
	examples/test/qore/classes/Program/lasting-subprogram-in-thread.qtest \
	examples/test/qore/classes/Program/program.qtest \
	examples/test/qore/classes/TreeMap/TreeMap.qtest \
	examples/test/qore/files/create-iso-8859-1-file.qtest \
	examples/test/qore/files/filetypes.qtest \
	examples/test/qore/files/read.qtest \
	examples/test/qore/functions/bzip.qtest \
	examples/test/qore/functions/call_builtin_function.qtest \
	examples/test/qore/functions/ceil.qtest \
	examples/test/qore/functions/crypto.qtest \
	examples/test/qore/functions/digests.qtest \
	examples/test/qore/functions/exists_function.qtest \
	examples/test/qore/functions/floor.qtest \
	examples/test/qore/functions/format_number.qtest \
	examples/test/qore/functions/functiontype.qtest \
	examples/test/qore/functions/gzip.qtest \
	examples/test/qore/functions/hmac.qtest \
	examples/test/qore/functions/html_encode.qtest \
	examples/test/qore/functions/parse_datasource.qtest \
	examples/test/qore/functions/parseurl.qtest \
	examples/test/qore/functions/pwd.qtest \
	examples/test/qore/functions/regex_extract.qtest \
	examples/test/qore/functions/sprintf.qtest \
	examples/test/qore/functions/stat.qtest \
	examples/test/qore/functions/strmul.qtest \
	examples/test/qore/functions/system.qtest \
	examples/test/qore/functions/type.qtest \
	examples/test/qore/functions/xrange.qtest \
	examples/test/qore/misc/access.qtest \
	examples/test/qore/misc/backquote.qtest \
	examples/test/qore/misc/cast.qtest \
	examples/test/qore/misc/classes.qtest \
	examples/test/qore/misc/const-init.qtest \
	examples/test/qore/misc/context.qtest \
	examples/test/qore/misc/curly-brackets.qtest \
	examples/test/qore/misc/exception.qtest \
	examples/test/qore/misc/gc.qtest \
	examples/test/qore/misc/module-loader/injection.qtest \
	examples/test/qore/misc/module-loader/modules.qtest \
	examples/test/qore/misc/module-loader/private-module.qtest \
	examples/test/qore/misc/module-loader/recursive-dependency.qtest \
	examples/test/qore/misc/object.qtest \
	examples/test/qore/misc/octal_const.qtest \
	examples/test/qore/misc/recursive.qtest \
	examples/test/qore/misc/regex.qtest \
	examples/test/qore/misc/rwlock.qtest \
	examples/test/qore/misc/socket.qtest \
	examples/test/qore/misc/thread-parsing.qtest \
	examples/test/qore/operators/hmap.qtest \
	examples/test/qore/operators/logic.qtest \
	examples/test/qore/operators/null_coalescing.qtest \
	examples/test/qore/operators/operators.qtest \
	examples/test/qore/operators/value_coalescing.qtest \
	examples/test/qore/stack/call-stack-overflow-exception.qtest \
	examples/test/qore/stack/exception-location.qtest \
	examples/test/qore/threads/background.qtest \
	examples/test/qore/threads/deadlock.qtest \
	examples/test/qore/threads/max-threads-count.qtest \
	examples/test/qore/threads/thread-object.qtest \
	examples/test/qore/threads/unlocked-thread.qtest \
	examples/test/qore/vars/argv.qtest \
	examples/test/qore/vars/array.qtest \
	examples/test/qore/vars/bare-ref-test.qtest \
	examples/test/qore/vars/closures.qtest \
	examples/test/qore/vars/constant.qtest \
	examples/test/qore/vars/date.qtest \
	examples/test/qore/vars/getencoding.qtest \
	examples/test/qore/vars/hash.qtest \
	examples/test/qore/vars/hash_time.qtest \
	examples/test/qore/vars/numbers.qtest \
	examples/test/qore/vars/overload.qtest \
	examples/test/qore/vars/statement.qtest \
	examples/test/qore/vars/string.qtest \
	examples/test/qore/vars/typecode.qtest

USER_MODULES = $(DOX_SRC_MODULES) \
	$(DOX_MODULES)

USER_MODS = $(basename $(notdir $(USER_MODULES)))

DOX_USER_MODS = $(basename $(notdir $(DOX_MODULES)))

# use native qpp for build when cross compiling
if COND_MINGWCC
QPP_PGM=qpp_native
else
QPP_PGM=qpp
endif
QPP = lib/$(QPP_PGM)

QDX_SRC=doxygen/qdx

# the QORE_MODULE_DIR=./qlib is there because of situation of fresh install
# when the qore is not in system (modules are searched in  eg. /usr/local/lib/qore-modules/0.8.4
# by default - and it does not exist in the fresh install)
if COND_MINGWCC
# when cross-compiling, we just hope that qore is installed also natively, otherwise the documentation can't be built
QDX=QORE_MODULE_DIR=./qlib $(QDX_SRC)
else
QDX=QORE_MODULE_DIR=./qlib ./qr $(QDX_SRC)
endif

LANG_QPP_SRC = lib/QC_AbstractSmartLock.qpp \
	lib/QC_AutoGate.qpp \
	lib/QC_AutoLock.qpp \
	lib/QC_AutoReadLock.qpp \
	lib/QC_AutoWriteLock.qpp \
	lib/QC_Condition.qpp \
	lib/QC_Counter.qpp \
	lib/QC_AbstractIterator.qpp \
	lib/QC_AbstractQuantifiedIterator.qpp \
	lib/QC_AbstractBidirectionalIterator.qpp \
	lib/QC_AbstractQuantifiedBidirectionalIterator.qpp \
	lib/QC_ListIterator.qpp \
	lib/QC_ListReverseIterator.qpp \
	lib/QC_HashIterator.qpp \
	lib/QC_HashReverseIterator.qpp \
	lib/QC_HashKeyIterator.qpp \
	lib/QC_HashKeyReverseIterator.qpp \
	lib/QC_HashPairIterator.qpp \
	lib/QC_HashPairReverseIterator.qpp \
	lib/QC_ObjectIterator.qpp \
	lib/QC_ObjectReverseIterator.qpp \
	lib/QC_ObjectKeyIterator.qpp \
	lib/QC_ObjectKeyReverseIterator.qpp \
	lib/QC_ObjectPairIterator.qpp \
	lib/QC_ObjectPairReverseIterator.qpp \
	lib/QC_HashListIterator.qpp \
	lib/QC_HashListReverseIterator.qpp \
	lib/QC_ListHashIterator.qpp \
	lib/QC_ListHashReverseIterator.qpp \
	lib/QC_DataLineIterator.qpp \
	lib/QC_FileLineIterator.qpp \
	lib/QC_SingleValueIterator.qpp \
	lib/QC_AbstractDatasource.qpp \
	lib/QC_RangeIterator.qpp \
	lib/QC_Datasource.qpp \
	lib/QC_DatasourcePool.qpp \
	lib/QC_Dir.qpp \
	lib/QC_ReadOnlyFile.qpp \
	lib/QC_File.qpp \
	lib/QC_FtpClient.qpp \
	lib/QC_Gate.qpp \
	lib/QC_GetOpt.qpp \
	lib/QC_HTTPClient.qpp \
	lib/QC_Mutex.qpp \
	lib/QC_Program.qpp \
	lib/QC_Queue.qpp \
	lib/QC_RWLock.qpp \
	lib/QC_SQLStatement.qpp \
	lib/QC_Sequence.qpp \
	lib/QC_Socket.qpp \
	lib/QC_TermIOS.qpp \
	lib/QC_TimeZone.qpp \
	lib/QC_SSLCertificate.qpp \
	lib/QC_SSLPrivateKey.qpp \
	lib/QC_ThreadPool.qpp \
	lib/QC_TreeMap.qpp \
	lib/Pseudo_QC_All.qpp \
	lib/Pseudo_QC_Nothing.qpp \
	lib/Pseudo_QC_Date.qpp \
	lib/Pseudo_QC_Object.qpp \
	lib/Pseudo_QC_Hash.qpp \
	lib/Pseudo_QC_String.qpp \
	lib/Pseudo_QC_Binary.qpp \
	lib/Pseudo_QC_List.qpp \
	lib/Pseudo_QC_Bool.qpp \
	lib/Pseudo_QC_Int.qpp \
	lib/Pseudo_QC_Float.qpp \
	lib/Pseudo_QC_Number.qpp \
	lib/Pseudo_QC_Closure.qpp \
	lib/Pseudo_QC_Callref.qpp \
	lib/ql_misc.qpp \
	lib/ql_compression.qpp \
	lib/ql_thread.qpp \
	lib/ql_crypto.qpp \
	lib/ql_lib.qpp \
	lib/ql_file.qpp \
	lib/ql_string.qpp \
	lib/ql_time.qpp \
	lib/ql_math.qpp \
	lib/ql_list.qpp \
	lib/ql_type.qpp \
	lib/ql_pwd.qpp \
	lib/ql_object.qpp \
	lib/ql_env.qpp \
	lib/ql_dbi.qpp \
	lib/ql_context.qpp \
	lib/qc_option.qpp \
	lib/qc_errno.qpp \
	lib/qc_qore.qpp

include_HEADERS = \
	include/qore/QoreValue.h \
	include/qore/ReferenceArgumentHelper.h \
	include/qore/QoreTypeSafeReferenceHelper.h \
	include/qore/qore_thread.h \
	include/qore/AbstractThreadResource.h \
	include/qore/QoreProgram.h \
	include/qore/QoreReferenceCounter.h \
	include/qore/AbstractPrivateData.h \
	include/qore/QoreIteratorBase.h \
	include/qore/QoreType.h \
	include/qore/QoreListNode.h \
	include/qore/QoreValueList.h \
	include/qore/QoreBigIntNode.h \
	include/qore/QoreBoolNode.h \
	include/qore/QoreFloatNode.h \
	include/qore/QoreNumberNode.h \
	include/qore/QoreNothingNode.h \
	include/qore/QoreNullNode.h \
	include/qore/QoreQueue.h \
	include/qore/QoreQueueHelper.h \
	include/qore/params.h \
	include/qore/node_types.h \
	include/qore/BuiltinFunctionList.h \
	include/qore/QoreThreadLock.h \
	include/qore/QoreThreadLocalStorage.h \
	include/qore/QoreClass.h \
	include/qore/QoreString.h \
	include/qore/qore-version.h \
	include/qore/common.h \
	include/qore/QoreEncoding.h \
	include/qore/DateTime.h \
	include/qore/support.h \
	include/qore/ExceptionSink.h \
	include/qore/AbstractQoreNode.h \
	include/qore/QoreStringNode.h \
	include/qore/DateTimeNode.h \
	include/qore/QoreHashNode.h \
	include/qore/ModuleManager.h \
	include/qore/QoreNamespace.h \
	include/qore/QoreObject.h \
	include/qore/macros.h \
	include/qore/macros-i386.h \
	include/qore/macros-x86_64.h \
	include/qore/macros-powerpc.h \
	include/qore/macros-sparc.h \
	include/qore/macros-ia64.h \
	include/qore/QoreSocket.h \
	include/qore/QoreSocketObject.h \
	include/qore/QoreNet.h \
	include/qore/QoreURL.h \
	include/qore/Restrictions.h \
	include/qore/QoreLib.h \
	include/qore/QoreCondition.h \
	include/qore/QoreCounter.h \
	include/qore/BinaryNode.h \
	include/qore/ReferenceNode.h \
	include/qore/CallReferenceNode.h \
	include/qore/QoreFtpClient.h \
	include/qore/safe_dslist \
	include/qore/qlist \
	include/qore/qore_bitopts.h \
	include/qore/QoreSSLBase.h \
	include/qore/Datasource.h \
	include/qore/SQLStatement.h \
	include/qore/SystemEnvironment.h \
	include/qore/ParseOptionMap.h \
	include/qore/QoreHTTPClient.h \
	include/qore/QoreHttpClientObject.h \
	include/qore/QoreRWLock.h \
	include/qore/DBI.h \
	include/qore/ScopeGuard.h \
	include/qore/ReferenceHolder.h \
	include/qore/QoreSSLCertificate.h \
	include/qore/QoreSSLPrivateKey.h \
	include/qore/AutoVLock.h \
	include/qore/QoreFile.h \
	include/qore/Qore.h \
	include/qore/QoreNodeEvalOptionalRefHolder.h \
	include/qore/QoreEvents.h

noinst_HEADERS = \
	include/qore/minitest.hpp \
	include/qore/macros-none.h \
	include/qore/intern/QoreThreadList.h \
	include/qore/intern/QoreHttpClientObjectIntern.h \
	include/qore/intern/xxhash.h \
	include/qore/intern/config.h \
	include/qore/intern/unix-config.h \
	include/qore/intern/win32-config.h \
	include/qore/intern/git-revision.h \
	include/qore/intern/glob.h \
	include/qore/intern/AbstractIteratorHelper.h \
	include/qore/intern/ParseReferenceNode.h \
	include/qore/intern/ReferenceHelper.h \
	include/qore/intern/ThreadResourceList.h \
	include/qore/intern/ssl_constants.h \
	include/qore/intern/qore_thread_intern.h \
	include/qore/intern/qore_list_private.h \
	include/qore/intern/inline_printf.h \
	include/qore/intern/qore_qf_private.h \
	include/qore/intern/VRMutex.h \
	include/qore/intern/Variable.h \
	include/qore/intern/QoreLValue.h \
	include/qore/intern/LocalVar.h \
	include/qore/intern/QoreQueueIntern.h \
	include/qore/intern/ModuleInfo.h \
	include/qore/intern/QoreTimeZoneManager.h \
	include/qore/intern/QoreTreeNode.h \
	include/qore/intern/BarewordNode.h \
	include/qore/intern/StaticClassVarRefNode.h \
	include/qore/intern/SelfVarrefNode.h \
	include/qore/intern/VarRefNode.h \
	include/qore/intern/BackquoteNode.h \
	include/qore/intern/ContextrefNode.h \
	include/qore/intern/ContextRowNode.h \
	include/qore/intern/ComplexContextrefNode.h \
	include/qore/intern/FunctionCallNode.h \
	include/qore/intern/ScopedRefNode.h \
	include/qore/intern/SmartMutex.h \
	include/qore/intern/Sequence.h \
	include/qore/intern/ScopedObjectCallNode.h \
	include/qore/intern/RWLock.h \
	include/qore/intern/SSLSocketHelper.h \
	include/qore/intern/QoreSignal.h \
	include/qore/intern/RegexSubstNode.h \
	include/qore/intern/RegexTransNode.h \
	include/qore/intern/QoreRegexNode.h \
	include/qore/intern/QoreRegexBase.h \
	include/qore/intern/QoreLibIntern.h \
	include/qore/intern/QoreGetOpt.h \
	include/qore/intern/QoreClassList.h \
	include/qore/intern/QException.h \
	include/qore/intern/ParserSupport.h \
	include/qore/intern/Operator.h \
	include/qore/intern/ObjectMethodReferenceNode.h \
	include/qore/intern/NamedScope.h \
	include/qore/intern/QoreNamespaceIntern.h \
	include/qore/intern/QoreNamespaceList.h \
	include/qore/intern/ManagedDatasource.h \
	include/qore/intern/qore_ds_private.h \
	include/qore/intern/qore_dbi_private.h \
	include/qore/intern/QoreSQLStatement.h \
	include/qore/intern/FunctionList.h \
	include/qore/intern/GlobalVariableList.h \
	include/qore/intern/DatasourcePool.h \
	include/qore/intern/Function.h \
	include/qore/intern/BuiltinFunction.h \
	include/qore/intern/FindNode.h \
	include/qore/intern/ExecArgList.h \
	include/qore/intern/Context.h \
	include/qore/intern/ConstantList.h \
	include/qore/intern/CaseNodeWithOperator.h \
	include/qore/intern/CaseNodeRegex.h \
	include/qore/intern/ClassRefNode.h \
	include/qore/intern/AbstractStatement.h \
	include/qore/intern/OnBlockExitStatement.h \
	include/qore/intern/RethrowStatement.h \
	include/qore/intern/ThreadExitStatement.h \
	include/qore/intern/BreakStatement.h \
	include/qore/intern/ContinueStatement.h \
	include/qore/intern/ReturnStatement.h \
	include/qore/intern/ExpressionStatement.h \
	include/qore/intern/StatementBlock.h \
	include/qore/intern/ContextStatement.h \
	include/qore/intern/SummarizeStatement.h \
	include/qore/intern/IfStatement.h \
	include/qore/intern/WhileStatement.h \
	include/qore/intern/DoWhileStatement.h \
	include/qore/intern/ForStatement.h \
	include/qore/intern/ForEachStatement.h \
	include/qore/intern/TryStatement.h \
	include/qore/intern/ThrowStatement.h \
	include/qore/intern/SwitchStatement.h \
	include/qore/intern/QoreClassIntern.h \
	include/qore/intern/QoreException.h \
	include/qore/intern/AbstractSmartLock.h \
	include/qore/intern/VLock.h \
	include/qore/intern/CallReferenceNode.h \
	include/qore/intern/CallReferenceCallNode.h \
	include/qore/intern/DatasourceStatementHelper.h \
	include/qore/intern/ParseNode.h \
	include/qore/intern/QoreClosureNode.h \
	include/qore/intern/QoreClosureParseNode.h \
	include/qore/intern/QoreParseHashNode.h \
	include/qore/intern/QoreImplicitArgumentNode.h \
	include/qore/intern/QoreImplicitElementNode.h \
	include/qore/intern/qore_string_private.h \
	include/qore/intern/qore_number_private.h \
	include/qore/intern/qore_date_private.h \
	include/qore/intern/sql_statement_private.h \
	include/qore/intern/qore_program_private.h \
	include/qore/intern/qore_socket_private.h \
	include/qore/intern/QoreDir.h \
	include/qore/intern/QoreTypeInfo.h \
	include/qore/intern/QoreSSLIntern.h \
	include/qore/intern/QoreObjectIntern.h \
	include/qore/intern/QoreHashNodeIntern.h \
	include/qore/intern/QoreOperatorNode.h \
	include/qore/intern/QoreDeleteOperatorNode.h \
	include/qore/intern/QoreRemoveOperatorNode.h \
	include/qore/intern/QoreSpliceOperatorNode.h \
	include/qore/intern/QoreExtractOperatorNode.h \
	include/qore/intern/QoreCastOperatorNode.h \
	include/qore/intern/QoreUnaryMinusOperatorNode.h \
	include/qore/intern/QoreLogicalNotOperatorNode.h \
	include/qore/intern/QoreDotEvalOperatorNode.h \
	include/qore/intern/QoreLogicalEqualsOperatorNode.h \
	include/qore/intern/QoreLogicalNotEqualsOperatorNode.h \
	include/qore/intern/QoreModulaOperatorNode.h \
	include/qore/intern/QoreAssignmentOperatorNode.h \
	include/qore/intern/QoreIntAssignmentOperatorNode.h \
	include/qore/intern/QorePlusEqualsOperatorNode.h \
	include/qore/intern/QoreIntPlusEqualsOperatorNode.h \
	include/qore/intern/QoreMinusEqualsOperatorNode.h \
	include/qore/intern/QoreIntMinusEqualsOperatorNode.h \
	include/qore/intern/QoreOrEqualsOperatorNode.h \
	include/qore/intern/QoreAndEqualsOperatorNode.h \
	include/qore/intern/QoreModulaEqualsOperatorNode.h \
	include/qore/intern/QoreMultiplyEqualsOperatorNode.h \
	include/qore/intern/QoreDivideEqualsOperatorNode.h \
	include/qore/intern/QoreXorEqualsOperatorNode.h \
	include/qore/intern/QoreShiftLeftEqualsOperatorNode.h \
	include/qore/intern/QoreShiftRightEqualsOperatorNode.h \
	include/qore/intern/QorePostIncrementOperatorNode.h \
	include/qore/intern/QoreIntPostIncrementOperatorNode.h \
	include/qore/intern/QorePostDecrementOperatorNode.h \
	include/qore/intern/QoreIntPostDecrementOperatorNode.h \
	include/qore/intern/QorePreIncrementOperatorNode.h \
	include/qore/intern/QoreIntPreIncrementOperatorNode.h \
	include/qore/intern/QorePreDecrementOperatorNode.h \
	include/qore/intern/QoreIntPreDecrementOperatorNode.h \
	include/qore/intern/QoreLogicalLessThanOperatorNode.h \
	include/qore/intern/QoreLogicalGreaterThanOperatorNode.h \
	include/qore/intern/QoreLogicalLessThanOrEqualsOperatorNode.h \
	include/qore/intern/QoreLogicalGreaterThanOrEqualsOperatorNode.h \
	include/qore/intern/QoreDivisionOperatorNode.h \
	include/qore/intern/QoreBinaryLValueOperatorNode.h \
	include/qore/intern/QoreQuestionMarkOperatorNode.h \
	include/qore/intern/QoreNullCoalescingOperatorNode.h \
	include/qore/intern/QoreValueCoalescingOperatorNode.h \
	include/qore/intern/QoreMapOperatorNode.h \
	include/qore/intern/QoreMapSelectOperatorNode.h \
	include/qore/intern/QoreHashMapOperatorNode.h \
	include/qore/intern/QoreHashMapSelectOperatorNode.h \
	include/qore/intern/QorePseudoMethods.h \
	include/qore/intern/QoreHashIterator.h \
	include/qore/intern/QoreListIterator.h \
	include/qore/intern/QoreObjectIterator.h \
	include/qore/intern/QoreHashListIterator.h \
	include/qore/intern/QoreListHashIterator.h \
	include/qore/intern/SingleValueIterator.h \
	include/qore/intern/RangeIterator.h \
	include/qore/intern/ThreadPool.h \
	include/qore/intern/qore_var_rwlock_priv.h \
	include/qore/intern/qore_qd_private.h \
	include/qore/intern/ql_string.h \
	include/qore/intern/ql_list.h \
	include/qore/intern/ql_misc.h \
	include/qore/intern/ql_math.h \
	include/qore/intern/ql_pwd.h \
	include/qore/intern/ql_env.h \
	include/qore/intern/ql_type.h \
	include/qore/intern/ql_lib.h \
	include/qore/intern/ql_time.h \
	include/qore/intern/ql_debug.h \
	include/qore/intern/ql_thread.h \
	include/qore/intern/ql_crypto.h \
	include/qore/intern/ql_file.h \
	include/qore/intern/ql_object.h \
	include/qore/intern/ql_compression.h \
	include/qore/intern/QC_TermIOS.h \
	include/qore/intern/QC_Queue.h \
	include/qore/intern/QC_Socket.h \
	include/qore/intern/QC_Sequence.h \
	include/qore/intern/QC_RWLock.h \
	include/qore/intern/QC_Program.h \
	include/qore/intern/QC_Mutex.h \
	include/qore/intern/QC_Condition.h \
	include/qore/intern/QC_Gate.h \
	include/qore/intern/QC_File.h \
	include/qore/intern/QC_Dir.h \
	include/qore/intern/QC_Counter.h \
	include/qore/intern/QC_Datasource.h \
	include/qore/intern/QC_DatasourcePool.h \
	include/qore/intern/QC_SQLStatement.h \
	include/qore/intern/QC_GetOpt.h \
	include/qore/intern/QC_FtpClient.h \
	include/qore/intern/QC_SSLCertificate.h \
	include/qore/intern/QC_SSLPrivateKey.h \
	include/qore/intern/QC_HTTPClient.h \
	include/qore/intern/QC_AutoGate.h \
	include/qore/intern/QC_AutoLock.h \
	include/qore/intern/QC_AutoReadLock.h \
	include/qore/intern/QC_AutoWriteLock.h \
	include/qore/intern/QC_AbstractSmartLock.h \
	include/qore/intern/QC_TimeZone.h \
	include/qore/intern/QC_TreeMap.h \
	lib/getopt_long.h \
	command-line.h

LIB_DOXYFILES_FIXED = \
	doxygen/lib/mainpage.doxygen \
	doxygen/lib/10_embedding.doxygen \
	doxygen/lib/20_qore_data.doxygen \
	doxygen/lib/30_qore_builtin_functions.doxygen \
	doxygen/lib/40_class_implementation.doxygen \
	doxygen/lib/70_qore_modules.doxygen \
	doxygen/lib/75_dbi_drivers.doxygen \
	doxygen/lib/90_cmake.doxygen

LIB_DOXYFILES_SRC = \
	$(LIB_DOXYFILES_FIXED) \
	doxygen/lib/Doxyfile.tmpl

LANG_TMP_DOXYFILES_SRC = doxygen/lang/mainpage.dox \
	doxygen/lang/000_intro.dox \
	doxygen/lang/105_desc_overview.dox \
	doxygen/lang/110_environment_variables.dox \
	doxygen/lang/115_conditional_parsing.dox \
	doxygen/lang/120_modules.dox \
	doxygen/lang/125_include_files.dox \
	doxygen/lang/130_identifiers.dox \
	doxygen/lang/135_comments.dox \
	doxygen/lang/140_variables.dox \
	doxygen/lang/145_basic_data_types.dox \
	doxygen/lang/150_container_data_types.dox \
	doxygen/lang/152_code_data_types.dox \
	doxygen/lang/155_data_type_declarations.dox \
	doxygen/lang/157_lvalue_references.dox \
	doxygen/lang/160_overloading.dox \
	doxygen/lang/165_time_zones.dox \
	doxygen/lang/170_character_encoding.dox \
	doxygen/lang/175_expressions.dox \
	doxygen/lang/180_operators.dox \
	doxygen/lang/185_qore_regex.dox \
	doxygen/lang/190_date_time_arithmetic.dox \
	doxygen/lang/195_statements.dox \
	doxygen/lang/200_functions.dox \
	doxygen/lang/202_code_flags.dox \
	doxygen/lang/205_namespaces.dox \
	doxygen/lang/210_constants.dox \
	doxygen/lang/215_classes.dox \
	doxygen/lang/220_threading.dox \
	doxygen/lang/225_exception_handling.dox \
	doxygen/lang/230_signal_handling.dox \
	doxygen/lang/235_event_handling.dox \
	doxygen/lang/240_command_line_processing.dox \
	doxygen/lang/245_parse_directives.dox \
	doxygen/lang/250_warnings.dox \
	doxygen/lang/255_keywords.dox \
	doxygen/lang/900_release_notes.dox

LANG_TMP_DOXYFILES_TMPL = $(addsuffix .tmpl,$(LANG_TMP_DOXYFILES_SRC))

LANG_DOXYFILES_SRC = \
	doxygen/dot \
	doxygen/dpp \
	doxygen/footer_template.html \
	doxygen/pdox \
	doxygen/qdx \
	doxygen/qlib/Doxyfile.tmpl \
	doxygen/lib/Doxyfile.tmpl \
	doxygen/lang/Doxyfile.tmpl \
	$(LANG_TMP_DOXYFILES_TMPL)

EXTRA_DIST = next_build.sh runtest.sh WHATISQORE RELEASE-NOTES \
	AUTHORS BUILDING \
	README README-LICENSE README-MODULES \
	COPYING.LGPL COPYING.GPL COPYING.MIT \
	$(TESTSCRIPTS) \
	examples/test/qore/misc/module-loader/A.qm \
	examples/test/qore/misc/module-loader/B.qm \
	examples/test/qore/misc/module-loader/C.qm \
	examples/test/qore/misc/module-loader/R1.qm \
	examples/test/qore/misc/module-loader/R2.qm \
	examples/test/qore/misc/module-loader/MyTestModule.qm \
	examples/test/qore/misc/module-loader/MyTestModule2.qm \
	examples/HelloWorld.q \
	examples/clisrv.q \
	examples/email.q \
	examples/exp.q \
	examples/getch.q \
	examples/getopt.q \
	examples/hash.q \
	examples/httpserver.q \
	examples/inherit.q \
	examples/inherit2.q \
	examples/obj2.q \
	examples/old2new \
	examples/pop3.q \
	examples/qget \
	examples/rest \
	examples/restserver.q \
	examples/schema \
	examples/sqlutil \
	examples/stmt.q \
	examples/telnet.q \
	qore.spec \
	qore.spec-fedora \
	qore.spec-multi \
	cmake/QoreMacros.cmake \
	$(LIB_DOXYFILES_SRC) \
	$(LANG_DOXYFILES_SRC) \
	$(LANG_QPP_SRC)

dist_modver_DATA = $(USER_MODULES)

TESTS = $(TESTSCRIPTS)
AM_TESTS_ENVIRONMENT = QORE_MODULE_DIR='$(srcdir)/qlib'; export QORE_MODULE_DIR;
LOG_COMPILER = ./runtest.sh

BUILT_SOURCES = dummy

dummy:
	$(top_srcdir)/next_build.sh

if COND_DOXYGEN
MOD_OUTPUT = $(addprefix docs/modules/,$(addsuffix /html,$(USER_MODS)))
DOXYGEN_OUTPUT = docs/library/html docs/lang/html $(MOD_OUTPUT)

LIB_TMP_DOXYFILES_SRC = \
	doxygen/lib/Doxyfile

LIB_TMP_DOXYFILES = $(LIB_TMP_DOXYFILES_SRC)

LIB_DOXYFILES = doxygen/lib/Doxyfile $(LIB_TMP_DOXYFILES_SRC) $(LIB_DOXYFILES_FIXED)

LANG_TMP_DOXYFILES = $(LANG_TMP_DOXYFILES_SRC) \
	doxygen/lang/Doxyfile \
	qore.tag

LANG_DOXYFILES = $(LANG_TMP_DOXYFILES_SRC) \
	doxygen/lang/Doxyfile

$(QPP): dummy
	cd lib; make $(QPP_PGM)

doxygen/lib/Doxyfile: doxygen/lib/Doxyfile.tmpl configure.ac
	doxygen/dpp "$(VERSION)" $< "$(top_srcdir)/doxygen/" > $@

lang-dox-src: dummy
	cd lib; make dox-src

doxygen/lang/Doxyfile: doxygen/lang/Doxyfile.tmpl configure.ac
	doxygen/dpp "$(VERSION)" $< "$(top_srcdir)/doxygen/" > $@

$(LANG_TMP_DOXYFILES_SRC): doxygen/lang/%.dox: doxygen/lang/%.dox.tmpl lib $(QPP) lang-dox-src
	$(QPP) -t $< -o $@

lib/libqore.la: dummy
	cd lib; make

if COND_MINGWCC
else
$(QDX): qr
endif

QDX_MTAG_ARGS = $(foreach m,$(basename $(notdir $(DOX_SRC_MODULES))),-t$(m).tag=../../$(m)/html)

# explicit Doxyfile rules for modules providing tags for other modules
doxygen/qlib/Doxyfile.Mime: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/Mime.qm:doxygen/qlib/Mime.qm.dox.h $< $@

doxygen/qlib/Doxyfile.Util: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/Util.qm:doxygen/qlib/Util.qm.dox.h $< $@

doxygen/qlib/Doxyfile.HttpServerUtil: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/HttpServerUtil.qm:doxygen/qlib/HttpServerUtil.qm.dox.h -tMime.tag=../../Mime/html -tUtil.tag=../../Util/html $< $@

doxygen/qlib/Doxyfile.HttpServer: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/HttpServer.qm:doxygen/qlib/HttpServer.qm.dox.h -tHttpServerUtil.tag=../../HttpServerUtil/html -tMime.tag=../../Mime/html -tUtil.tag=../../Util/html $< $@

doxygen/qlib/Doxyfile.MailMessage: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/MailMessage.qm:doxygen/qlib/MailMessage.qm.dox.h -tMime.tag=../../Mime/html $< $@

doxygen/qlib/Doxyfile.SqlUtil: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/SqlUtil.qm:doxygen/qlib/SqlUtil.qm.dox.h -tUtil.tag=../../Util/html $< $@

doxygen/qlib/Doxyfile.WebSocketUtil: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/WebSocketUtil.qm:doxygen/qlib/WebSocketUtil.qm.dox.h $< $@

doxygen/qlib/Doxyfile.Mapper: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/Mapper.qm:doxygen/qlib/Mapper.qm.dox.h $< $@

DOXYFILE_TARG = $(addprefix doxygen/qlib/Doxyfile.,$(DOX_USER_MODS))
$(DOXYFILE_TARG): doxygen/qlib/Doxyfile.%: doxygen/qlib/Doxyfile.tmpl $(QDX_SRC)
	$(QDX) -M=qlib/$*.qm:doxygen/qlib/$*.qm.dox.h $(QDX_MTAG_ARGS) $< $@

TMP_MOD_TARG = $(addprefix doxygen/qlib/,$(addsuffix .qm.dox.h,$(USER_MODS)))
$(TMP_MOD_TARG): doxygen/qlib/%.qm.dox.h: qlib/%.qm $(QDX_SRC)
	$(QDX) $< $@

QLIB_TMP_DOXYFILES = $(TMP_MOD_TARG) $(addsuffix .tag,$(USER_MODS))

DX_CLEANFILES = $(LIB_TMP_DOXYFILES) $(LANG_TMP_DOXYFILES) $(QLIB_TMP_DOXYFILES)

docs/library/html: $(include_HEADERS) $(LIB_DOXYFILES)
	$(MKDIR_P) $@
	$(DOXYGEN_CMD) doxygen/lib/Doxyfile

docs/lang/html: $(LANG_DOXYFILES) $(LANG_QPP_SRC)
	$(MKDIR_P) $@
	$(DOXYGEN_CMD) doxygen/lang/Doxyfile
	$(top_srcdir)/doxygen/pdox docs/lang/html docs/lang/html/search
	$(top_srcdir)/doxygen/pdox -t docs/lang/latex

TMP_MOD_HTML_TARG = $(addsuffix /html,$(addprefix docs/modules/,$(USER_MODS)))
$(TMP_MOD_HTML_TARG): docs/modules/%/html: doxygen/qlib/%.qm.dox.h doxygen/qlib/Doxyfile.% #docs/lang/html
	$(MKDIR_P) $@
	$(DOXYGEN_CMD) doxygen/qlib/Doxyfile.$*
	$(top_srcdir)/doxygen/pdox $@ $@/search

doxygen-doc: $(DOXYGEN_OUTPUT)

EXTRA_DIST += $(DOXYGEN_OUTPUT)

html-local: $(DOXYGEN_OUTPUT)

install-html-local:
	$(MKDIR_P) $(DESTDIR)$(datadir)/$(PACKAGE)/docs/lang/html/search
	$(MKDIR_P) $(DESTDIR)$(datadir)/$(PACKAGE)/docs/library/html/search
	for mod in $(USER_MODS); do $(MKDIR_P) $(DESTDIR)$(datadir)/$(PACKAGE)/docs/modules/$$mod/html/search; done
	for dir in $(DOXYGEN_OUTPUT); do \
	   for f in $$dir/*; do if [ -f $$f ]; then $(INSTALL_DATA) $$f $(DESTDIR)$(datadir)/$(PACKAGE)/$$dir; fi; done; \
	   for f in $$dir/search/*; do $(INSTALL_DATA) $$f $(DESTDIR)$(datadir)/$(PACKAGE)/$$dir/search; done; \
	done

clean-local:
	$(RM) -rf $(DOXYGEN_OUTPUT)

uninstall-local:
	$(RM) -rf $(DESTDIR)$(datadir)/$(PACKAGE)/docs

else
doxygen-doc:
	echo no doxygen support
endif

MOSTLYCLEANFILES = $(DX_CLEANFILES)

dist_man_MANS = qore.1

#$(dist_man_MANS): doxygen-doc

qore_LDADD = lib/libqore.la
qr_LDADD = lib/libqore.la

ACLOCAL_AMFLAGS=-I m4

AM_CXXFLAGS = ${PTHREAD_CFLAGS}

if COND_PROFILE
AM_CXXFLAGS += -g -pg
qore_LDFLAGS=-g -pg
qr_LDFLAGS=-g -pg
else
if COND_DEBUG
AM_CXXFLAGS += -g
qore_LDFLAGS=-g
qr_LDFLAGS=-g
else
##qore_LDFLAGS=-s
endif
endif

bin_PROGRAMS = qore qr
dist_bin_SCRIPTS = doxygen/qdx
if COND_SINGLE_COMPILATION_UNIT
qore_SOURCES = qore-single-compilation-unit.cpp
qr_SOURCES = qr-single-compilation-unit.cpp
else
qore_SOURCES = \
	qore-main.cpp \
	command-line.cpp
qr_SOURCES = \
	qr-main.cpp \
	command-line.cpp
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = qore.pc

# cmake support
cmakedir = $(prefix)/lib${LIBSUFFIX}/cmake/Qore
cmake_DATA = cmake/QoreConfig.cmake cmake/QoreConfigVersion.cmake cmake/QoreMacros.cmake

BUILD=$(shell $(top_srcdir)/next_build.sh -b)
PACKAGE_VERSION=$(shell grep PACKAGE_VERSION include/qore/intern/unix-config.h|cut -f3 -d\ |sed s/\"//g)
TAR_NAME := arch/qore-v$(PACKAGE_VERSION)-$(BUILD)-$(shell date +%Y%m%d).tar

tar: clean
	find $(top_srcdir) -name \*~ -exec rm {} \;
	cd ..; tar cvf $(TAR_NAME) src && gzip $(TAR_NAME)

tests:
	@for i in $(TESTSCRIPTS); do qore $$i --format=plain; done

tests-ci:
	@for i in $(TESTSCRIPTS); do qore $$i --format=junit --verbose; done
