#ifdef DEBUG

#include "common.h"

namespace sybase_tests_976592 {

//------------------------------------------------------------------------------
TEST()
{
  // test command
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
  printf("test with single command works\n");
}

//------------------------------------------------------------------------------
TEST()
{
  // test several commands together
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  command cmd1(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  command cmd2(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  command cmd3(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  printf("test with several commands works\n");
}

} // namespace
#endif

// EOF

