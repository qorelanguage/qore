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
	single-compilation-unit.cc

parser.cc: parser.yy
scanner.cc: scanner.ll
single-compilation-unit.cc: parser.cc scanner.cc

else
libqore_la_SOURCES = \
	parser.yy \
	scanner.ll \
	Operator.cc \
	QoreTreeNode.cc \
	BarewordNode.cc \
	SelfVarrefNode.cc \
	BackquoteNode.cc \
	ContextrefNode.cc \
	ComplexContextrefNode.cc \
	ContextRowNode.cc \
	VarRefNode.cc \
	FunctionCallNode.cc \
	ReferenceNode.cc \
	ConstantNode.cc \
	ClassRefNode.cc \
	ScopedObjectCallNode.cc \
	AbstractQoreNode.cc \
	QoreStringNode.cc \
	DateTimeNode.cc \
	QoreHashNode.cc \
	BinaryNode.cc \
	QoreBigIntNode.cc \
	QoreBoolNode.cc \
	QoreFloatNode.cc \
	QoreNullNode.cc \
	QoreNothingNode.cc \
	Function.cc \
	BuiltinFunctionList.cc \
	UserFunctionList.cc \
	GlobalVariableList.cc \
	ImportedFunctionList.cc \
	AbstractStatement.cc \
	OnBlockExitStatement.cc \
	ExpressionStatement.cc \
	ReturnStatement.cc \
	StatementBlock.cc \
	ContextStatement.cc \
	SummarizeStatement.cc \
	IfStatement.cc \
	WhileStatement.cc \
	DoWhileStatement.cc \
	ForStatement.cc \
	ForEachStatement.cc \
	DeleteStatement.cc \
	TryStatement.cc \
	ThrowStatement.cc \
	SwitchStatement.cc \
	Variable.cc \
	support.cc \
	QoreSignal.cc \
	QoreType.cc \
	ModuleManager.cc \
	QoreException.cc \
	ExceptionSink.cc \
	QoreClass.cc \
	Context.cc \
	FindNode.cc \
	charset.cc \
	QoreProgram.cc \
	QoreNamespace.cc \
	QoreNet.cc \
	QoreURL.cc \
	QoreFile.cc \
	QoreDir.cc \
	QoreSocket.cc \
	DateTime.cc \
	QoreLib.cc \
	QoreString.cc \
	QoreObject.cc \
	QoreListNode.cc \
	qore-main.cc \
	QoreGetOpt.cc \
	QoreFtpClient.cc \
	DBI.cc \
	Datasource.cc \
	DatasourcePool.cc \
	ManagedDatasource.cc \
	ReferenceArgumentHelper.cc \
	ReferenceHelper.cc \
	ConstantList.cc \
	QoreClassList.cc \
	ThreadResourceList.cc \
	thread.cc \
	VRMutex.cc \
	VLock.cc \
	AbstractSmartLock.cc \
	RMutex.cc \
	ExecArgList.cc \
	NamedScope.cc \
	RWLock.cc \
	QoreSSLBase.cc \
	QoreSSLCertificate.cc \
	QoreSSLPrivateKey.cc \
	mySocket.cc \
	QoreCondition.cc \
	QoreQueue.cc \
	QoreRegexNode.cc \
	QoreRegexBase.cc \
	RegexSubstNode.cc \
	RegexTransNode.cc \
	Sequence.cc \
	QoreReferenceCounter.cc \
	SystemEnvironment.cc \
	SmartMutex.cc \
	QoreCounter.cc \
	CallReferenceNode.cc \
	QoreClosureParseNode.cc \
	QoreClosureNode.cc \
	QoreImplicitArgumentNode.cc \
	ParseOptionMap.cc \
	ql_thread.cc \
	ql_io.cc \
	ql_time.cc \
	ql_lib.cc \
	ql_math.cc \
	ql_type.cc \
	ql_env.cc \
	ql_string.cc \
	ql_pwd.cc \
	ql_misc.cc \
	ql_list.cc \
	ql_xml.cc \
	ql_json.cc \
	ql_crypto.cc \
	ql_object.cc \
	ql_file.cc \
	ql_bzip.cc \
	QC_Socket.cc \
	QC_Program.cc \
	QC_File.cc \
	QC_Dir.cc \
	QC_GetOpt.cc \
	QC_FtpClient.cc \
	QC_Datasource.cc \
	QC_DatasourcePool.cc \
	QC_Queue.cc \
	QC_Mutex.cc \
	QC_Condition.cc \
	QC_RWLock.cc \
	QC_Gate.cc \
	QC_Sequence.cc \
	QC_Counter.cc \
	QC_SSLCertificate.cc \
	QC_SSLPrivateKey.cc \
	QC_HTTPClient.cc \
	QC_XmlRpcClient.cc \
	QC_JsonRpcClient.cc \
	QC_AutoLock.cc \
	QC_AutoGate.cc \
	QC_AutoReadLock.cc \
	QC_AutoWriteLock.cc \
	QC_TermIOS.cc \
	QC_XmlDoc.cc \
	QC_XmlNode.cc \
	QC_XmlReader.cc \
	QoreHTTPClient.cc \
	minitest.cc

if COND_DEBUG
libqore_la_SOURCES += ql_debug.cc
endif
if COND_STACK_TRACE
libqore_la_SOURCES += CallStack.cc
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

libqore_la_LDFLAGS = -version-info 8:1:3 -no-undefined ${QORE_LIB_LDFLAGS}
AM_CPPFLAGS = ${QORE_LIB_CPPFLAGS}
AM_CXXFLAGS = ${QORE_LIB_CXXFLAGS}
