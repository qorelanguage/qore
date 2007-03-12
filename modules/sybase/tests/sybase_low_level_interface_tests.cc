#ifdef DEBUG

#include "sybase_tests_common.h"

namespace sybase_tests_78620983 {

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_commit()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  int res = sybase_low_level_commit(&conn, &xsink);
  if (res != 1) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_rollback()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  int res = sybase_low_level_rollback(&conn, &xsink);
  if (res != 1) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // several commit/rollbacks
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  int res = sybase_low_level_commit(&conn, &xsink);
  if (res != 1) {
    assert(false);
  }
  res = sybase_low_level_commit(&conn, &xsink);
  assert(res == 1);
  res = sybase_low_level_rollback(&conn, &xsink);
  assert(res == 1);
  res = sybase_low_level_commit(&conn, &xsink);
  assert(res == 1);
  res = sybase_low_level_rollback(&conn, &xsink);
  assert(res == 1);
  res = sybase_low_level_commit(&conn, &xsink);
  assert(res == 1);
  res = sybase_low_level_rollback(&conn, &xsink);
  assert(res == 1);
}

//------------------------------------------------------------------------------
TEST()
{
  // test direct command execution (create table, insert something, drop)
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  // cleanup
  const char* cmd = "drop table my_test_672";
  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    xsink.clear();
  }

  // test start
  cmd =  "create table my_test_672 ("
    "int_col INTEGER, "
    "varchar_col VARCHAR(30))";

  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  cmd = "insert into my_test_672 values(11, 'aaa')";
  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  cmd = "insert into my_test_672 (varchar_col, int_col) values('bbb', 22)";
  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  int res = sybase_low_level_commit(&conn, &xsink);
  assert(res == 1);

  cmd = "drop table my_test_672";
  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  res = sybase_low_level_commit(&conn, &xsink);
  assert(res == 1);
}

//------------------------------------------------------------------------------
TEST()
{
  // test failing direct command execution
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  // cleanup
  const char* cmd = "drop table my_test_173";
  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    xsink.clear();
  }

  cmd =  "create table my_test_173 ("
    "int_col INTEGER, "
    "varchar_col VARCHAR(30))";

  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  int res = sybase_low_level_commit(&conn, &xsink);
  assert(res == 1);

  cmd = "select count(*) from my_test_173";
  for (int i = 0; i < 100; ++i) {
    sybase_low_level_execute_directly_command(c, cmd, &xsink);
    if (xsink.isException()) {
      xsink.clear();
    } else {
      assert(false);
    }
  }

  cmd = "drop table my_test_173";
  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  res = sybase_low_level_commit(&conn, &xsink);
  assert(res == 1);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_command_wrapper class
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // test several sybase_command_wrappers together
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w1(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_command_wrapper w2(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_command_wrapper w3(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_prepare_command() for a command
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  // cleanup
  const char* cmd = "drop table my_test_253";
  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    xsink.clear();
  }

  sybase_low_level_prepare_command(w, "create table my_test_253 (int_col INTEGER)", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_prepare_command() for a select
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "select * from syskeys", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_prepare_command() for a procedure
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "exec sp_transactions", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_input_parameters_info() for a command
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  // cleanup
  const char* cmd = "drop table my_test_254";
  sybase_low_level_execute_directly_command(c, cmd, &xsink);
  if (xsink.isException()) {
    xsink.clear();
  }

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "create table my_test_254 (int_col INTEGER)", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> inputs = sybase_low_level_get_input_parameters_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 0);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_input_parameters_info() 
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "select * from syskeys where id = 1", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> inputs = sybase_low_level_get_input_parameters_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 0);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_input_parameters_info()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "select * from syskeys where id = ?", &xsink); // 1 input parameter
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> inputs = sybase_low_level_get_input_parameters_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 1);
  assert(inputs[0].m_name == "unnamed parameter #1");
  assert(inputs[0].m_type == CS_INT_TYPE);
  assert(inputs[0].m_max_size == sizeof(CS_INT));
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_input_parameters_info() 
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "select * from syskeys where id = ? or id > ?", &xsink); // 2 input parameters
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> inputs = sybase_low_level_get_input_parameters_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 2);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_input_parameters_info()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "exec sp_help", &xsink); 
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> inputs = sybase_low_level_get_input_parameters_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 0);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_input_parameters_info()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "exec sp_drop_qplan 11111", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> inputs = sybase_low_level_get_input_parameters_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 0);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_input_parameters_info()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "execute sp_drop_qplan", &xsink); // parameter omitted
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> inputs = sybase_low_level_get_input_parameters_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 0);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_output_data_info() for a command
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "create table my_test_253 (int_col INTEGER)", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> outputs = sybase_low_level_get_output_data_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(outputs.size() == 0);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_output_data_info()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "select * from syskeys where id = 1", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> outputs = sybase_low_level_get_output_data_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(outputs.size() == 22); // on my machine
  assert(outputs[0].m_name == "id");
  assert(outputs[0].m_type == CS_INT_TYPE);
  assert(outputs[0].m_max_size == sizeof(CS_INT));
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_output_data_info()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  sybase_low_level_prepare_command(w, "execute sp_transactions", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<parameter_info_t> outputs = sybase_low_level_get_output_data_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(outputs.size() == 0);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_low_level_get_default_encoding()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  for (unsigned i = 0; i < 100; ++i) {
    std::string s = sybase_low_level_get_default_encoding(conn, &xsink);
    if (xsink.isException()) {
      assert(false);
    }
    assert(s == "utf8"); // on my machine
  }
}

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
  CS_CONNECTION* c = conn.getConnection();

  sybase_command_wrapper w(c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<RPC_parameter_info_t> vec;
  CS_INT val = 100;
  vec.push_back(RPC_parameter_info_t(CS_INT_TYPE, sizeof(CS_INT), &val));

  execute_RPC_call(w, "my_sample_rpc2", vec, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
} 

} // namespace
#endif

// EOF

