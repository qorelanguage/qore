// Unit tests for List.cc

#ifdef DEBUG
namespace List_tests {

TEST()
{
  printf("testing QoreListNode::derefAndDelete()\n");
  QoreListNode* l = new List;
  AbstractQoreNode* val1 = new AbstractQoreNode(true);
  AbstractQoreNode* val2 = new AbstractQoreNode(1.1);
  l->push(val1);
  l->push(val2);

  ExceptionSink xsink;
  l->derefAndDelete(&xsink);
  assert(!xsink);
}

} // namespace
#endif // DEBUG

// EOF

