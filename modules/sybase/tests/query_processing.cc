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

} // namespace
#endif

// EOF

