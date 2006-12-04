// Unit tests for Namespace.cc

#ifdef DEBUG
namespace Namespace_tests {

TEST()
{
  Namespace ns("ns"); // checking that it "owns" itself
}

TEST()
{
  NamedScope ns(strdup("aaa")); // checking that it "owns" itself
}

TEST()
{
  NamedScope ns(strdup("aaa::bbb")); // checking complex name
}

TEST()
{
  // checking that Namespace + NamedScope work together
  Namespace ns("ns");
  NamedScope scope(strdup("bbb"));

  ExceptionSink xsink;
  Hash* h = new Hash;
  h->setKeyValue("name", new QoreNode("value"), &xsink);
  assert(!xsink);
  QoreNode* n = new QoreNode(h);

  ns.addConstant(&scope, n);
}

TEST()
{
  // check that nesting of namespaces work
  Namespace* inner = new Namespace("xyz");
  Namespace outer("a");
  outer.addNamespace(inner);
}

TEST()
{
  printf("tetsing QoreClass 1\n");
  QoreClass* c = new QoreClass(strdup("aaa"));
  delete c;
}

TEST()
{
  printf("testing adding QoreClass into QoreClassList\n");
  QoreClass* c1 = new QoreClass(strdup("aaa"));
  QoreClass* c2 = new QoreClass(strdup("aaa"));
  QoreClass* c3 = new QoreClass(strdup("bbb"));

  QoreClassList l;
  int res = l.add(c1);
  assert(!res);
  res = l.add(c3);
  assert(!res);
  res = l.add(c2);  
  assert(res); // should not be added
  delete c2;
  res = l.add(c1);
  assert(res); // should not be added as well
}

} // namespace
#endif // DEBUG

// EOF

