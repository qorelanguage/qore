libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

SUBDIRS =

INCLUDES = -I$(top_srcdir)/include
AM_YFLAGS = -d

lib_LTLIBRARIES = libqore.la
libqore_la_SOURCES = \
	parser.yy \
	scanner.ll \
	Operator.cc \
	QoreNode.cc \
	Function.cc \
	BuiltinFunctionList.cc \
	Statement.cc \
	Variable.cc \
	support.cc \
	QoreSignal.cc \
	QoreType.cc \
	ModuleManager.cc \
	Exception.cc \
	QoreClass.cc \
	Context.cc \
	charset.cc \
	QoreProgram.cc \
	Namespace.cc \
	QoreNet.cc \
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
	StringList.cc \
	thread.cc \
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
	ql_xml.cc \
	ql_crypto.cc \
	QC_Socket.cc \
	QC_Program.cc \
	QC_File.cc \
	QC_GetOpt.cc \
	QC_FtpClient.cc \
	QC_Datasource.cc \
	QC_Queue.cc \
	QC_Mutex.cc \
	QC_RMutex.cc \
	QC_Condition.cc \
	QC_RWLock.cc \
	QC_Gate.cc \
	QC_Sequence.cc \
	QC_Counter.cc \
	QC_SSLCertificate.cc \
	QC_SSLPrivateKey.cc

libqore_la_LDFLAGS = -version-info 0:0:0 ${XML_LIBS} $(PCRE_LDFLAGS) $(OPENSSL_LDFLAGS)
libqore_la_LIBADD = -ldl ${PTHREAD_LIBS}
AM_CPPFLAGS = -DMODULE_DIR=\"$(pkglibdir)\" $(PCRE_CPPFLAGS) $(OPENSSL_CPPFLAGS)
AM_CXXFLAGS = ${XML_CFLAGS} ${PTHREAD_CFLAGS}

if COND_PROFILE
BUILD_TYPE=prof
AM_CXXFLAGS += -g -pg
libqore_la_LDFLAGS += -g -pg
else
if COND_DEBUG
libqore_la_SOURCES += ql_debug.cc
BUILD_TYPE=dev
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
endif
