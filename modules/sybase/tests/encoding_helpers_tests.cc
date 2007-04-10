#ifdef DEBUG

#include "common.h"

namespace sybase_tests_10626 {

//------------------------------------------------------------------------------
#ifdef SYBASE
// FreeTDS 0.64 returns empty string as encoding

TEST()
{
  // test sybase_low_level_get_default_encoding()
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  for (unsigned i = 0; i < 100; ++i) {
    std::string s = get_default_Sybase_encoding(conn, &xsink);
    if (xsink.isException()) {
      assert(false);
    }
    assert(s == "utf8"); // on my machine
  }
  printf("default encoding chheck went OK\n");
}
#endif

} // namespace
#endif

// EOF

