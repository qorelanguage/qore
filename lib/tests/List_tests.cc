// Unit tests for List.cc

#ifdef DEBUG
namespace List_tests {

TEST()
{
  printf("testing List::derefAndDelete()\n");
  List* l = new List;
  QoreNode* val1 = new QoreNode(true);
  QoreNode* val2 = new QoreNode(1.1);
  l->push(val1);
  l->push(val2);

  ExceptionSink xsink;
  l->derefAndDelete(&xsink);
  assert(!xsink);
}

} // namespace
#endif // DEBUG

// EOF

