#ifdef DEBUG

#include "sybase_tests_common.h"
#include <qore/QoreNode.h>
#include <math.h>

namespace sybase_tests_78620983 {

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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w1(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_command_wrapper w2(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_command_wrapper w3(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  // cleanup
  const char* cmd = "drop table my_test_253";
  sybase_low_level_execute_directly_command(conn.getConnection(), cmd, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

  // cleanup
  const char* cmd = "drop table my_test_254";
  sybase_low_level_execute_directly_command(conn.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    xsink.clear();
  }

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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
  assert(inputs[0].m_name == "");
  assert(inputs[0].m_type == CS_INT_TYPE);
  assert(inputs[0].m_max_size == sizeof(CS_INT));

  std::vector<parameter_info_t> outputs = sybase_low_level_get_output_data_info(w, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(outputs.size() > 1);
  assert(outputs[0].m_name == "id");
  assert(outputs[0].m_type == CS_INT_TYPE);
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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

  sybase_command_wrapper w(conn, &xsink);
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

} // namespace
#endif

// EOF

