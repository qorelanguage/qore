#ifdef DEBUG

// Unit tests for ReferenceHolder utility
#include <qore/ReferenceHolder.h>

TEST()
{
  printf("testing ReferenceHolder\n");
  ReferenceHolder<QoreNode> h(new QoreNode(1.1));
}

#endif

// EOF

