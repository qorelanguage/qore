libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

SUBDIRS =

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
	Tree.cc \
	ClassRef.cc \
	ScopedObjectCall.cc \
	QoreNode.cc \
	Function.cc \
	BuiltinFunctionList.cc \
	UserFunctionList.cc \
	GlobalVariableList.cc \
	ImportedFunctionList.cc \
	AbstractStatement.cc \
	OnBlockExitStatement.cc \
	ExpressionStatement.cc \
	ReturnStatement.cc \
	StatementBLock.cc \
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
	Exception.cc \
	QoreClass.cc \
	Context.cc \
	Find.cc \
	charset.cc \
	QoreProgram.cc \
	QoreProgramStack.cc \
	Namespace.cc \
	QoreNet.cc \
	QoreURL.cc \
	QoreFile.cc \
	QoreSocket.cc \
	DateTime.cc \
	QoreLib.cc \
	QoreString.cc \
	Hash.cc \
	Object.cc \
	List.cc \
	qore-main.cc \
	QoreGetOpt.cc \
	FtpClient.cc \
	DBI.cc \
	Datasource.cc \
	ManagedDatasource.cc \
	StringList.cc \
	ConstantList.cc \
	QoreClassList.cc \
	ThreadResourceList.cc \
	thread.cc \
	VRMutex.cc \
	VLock.cc \
	AbstractSmartLock.cc \
	RMutex.cc \
	CallStack.cc \
	ExecArgList.cc \
	Gate.cc \
	SingleExitGate.cc \
	NamedScope.cc \
	RWLock.cc \
	QoreSSLBase.cc \
	QoreSSLCertificate.cc \
	QoreSSLPrivateKey.cc \
	mySocket.cc \
	QoreCondition.cc \
	QoreQueue.cc \
	QoreRegex.cc \
	QoreRegexBase.cc \
	RegexSubst.cc \
	RegexTrans.cc \
	Sequence.cc \
	ReferenceObject.cc \
	Environment.cc \
	SmartMutex.cc \
	QoreCounter.cc \
	QT_NOTHING.cc \
	QT_bigint.cc \
	QT_float.cc \
	QT_string.cc \
	QT_date.cc \
	QT_boolean.cc \
	QT_NULL.cc \
	QT_binary.cc \
	QT_list.cc \
	QT_hash.cc \
	QT_object.cc \
	QT_backquote.cc \
	QT_context.cc \
	QT_varref.cc \
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
	QC_Socket.cc \
	QC_Program.cc \
	QC_File.cc \
	QC_GetOpt.cc \
	QC_FtpClient.cc \
	QC_Datasource.cc \
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
	QoreHTTPClient.cc \
	minitest.cc \
	inline_printf.cc \
	QException.cc

if COND_DEBUG
libqore_la_SOURCES += ql_debug.cc
endif

endif

libqore_la_LDFLAGS = -version-info 3:0:0 -no-undefined ${LIBXML2_LDFLAGS} $(PCRE_LDFLAGS) $(OPENSSL_LDFLAGS) $(ZLIB_LDFLAGS) $(LDFLAGS) -lm
libqore_la_LIBADD = ${PTHREAD_LIBS} 
AM_CPPFLAGS = -D_QORE_LIB_INTERN -DMODULE_DIR=\"$(pkglibdir)\" $(PCRE_CPPFLAGS) $(OPENSSL_CPPFLAGS) $(LIBXML2_CPPFLAGS)
AM_CXXFLAGS = ${PTHREAD_CFLAGS}

if COND_DEBUG
BUILD_TYPE=dev
endif

if COND_PROFILE
BUILD_TYPE=prof
AM_CXXFLAGS += -g -pg
libqore_la_LDFLAGS += -g -pg
else
if COND_DEBUG
AM_CXXFLAGS += -g
libqore_la_LDFLAGS += -g
else
BUILD_TYPE=prod
##qore_LDFLAGS=-s
endif
endif

if COND_MONOLITHIC
if COND_TIBRV
libqore_la_LIBADD += ../modules/TIBCO/libtibrv.la
endif
if COND_TIBAE
libqore_la_LIBADD += ../modules/TIBCO/libtibae.la
endif
if COND_ORACLE
libqore_la_LIBADD += ../modules/oracle/liboracle.la
endif
if COND_MYSQL
libqore_la_LIBADD += ../modules/mysql/libmysql.la
endif
if COND_NCURSES
libqore_la_LIBADD += ../modules/ncurses/libncurses.la
endif
if COND_TUXEDO
libqore_la_LIBADD += ../modules/Tuxedo/libtuxedo.la
endif
if COND_SYBASE
libqore_la_LIBADD += ../modules/sybase/libsybase.la
endif
if COND_PGSQL
libqore_la_LIBADD += ../modules/pgsql/libpgsql.la
endif
endif
