#ifdef DEBUG

#include "sybase_tests_common.h"

namespace sybase_tests_672738 {

//------------------------------------------------------------------------------
TEST()
{
  // Basic test if we can connect. If this fails Sybase server does not run.
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

}

#endif

// EOF



