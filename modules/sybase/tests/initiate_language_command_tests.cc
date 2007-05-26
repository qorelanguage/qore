#ifdef DEBUG

#include "common.h"
#include "connection.h"

namespace sybase_tests_18307654002 {

//------------------------------------------------------------------------------
TEST()
{
  // test command w/o parameters
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
  printf("initiate_language_command() for simple query works\n");
}

//------------------------------------------------------------------------------
TEST()
{
  // test command with parameters
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
  cmd.initiate_language_command("select * from syskeys where id > @par1 and id < @par2", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  printf("initiate_language_command() for query with parameters works\n");
}


} // namespace
#endif

// EOF

