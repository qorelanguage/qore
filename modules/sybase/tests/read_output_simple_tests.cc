#ifdef DEBUG

#include "common.h"
#include "connection.h"
#include "initiate_language_command.h"
#include "send_command.h"

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
  initiate_language_command(cmd, "select count(*) from syskeys", &xsink);
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
  assert(result->val.hash->size() == 1); // one column
  QoreNode* col = result->val.hash->getKeyValue("column1");
  assert(col);
  assert(col->type == NT_INT);
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
  initiate_language_command(cmd, "select * from syskeys", &xsink);
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
  assert(result->type == NT_LIST);
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

