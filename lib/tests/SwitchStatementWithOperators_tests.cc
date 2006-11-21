// Unit tests for SwitchStatementWithOperators.cc

#ifdef DEBUG
namespace SwitchStatementWithOperators_tests {

TEST()
{
  printf("testing CaseNodeWithOperator::LessOrEqual 1\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::LessOrEqual);
  QoreNode* lhs = new QoreNode((int64)1);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::LessOrEqual 2\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::LessOrEqual);
  QoreNode* lhs = new QoreNode(1.0);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::LessOrEqual 3\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::LessOrEqual);
  QoreNode* lhs = new QoreNode((int64)0);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::LessOrEqual 4\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::LessOrEqual);
  QoreNode* lhs = new QoreNode(0.0);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator with non-numeric\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::LessOrEqual);
  QoreNode* lhs = new QoreNode("aa");
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::LessOrEqual 5\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::LessOrEqual);
  QoreNode* lhs = new QoreNode((int64)2);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::LessOrEqual 6\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::LessOrEqual);
  QoreNode* lhs = new QoreNode(2.1);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Less 1\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Less);
  QoreNode* lhs = new QoreNode((int64)1);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Less 2\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Less);
  QoreNode* lhs = new QoreNode(1.0);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Less 3\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Less);
  QoreNode* lhs = new QoreNode((int64)0);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Less 4\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Less);
  QoreNode* lhs = new QoreNode(-0.1);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Less 5\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Less);
  QoreNode* lhs = new QoreNode((int64)11);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Less 6\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Less);
  QoreNode* lhs = new QoreNode(1.01);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::GreaterOrEqual 1\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::GreaterOrEqual);
  QoreNode* lhs = new QoreNode((int64)1);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::GreaterOrEqual 2\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::GreaterOrEqual);
  QoreNode* lhs = new QoreNode(1.0);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::GreaterOrEqual 3\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::GreaterOrEqual);
  QoreNode* lhs = new QoreNode((int64)0);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::GreaterOrEqual 4\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::GreaterOrEqual);
  QoreNode* lhs = new QoreNode(-1.0);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::GreaterOrEqual 5\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::GreaterOrEqual);
  QoreNode* lhs = new QoreNode((int64)2);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::GreaterOrEqual 6\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::GreaterOrEqual);
  QoreNode* lhs = new QoreNode(2.0);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Greater 1\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Greater);
  QoreNode* lhs = new QoreNode((int64)1);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Greater 2\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Greater);
  QoreNode* lhs = new QoreNode(1.0);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Greater 3\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Greater);
  QoreNode* lhs = new QoreNode((int64)0);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Greater 4\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Greater);
  QoreNode* lhs = new QoreNode(0.0);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Greater 5\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Greater);
  QoreNode* lhs = new QoreNode((int64)2);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Greater 6\n");

  QoreNode* n = new QoreNode((int64)1);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Greater);
  QoreNode* lhs = new QoreNode(2.2);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::Greater 7\n");
  // both floats
  QoreNode* n = new QoreNode(1.0);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Greater);
  QoreNode* lhs = new QoreNode(1.0);
  bool b = cmp.matches(lhs);
  assert(!b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}


TEST()
{
  printf("testing CaseNodeWithOperator::Greater 8\n");
  // both floats
  QoreNode* n = new QoreNode(1.0);
  CaseNodeWithOperator cmp(n, 0, CaseNodeWithOperator::Greater);
  QoreNode* lhs = new QoreNode(2.0);
  bool b = cmp.matches(lhs);
  assert(b);

  ExceptionSink xsink;
  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing switch with simple relational operators 1\n");
  // test <
  char* s = "qore -e '\n"
    "$a = 1;\n"
    "switch ($a) {\n"
    "case 2: exit(11); break;\n"
    "case < 2 : exit(10); break;\n"
    "}\n"
    "exit(12);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators 1a\n");
  // test <
  char* s = "qore -e '\n"
    "$a = 1;\n"
    "switch ($a) {\n"
    "case < 2 : exit(10); break;\n"
    "}\n"
    "exit(12);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators 2\n");
   // test >
  char* s = "qore -e '\n"
    "$a = 1;\n"
    "switch ($a) {\n"
    "case 2: exit(11); break;\n"
    "case > 0 : exit(10); break;\n"
    "}\n"
    "exit(12);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators 3\n");
  // test <=, also duplicate
  char* s = "qore -e '\n"
    "$a = 99;\n"
    "switch ($a) {\n"
    "case <=99: exit(10); break;\n"
    "case >99: exit(11); break;\n"
    "}\n"
    "exit(11);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators 4\n");
  // test >=
  char* s = "qore -e '\n"
    "$a = 10;\n"
    "switch ($a) {\n"
    "case >= 1: exit(10); break;\n"
    "case > 0 : exit(11); break;\n"
    "}\n"
    "exit(11);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators 5\n");
  // test all operators together
  char* s = "qore -e '\n"
    "$a = 1;\n"
    "switch ($a) {\n"
    "case < 0: exit(11); break;\n"
    "case >1 : exit(12); break;\n"
    "case 0: exit(13);\n" // nobreak
    "case >= 2: exit(14);\n" // nobreak
    "case <= -1.0: exit(15); break;\n"
    "default: exit(10); break;\n"
    "}\n"
    "exit(16);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators 6\n");
  // test float/int mix
  char* s = "qore -e '\n"
    "$a = 1.23;\n"
    "switch ($a) {\n"
    "case >= 1: exit(10); break;\n"
    "case 1.23 : exit(11); break;\n"
    "}\n"
    "exit(11);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators 7\n");
  // test float
  char* s = "qore -e '\n"
    "$a = 1.2;\n"
    "switch ($a) {\n"
    "case 2.0: exit(11); break;\n"
    "case > 0.1 : exit(10); break;\n"
    "}\n"
    "exit(11);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators 8\n");
  // test string
  char* s = "qore -e '\n"
    "$a = \"aaa\";\n"
    "switch ($a) {\n"
    "case 2: exit(11); break;\n"
    "case > 0 : exit(11); break;\n"
    "default: exit(10); break;\n"
    "}\n"
    "exit(11);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing switch with simple relational operators and empty body\n");
  // test float/int mix
  char* s = "qore -e '\n"
    "$a = 1.23;\n"
    "switch ($a) {\n"
    "case >= 10:\n"
    "case <= 1.0:\n"
    "case >5:\n"
    "case <-0.1:\n"
    "case 22:\n"
    "  exit(11);\n"
    "case 1.23 : exit(10); break;\n"
    "}\n"
    "exit(11);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

} // namespace
#endif // DEBUG

// EOF

