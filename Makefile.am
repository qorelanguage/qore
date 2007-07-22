libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

## use automake to create the Makefile.in for configure

AUTOMAKE_OPTIONS = foreign

SUBDIRS = modules lib

INCLUDES = -I$(top_srcdir)/include

noinst_HEADERS = \
	include/qore/ObjectMethodReference.h \
	include/qore/FunctionReference.h \
	include/qore/AutoNamespaceList.h \
	include/qore/types/Number.h \
	include/qore/Context.h \
	include/qore/qore_thread.h \
	include/qore/ThreadResourceList.h \
	include/qore/AbstractThreadResource.h \
	include/qore/QoreProgram.h \
	include/qore/ReferenceObject.h \
	include/qore/AbstractPrivateData.h \
	include/qore/AbstractSmartLock.h \
	include/qore/SmartMutex.h \
	include/qore/QoreType.h \
	include/qore/List.h \
	include/qore/params.h \
	include/qore/node_types.h \
	include/qore/Function.h \
	include/qore/BuiltinMethod.h \
	include/qore/BuiltinFunctionList.h \
	include/qore/UserFunctionList.h \
	include/qore/QoreClassList.h \
	include/qore/ConstantList.h \
	include/qore/GlobalVariableList.h \
	include/qore/ImportedFunctionList.h \
	include/qore/Variable.h \
	include/qore/TypeConstants.h \
	include/qore/Sequence.h \
	include/qore/CallStack.h \
	include/qore/LockedObject.h \
	include/qore/RWLock.h \
	include/qore/RMutex.h \
	include/qore/VRMutex.h \
	include/qore/VLock.h \
	include/qore/QoreClass.h \
	include/qore/Operator.h \
	include/qore/QoreString.h \
	include/qore/Reference.h \
	include/qore/AbstractStatement.h \
	include/qore/OnBlockExitStatement.h \
	include/qore/RethrowStatement.h \
	include/qore/ThreadExitStatement.h \
	include/qore/BreakStatement.h \
	include/qore/ContinueStatement.h \
	include/qore/ReturnStatement.h \
	include/qore/ExpressionStatement.h \
	include/qore/StatementBlock.h \
	include/qore/ContextStatement.h \
	include/qore/SummarizeStatement.h \
	include/qore/IfStatement.h \
	include/qore/WhileStatement.h \
	include/qore/DoWhileStatement.h \
	include/qore/ForStatement.h \
	include/qore/ForEachStatement.h \
	include/qore/DeleteStatement.h \
	include/qore/TryStatement.h \
	include/qore/ThrowStatement.h \
	include/qore/common.h \
	include/qore/config.h \
	include/qore/proto.h \
	include/qore/charset.h \
	include/qore/ParserSupport.h \
	include/qore/DateTime.h \
	include/qore/support.h \
	include/qore/Exception.h \
	include/qore/Hash.h \
	include/qore/Find.h \
	include/qore/QoreNumber.h \
	include/qore/ErrnoConstants.h \
	include/qore/Tree.h \
	include/qore/ClassRef.h \
	include/qore/ScopedObjectCall.h \
	include/qore/QoreNode.h \
	include/qore/ModuleManager.h \
	include/qore/Namespace.h \
	include/qore/NamedScope.h \
	include/qore/Object.h \
	include/qore/macros-i386.h \
	include/qore/macros-powerpc.h \
	include/qore/macros-sparc.h \
	include/qore/macros-none.h \
	include/qore/QoreSocket.h \
	include/qore/QoreNet.h \
	include/qore/QoreURL.h \
	include/qore/Restrictions.h \
	include/qore/QoreFile.h \
	include/qore/QoreLib.h \
	include/qore/QoreCondition.h \
	include/qore/QoreCounter.h \
	include/qore/BinaryObject.h \
	include/qore/RegexSubst.h \
	include/qore/RegexTrans.h \
	include/qore/hash_map.h \
	include/qore/ExecArgList.h \
	include/qore/SwitchStatement.h \
	include/qore/CaseNodeWithOperator.h \
	include/qore/CaseNodeRegex.h \
	include/qore/QoreRegex.h \
	include/qore/QoreRegexBase.h \
	include/qore/QoreGetOpt.h \
	include/qore/FtpClient.h \
	include/qore/StringList.h \
	include/qore/safe_dslist \
	include/qore/qore_bitopts.h \
	include/qore/QoreSignal.h \
	include/qore/ssl_constants.h \
	include/qore/QoreQueue.h \
	include/qore/QoreSSLBase.h \
	include/qore/QoreWarnings.h \
	include/qore/Datasource.h \
	include/qore/DatasourcePool.h \
	include/qore/ManagedDatasource.h \
	include/qore/Environment.h \
	include/qore/ParseOptionMap.h \
	include/qore/QT_NOTHING.h \
	include/qore/QT_bigint.h \
	include/qore/QT_float.h \
	include/qore/QT_string.h \
	include/qore/QT_date.h \
	include/qore/QT_boolean.h \
	include/qore/QT_NULL.h \
	include/qore/QT_binary.h \
	include/qore/QT_list.h \
	include/qore/QT_hash.h \
	include/qore/QT_object.h \
	include/qore/QT_backquote.h \
	include/qore/QT_context.h \
	include/qore/QT_varref.h \
	include/qore/ql_string.h \
	include/qore/ql_list.h \
	include/qore/ql_misc.h \
	include/qore/ql_math.h \
	include/qore/ql_io.h \
	include/qore/ql_pwd.h \
	include/qore/ql_env.h \
	include/qore/ql_type.h \
	include/qore/ql_lib.h \
	include/qore/ql_time.h \
	include/qore/ql_debug.h \
	include/qore/ql_thread.h \
	include/qore/ql_xml.h \
	include/qore/ql_json.h \
	include/qore/ql_crypto.h \
	include/qore/ql_file.h \
	include/qore/ql_object.h \
	include/qore/QC_Queue.h \
	include/qore/QC_Socket.h \
	include/qore/QC_Sequence.h \
	include/qore/QC_RWLock.h \
	include/qore/QC_Program.h \
	include/qore/QC_Mutex.h \
	include/qore/QC_Condition.h \
	include/qore/QC_Gate.h \
	include/qore/QC_File.h \
	include/qore/QC_Counter.h \
	include/qore/QC_Datasource.h \
	include/qore/QC_DatasourcePool.h \
	include/qore/QC_GetOpt.h \
	include/qore/QC_FtpClient.h \
	include/qore/QC_SSLCertificate.h \
	include/qore/QC_SSLPrivateKey.h \
	include/qore/QC_HTTPClient.h \
	include/qore/QC_XmlRpcClient.h \
	include/qore/QC_JsonRpcClient.h \
	include/qore/QC_AutoGate.h \
	include/qore/QC_AutoLock.h \
	include/qore/QC_AutoReadLock.h \
	include/qore/QC_AutoWriteLock.h \
	include/qore/QoreHTTPClient.h \
	include/qore/DBI.h \
	include/qore/ScopeGuard.h \
	include/qore/minitest.hpp \
	include/qore/inline_printf.h \
	include/qore/QException.h \
	include/qore/ReferenceHolder.h \
	modules/oracle/oracle-config.h \
	modules/oracle/oracle.h \
	modules/oracle/oracle-module.h \
	modules/mysql/qore-mysql-module.h \
	modules/mysql/qore-mysql.h \
	modules/pgsql/pgsql.h \
	modules/pgsql/QorePGConnection.h \
	modules/pgsql/QorePGMapper.h \
	modules/tibrv/tibrv-module.h \
	modules/tibrv/qore_tibrv.h \
	modules/tibrv/QoreTibrvTransport.h \
	modules/tibrv/QoreTibrvListener.h \
	modules/tibrv/QoreTibrvSender.h \
	modules/tibrv/QoreTibrvFtMember.h \
	modules/tibrv/QoreTibrvFtMonitor.h \
	modules/tibrv/QoreTibrvCmTransport.h \
	modules/tibrv/QoreTibrvCmSender.h \
	modules/tibrv/QoreTibrvCmListener.h \
	modules/tibrv/QoreTibrvDistributedQueue.h \
	modules/tibrv/QC_TibrvListener.h \
	modules/tibrv/QC_TibrvSender.h \
	modules/tibrv/QC_TibrvFtMember.h \
	modules/tibrv/QC_TibrvFtMonitor.h \
	modules/tibrv/QC_TibrvCmSender.h \
	modules/tibrv/QC_TibrvCmListener.h \
	modules/tibrv/QC_TibrvDistributedQueue.h \
	modules/tibae/tibae-module.h \
	modules/tibae/QC_TibcoAdapter.h \
	modules/tibae/TibCommandLine.h \
	modules/tibae/QoreApp.h \
	modules/tibae/tibae.h \
	modules/ncurses/QC_Window.h \
	modules/ncurses/QC_Panel.h \
	modules/ncurses/ncurses-module.h \
	modules/Tuxedo/tuxedo_module.h \
	modules/Tuxedo/QC_TuxedoAdapter.h \
	modules/Tuxedo/QoreTuxedoAdapter.h \
	modules/sybase/sybase.h \
	modules/sybase/connection.h \
	modules/sybase/conversions.h \
	modules/sybase/command.h \
	modules/sybase/encoding_helpers.h \
	modules/sybase/sybase_query.h \
	modules/sybase/common_constants.h \
	modules/sybase/row_output_buffers.h \
	modules/qt/QC_QApplication.h \
	modules/qt/QC_QPushButton.h \
	modules/qt/QC_QFont.h \
	modules/qt/QC_QWidget.h \
	modules/qt/QC_QObject.h \
	modules/qt/QoreAbstractQObject.h \
	command-line.h \
	include/qore/Qore.h

EXTRA_DIST = .build next_build.sh WHATISQORE RELEASE-NOTES \
        test/thread-object-test.q \
        test/create-iso-8859-1-file.q \
        test/empty.q \
        test/file-test.q \
        test/ftp-test.q \
        test/gate-test.q \
        test/max-threads-test.q \
	test/ncurses.q \
        test/new.dat \
        test/object-test.q \
        test/rwlock-test.q \
        test/socket-test.q \
        test/subprogram-thread-test.q \
        test/system-test.q \
        test/test-exception.q \
        test/test.q \
        test/Test.qc \
        test/tibae-listener.q \
        test/tibae-test.q \
	test/tibrv-test.q \
        test/unlocked-thread-test.q \
	test/regex.q \
	test/deadlock.q \
	examples/HTTPServer.qc \
        examples/XmlRpcHandler.qc \
        examples/XmlRpcServerValidation.q \
	examples/stmt.q \
	examples/exp.q \
	examples/obj2.q \
	examples/hash.q \
	examples/hanoi.q \
	examples/clisrv.q \
	examples/xml-rpc-client.q \
	examples/inherit.q \
	examples/inherit2.q \
	examples/getopt.q \
	examples/worm.q \
	examples/qt.q \
	docs/qore.html \
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
	docs/roadmap.html \
	qore-0.7.0-1.spec \
	CHANGELOG

dist_man_MANS = qore.1

qore_LDADD = lib/libqore.la ${PTHREAD_LIBS}

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
qore_SOURCES = single-compilation-unit.cc
else
qore_SOURCES = \
	main.cc \
	command-line.cc
endif

BUILD=$(shell cat $(top_srcdir)/.build)
PACKAGE_VERSION=$(shell grep PACKAGE_VERSION include/qore/unix-config.h | cut -f3 -d\  | sed s/\"//g)
TAR_NAME := arch/qore-v$(PACKAGE_VERSION)-$(BUILD)-$(shell date +%Y%m%d).tar

tar: clean
	find $(top_srcdir) -name \*~ -exec rm {} \;
	cd ..; tar cvf $(TAR_NAME) src && gzip $(TAR_NAME)
