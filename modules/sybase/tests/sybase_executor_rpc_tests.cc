#ifdef DEBUG

#include "sybase_tests_common.h"
#include "../sybase_low_level_interface.h"

namespace sybase_tests_7861038972 {

//-------------------------------------------------------------------------------
static void create_test_procedure()
{
  const char* cmd =
    "create proc my_sample_rpc ("
       "@intparam int, "
       "@sintparam smallint output, "
       "@floatparam float output, "
       "@moneyparam money output, "
       "@dateparam datetime output, "
       "@charparam char(20) output, "
       "@binaryparam  binary(20) output) as "

       "select @intparam, @sintparam, @floatparam, @moneyparam, "
              "@dateparam, @charparam, @binaryparam "
              "select @sintparam = @sintparam + @intparam "
              "select @floatparam = @floatparam + @intparam "
              "select @moneyparam = @moneyparam + convert(money, @intparam) "
              "select @dateparam = getdate() "
              "select @charparam = \"The char parameters\" "
              "select @binaryparam = @binaryparam "
              "print \"This is the message printed out by sample_rpc.\"";

  sybase_connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//-------------------------------------------------------------------------------
static void create_test_procedure2()
{
  const char* cmd =
    "create proc my_sample_rpc2 ("
      "@intparam int) as "
      "print \"This is the message printed out by sample_rpc2.\"";

  sybase_connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//-------------------------------------------------------------------------------
static void drop_test_procedure(bool quiet = false)
{
  const char* cmd = "drop proc my_sample_rpc";

  sybase_connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    if (quiet) {
      xsink.clear();
    } else {
      assert(false);
    }
  }
}

//-------------------------------------------------------------------------------
static void drop_test_procedure2(bool quiet = false)
{
  const char* cmd = "drop proc my_sample_rpc2";

  sybase_connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
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
  drop_test_procedure(true);

  create_test_procedure();
  drop_test_procedure();
  create_test_procedure();
  drop_test_procedure();
  create_test_procedure();
  drop_test_procedure();
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  drop_test_procedure2(true);

  create_test_procedure2();
  drop_test_procedure2();
  create_test_procedure2();
  drop_test_procedure2();
  create_test_procedure2();
  drop_test_procedure2();
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  drop_test_procedure2(true);
  create_test_procedure2();
  ON_BLOCK_EXIT(drop_test_procedure2);

  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_command_wrapper w(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<RPC_parameter_info_t> vec;
  QoreNode* n = new QoreNode((int64)111);
  vec.push_back(RPC_parameter_info_t(CS_INT_TYPE, n));

  execute_RPC_call(w, 0, "my_sample_rpc2", vec, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  n->deref(&xsink);
}


} // namespace
#endif

// EOF



