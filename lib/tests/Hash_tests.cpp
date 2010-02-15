// Unit tests for Hash.cc. 

#ifdef DEBUG
namespace  Hash_tests {

TEST()
{
  printf("testing QoreHashNode::derefAndDelete()\n");
  ExceptionSink xsink;
  QoreHashNode* h = new QoreHashNode;
  h->derefAndDelete(&xsink);
  assert(!xsink);

  h = new QoreHashNode;
  h->setKeyValue("aaa", new AbstractQoreNode(true), &xsink);
  assert(!xsink);
  h->setKeyValue("bbbb", new AbstractQoreNode(1.1), &xsink);
  assert(!xsink);
  h->setKeyValue("bbb", new AbstractQoreNode(0.0), &xsink); // the same key
  h->derefAndDelete(&xsink);
  assert(!xsink);
}

} // namespace
#endif // DEBUG

// EOF
