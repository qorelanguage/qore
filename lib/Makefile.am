libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/$(mypkglibdir)

SUBDIRS =

BUILT_SOURCES = dummy

dummy:
	$(top_srcdir)/next_build.sh

INCLUDES = -I$(top_srcdir)/include
AM_YFLAGS = -d

lib_LTLIBRARIES = libqore.la

if COND_SINGLE_COMPILATION_UNIT
libqore_la_SOURCES = \
	single-compilation-unit.cpp

parser.cpp: parser.ypp
scanner.cpp: scanner.lpp
single-compilation-unit.cpp: parser.cpp scanner.cpp

else
libqore_la_SOURCES = \
	parser.ypp \
	scanner.lpp \
	Operator.cpp \
	QoreTreeNode.cpp \
	BarewordNode.cpp \
	SelfVarrefNode.cpp \
	BackquoteNode.cpp \
	ContextrefNode.cpp \
	ComplexContextrefNode.cpp \
	ContextRowNode.cpp \
	VarRefNode.cpp \
	FunctionCallNode.cpp \
	ReferenceNode.cpp \
	ConstantNode.cpp \
	ClassRefNode.cpp \
	AbstractQoreNode.cpp \
	QoreStringNode.cpp \
	DateTimeNode.cpp \
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
	DeleteStatement.cpp \
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
	QoreString.cpp \
	QoreObject.cpp \
	QoreListNode.cpp \
	qore-main.cpp \
	QoreGetOpt.cpp \
	QoreFtpClient.cpp \
	DBI.cpp \
	Datasource.cpp \
	DatasourcePool.cpp \
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
	RMutex.cpp \
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
	ParseOptionMap.cpp \
	QoreTypeInfo.cpp \
	ql_thread.cpp \
	ql_io.cpp \
	ql_time.cpp \
	ql_lib.cpp \
	ql_math.cpp \
	ql_type.cpp \
	ql_env.cpp \
	ql_string.cpp \
	ql_pwd.cpp \
	ql_misc.cpp \
	ql_list.cpp \
	ql_xml.cpp \
	ql_json.cpp \
	ql_crypto.cpp \
	ql_object.cpp \
	ql_file.cpp \
	ql_bzip.cpp \
	QC_Socket.cpp \
	QC_Program.cpp \
	QC_File.cpp \
	QC_Dir.cpp \
	QC_GetOpt.cpp \
	QC_FtpClient.cpp \
	QC_Datasource.cpp \
	QC_DatasourcePool.cpp \
	QC_Queue.cpp \
	QC_Mutex.cpp \
	QC_Condition.cpp \
	QC_RWLock.cpp \
	QC_Gate.cpp \
	QC_Sequence.cpp \
	QC_Counter.cpp \
	QC_SSLCertificate.cpp \
	QC_SSLPrivateKey.cpp \
	QC_HTTPClient.cpp \
	QC_XmlRpcClient.cpp \
	QC_JsonRpcClient.cpp \
	QC_AutoLock.cpp \
	QC_AutoGate.cpp \
	QC_AutoReadLock.cpp \
	QC_AutoWriteLock.cpp \
	QC_TermIOS.cpp \
	QC_XmlDoc.cpp \
	QC_XmlNode.cpp \
	QC_XmlReader.cpp \
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

libqore_la_LDFLAGS = -version-info 9:0:4 -no-undefined ${QORE_LIB_LDFLAGS}
AM_CPPFLAGS = ${QORE_LIB_CPPFLAGS}
AM_CXXFLAGS = ${QORE_LIB_CXXFLAGS}
