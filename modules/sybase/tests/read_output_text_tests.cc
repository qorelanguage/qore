#ifdef DEBUG

#include "common.h"
#include "connection.h"
#include "initiate_language_command.h"
#include "send_command.h"
#include <qore/charset.h>
#include <qore/QoreType.h>
#include <qore/ScopeGuard.h>

namespace sybase_tests_5102086165 {

//------------------------------------------------------------------------------
static void create_text_table()
{
  const char* cmd =  "create table text_table (text_col text )";

  connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  cmd = "insert into text_table values ('kjhskjhkghkjghiugyiugh')";
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
static void delete_text_table(bool quiet = false)
{
  const char* cmd =  "drop table text_table";

  connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    if (quiet) {
      xsink.clear();
    } else {
      assert(false);
    }
  }
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_text_table(true);
  create_text_table();
  ON_BLOCK_EXIT(delete_text_table, false);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  command cmd(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  initiate_language_command(cmd, "select * from text_table", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  send_command(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* result = read_output(conn, cmd, QCS_DEFAULT, true, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(result);
  assert(result->type == NT_HASH);
  assert(result->val.hash->size() == 1);
  
  result->deref(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  printf("test for text datatype is OK\n");
}

} // namespace
#endif

// EOF

