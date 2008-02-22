#ifdef DEBUG

#include "common.h"
#include "connection.h"

namespace sybase_tests_9752936571 {

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
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
  cmd.initiate_language_command("select count(*) from syskeys", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  cmd.send(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  AbstractQoreNode* result = read_output(conn, cmd, QCS_DEFAULT, true, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(result);
  assert(result->getType() == NT_HASH);
  assert(result->val.hash->size() == 1); // one column
  AbstractQoreNode* col = result->val.hash->getKeyValue("column1");
  assert(col);
  assert(col->getType() == NT_INT);
  int n = (int)col->val.intval; // 22 for Sybase on my machine
  assert(n >= 10 && n <= 60);
  
  result->deref(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
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
  cmd.initiate_language_command("select * from syskeys", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  cmd.send(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  AbstractQoreNode* result = read_output(conn, cmd, QCS_DEFAULT, true, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(result);
  assert(result->getType() == NT_LIST);
  int n = (int)result->val.list->size(); // 48 for sybase on my machine
  assert(n >= 30 && n <= 100);

  result->deref(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  printf("Simple read_output test is OK\n");
}

} // namespace
#endif

// EOF

