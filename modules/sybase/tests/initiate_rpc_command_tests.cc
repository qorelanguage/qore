#ifdef DEBUG

#include "common.h"
#include "connection.h"
#include <qore/ScopeGuard.h>

namespace sybase_tests_420792 {

//-------------------------------------------------------------------------------
static void create_test_procedure()
{
  const char* cmd =
    "create proc my_sample_rpc1 ("
      "@intparam int) as "
      "print \"This is the message printed out by sample_rpc2.\"";

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
}

//-------------------------------------------------------------------------------
static void drop_test_procedure(bool quiet = false)
{
  const char* cmd = "drop proc my_sample_rpc1";

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
  // test command w/o parameters
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  drop_test_procedure(true);
  create_test_procedure();
  ON_BLOCK_EXIT(drop_test_procedure, false);

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
  cmd.initiate_rpc_command("my_sample_rpc1", &xsink);
  if (xsink.isException()) {
    assert(false);
  }  
  printf("initiate_rpc_command() works\n");
}

} // namespace
#endif

// EOF

