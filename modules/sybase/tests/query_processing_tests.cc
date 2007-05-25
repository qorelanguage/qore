#ifdef DEBUG

#include "common.h"

namespace sybase_tests_96142972 {

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  assert(is_query_procedure_call("EXEC foo"));
  assert(is_query_procedure_call(" Execute foo(%v, %v, :aaa)  "));
  assert(is_query_procedure_call("exec foo(:aaa)"));
  assert(!is_query_procedure_call("foo"));
}

//------------------------------------------------------------------------------
TEST()
{
  // test the routing making @parX from %v and %d
  ExceptionSink xsink;

  const char* cmd = "select x from y";
  processed_language_command_t res;
  res.init(cmd, 0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!res.m_cmd.compare("select x from y"));
  assert(res.m_parameter_types.empty());

  cmd = "select x from %v";
  res.init(cmd, 0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!res.m_cmd.compare("select x from @par1"));
  assert(res.m_parameter_types.size() == 1);
  assert(res.m_parameter_types[0] == 'v');

  cmd = "select x from %v where z = %d";
  res.init(cmd, 0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!res.m_cmd.compare("select x from @par1 where z = @par2"));
  assert(res.m_parameter_types.size() == 2);
  assert(res.m_parameter_types[0] == 'v');
  assert(res.m_parameter_types[1] == 'd');

  // test that inner strings work
  cmd = "select x from \"%v\" and '%d'";
  res.init(cmd, 0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_cmd.compare("select x from \"%v\" and '%d'"));
  assert(res.m_parameter_types.empty());

  cmd = "elect x from \"\\\"?\" and '\\\'?'";
  res.init(cmd, 0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_cmd.compare("elect x from \"\\\"?\" and '\\\'?'"));
  assert(res.m_parameter_types.empty());
}

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  processed_procedure_call_t res = process_procedure_call("exec foo", &xsink);
  assert(!xsink.isException());
  assert(res.m_cmd == "foo");
  assert(res.m_parameters.empty());

  res = process_procedure_call("execute  foo ", &xsink);
  assert(!xsink.isException());
  assert(res.m_cmd == "foo");
  assert(res.m_parameters.empty());

  res = process_procedure_call("execute  foo ( )", &xsink);
  assert(!xsink.isException());
  assert(res.m_cmd == "foo");
  assert(res.m_parameters.empty());

  res = process_procedure_call("execute  foo()", &xsink);
  assert(!xsink.isException());
  assert(res.m_cmd == "foo");
  assert(res.m_parameters.empty());

  res = process_procedure_call("execute  foo(%d, %v,%v,:abc,:def,  :ghi , %d)", &xsink);
  assert(!xsink.isException());
  assert(res.m_cmd == "foo");
  assert(res.m_parameters.size() == 7);

  assert(res.m_parameters[0].first == false);
  assert(res.m_parameters[0].second == "d");
  assert(res.m_parameters[1].first == false);
  assert(res.m_parameters[1].second == "v");
  assert(res.m_parameters[2].first == false);
  assert(res.m_parameters[2].second == "v");
  assert(res.m_parameters[3].first == true);
  assert(res.m_parameters[3].second == "abc");
  assert(res.m_parameters[4].first == true);
  assert(res.m_parameters[4].second == "def");
  assert(res.m_parameters[5].first == true);
  assert(res.m_parameters[5].second == "ghi");
  assert(res.m_parameters[6].first == false);
  assert(res.m_parameters[6].second == "d");
}

} // namespace
#endif

// EOF

