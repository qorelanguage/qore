libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/$(mypkglibdir)

SUBDIRS =

BUILT_SOURCES = dummy

dummy:
	$(top_srcdir)/next_build.sh

INCLUDES = -I$(top_srcdir)/include
AM_YFLAGS = -d

QPP=./qpp

lib_LTLIBRARIES = libqore.la

bin_PROGRAMS = qpp

qpp_SOURCES = qpp.cpp

if COND_NEED_GETOPT_LONG
qpp_SOURCES += getopt_long.cpp
endif

QC_Queue.cpp: QC_Queue.qpp qpp
	$(QPP) $<

QC_Socket.cpp: QC_Socket.qpp qpp
	$(QPP) $<

QC_File.cpp: QC_File.qpp qpp
	$(QPP) $<

QC_AbstractSmartLock.cpp: QC_AbstractSmartLock.qpp qpp
	$(QPP) $<

QC_Mutex.cpp: QC_Mutex.qpp qpp
	$(QPP) $<

QC_AutoLock.cpp: QC_AutoLock.qpp qpp
	$(QPP) $<

QC_Gate.cpp: QC_Gate.qpp qpp
	$(QPP) $<

QC_AutoGate.cpp: QC_AutoGate.qpp qpp
	$(QPP) $<

QC_RWLock.cpp: QC_RWLock.qpp qpp
	$(QPP) $<

QC_AutoReadLock.cpp: QC_AutoReadLock.qpp qpp
	$(QPP) $<

QC_AutoWriteLock.cpp: QC_AutoWriteLock.qpp qpp
	$(QPP) $<

QC_Condition.cpp: QC_Condition.qpp qpp
	$(QPP) $<

QC_Sequence.cpp: QC_Sequence.qpp qpp
	$(QPP) $<

QC_Counter.cpp: QC_Counter.qpp qpp
	$(QPP) $<

QC_HTTPClient.cpp: QC_HTTPClient.qpp qpp
	$(QPP) $<

QC_FtpClient.cpp: QC_FtpClient.qpp qpp
	$(QPP) $<

QC_Datasource.cpp: QC_Datasource.qpp qpp
	$(QPP) $<

QC_DatasourcePool.cpp: QC_DatasourcePool.qpp qpp
	$(QPP) $<

QC_SQLStatement.cpp: QC_SQLStatement.qpp qpp
	$(QPP) $<

QC_Dir.cpp: QC_Dir.qpp qpp
	$(QPP) $<

QC_Program.cpp: QC_Program.qpp qpp
	$(QPP) $<

QC_GetOpt.cpp: QC_GetOpt.qpp qpp
	$(QPP) $<

QC_TermIOS.cpp: QC_TermIOS.qpp qpp
	$(QPP) $<

QC_TimeZone.cpp: QC_TimeZone.qpp qpp
	$(QPP) $<

QC_SSLCertificate.cpp: QC_SSLCertificate.qpp qpp
	$(QPP) $<

QC_SSLPrivateKey.cpp: QC_SSLPrivateKey.qpp qpp
	$(QPP) $<

ql_misc.cpp: ql_misc.qpp qpp
	$(QPP) $<

ql_compression.cpp: ql_compression.qpp qpp
	$(QPP) $<

ql_thread.cpp: ql_thread.qpp qpp
	$(QPP) $<

ql_crypto.cpp: ql_crypto.qpp qpp
	$(QPP) $<

QORE_GENERATED_SOURCES = \
	QC_Queue.cpp QC_Socket.cpp QC_File.cpp QC_AbstractSmartLock.cpp QC_Mutex.cpp QC_AutoLock.cpp \
	QC_Gate.cpp QC_AutoGate.cpp QC_RWLock.cpp QC_AutoReadLock.cpp QC_AutoWriteLock.cpp \
	QC_Condition.cpp QC_Sequence.cpp QC_Counter.cpp QC_HTTPClient.cpp QC_FtpClient.cpp \
	QC_Datasource.cpp QC_DatasourcePool.cpp QC_SQLStatement.cpp QC_Dir.cpp QC_Program.cpp \
	QC_GetOpt.cpp QC_TermIOS.cpp QC_TimeZone.cpp QC_SSLCertificate.cpp QC_SSLPrivateKey.cpp \
	ql_misc.cpp ql_compression.cpp ql_thread.cpp ql_crypto.cpp

CLEANFILES = $(QORE_GENERATED_SOURCES)

if COND_SINGLE_COMPILATION_UNIT
libqore_la_SOURCES = \
	single-compilation-unit.cpp

parser.cpp: parser.ypp
scanner.cpp: scanner.lpp
single-compilation-unit.cpp: parser.cpp scanner.cpp $(QORE_GENERATED_SOURCES)

else
nodist_libqore_la_SOURCES = $(QORE_GENERATED_SOURCES)

libqore_la_SOURCES = \
	parser.ypp \
	scanner.lpp \
	Operator.cpp \
	QoreTreeNode.cpp \
	BarewordNode.cpp \
	SelfVarrefNode.cpp \
	StaticClassVarRefNode.cpp \
	BackquoteNode.cpp \
	ContextrefNode.cpp \
	ComplexContextrefNode.cpp \
	ContextRowNode.cpp \
	VarRefNode.cpp \
	FunctionCallNode.cpp \
	ReferenceNode.cpp \
	ScopedRefNode.cpp \
	ClassRefNode.cpp \
	AbstractQoreNode.cpp \
	QoreStringNode.cpp \
	DateTimeNode.cpp \
	qore_date_private.cpp \
	QoreHashNode.cpp \
	BinaryNode.cpp \
	QoreBigIntNode.cpp \
	QoreBoolNode.cpp \
	QoreFloatNode.cpp \
	QoreNullNode.cpp \
	QoreNothingNode.cpp \
	Function.cpp \
	BuiltinFunction.cpp \
	BuiltinFunctionList.cpp \
	UserFunctionList.cpp \
	GlobalVariableList.cpp \
	ImportedFunctionList.cpp \
	AbstractStatement.cpp \
	OnBlockExitStatement.cpp \
	ExpressionStatement.cpp \
	ReturnStatement.cpp \
	StatementBlock.cpp \
	ContextStatement.cpp \
	SummarizeStatement.cpp \
	IfStatement.cpp \
	WhileStatement.cpp \
	DoWhileStatement.cpp \
	ForStatement.cpp \
	ForEachStatement.cpp \
	TryStatement.cpp \
	ThrowStatement.cpp \
	SwitchStatement.cpp \
	Variable.cpp \
	support.cpp \
	QoreType.cpp \
	ModuleManager.cpp \
	QoreException.cpp \
	ExceptionSink.cpp \
	QoreClass.cpp \
	Context.cpp \
	FindNode.cpp \
	charset.cpp \
	QoreProgram.cpp \
	QoreNamespace.cpp \
	QoreNet.cpp \
	QoreURL.cpp \
	QoreFile.cpp \
	QoreDir.cpp \
	QoreSocket.cpp \
	DateTime.cpp \
	QoreLib.cpp \
	QoreTimeZoneManager.cpp \
	QoreString.cpp \
	QoreObject.cpp \
	QoreListNode.cpp \
	qore-main.cpp \
	QoreGetOpt.cpp \
	QoreFtpClient.cpp \
	DBI.cpp \
	Datasource.cpp \
	DatasourcePool.cpp \
	SQLStatement.cpp \
	QoreSQLStatement.cpp \
	ManagedDatasource.cpp \
	ReferenceArgumentHelper.cpp \
	ReferenceHelper.cpp \
	ConstantList.cpp \
	QoreClassList.cpp \
	ThreadResourceList.cpp \
	thread.cpp \
	VRMutex.cpp \
	VLock.cpp \
	AbstractSmartLock.cpp \
	ExecArgList.cpp \
	NamedScope.cpp \
	RWLock.cpp \
	QoreSSLBase.cpp \
	QoreSSLCertificate.cpp \
	QoreSSLPrivateKey.cpp \
	mySocket.cpp \
	QoreCondition.cpp \
	QoreQueue.cpp \
	QoreRegexNode.cpp \
	QoreRegexBase.cpp \
	RegexSubstNode.cpp \
	RegexTransNode.cpp \
	Sequence.cpp \
	QoreReferenceCounter.cpp \
	SystemEnvironment.cpp \
	SmartMutex.cpp \
	QoreCounter.cpp \
	CallReferenceNode.cpp \
	QoreClosureParseNode.cpp \
	QoreClosureNode.cpp \
	QoreImplicitArgumentNode.cpp \
	QoreImplicitElementNode.cpp \
	ParseOptionMap.cpp \
	QoreTypeInfo.cpp \
	QoreDeleteOperatorNode.cpp \
	QoreRemoveOperatorNode.cpp \
	QoreSpliceOperatorNode.cpp \
	QoreExtractOperatorNode.cpp \
	QoreCastOperatorNode.cpp \
	QoreUnaryMinusOperatorNode.cpp \
	QoreDotEvalOperatorNode.cpp \
	QoreLogicalEqualsOperatorNode.cpp \
	QoreAssignmentOperatorNode.cpp \
	QoreIntAssignmentOperatorNode.cpp \
	QorePlusEqualsOperatorNode.cpp \
	QoreIntPlusEqualsOperatorNode.cpp \
	QoreMinusEqualsOperatorNode.cpp \
	QoreIntMinusEqualsOperatorNode.cpp \
        QoreOrEqualsOperatorNode.cpp \
        QoreIntOrEqualsOperatorNode.cpp \
        QoreAndEqualsOperatorNode.cpp \
        QoreIntAndEqualsOperatorNode.cpp \
        QoreModulaEqualsOperatorNode.cpp \
        QoreIntModulaEqualsOperatorNode.cpp \
        QoreMultiplyEqualsOperatorNode.cpp \
        QoreIntMultiplyEqualsOperatorNode.cpp \
        QoreDivideEqualsOperatorNode.cpp \
        QoreIntDivideEqualsOperatorNode.cpp \
        QoreXorEqualsOperatorNode.cpp \
        QoreIntXorEqualsOperatorNode.cpp \
        QoreShiftLeftEqualsOperatorNode.cpp \
        QoreIntShiftLeftEqualsOperatorNode.cpp \
        QoreShiftRightEqualsOperatorNode.cpp \
        QoreIntShiftRightEqualsOperatorNode.cpp \
	QorePostIncrementOperatorNode.cpp \
	QorePostDecrementOperatorNode.cpp \
	QorePreIncrementOperatorNode.cpp \
	QorePreDecrementOperatorNode.cpp \
	QoreLogicalLessThanOperatorNode.cpp \
	QoreLogicalGreaterThanOperatorNode.cpp \
	ql_io.cpp \
	ql_time.cpp \
	ql_lib.cpp \
	ql_math.cpp \
	ql_type.cpp \
	ql_env.cpp \
	ql_string.cpp \
	ql_misc.cpp \
	ql_list.cpp \
	ql_crypto.cpp \
	ql_object.cpp \
	ql_file.cpp \
	ql_compression.cpp \
	ql_pwd.cpp \
	QC_Socket.cpp \
	QC_Program.cpp \
	QC_File.cpp \
	QC_Dir.cpp \
	QC_GetOpt.cpp \
	QC_FtpClient.cpp \
	QC_Datasource.cpp \
	QC_DatasourcePool.cpp \
	QC_SQLStatement.cpp \
	QC_Mutex.cpp \
	QC_Condition.cpp \
	QC_RWLock.cpp \
	QC_Gate.cpp \
	QC_Sequence.cpp \
	QC_Counter.cpp \
	QC_SSLCertificate.cpp \
	QC_SSLPrivateKey.cpp \
	QC_HTTPClient.cpp \
	QC_AutoLock.cpp \
	QC_AutoGate.cpp \
	QC_AutoReadLock.cpp \
	QC_AutoWriteLock.cpp \
	QC_AbstractSmartLock.cpp \
	QC_TimeZone.cpp \
	QC_TermIOS.cpp \
	QorePseudoMethods.cpp \
	QoreHTTPClient.cpp \
	minitest.cpp

if COND_SIGNAL_HANDLING
libqore_la_SOURCES += QoreSignal.cpp
endif
if COND_DEBUG
libqore_la_SOURCES += ql_debug.cpp
endif
if COND_STACK_TRACE
libqore_la_SOURCES += CallStack.cpp
endif
if COND_NEED_GLOB
libqore_la_SOURCES += glob.cpp
endif
if COND_NEED_INET_NTOP
libqore_la_SOURCES += inet_ntop.cpp
endif
if COND_NEED_INET_PTON
libqore_la_SOURCES += inet_pton.cpp
endif
endif

if COND_SOLARIS_CC_X86_64
libqore_la_SOURCES += sunpro-x86_64.s
endif
if COND_SOLARIS_CC_I386
libqore_la_SOURCES += sunpro-i386.s
endif
if COND_SOLARIS_CC_SPARC32
libqore_la_SOURCES += sunpro-sparc32.s
endif
#if COND_SOLARIS_CC_SPARC64
#libqore_la_SOURCES += sunpro-sparc64.s
#endif
if COND_HPUX_ACC_PARISC32
libqore_la_SOURCES += acc-parisc32.s
endif
if COND_HPUX_ACC_64BIT_IA64
libqore_la_SOURCES += acc-64bit-ia64.s
endif

libqore_la_LDFLAGS = -version-info 12:0:7 -no-undefined ${QORE_LIB_LDFLAGS}
AM_CPPFLAGS = ${QORE_LIB_CPPFLAGS}
AM_CXXFLAGS = ${QORE_LIB_CXXFLAGS}
