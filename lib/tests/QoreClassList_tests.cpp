// Unit tests for QoreClassList
//

#ifdef DEBUG
namespace QoreClassList_tests
{

TEST()
{
  // a check whether redundant class gets rejected
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
#endif

// EOF

