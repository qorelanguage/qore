libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/$(mypkglibdir)
modverdir = $(datarootdir)/$(mymodverdir)
includedir = $(prefix)/include/qore
qoredatadir = $(datarootdir)/qore

## use automake to create the Makefile.in for configure

AUTOMAKE_OPTIONS = foreign

SUBDIRS = lib modules/astparser

AM_CPPFLAGS = -I$(top_srcdir)/include

# user modules that have dox referenced from other user modules (docs for these must be built first)
DOX_SRC_MODULES = qlib/Mime.qm \
	qlib/MailMessage.qm \
	qlib/Util.qm \
	qlib/HttpServerUtil.qm \
	qlib/HttpServer.qm \
	qlib/WebSocketUtil.qm \
	qlib/SqlUtil.qm \
	qlib/Mapper.qm \
	qlib/ConnectionProvider.qm \
	qlib/RestSchemaValidator.qm \
	qlib/Swagger.qm \
	qlib/RestClient.qm \
	qlib/DebugUtil.qm \
	qlib/DebugProgramControl.qm \
	qlib/WebSocketHandler.qm \
	qlib/WebSocketClient.qm

DOX_MODULES = qlib/BulkSqlUtil.qm \
	qlib/CsvUtil.qm \
	qlib/DatasourceProvider.qm \
	qlib/DebugCmdLine.qm \
	qlib/DebugLinenoiseCmdLine.qm \
	qlib/DebugHandler.qm \
	qlib/Diff.qm \
	qlib/FilePoller.qm \
	qlib/FixedLengthUtil.qm \
	qlib/FreetdsSqlUtil.qm \
	qlib/Logger.qm \
	qlib/OracleSqlUtil.qm \
	qlib/PgsqlSqlUtil.qm \
	qlib/Pop3Client.qm \
	qlib/Qdx.qm \
	qlib/Qorize.qm \
	qlib/QUnit.qm \
	qlib/MysqlSqlUtil.qm \
	qlib/RestHandler.qm \
	qlib/SalesforceRestClient.qm \
	qlib/SewioRestClient.qm \
	qlib/SewioWebSocketClient.qm \
	qlib/Schema.qm \
	qlib/SchemaReverse.qm \
	qlib/SmtpClient.qm \
	qlib/TableMapper.qm \
	qlib/TelnetClient.qm \
	qlib/TextWrap.qm \
	qlib/WebUtil.qm

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
QDX_DEP = $(QDX_SRC)
else
QDX=QORE_MODULE_DIR=./qlib ./qore $(QDX_SRC)
QDX_DEP = $(QDX_SRC) qore
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
	lib/QC_AbstractLineIterator.qpp \
	lib/QC_DataLineIterator.qpp \
	lib/QC_FileLineIterator.qpp \
	lib/QC_InputStreamLineIterator.qpp \
	lib/QC_SingleValueIterator.qpp \
	lib/QC_AbstractDatasource.qpp \
	lib/QC_AbstractSQLStatement.qpp \
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
	lib/QC_ProgramControl.qpp \
	lib/QC_DebugProgram.qpp \
	lib/QC_Breakpoint.qpp \
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
	lib/QC_AbstractThreadResource.qpp \
	lib/QC_InputStream.qpp \
	lib/QC_BinaryInputStream.qpp \
	lib/QC_StringInputStream.qpp \
	lib/QC_FileInputStream.qpp \
	lib/QC_EncodingConversionInputStream.qpp \
	lib/QC_OutputStream.qpp \
	lib/QC_BinaryOutputStream.qpp \
	lib/QC_StringOutputStream.qpp \
	lib/QC_FileOutputStream.qpp \
	lib/QC_EncodingConversionOutputStream.qpp \
	lib/QC_StreamPipe.qpp \
	lib/QC_PipeInputStream.qpp \
	lib/QC_PipeOutputStream.qpp \
	lib/QC_StreamWriter.qpp \
	lib/QC_StreamReader.qpp \
	lib/QC_BufferedStreamReader.qpp \
	lib/QC_Transform.qpp \
	lib/QC_TransformInputStream.qpp \
	lib/QC_TransformOutputStream.qpp \
	lib/QC_StdoutOutputStream.qpp \
	lib/QC_StderrOutputStream.qpp \
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

MODULE_EXTRA_SRC = modules/astparser/src/QC_AstParser.qpp \
	modules/astparser/src/QC_AstTree.qpp \
	modules/astparser/src/QC_AstTreeSearcher.qpp \
	modules/astparser/src/ql_ast.qpp \
	modules/astparser/src/AstParseErrorLog.h \
	modules/astparser/src/AstParser.h \
	modules/astparser/src/AstParserHolder.h \
	modules/astparser/src/AstPrinter.h \
	modules/astparser/src/AstTreeHolder.h \
	modules/astparser/src/AstTreePrinter.h \
	modules/astparser/src/AstTreeSearcher.h \
	modules/astparser/src/QC_AstParser.h \
	modules/astparser/src/QC_AstTree.h \
	modules/astparser/src/QC_AstTreeSearcher.h \
	modules/astparser/src/ast_scanner.lpp \
	modules/astparser/src/ast_parser.ypp \
	modules/astparser/src/ql_ast.h \
	modules/astparser/src/ast/ASTSymbolKind.h \
	modules/astparser/src/ast/ASTDeclarationKind.h \
	modules/astparser/src/ast/ASTSymbolUsageKind.h \
	modules/astparser/src/ast/ASTExpressionKind.h \
	modules/astparser/src/ast/ASTStatementKind.h \
	modules/astparser/src/ast/ASTNodeType.h \
	modules/astparser/src/ast/ASTModifiers.h \
	modules/astparser/src/ast/ASTTree.h \
	modules/astparser/src/ast/ASTStatement.h \
	modules/astparser/src/ast/ASTOperator.h \
	modules/astparser/src/ast/ASTExpression.h \
	modules/astparser/src/ast/ASTSymbolInfo.h \
	modules/astparser/src/ast/ASTParseOption.h \
	modules/astparser/src/ast/ASTParseLocation.h \
	modules/astparser/src/ast/ASTName.h \
	modules/astparser/src/ast/AST.h \
	modules/astparser/src/ast/expressions/ASTRangeExpression.h \
	modules/astparser/src/ast/expressions/ASTImplicitElemExpression.h \
	modules/astparser/src/ast/expressions/ASTContextModExpression.h \
	modules/astparser/src/ast/expressions/ASTReturnsExpression.h \
	modules/astparser/src/ast/expressions/ASTTernaryExpression.h \
	modules/astparser/src/ast/expressions/ASTListExpression.h \
	modules/astparser/src/ast/expressions/ASTAccessExpression.h \
	modules/astparser/src/ast/expressions/ASTRegexTransExpression.h \
	modules/astparser/src/ast/expressions/ASTRegexExpression.h \
	modules/astparser/src/ast/expressions/ASTBackquoteExpression.h \
	modules/astparser/src/ast/expressions/ASTCaseExpression.h \
	modules/astparser/src/ast/expressions/ASTBinaryExpression.h \
	modules/astparser/src/ast/expressions/ASTNameExpression.h \
	modules/astparser/src/ast/expressions/ASTLiteralExpression.h \
	modules/astparser/src/ast/expressions/ASTConstrInitExpression.h \
	modules/astparser/src/ast/expressions/ASTCallExpression.h \
	modules/astparser/src/ast/expressions/ASTHashExpression.h \
	modules/astparser/src/ast/expressions/ASTRegexSubstExpression.h \
	modules/astparser/src/ast/expressions/ASTContextRowExpression.h \
	modules/astparser/src/ast/expressions/ASTCastExpression.h \
	modules/astparser/src/ast/expressions/ASTAssignmentExpression.h \
	modules/astparser/src/ast/expressions/ASTIndexExpression.h \
	modules/astparser/src/ast/expressions/ASTClosureExpression.h \
	modules/astparser/src/ast/expressions/ASTImplicitArgExpression.h \
	modules/astparser/src/ast/expressions/ASTFindExpression.h \
	modules/astparser/src/ast/expressions/ASTDeclExpression.h \
	modules/astparser/src/ast/expressions/ASTUnaryExpression.h \
	modules/astparser/src/ast/expressions/ASTSwitchBodyExpression.h \
	modules/astparser/src/ast/expressions/ASTHashElementExpression.h \
	modules/astparser/src/ast/expressions/ASTHashdeclHashExpression.h \
	modules/astparser/src/ast/statements/ASTRethrowStatement.h \
	modules/astparser/src/ast/statements/ASTOnBlockExitStatement.h \
	modules/astparser/src/ast/statements/ASTIfStatement.h \
	modules/astparser/src/ast/statements/ASTDoWhileStatement.h \
	modules/astparser/src/ast/statements/ASTSummarizeStatement.h \
	modules/astparser/src/ast/statements/ASTThrowStatement.h \
	modules/astparser/src/ast/statements/ASTForeachStatement.h \
	modules/astparser/src/ast/statements/ASTWhileStatement.h \
	modules/astparser/src/ast/statements/ASTExpressionStatement.h \
	modules/astparser/src/ast/statements/ASTContextStatement.h \
	modules/astparser/src/ast/statements/ASTSwitchStatement.h \
	modules/astparser/src/ast/statements/ASTForStatement.h \
	modules/astparser/src/ast/statements/ASTContinueStatement.h \
	modules/astparser/src/ast/statements/ASTReturnStatement.h \
	modules/astparser/src/ast/statements/ASTThreadExitStatement.h \
	modules/astparser/src/ast/statements/ASTBreakStatement.h \
	modules/astparser/src/ast/statements/ASTStatementBlock.h \
	modules/astparser/src/ast/statements/ASTTryStatement.h \
	modules/astparser/src/ast/statements/ASTCallStatement.h \
	modules/astparser/src/ast/ASTNode.h \
	modules/astparser/src/ast/ASTDeclaration.h \
	modules/astparser/src/ast/declarations/ASTVariableDeclaration.h \
	modules/astparser/src/ast/declarations/ASTMemberGroupDeclaration.h \
	modules/astparser/src/ast/declarations/ASTVarListDeclaration.h \
	modules/astparser/src/ast/declarations/ASTSuperclassDeclaration.h \
	modules/astparser/src/ast/declarations/ASTClassDeclaration.h \
	modules/astparser/src/ast/declarations/ASTHashMemberDeclaration.h \
	modules/astparser/src/ast/declarations/ASTClosureDeclaration.h \
	modules/astparser/src/ast/declarations/ASTNamespaceDeclaration.h \
	modules/astparser/src/ast/declarations/ASTHashDeclaration.h \
	modules/astparser/src/ast/declarations/ASTFunctionDeclaration.h \
	modules/astparser/src/ast/declarations/ASTConstantDeclaration.h \
	modules/astparser/src/ast/ASTParseError.h \
	modules/astparser/src/queries/FindMatchingSymbolsQuery.cpp \
	modules/astparser/src/queries/FindMatchingSymbolsQuery.h \
	modules/astparser/src/queries/FindNodeAndParentsQuery.cpp \
	modules/astparser/src/queries/FindNodeAndParentsQuery.h \
	modules/astparser/src/queries/FindNodeQuery.cpp \
	modules/astparser/src/queries/FindNodeQuery.h \
	modules/astparser/src/queries/FindReferencesQuery.cpp \
	modules/astparser/src/queries/FindReferencesQuery.h \
	modules/astparser/src/queries/FindSymbolInfoQuery.cpp \
	modules/astparser/src/queries/FindSymbolInfoQuery.h \
	modules/astparser/src/queries/FindSymbolsQuery.cpp \
	modules/astparser/src/queries/FindSymbolsQuery.h \
	modules/astparser/src/queries/GetNodesInfoQuery.cpp \
	modules/astparser/src/queries/GetNodesInfoQuery.h \
	modules/astparser/src/queries/SymbolInfoFixes.cpp \
	modules/astparser/src/queries/SymbolInfoFixes.h

include_HEADERS = \
	include/qore/QoreValue.h \
	include/qore/ReferenceArgumentHelper.h \
	include/qore/QoreTypeSafeReferenceHelper.h \
	include/qore/qore_thread.h \
	include/qore/AbstractThreadResource.h \
	include/qore/QoreProgram.h \
	include/qore/QoreDebugProgram.h \
	include/qore/QoreReferenceCounter.h \
	include/qore/AbstractPrivateData.h \
	include/qore/QoreIteratorBase.h \
	include/qore/QoreType.h \
	include/qore/QoreListNode.h \
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
	include/qore/QoreThreadLock.h \
	include/qore/QoreThreadLocalStorage.h \
	include/qore/QoreClass.h \
	include/qore/TypedHashDecl.h \
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
	include/qore/macros-parisc.h \
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
	include/qore/vector_map \
	include/qore/vector_set \
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
	include/qore/QoreEvents.h \
	include/qore/StreamBase.h \
	include/qore/InputStream.h \
	include/qore/OutputStream.h \
	include/qore/Transform.h \
	include/qore/AbstractException.h \
	include/qore/QoreStandardException.h

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
	include/qore/intern/RSet.h \
	include/qore/intern/AbstractIteratorHelper.h \
	include/qore/intern/ParseReferenceNode.h \
	include/qore/intern/ThreadResourceList.h \
	include/qore/intern/ssl_constants.h \
	include/qore/intern/lvalue_ref.h \
	include/qore/intern/qore_thread_intern.h \
	include/qore/intern/qore_list_private.h \
	include/qore/intern/inline_printf.h \
	include/qore/intern/qore_qf_private.h \
	include/qore/intern/qore_encoding_private.h \
	include/qore/intern/VRMutex.h \
	include/qore/intern/Variable.h \
	include/qore/intern/QoreLValue.h \
	include/qore/intern/LocalVar.h \
	include/qore/intern/QoreQueueIntern.h \
	include/qore/intern/ModuleInfo.h \
	include/qore/intern/QoreTimeZoneManager.h \
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
	include/qore/intern/NewComplexTypeNode.h \
	include/qore/intern/RWLock.h \
	include/qore/intern/SSLSocketHelper.h \
	include/qore/intern/QoreSignal.h \
	include/qore/intern/QoreRegexSubst.h \
	include/qore/intern/QoreTransliteration.h \
	include/qore/intern/QoreRegex.h \
	include/qore/intern/QoreRegexBase.h \
	include/qore/intern/QoreLibIntern.h \
	include/qore/intern/QoreGetOpt.h \
	include/qore/intern/QoreClassList.h \
	include/qore/intern/HashDeclList.h \
	include/qore/intern/typed_hash_decl_private.h \
	include/qore/intern/QException.h \
	include/qore/intern/ParserSupport.h \
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
	include/qore/intern/QoreParseListNode.h \
	include/qore/intern/QoreImplicitArgumentNode.h \
	include/qore/intern/QoreImplicitElementNode.h \
	include/qore/intern/qore_string_private.h \
	include/qore/intern/qore_number_private.h \
	include/qore/intern/qore_date_private.h \
	include/qore/intern/sql_statement_private.h \
	include/qore/intern/ThreadLocalVariableData.h \
	include/qore/intern/ThreadClosureVariableStack.h \
	include/qore/intern/qore_program_private.h \
	include/qore/intern/qore_socket_private.h \
	include/qore/intern/QoreDir.h \
	include/qore/intern/QoreTypeInfo.h \
	include/qore/intern/QoreSSLIntern.h \
	include/qore/intern/QoreObjectIntern.h \
	include/qore/intern/RSection.h \
	include/qore/intern/QoreHashNodeIntern.h \
	include/qore/intern/QoreOperatorNode.h \
	include/qore/intern/QoreDeleteOperatorNode.h \
	include/qore/intern/QoreRemoveOperatorNode.h \
	include/qore/intern/QoreSpliceOperatorNode.h \
	include/qore/intern/QoreExtractOperatorNode.h \
	include/qore/intern/QoreCastOperatorNode.h \
	include/qore/intern/QoreKeysOperatorNode.h \
	include/qore/intern/QoreUnaryMinusOperatorNode.h \
	include/qore/intern/QoreUnaryPlusOperatorNode.h \
	include/qore/intern/QoreLogicalNotOperatorNode.h \
	include/qore/intern/QoreDotEvalOperatorNode.h \
	include/qore/intern/QoreLogicalEqualsOperatorNode.h \
	include/qore/intern/QoreLogicalNotEqualsOperatorNode.h \
	include/qore/intern/QoreLogicalAbsoluteEqualsOperatorNode.h \
	include/qore/intern/QoreLogicalAbsoluteNotEqualsOperatorNode.h \
	include/qore/intern/QoreModuloOperatorNode.h \
	include/qore/intern/QoreBinaryAndOperatorNode.h \
	include/qore/intern/QoreBinaryOrOperatorNode.h \
	include/qore/intern/QoreBinaryXorOperatorNode.h \
	include/qore/intern/QoreBinaryNotOperatorNode.h \
	include/qore/intern/QoreShiftLeftOperatorNode.h \
	include/qore/intern/QoreShiftRightOperatorNode.h \
	include/qore/intern/QoreExistsOperatorNode.h \
	include/qore/intern/QoreElementsOperatorNode.h \
	include/qore/intern/QoreInstanceOfOperatorNode.h \
	include/qore/intern/QoreHashObjectDereferenceOperatorNode.h \
	include/qore/intern/QoreRegexMatchOperatorNode.h \
	include/qore/intern/QoreRegexNMatchOperatorNode.h \
	include/qore/intern/QoreRegexExtractOperatorNode.h \
	include/qore/intern/QoreRegexSubstOperatorNode.h \
	include/qore/intern/QoreTransliterationOperatorNode.h \
	include/qore/intern/QoreAssignmentOperatorNode.h \
	include/qore/intern/QoreListAssignmentOperatorNode.h \
	include/qore/intern/QorePlusEqualsOperatorNode.h \
	include/qore/intern/QoreIntPlusEqualsOperatorNode.h \
	include/qore/intern/QoreMinusEqualsOperatorNode.h \
	include/qore/intern/QoreIntMinusEqualsOperatorNode.h \
	include/qore/intern/QoreOrEqualsOperatorNode.h \
	include/qore/intern/QoreAndEqualsOperatorNode.h \
	include/qore/intern/QoreModuloEqualsOperatorNode.h \
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
	include/qore/intern/QoreFoldlOperatorNode.h \
	include/qore/intern/QoreSelectOperatorNode.h \
	include/qore/intern/QoreChompOperatorNode.h \
	include/qore/intern/QoreTrimOperatorNode.h \
	include/qore/intern/QoreSquareBracketsOperatorNode.h \
	include/qore/intern/QoreShiftOperatorNode.h \
	include/qore/intern/QoreUnshiftOperatorNode.h \
	include/qore/intern/QorePopOperatorNode.h \
	include/qore/intern/QorePushOperatorNode.h \
	include/qore/intern/QoreLogicalAndOperatorNode.h \
	include/qore/intern/QoreLogicalOrOperatorNode.h \
	include/qore/intern/QoreLogicalComparisonOperatorNode.h \
	include/qore/intern/QorePlusOperatorNode.h \
	include/qore/intern/QoreMinusOperatorNode.h \
	include/qore/intern/QoreMultiplicationOperatorNode.h \
	include/qore/intern/QoreBackgroundOperatorNode.h \
	include/qore/intern/QoreRangeOperatorNode.h \
	include/qore/intern/QoreSquareBracketsRangeOperatorNode.h \
	include/qore/intern/QorePseudoMethods.h \
	include/qore/intern/QoreHashIterator.h \
	include/qore/intern/QoreListIterator.h \
	include/qore/intern/QoreObjectIterator.h \
	include/qore/intern/QoreHashListIterator.h \
	include/qore/intern/QoreListHashIterator.h \
	include/qore/intern/WeakReferenceNode.h \
	include/qore/intern/SingleValueIterator.h \
	include/qore/intern/RangeIterator.h \
	include/qore/intern/ThreadPool.h \
	include/qore/intern/FunctionalOperatorInterface.h \
	include/qore/intern/FunctionalOperator.h \
	include/qore/intern/qore_var_rwlock_priv.h \
	include/qore/intern/qore_qd_private.h \
	include/qore/intern/BinaryInputStream.h \
	include/qore/intern/BinaryOutputStream.h \
	include/qore/intern/EncodingConversionInputStream.h \
	include/qore/intern/EncodingConversionOutputStream.h \
	include/qore/intern/FileInputStream.h \
	include/qore/intern/FileOutputStream.h \
	include/qore/intern/InputStreamLineIterator.h \
	include/qore/intern/InputStreamWrapper.h \
	include/qore/intern/OutputStreamWrapper.h \
	include/qore/intern/StreamPipe.h \
	include/qore/intern/StreamWriter.h \
	include/qore/intern/StreamReader.h \
	include/qore/intern/BufferedStreamReader.h \
	include/qore/intern/StringInputStream.h \
	include/qore/intern/StringOutputStream.h \
	include/qore/intern/TransformInputStream.h \
	include/qore/intern/TransformOutputStream.h \
	include/qore/intern/StdoutOutputStream.h \
	include/qore/intern/StderrOutputStream.h \
	include/qore/intern/StringReaderHelper.h \
	include/qore/intern/CompressionTransforms.h \
	include/qore/intern/EncryptionTransforms.h \
	include/qore/intern/IconvHelper.h \
	include/qore/intern/FileLineIterator.h \
	include/qore/intern/DataLineIterator.h \
	include/qore/intern/EncodingConvertor.h \
	include/qore/intern/QoreListNodeEvalOptionalRefHolder.h \
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
	include/qore/intern/QC_ProgramControl.h \
	include/qore/intern/QC_DebugProgram.h \
	include/qore/intern/QC_Breakpoint.h \
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
	include/qore/intern/QC_AbstractThreadResource.h \
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
	doxygen/lang/217_hashdecl.dox \
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

EXTRA_DIST = next_build.sh run_tests.sh ABOUT RELEASE-NOTES \
	AUTHORS BUILDING \
	README.md README-LICENSE README-MODULES \
	COPYING.LGPL COPYING.GPL COPYING.MIT \
	examples/test \
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
	examples/restserver.q \
	examples/schema \
	examples/stmt.q \
	examples/telnet.q \
	qore.spec \
	qore.spec-fedora \
	qore.spec-multi \
	cmake/QoreMacros.cmake \
	$(LIB_DOXYFILES_SRC) \
	$(LANG_DOXYFILES_SRC) \
	$(LANG_QPP_SRC) \
	$(MODULE_EXTRA_SRC)

dist_modver_DATA = $(USER_MODULES)

dist_qoredata_DATA = doxygen/Doxyfile.in

include $(srcdir)/qoretests.mk

# all tests
TESTS = $(qore_tests)
#TESTS = `find ./examples/test/ -name "*.qtest"`

AM_TESTS_ENVIRONMENT = QORE_MODULE_DIR='$(srcdir)/qlib'; export QORE_MODULE_DIR;

BUILT_SOURCES = dummy

dummy:
	$(top_srcdir)/next_build.sh

dist-hook:
	$(RM) -rf `find $(distdir)/examples/test -type f -a \! -name \*.qtest -a \! -name \*.qm -a \! -name Europe_Vienna`

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

QDX_MTAG_ARGS = $(foreach m,$(basename $(notdir $(DOX_SRC_MODULES))),-t$(m).tag=../../$(m)/html)

# explicit Doxyfile rules for modules providing tags for other modules
doxygen/qlib/Doxyfile.Mime: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/Mime.qm:doxygen/qlib/Mime.qm.dox.h $< $@

doxygen/qlib/Doxyfile.Util: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/Util.qm:doxygen/qlib/Util.qm.dox.h $< $@

doxygen/qlib/Doxyfile.HttpServerUtil: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/HttpServerUtil.qm:doxygen/qlib/HttpServerUtil.qm.dox.h -tMime.tag=../../Mime/html -tUtil.tag=../../Util/html $< $@

doxygen/qlib/Doxyfile.HttpServer: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/HttpServer.qm:doxygen/qlib/HttpServer.qm.dox.h -tHttpServerUtil.tag=../../HttpServerUtil/html -tMime.tag=../../Mime/html -tUtil.tag=../../Util/html $< $@

doxygen/qlib/Doxyfile.MailMessage: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/MailMessage.qm:doxygen/qlib/MailMessage.qm.dox.h -tMime.tag=../../Mime/html $< $@

doxygen/qlib/Doxyfile.SqlUtil: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/SqlUtil.qm:doxygen/qlib/SqlUtil.qm.dox.h -tUtil.tag=../../Util/html $< $@

doxygen/qlib/Doxyfile.WebSocketUtil: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/WebSocketUtil.qm:doxygen/qlib/WebSocketUtil.qm.dox.h $< $@

doxygen/qlib/Doxyfile.Mapper: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/Mapper.qm:doxygen/qlib/Mapper.qm.dox.h $< $@

doxygen/qlib/Doxyfile.ConnectionProvider: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/ConnectionProvider.qm:doxygen/qlib/ConnectionProvider.qm.dox.h -tMime.tag=../../Mime/html $< $@

doxygen/qlib/Doxyfile.RestSchemaValidator: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/RestSchemaValidator.qm:doxygen/qlib/RestSchemaValidator.qm.dox.h -tHttpServerUtil.tag=../../HttpServerUtil/html $< $@

doxygen/qlib/Doxyfile.Swagger: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/Swagger.qm:doxygen/qlib/Swagger.qm.dox.h -tMime.tag=../../Mime/html -tUtil.tag=../../Util/html -tHttpServerUtil.tag=../../HttpServerUtil/html -tRestSchemaValidator.tag=../../RestSchemaValidator/html $< $@

doxygen/qlib/Doxyfile.RestClient: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/RestClient.qm:doxygen/qlib/RestClient.qm.dox.h -tMime.tag=../../Mime/html -tConnectionProvider.tag=../../ConnectionProvider/html -tRestSchemaValidator.tag=../../RestSchemaValidator/html -tSwagger.tag=../../Swagger/html $< $@

doxygen/qlib/Doxyfile.DebugUtil: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/DebugUtil.qm:doxygen/qlib/DebugUtil.qm.dox.h $< $@

doxygen/qlib/Doxyfile.DebugProgramControl: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/DebugProgramControl.qm:doxygen/qlib/DebugProgramControl.qm.dox.h -tDebugUtil.tag=../../DebugUtil/html $< $@

doxygen/qlib/Doxyfile.WebSocketClient: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/WebSocketClient.qm:doxygen/qlib/WebSocketClient.qm.dox.h -tUtil.tag=../../Util/html -tConnectionProvider.tag=../../ConnectionProvider/html -tWebSocketUtil.tag=../../WebSocketUtil/html $< $@

doxygen/qlib/Doxyfile.WebSocketHandler: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/WebSocketHandler.qm:doxygen/qlib/WebSocketHandler.qm.dox.h -tHttpServerUtil.tag=../../HttpServerUtil/html -tWebSocketUtil.tag=../../WebSocketUtil/html $< $@

DOXYFILE_TARG = $(addprefix doxygen/qlib/Doxyfile.,$(DOX_USER_MODS))
$(DOXYFILE_TARG): doxygen/qlib/Doxyfile.%: doxygen/qlib/Doxyfile.tmpl $(QDX_DEP)
	$(QDX) -M=qlib/$*.qm:doxygen/qlib/$*.qm.dox.h $(QDX_MTAG_ARGS) $< $@

TMP_MOD_TARG = $(addprefix doxygen/qlib/,$(addsuffix .qm.dox.h,$(USER_MODS)))
$(TMP_MOD_TARG): doxygen/qlib/%.qm.dox.h: qlib/%.qm $(QDX_DEP)
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
#qr_LDADD = lib/libqore.la

ACLOCAL_AMFLAGS=-I m4

AM_CXXFLAGS = ${PTHREAD_CFLAGS}

if COND_PROFILE
AM_CXXFLAGS += -g -pg
qore_LDFLAGS=-g -pg
#qr_LDFLAGS=-g -pg
else
if COND_DEBUG
AM_CXXFLAGS += -g
qore_LDFLAGS=-g
#qr_LDFLAGS=-g
else
##qore_LDFLAGS=-s
endif
endif

bin_PROGRAMS = qore
# qr
dist_bin_SCRIPTS = doxygen/qdx bin/qget bin/rest bin/sfrest bin/sqlutil bin/schema-reverse bin/qdbg bin/qdbg-server bin/qdbg-remote
if COND_SINGLE_COMPILATION_UNIT
qore_SOURCES = qore-single-compilation-unit.cpp
#qr_SOURCES = qr-single-compilation-unit.cpp
else
qore_SOURCES = \
	qore-main.cpp \
	command-line.cpp
#qr_SOURCES = \
#	qr-main.cpp \
#	command-line.cpp
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
	./run_tests.sh

tests-ci:
	./run_tests.sh -j
