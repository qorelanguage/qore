qoretests =  examples/test/TestTemplateInjected.qtest \
  examples/test/qore/namespaces/ns.qtest \
  examples/test/qore/operators/logic.qtest \
  examples/test/qore/operators/broken-list-parsing.qtest \
  examples/test/qore/operators/hmap.qtest \
  examples/test/qore/operators/unary_plus_minus.qtest \
  examples/test/qore/operators/null_coalescing.qtest \
  examples/test/qore/operators/value_coalescing.qtest \
  examples/test/qore/operators/operators.qtest \
  examples/test/qore/operators/plus-eq-nothing.qtest \
  examples/test/qore/operators/assignment-op-precedence.qtest \
  examples/test/qore/operators/list_ops.qtest \
  examples/test/qore/vars/hash_time.qtest \
  examples/test/qore/vars/numbers.qtest \
  examples/test/qore/vars/constant.qtest \
  examples/test/qore/vars/int.qtest \
  examples/test/qore/vars/overload.qtest \
  examples/test/qore/vars/typecode.qtest \
  examples/test/qore/vars/hash.qtest \
  examples/test/qore/vars/getencoding.qtest \
  examples/test/qore/vars/closures.qtest \
  examples/test/qore/vars/float.qtest \
  examples/test/qore/vars/date.qtest \
  examples/test/qore/vars/brt.qtest \
  examples/test/qore/vars/array.qtest \
  examples/test/qore/vars/bare-ref-test.qtest \
  examples/test/qore/vars/argv.qtest \
  examples/test/qore/vars/statement.qtest \
  examples/test/qore/vars/members.qtest \
  examples/test/qore/vars/string.qtest \
  examples/test/qore/classes/HashListIterator/HashListIterator.qtest \
  examples/test/qore/classes/GetOpt/GetOpt.qtest \
  examples/test/qore/classes/DataLineIterator/DataLineIterator.qtest \
  examples/test/qore/classes/TreeMap/TreeMap.qtest \
  examples/test/qore/classes/HTTPClient/HTTPClient.qtest \
  examples/test/qore/classes/Program/program.qtest \
  examples/test/qore/classes/Program/lasting-subprogram-in-thread.qtest \
  examples/test/qore/classes/Dir/Dir.qtest \
  examples/test/qore/classes/Queue/Queue.qtest \
  examples/test/qore/classes/FtpClient/FtpClient.qtest \
  examples/test/qore/classes/SQLStatement/SQLStatement.qtest \
  examples/test/qore/stack/exception-location.qtest \
  examples/test/qore/stack/call-stack-overflow-exception.qtest \
  examples/test/qore/files/create-iso-8859-1-file.qtest \
  examples/test/qore/files/read.qtest \
  examples/test/qore/files/filetypes.qtest \
  examples/test/qore/functions/exists_function.qtest \
  examples/test/qore/functions/digests.qtest \
  examples/test/qore/functions/xrange.qtest \
  examples/test/qore/functions/gzip.qtest \
  examples/test/qore/functions/pwd.qtest \
  examples/test/qore/functions/glob.qtest \
  examples/test/qore/functions/parse_url.qtest \
  examples/test/qore/functions/html_encode.qtest \
  examples/test/qore/functions/bzip.qtest \
  examples/test/qore/functions/crypto.qtest \
  examples/test/qore/functions/regex_extract.qtest \
  examples/test/qore/functions/strmul.qtest \
  examples/test/qore/functions/call_builtin_function.qtest \
  examples/test/qore/functions/functiontype.qtest \
  examples/test/qore/functions/sprintf.qtest \
  examples/test/qore/functions/parse_datasource.qtest \
  examples/test/qore/functions/hmac.qtest \
  examples/test/qore/functions/system.qtest \
  examples/test/qore/functions/format_number.qtest \
  examples/test/qore/functions/ceil.qtest \
  examples/test/qore/functions/type.qtest \
  examples/test/qore/functions/stat.qtest \
  examples/test/qore/functions/floor.qtest \
  examples/test/qore/functions/parseurl.qtest \
  examples/test/qore/misc/module-loader/recursive-dependency.qtest \
  examples/test/qore/misc/module-loader/modules.qtest \
  examples/test/qore/misc/module-loader/reexport.qtest \
  examples/test/qore/misc/module-loader/private-module.qtest \
  examples/test/qore/misc/module-loader/injection.qtest \
  examples/test/qore/misc/parse_directives.qtest \
  examples/test/qore/misc/gc.qtest \
  examples/test/qore/misc/recursive.qtest \
  examples/test/qore/misc/const-init.qtest \
  examples/test/qore/misc/locals-in-class.qtest \
  examples/test/qore/misc/octal_const.qtest \
  examples/test/qore/misc/access.qtest \
  examples/test/qore/misc/thread-parsing.qtest \
  examples/test/qore/misc/class-init-order.qtest \
  examples/test/qore/misc/curly-brackets.qtest \
  examples/test/qore/misc/exception.qtest \
  examples/test/qore/misc/underscores.qtest \
  examples/test/qore/misc/socket.qtest \
  examples/test/qore/misc/context.qtest \
  examples/test/qore/misc/catch.qtest \
  examples/test/qore/misc/rwlock.qtest \
  examples/test/qore/misc/classes.qtest \
  examples/test/qore/misc/backquote.qtest \
  examples/test/qore/misc/cast.qtest \
  examples/test/qore/misc/empty_hash_ambiguity.qtest \
  examples/test/qore/misc/object.qtest \
  examples/test/qore/misc/empty_statements.qtest \
  examples/test/qore/misc/regex.qtest \
  examples/test/qore/threads/thread-object.qtest \
  examples/test/qore/threads/max-threads-count.qtest \
  examples/test/qore/threads/deadlock.qtest \
  examples/test/qore/threads/background.qtest \
  examples/test/qore/threads/unlocked-thread.qtest \
  examples/test/qore/threads/thread-resources.qtest \
  examples/test/TestTemplate.qtest \
  examples/test/qlib/FixedLengthUtil/transition_check_iterator.qtest \
  examples/test/qlib/FixedLengthUtil/FixedLengthDataIterator.qtest \
  examples/test/qlib/FixedLengthUtil/transition_check_writer.qtest \
  examples/test/qlib/FixedLengthUtil/FixedLengthFileWriter.qtest \
  examples/test/qlib/FixedLengthUtil/FixedLengthFileIterator.qtest \
  examples/test/qlib/FixedLengthUtil/FixedLengthDataWriter.qtest \
  examples/test/qlib/FixedLengthUtil/FixedLengthUtil.qtest \
  examples/test/qlib/HttpServerUtil/HttpServerUtil.qtest \
  examples/test/qlib/CsvUtil/test.qtest \
  examples/test/qlib/CsvUtil/csvutil.qtest \
  examples/test/qlib/QUnit/results.qtest \
  examples/test/qlib/QUnit/inject.qtest \
  examples/test/qlib/QUnit/tests.qtest \
  examples/test/qlib/SSL/ssl.qtest \
  examples/test/qlib/TableMapper/TableMapper.qtest \
  examples/test/qlib/TableMapper/SqlStatementOutboundMapper.qtest \
  examples/test/qlib/TableMapper/SqlStatementMapperIterator.qtest \
  examples/test/qlib/Mapper/mapper.qtest \
  examples/test/qlib/Mime/mime.qtest \
  examples/test/qlib/Diff/Diff.qtest \
  examples/test/qlib/Util/glob_to_regex.qtest \
  examples/test/qlib/Util/normalize_dir.qtest \
  examples/test/qlib/Util/padding.qtest \
  examples/test/qlib/Util/plural.qtest \
  examples/test/qlib/Util/regex_escape.qtest \
  examples/test/qlib/Util/slice.qtest \
  examples/test/qlib/Util/same.qtest \
  examples/test/qlib/Util/tmp_location.qtest \
  examples/test/qlib/Util/ordinal.qtest \
  examples/test/qlib/FilePoller/FilePoller.qtest \
  examples/test/qlib/Qorize/qorize.qtest \
  examples/test/qlib/Qorize/realWorldCase.qtest \
  examples/test/qlib/MailMessage/MailMessage.qtest \
  examples/test/qlib/HttpServer/HttpServer.qtest \
  examples/test/qlib/SqlUtil/OracleSqlUtil.qtest \
  examples/test/qlib/SqlUtil/PgsqlSqlUtil.qtest \
  examples/test/qlib/SqlUtil/MysqlSqlUtil.qtest \
  examples/test/qlib/SqlUtil/FreetdsSqlUtil.qtest
