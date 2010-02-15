// Unit tests for Namespace.cc

#ifdef DEBUG
namespace Namespace_tests {

TEST()
{
  QoreNamespace ns("ns"); // checking that it "owns" itself
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
  // checking that QoreNamespace + NamedScope work together
  QoreNamespace ns("ns");
  NamedScope scope(strdup("bbb"));

  ExceptionSink xsink;
  QoreHashNode* h = new QoreHashNode;
  h->setKeyValue("name", new AbstractQoreNode("value"), &xsink);
  assert(!xsink);
  AbstractQoreNode* n = new AbstractQoreNode(h);

  ns.addConstant(&scope, n);
}

TEST()
{
  // check that nesting of namespaces work
  QoreNamespace* inner = new QoreNamespace("xyz");
  QoreNamespace outer("a");
  outer.addNamespace(inner);
}

} // namespace
#endif // DEBUG

// EOF

