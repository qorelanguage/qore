libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/$(mypkglibdir)
includedir = $(prefix)/include/qore

## use automake to create the Makefile.in for configure

AUTOMAKE_OPTIONS = foreign

SUBDIRS = lib

INCLUDES = -I$(top_srcdir)/include

LANG_QPP_SRC = lib/QC_AbstractSmartLock.qpp \
	lib/QC_AutoGate.qpp \
	lib/QC_AutoLock.qpp \
	lib/QC_AutoReadLock.qpp \
	lib/QC_AutoWriteLock.qpp \
	lib/QC_Condition.qpp \
	lib/QC_Counter.qpp \
	lib/QC_Datasource.qpp \
	lib/QC_DatasourcePool.qpp \
	lib/QC_Dir.qpp \
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
	lib/Pseudo_QC_All.qpp \
	lib/Pseudo_QC_Date.qpp \
	lib/Pseudo_QC_Object.qpp \
	lib/Pseudo_QC_Hash.qpp \
	lib/Pseudo_QC_String.qpp \
	lib/Pseudo_QC_Binary.qpp \
	lib/Pseudo_QC_List.qpp \
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

LIB_DOXYFILES = \
	doxygen/lib/Doxyfile \
	doxygen/lib/mainpage.doxygen \
	doxygen/lib/10_embedding.doxygen \
	doxygen/lib/20_qore_data.doxygen \
	doxygen/lib/30_qore_builtin_functions.doxygen \
	doxygen/lib/40_class_implementation.doxygen \
	doxygen/lib/70_qore_modules.doxygen \
	doxygen/lib/75_dbi_drivers.doxygen

LANG_DOXYFILES_SRC = \
	doxygen/lang/Doxyfile.tmpl \
	doxygen/lang/mainpage.dox.tmpl \
	doxygen/lang/000_intro.dox.tmpl \
	doxygen/lang/105_desc_overview.dox.tmpl \
	doxygen/lang/110_environment_variables.dox.tmpl \
	doxygen/lang/115_conditional_parsing.dox.tmpl \
	doxygen/lang/120_modules.dox.tmpl \
	doxygen/lang/125_include_files.dox.tmpl \
	doxygen/lang/130_identifiers.dox.tmpl \
	doxygen/lang/135_comments.dox.tmpl \
	doxygen/lang/140_variables.dox.tmpl \
	doxygen/lang/145_basic_data_types.dox.tmpl \
	doxygen/lang/150_container_data_types.dox.tmpl \
	doxygen/lang/155_data_type_declarations.dox.tmpl \
	doxygen/lang/160_overloading.dox.tmpl \
	doxygen/lang/165_time_zones.dox.tmpl \
	doxygen/lang/170_character_encoding.dox.tmpl \
	doxygen/lang/175_expressions.dox.tmpl \
	doxygen/lang/180_operators.dox.tmpl \
	doxygen/lang/185_qore_regex.dox.tmpl \
	doxygen/lang/190_date_time_arithmetic.dox.tmpl \
	doxygen/lang/195_statements.dox.tmpl \
	doxygen/lang/200_functions.dox.tmpl \
	doxygen/lang/202_code_flags.dox.tmpl \
	doxygen/lang/205_namespaces.dox.tmpl \
	doxygen/lang/210_constants.dox.tmpl \
	doxygen/lang/215_classes.dox.tmpl \
	doxygen/lang/220_threading.dox.tmpl \
	doxygen/lang/225_exception_handling.dox.tmpl \
	doxygen/lang/230_signal_handling.dox.tmpl \
	doxygen/lang/235_event_handling.dox.tmpl

LANG_TMP_DOXYFILES = doxygen/lang/Doxyfile \
	doxygen/lang/mainpage.dox \
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
	doxygen/lang/155_data_type_declarations.dox \
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
	doxygen/lang/235_event_handling.dox

LANG_DOXYFILES = doxygen/lang/Doxyfile $(LANG_TMP_DOXYFILES)

doxygen/lang/Doxyfile: doxygen/lang/Doxyfile.tmpl
	doxygen/dpp $< "$(top_srcdir)/doxygen/" > $@

doxygen/lang/mainpage.dox: doxygen/lang/mainpage.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/000_intro.dox: doxygen/lang/000_intro.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/105_desc_overview.dox: doxygen/lang/105_desc_overview.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/110_environment_variables.dox: doxygen/lang/110_environment_variables.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/115_conditional_parsing.dox: doxygen/lang/115_conditional_parsing.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/120_modules.dox: doxygen/lang/120_modules.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/125_include_files.dox: doxygen/lang/125_include_files.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/130_identifiers.dox: doxygen/lang/130_identifiers.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/135_comments.dox: doxygen/lang/135_comments.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/140_variables.dox: doxygen/lang/140_variables.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/145_basic_data_types.dox: doxygen/lang/145_basic_data_types.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/150_container_data_types.dox: doxygen/lang/150_container_data_types.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/155_data_type_declarations.dox: doxygen/lang/155_data_type_declarations.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/160_overloading.dox: doxygen/lang/160_overloading.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/165_time_zones.dox: doxygen/lang/165_time_zones.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/170_character_encoding.dox: doxygen/lang/170_character_encoding.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/175_expressions.dox: doxygen/lang/175_expressions.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/180_operators.dox: doxygen/lang/180_operators.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/185_qore_regex.dox: doxygen/lang/185_qore_regex.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/190_date_time_arithmetic.dox: doxygen/lang/190_date_time_arithmetic.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/195_statements.dox: doxygen/lang/195_statements.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/200_functions.dox: doxygen/lang/200_functions.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/202_code_flags.dox: doxygen/lang/202_code_flags.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/205_namespaces.dox: doxygen/lang/205_namespaces.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/210_constants.dox: doxygen/lang/210_constants.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/215_classes.dox: doxygen/lang/215_classes.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/220_threading.dox: doxygen/lang/220_threading.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/225_exception_handling.dox: doxygen/lang/225_exception_handling.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/230_signal_handling.dox: doxygen/lang/230_signal_handling.dox.tmpl lib
	lib/qpp -t $< -o $@

doxygen/lang/235_event_handling.dox: doxygen/lang/235_event_handling.dox.tmpl lib
	lib/qpp -t $< -o $@

include_HEADERS = \
	include/qore/ReferenceArgumentHelper.h \
	include/qore/QoreTypeSafeReferenceHelper.h \
	include/qore/qore_thread.h \
	include/qore/AbstractThreadResource.h \
	include/qore/QoreProgram.h \
	include/qore/QoreReferenceCounter.h \
	include/qore/AbstractPrivateData.h \
	include/qore/QoreType.h \
	include/qore/QoreListNode.h \
	include/qore/QoreBigIntNode.h \
	include/qore/QoreBoolNode.h \
	include/qore/QoreFloatNode.h \
	include/qore/QoreNothingNode.h \
	include/qore/QoreNullNode.h \
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
	include/qore/QoreNodeEvalOptionalRefHolder.h \
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
	include/qore/qore_bitopts.h \
	include/qore/QoreSSLBase.h \
	include/qore/Datasource.h \
	include/qore/SQLStatement.h \
	include/qore/SystemEnvironment.h \
	include/qore/ParseOptionMap.h \
	include/qore/QoreHTTPClient.h \
	include/qore/QoreRWLock.h \
	include/qore/DBI.h \
	include/qore/ScopeGuard.h \
	include/qore/ReferenceHolder.h \
	include/qore/QoreSSLCertificate.h \
	include/qore/QoreSSLPrivateKey.h \
	include/qore/AutoVLock.h \
	include/qore/QoreFile.h \
	include/qore/Qore.h \
	include/qore/QoreEvents.h

noinst_HEADERS = \
	include/qore/minitest.hpp \
	include/qore/macros-none.h \
	include/qore/intern/config.h \
	include/qore/intern/unix-config.h \
	include/qore/intern/win32-config.h \
	include/qore/intern/svn-revision.h \
	include/qore/intern/glob.h \
	include/qore/intern/ReferenceHelper.h \
	include/qore/intern/ThreadResourceList.h \
	include/qore/intern/ssl_constants.h \
	include/qore/intern/qore_thread_intern.h \
	include/qore/intern/inline_printf.h \
	include/qore/intern/VRMutex.h \
	include/qore/intern/Variable.h \
	include/qore/intern/LocalVar.h \
	include/qore/intern/ModuleInfo.h \
	include/qore/intern/QoreTimeZoneManager.h \
	include/qore/intern/UserFunctionList.h \
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
	include/qore/intern/QoreQueue.h \
	include/qore/intern/QoreNumber.h \
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
	include/qore/intern/QoreSQLStatement.h \
	include/qore/intern/ImportedFunctionList.h \
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
	include/qore/intern/CallStack.h \
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
	include/qore/intern/AutoNamespaceList.h \
	include/qore/intern/CallReferenceNode.h \
	include/qore/intern/CallReferenceCallNode.h \
	include/qore/intern/DatasourceStatementHelper.h \
	include/qore/intern/ParseNode.h \
	include/qore/intern/QoreClosureNode.h \
	include/qore/intern/QoreClosureParseNode.h \
	include/qore/intern/QoreImplicitArgumentNode.h \
	include/qore/intern/QoreImplicitElementNode.h \
	include/qore/intern/qore_string_private.h \
	include/qore/intern/qore_date_private.h \
	include/qore/intern/sql_statement_private.h \
	include/qore/intern/qore_program_private.h \
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
	include/qore/intern/QoreDotEvalOperatorNode.h \
	include/qore/intern/QoreLogicalEqualsOperatorNode.h \
	include/qore/intern/QoreLogicalNotEqualsOperatorNode.h \
	include/qore/intern/QoreAssignmentOperatorNode.h \
	include/qore/intern/QoreIntAssignmentOperatorNode.h \
	include/qore/intern/QorePlusEqualsOperatorNode.h \
	include/qore/intern/QoreIntPlusEqualsOperatorNode.h \
	include/qore/intern/QoreMinusEqualsOperatorNode.h \
	include/qore/intern/QoreIntMinusEqualsOperatorNode.h \
	include/qore/intern/QoreOrEqualsOperatorNode.h \
	include/qore/intern/QoreIntOrEqualsOperatorNode.h \
	include/qore/intern/QoreAndEqualsOperatorNode.h \
	include/qore/intern/QoreIntAndEqualsOperatorNode.h \
	include/qore/intern/QoreModulaEqualsOperatorNode.h \
	include/qore/intern/QoreIntModulaEqualsOperatorNode.h \
	include/qore/intern/QoreMultiplyEqualsOperatorNode.h \
	include/qore/intern/QoreIntMultiplyEqualsOperatorNode.h \
	include/qore/intern/QoreDivideEqualsOperatorNode.h \
	include/qore/intern/QoreIntDivideEqualsOperatorNode.h \
	include/qore/intern/QoreXorEqualsOperatorNode.h \
	include/qore/intern/QoreIntXorEqualsOperatorNode.h \
	include/qore/intern/QoreShiftLeftEqualsOperatorNode.h \
	include/qore/intern/QoreIntShiftLeftEqualsOperatorNode.h \
	include/qore/intern/QoreShiftRightEqualsOperatorNode.h \
	include/qore/intern/QoreIntShiftRightEqualsOperatorNode.h \
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
	include/qore/intern/QorePseudoMethods.h \
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
	command-line.h

EXTRA_DIST = next_build.sh WHATISQORE RELEASE-NOTES \
	ChangeLog AUTHORS BUILD \
	README README-LICENSE README-MODULES \
	COPYING.LGPL COPYING.GPL \
        test/thread-object-test.q \
        test/create-iso-8859-1-file.q \
        test/empty.q \
        test/file-test.q \
        test/ftp-test.q \
        test/max-threads-test.q \
        test/stack-test.q \
        test/object-test.q \
        test/rwlock-test.q \
        test/socket-test.q \
        test/subprogram-thread-test.q \
        test/system-test.q \
        test/test-exception.q \
        test/test.q \
        test/Test.qc \
        test/unlocked-thread-test.q \
	test/regex.q \
	test/deadlock.q \
	test/optest.q \
	examples/HttpServer.qc \
	examples/stmt.q \
	examples/exp.q \
	examples/obj2.q \
	examples/hash.q \
	examples/clisrv.q \
	examples/inherit.q \
	examples/inherit2.q \
	examples/getopt.q \
	examples/getch.q \
	docs/*.html \
	docs/qore-style.css \
	docs/img/acrobat_a.gif \
	docs/img/acrobat.gif \
	docs/img/disk.gif \
	docs/img/qbullet-note_a.gif \
	docs/img/qbullet-note.gif \
	docs/img/remote_a.gif \
	docs/img/remote.gif \
	docs/img/scrollup_a.gif \
	docs/img/scrollup.gif \
	docs/img/word.gif \
	docs/library/html \
	qore.spec \
	$(LIB_DOXYFILES) \
	$(LANG_DOXYFILES_SRC) \
	$(LANG_QPP_SRC)

BUILT_SOURCES = dummy

dummy:
	./next_build.sh

if COND_DOXYGEN
DX_CLEANFILES = docs/library $(LANG_TMP_DOXYFILES)

docs/library/html/index.html: $(include_HEADERS) $(LIB_DOXYFILES)
	$(DOXYGEN_CMD) doxygen/lib/Doxyfile

docs/lang/html/index.html: $(LANG_DOXYFILES) $(LANG_QPP_SRC)
	$(DOXYGEN_CMD) doxygen/lang/Doxyfile
	$(top_srcdir)/doxygen/pdox docs/lang/html docs/lang/html/search

doxygen-doc: docs/library/html/index.html docs/lang/html/index.html

else
doxygen-doc:
	echo no doxygen support
endif

MOSTLYCLEANFILES = $(DX_CLEANFILES)

dist_man_MANS = qore.1

$(dist_man_MANS): doxygen-doc

qore_LDADD = lib/libqore.la

ACLOCAL_AMFLAGS=-I m4

AM_CXXFLAGS = ${PTHREAD_CFLAGS}

if COND_PROFILE
AM_CXXFLAGS += -g -pg
qore_LDFLAGS=-g -pg
else
if COND_DEBUG
AM_CXXFLAGS += -g
qore_LDFLAGS=-g
else
##qore_LDFLAGS=-s
endif
endif

bin_PROGRAMS = qore
if COND_SINGLE_COMPILATION_UNIT
qore_SOURCES = single-compilation-unit.cpp
else
qore_SOURCES = \
	main.cpp \
	command-line.cpp
endif


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = qore.pc


BUILD=$(shell $(top_srcdir)/next_build.sh -b)
PACKAGE_VERSION=$(shell grep PACKAGE_VERSION include/qore/intern/unix-config.h | cut -f3 -d\  | sed s/\"//g)
TAR_NAME := arch/qore-v$(PACKAGE_VERSION)-$(BUILD)-$(shell date +%Y%m%d).tar

tar: clean
	find $(top_srcdir) -name \*~ -exec rm {} \;
	cd ..; tar cvf $(TAR_NAME) src && gzip $(TAR_NAME)
