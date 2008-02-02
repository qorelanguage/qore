#ifdef DEBUG

// Unit tests for ReferenceHolder utility
#include <qore/ReferenceHolder.h>

namespace ReferenceHolder_tests {
TEST()
{
  printf("testing ReferenceHolder 1\n");
  ReferenceHolder<AbstractQoreNode> h(new AbstractQoreNode(1.1), NULL);
  assert(h);
}

TEST()
{
  printf("testing ReferenceHolder 2\n");
  ExceptionSink xsink;
  ReferenceHolder<AbstractQoreNode> h(new AbstractQoreNode(false), &xsink);   
}

} // namespace
#endif

// EOF

