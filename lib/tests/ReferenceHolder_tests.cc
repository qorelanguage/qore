#ifdef DEBUG

// Unit tests for ReferenceHolder utility
#include <qore/ReferenceHolder.h>

TEST()
{
  printf("testing ReferenceHolder 1\n");
  ReferenceHolder<QoreNode> h(new QoreNode(1.1));
  assert(h);
}

#endif

// EOF

