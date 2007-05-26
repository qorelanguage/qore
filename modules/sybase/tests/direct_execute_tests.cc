#ifdef DEBUG

#include "common.h"

namespace sybase_tests_9612093768 {

//------------------------------------------------------------------------------
TEST()
{
  // test direct command execution (create table, insert something, drop)
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  // cleanup
  const char* cmd = "drop table my_test_672";
  conn.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    xsink.clear();
  }

  // test start
  cmd =  "create table my_test_672 ("
    "int_col INTEGER, "
    "varchar_col VARCHAR(30))";

  conn.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  cmd = "insert into my_test_672 values(11, 'aaa')";
  conn.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  cmd = "insert into my_test_672 (varchar_col, int_col) values('bbb', 22)";
  conn.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  cmd = "commit tran";
  conn.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  cmd = "drop table my_test_672";
  conn.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  cmd = "commit tran";
  conn.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  printf("direct execution of commands works\n");
}

} // namespace
#endif

// EOF

