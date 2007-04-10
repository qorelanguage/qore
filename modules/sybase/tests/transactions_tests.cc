#ifdef DEBUG

#include "common.h"

namespace sybase_tests_102617298 {

//------------------------------------------------------------------------------
TEST()
{
  // test commit()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  int res = commit(conn, &xsink);
  if (res != 1) {
    assert(false);
  }
  printf("commit works\n");
}

//------------------------------------------------------------------------------
TEST()
{
  // test rollback()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  int res = rollback(conn, &xsink);
  if (res != 1) {
    assert(false);
  }
  printf("rollback works\n");
}

//------------------------------------------------------------------------------
TEST()
{
  // several commit/rollbacks
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  int res = commit(conn, &xsink);
  if (res != 1) {
    assert(false);
  }
  res = commit(conn, &xsink);
  assert(res == 1);
  res = rollback(conn, &xsink);
  assert(res == 1);
  res = commit(conn, &xsink);
  assert(res == 1);
  res = rollback(conn, &xsink);
  assert(res == 1);
  res = commit(conn, &xsink);
  assert(res == 1);
  res = rollback(conn, &xsink);
  assert(res == 1);
  printf("test of several commits/rollbacks went OK\n");
}

} // namespace
#endif

// EOF

