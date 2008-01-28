// Unit tests for QoreString.cc

#ifdef DEBUG
namespace QoreString_tests {

TEST()
{
  printf("testing QoreString::QoreString(bool)\n");
  QoreString s1(true);
  assert(!strcmp(s1.getBuffer(), "1"));
  QoreString s2(false);
  assert(!strcmp(s2.getBuffer(), "0"));
}

TEST()
{
  printf("testing QoreString::QoreString(BinaryNode*)\n");
  BinaryNode b1;
  void* p = malloc(100);
  BinaryNode b2(p, 100);
  QoreString s1(&b1);
  QoreString s2(&b2);
}

TEST()
{
  printf("testing QoreString::concatAndHTMLEncode()\n");
  QoreString s("<>&\"<<>>&&\"\"< > & \" ");
  ExceptionSink xsink;

  QoreString s2;
  s2.concatAndHTMLEncode(&s, &xsink);
  assert(!xsink);
  if (strcmp(s2.getBuffer(), "&lt;&gt;&amp;&quot;&lt;&lt;&gt;&gt;&amp;&amp;&quot;&quot;&lt; &gt; &amp; &quot; ")) {
    assert(false);
  }

  QoreString s3("aaaa");
  s3.concatAndHTMLEncode(&s, &xsink);
  assert(!xsink);
  if (strcmp(s3.getBuffer(), "aaaa&lt;&gt;&amp;&quot;&lt;&lt;&gt;&gt;&amp;&amp;&quot;&quot;&lt; &gt; &amp; &quot; ")) {
    assert(false);
  }
}

TEST()
{
  printf("testing QoreString::concatAndHTMLDecode()\n");
  QoreString s1("&lt;&gt;&amp;&quot;&lt;&lt;&gt;&gt;&amp;&amp;&quot;&quot;&lt; &gt; &amp; &quot; ");

  QoreString s2;
  s2.concatAndHTMLDecode(&s1);
  if (strcmp(s2.getBuffer(), "<>&\"<<>>&&\"\"< > & \" ")) {
    assert(false);
  }

  QoreString s3("bbbb");
  s3.concatAndHTMLDecode(&s1);
  if (strcmp(s3.getBuffer(), "bbbb<>&\"<<>>&&\"\"< > & \" ")) {
    assert(false);
  }
}

TEST()
{
  printf("testing QoreString::ensureBufferSize()\n");
  QoreString s;
  s.ensureBufferSize(0);
  s.ensureBufferSize(100);
  s.ensureBufferSize(10);
  int x = 1234;
  s.ensureBufferSize(x);
}

TEST()
{
  printf("running test in %s[%d]\n", __FILE__, __LINE__);
  QoreString s1;
  assert(s1.length() == 0);
  s1.clear();
  assert(s1.length() == 0);
  s1.set("xyz");
  assert(s1.length() == 3);

  QoreString s2("aaa");
  assert(s2.length() == 3);
  s2.clear();
  assert(s2.length() == 0);
  s2.set("bbbb");
  assert(s2.length() == 4);
  s2.clear();
  assert(s2.length() == 0);
}

} // namespace

#endif // DEBUG


// EOF

