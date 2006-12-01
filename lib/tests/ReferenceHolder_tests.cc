#ifdef DEBUG

// Unit tests for ReferenceHolder utility
#include <qore/ReferenceHolder.h>

namespace ReferenceHolder_tests {
TEST()
{
  printf("testing ReferenceHolder 1\n");
  ReferenceHolder<QoreNode> h(new QoreNode(1.1));
  assert(h);
}

} // namespace
#endif

// EOF

