// Unit tests for Hash.cc. 

#ifdef DEBUG
namespace  Hash_tests {

TEST()
{
  printf("testing QoreHash::derefAndDelete()\n");
  ExceptionSink xsink;
  QoreHash* h = new QoreHash;
  h->derefAndDelete(&xsink);
  assert(!xsink);

  h = new QoreHash;
  h->setKeyValue("aaa", new QoreNode(true), &xsink);
  assert(!xsink);
  h->setKeyValue("bbbb", new QoreNode(1.1), &xsink);
  assert(!xsink);
  h->setKeyValue("bbb", new QoreNode(0.0), &xsink); // the same key
  h->derefAndDelete(&xsink);
  assert(!xsink);
}

} // namespace
#endif // DEBUG

// EOF
