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
  QoreHash* h = new QoreHash;
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

} // namespace
#endif // DEBUG

// EOF

