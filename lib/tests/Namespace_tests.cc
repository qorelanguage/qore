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
  // checking that Namespace + NamedScope work together
  Namespace ns("ns");
  NamedScope scope(strdup("aaa"));

  ExceptionSink xsink;
  Hash* h = new Hash;
  h->setKeyValue("name", new QoreNode("value"), &xsink);
  assert(!xsink);
  QoreNode* n = new QoreNode(h);

  ns.addConstant(&scope, n);
}

} // namespace
#endif // DEBUG

// EOF

