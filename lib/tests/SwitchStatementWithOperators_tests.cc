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
  QoreString str(
    "sub test() {"
    "$a = 1;"
    "switch ($a) {"
    "case 2: return False;"
    "case < 2 : return True;"
    "}"
    "return False;"
    "}");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators 1a\n");
  // test <
  QoreString str(
    "sub test() {"
    "$a = 1;"
    "switch ($a) {"
    "case < 2 : return True;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators 2\n");
   // test >
  QoreString str(
    "sub test() {"
    "$a = 1;"
    "switch ($a) {"
    "case 2: return False;"
    "case > 0 : return True;"
    "}\n"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators 3\n");
  // test <=, also duplicate
  QoreString str(
    "sub test() {"
    "$a = 99;"
    "switch ($a) {"
    "case <=99: return True;\n"
    "case >99: return False;\n"
    "}\n"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators 4\n");
  // test >=
  QoreString str(
    "sub test() {"
    "$a = 10;"
    "switch ($a) {"
    "case >= 1: return True;"
    "case > 0 : return False;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators 5\n");
  // test all operators together
  QoreString str(
    "sub test() {"
    "$a = 1;"
    "switch ($a) {"
    "case < 0: return False;"
    "case >1 : return False;"
    "case 0: return False;"
    "case >= 2: return False;"
    "case <= -1.0: return False;"
    "default: return True;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators 6\n");
  // test float/int mix
  QoreString str(
    "sub test() {"
    "$a = 1.23;"
    "switch ($a) {"
    "case >= 1: return True;"
    "case 1.23 : return False;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators 7\n");
  // test float
  QoreString str(
    "sub test() {"
    "$a = 1.2;"
    "switch ($a) {"
    "case 2.0: return False;"
    "case > 0.1 : return True;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators 8\n");
  // test string
  QoreString str(
    "sub test() {"
    "$a = \"aaa\";"
    "switch ($a) {"
    "case 2: return False;"
    "case > 0 : return False;"
    "default: return True;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("testing switch with simple relational operators and empty body 1\n");

  // test float/int mix
  QoreString str(
    "sub test() {"
    "$a = 1.23;"
    "switch ($a) {"
    "case >= 10:"
    "case <= 1.0:"
    "case >5:"
    "case <-0.1:"
    "case 22:"
    "  return False;"
    "case 1.23 : return True;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

} // namespace
#endif // DEBUG

// EOF

