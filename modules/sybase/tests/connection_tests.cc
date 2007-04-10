#ifdef DEBUG

#include "common.h"

namespace sybase_tests_672738 {

//------------------------------------------------------------------------------
TEST()
{
  // Basic test if we can connect. If this fails Sybase server does not run.
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  printf("connection to a database established\n");
}

} // namespace
#endif

// EOF



