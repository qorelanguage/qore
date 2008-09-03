libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/$(mypkglibdir)
includedir = $(prefix)/include/qore

## use automake to create the Makefile.in for configure

AUTOMAKE_OPTIONS = foreign

SUBDIRS = modules lib

INCLUDES = -I$(top_srcdir)/include

DOXYFILES = \
	doxygen/Doxyfile \
	doxygen/mainpage.doxygen \
	doxygen/10_embedding.doxygen \
	doxygen/20_qore_data.doxygen \
	doxygen/30_qore_builtin_functions.doxygen \
	doxygen/40_class_implementation.doxygen \
	doxygen/70_qore_modules.doxygen \
	doxygen/75_dbi_drivers.doxygen

include_HEADERS = \
	include/qore/ReferenceArgumentHelper.h \
	include/qore/ReferenceHelper.h \
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
	include/qore/hash_map.h \
	include/qore/QoreFtpClient.h \
	include/qore/safe_dslist \
	include/qore/qore_bitopts.h \
	include/qore/QoreSSLBase.h \
	include/qore/Datasource.h \
	include/qore/SystemEnvironment.h \
	include/qore/ParseOptionMap.h \
	include/qore/QoreHTTPClient.h \
	include/qore/DBI.h \
	include/qore/ScopeGuard.h \
	include/qore/ReferenceHolder.h \
	include/qore/QoreSSLCertificate.h \
	include/qore/QoreSSLPrivateKey.h \
	include/qore/AutoVLock.h \
	include/qore/QoreFile.h \
	include/qore/QoreDir.h \
	include/qore/Qore.h

noinst_HEADERS = \
	include/qore/minitest.hpp \
	include/qore/macros-i386.h \
	include/qore/macros-powerpc.h \
	include/qore/macros-sparc.h \
	include/qore/macros-none.h \
	include/qore/intern/config.h \
	include/qore/intern/unix-config.h \
	include/qore/intern/win32-config.h \
	include/qore/intern/svn-revision.h \
	include/qore/intern/ThreadResourceList.h \
	include/qore/intern/ssl_constants.h \
	include/qore/intern/qore_thread_intern.h \
	include/qore/intern/inline_printf.h \
	include/qore/intern/VRMutex.h \
	include/qore/intern/Variable.h \
	include/qore/intern/LocalVar.h \
	include/qore/intern/ModuleInfo.h \
	include/qore/intern/UserFunctionList.h \
	include/qore/intern/TypeConstants.h \
	include/qore/intern/QoreTreeNode.h \
	include/qore/intern/BarewordNode.h \
	include/qore/intern/SelfVarrefNode.h \
	include/qore/intern/VarRefNode.h \
	include/qore/intern/BackquoteNode.h \
	include/qore/intern/ContextrefNode.h \
	include/qore/intern/ContextRowNode.h \
	include/qore/intern/ComplexContextrefNode.h \
	include/qore/intern/FunctionCallNode.h \
	include/qore/intern/ConstantNode.h \
	include/qore/intern/SmartMutex.h \
	include/qore/intern/Sequence.h \
	include/qore/intern/ScopedObjectCallNode.h \
	include/qore/intern/RWLock.h \
	include/qore/intern/RMutex.h \
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
	include/qore/intern/QoreNamespaceList.h \
	include/qore/intern/ManagedDatasource.h \
	include/qore/intern/ImportedFunctionList.h \
	include/qore/intern/GlobalVariableList.h \
	include/qore/intern/DatasourcePool.h \
	include/qore/intern/Function.h \
	include/qore/intern/FindNode.h \
	include/qore/intern/ExecArgList.h \
	include/qore/intern/ErrnoConstants.h \
	include/qore/intern/Context.h \
	include/qore/intern/ConstantList.h \
	include/qore/intern/CaseNodeWithOperator.h \
	include/qore/intern/CaseNodeRegex.h \
	include/qore/intern/ClassRefNode.h \
	include/qore/intern/CallStack.h \
	include/qore/intern/BuiltinMethod.h \
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
	include/qore/intern/DeleteStatement.h \
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
	include/qore/intern/ParseNode.h \
	include/qore/intern/QoreClosureNode.h \
	include/qore/intern/QoreClosureParseNode.h \
	include/qore/intern/QoreImplicitArgumentNode.h \
	include/qore/intern/qore_string_private.h \
	include/qore/intern/qore_date_private.h \
	include/qore/intern/ql_string.h \
	include/qore/intern/ql_list.h \
	include/qore/intern/ql_misc.h \
	include/qore/intern/ql_math.h \
	include/qore/intern/ql_io.h \
	include/qore/intern/ql_pwd.h \
	include/qore/intern/ql_env.h \
	include/qore/intern/ql_type.h \
	include/qore/intern/ql_lib.h \
	include/qore/intern/ql_time.h \
	include/qore/intern/ql_debug.h \
	include/qore/intern/ql_thread.h \
	include/qore/intern/ql_xml.h \
	include/qore/intern/ql_json.h \
	include/qore/intern/ql_crypto.h \
	include/qore/intern/ql_file.h \
	include/qore/intern/ql_object.h \
	include/qore/intern/ql_bzip.h \
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
	include/qore/intern/QC_GetOpt.h \
	include/qore/intern/QC_FtpClient.h \
	include/qore/intern/QC_SSLCertificate.h \
	include/qore/intern/QC_SSLPrivateKey.h \
	include/qore/intern/QC_HTTPClient.h \
	include/qore/intern/QC_XmlRpcClient.h \
	include/qore/intern/QC_JsonRpcClient.h \
	include/qore/intern/QC_AutoGate.h \
	include/qore/intern/QC_AutoLock.h \
	include/qore/intern/QC_AutoReadLock.h \
	include/qore/intern/QC_AutoWriteLock.h \
	modules/oracle/oracle-config.h \
	modules/oracle/oracle.h \
	modules/oracle/oracle-module.h \
	modules/mysql/qore-mysql-module.h \
	modules/mysql/qore-mysql.h \
	modules/pgsql/pgsql.h \
	modules/pgsql/QorePGConnection.h \
	modules/pgsql/QorePGMapper.h \
	modules/tibae/tibae-module.h \
	modules/tibae/QC_TibcoAdapter.h \
	modules/tibae/TibCommandLine.h \
	modules/tibae/QoreApp.h \
	modules/tibae/tibae.h \
	modules/sybase/sybase.h \
	modules/sybase/connection.h \
	modules/sybase/conversions.h \
	modules/sybase/command.h \
	modules/sybase/encoding_helpers.h \
	modules/sybase/sybase_query.h \
	modules/sybase/common_constants.h \
	modules/sybase/row_output_buffers.h \
	command-line.h

EXTRA_DIST = next_build.sh WHATISQORE RELEASE-NOTES \
	CHANGELOG AUTHORS \
	README README-LICENSE \
	COPYING.LGPL COPYING.GPL \
        test/thread-object-test.q \
        test/create-iso-8859-1-file.q \
        test/empty.q \
        test/file-test.q \
        test/ftp-test.q \
        test/gate-test.q \
        test/max-threads-test.q \
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
	examples/clisrv.q \
	examples/xml-rpc-client.q \
	examples/inherit.q \
	examples/inherit2.q \
	examples/getopt.q \
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
	docs/library/ \
	qore-0.7.0-1.spec \
	$(DOXYFILES)

BUILT_SOURCES = dummy

dummy:
	./next_build.sh

if COND_DOXYGEN
DX_CLEANFILES = docs/library

docs/library/html/index.html: $(INCLUDE_HEADERS) $(DOXYFILES)
	$(DOXYGEN_CMD) doxygen/Doxyfile

doxygen-doc: docs/library/html/index.html

else
doxygen-doc:
	echo no doxygen support
endif

MOSTLYCLEANFILES = $(DX_CLEANFILES)

dist_man_MANS = qore.1

$(dist_man_MANS): doxygen-doc

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

BUILD=$(shell $(top_srcdir)/next_build.sh -b)
PACKAGE_VERSION=$(shell grep PACKAGE_VERSION include/qore/intern/unix-config.h | cut -f3 -d\  | sed s/\"//g)
TAR_NAME := arch/qore-v$(PACKAGE_VERSION)-$(BUILD)-$(shell date +%Y%m%d).tar

tar: clean
	find $(top_srcdir) -name \*~ -exec rm {} \;
	cd ..; tar cvf $(TAR_NAME) src && gzip $(TAR_NAME)
