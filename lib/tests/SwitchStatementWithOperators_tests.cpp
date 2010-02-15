// Unit tests for SwitchStatementWithOperators.cc

#ifdef DEBUG
namespace SwitchStatementWithOperators_tests {

TEST()
{
  printf("testing CaseNodeWithOperator::LessOrEqual 1\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LE);
  AbstractQoreNode* lhs = new QoreBigIntNode(1);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing CaseNodeWithOperator::LessOrEqual 2\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LE);
  AbstractQoreNode* lhs = new AbstractQoreNode(1.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LE 3\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LE);
  AbstractQoreNode* lhs = new QoreBigIntNode(0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LE 4\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LE);
  AbstractQoreNode* lhs = new AbstractQoreNode(0.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
/*
  printf("testing CaseNodeWithOperator with non-numeric\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LE);
  AbstractQoreNode* lhs = new AbstractQoreNode("aa");
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
*/
}

TEST()
{
  printf("testing OP_LOG_LE 5\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LE);
  AbstractQoreNode* lhs = new QoreBigIntNode(2);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LE 6\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LE);
  AbstractQoreNode* lhs = new AbstractQoreNode(2.1);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LT 1\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LT);
  AbstractQoreNode* lhs = new QoreBigIntNode(1);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LT 2\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LT);
  AbstractQoreNode* lhs = new AbstractQoreNode(1.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LT 3\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LT);
  AbstractQoreNode* lhs = new QoreBigIntNode(0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LT 4\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LT);
  AbstractQoreNode* lhs = new AbstractQoreNode(-0.1);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LT 5\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LT);
  AbstractQoreNode* lhs = new QoreBigIntNode(11);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_LT 6\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_LT);
  AbstractQoreNode* lhs = new AbstractQoreNode(1.01);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GE 1\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GE);
  AbstractQoreNode* lhs = new QoreBigIntNode(1);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GE 2\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GE);
  AbstractQoreNode* lhs = new AbstractQoreNode(1.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GE 3\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GE);
  AbstractQoreNode* lhs = new QoreBigIntNode(0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GE 4\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GE);
  AbstractQoreNode* lhs = new AbstractQoreNode(-1.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GE 5\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GE);
  AbstractQoreNode* lhs = new QoreBigIntNode(2);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GE 6\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GE);
  AbstractQoreNode* lhs = new AbstractQoreNode(2.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GT 1\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GT);
  AbstractQoreNode* lhs = new QoreBigIntNode(1);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GT 2\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GT);
  AbstractQoreNode* lhs = new AbstractQoreNode(1.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GT 3\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GT);
  AbstractQoreNode* lhs = new QoreBigIntNode(0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GT 4\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GT);
  AbstractQoreNode* lhs = new AbstractQoreNode(0.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GT 5\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GT);
  AbstractQoreNode* lhs = new QoreBigIntNode(2);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GT 6\n");

  AbstractQoreNode* n = new QoreBigIntNode(1);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GT);
  AbstractQoreNode* lhs = new AbstractQoreNode(2.2);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

  lhs->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  printf("testing OP_LOG_GT 7\n");
  // both floats
  AbstractQoreNode* n = new AbstractQoreNode(1.0);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GT);
  AbstractQoreNode* lhs = new AbstractQoreNode(1.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(!b);

  lhs->deref(&xsink);
  assert(!xsink);
}


TEST()
{
  printf("testing OP_LOG_GT 8\n");
  // both floats
  AbstractQoreNode* n = new AbstractQoreNode(1.0);
  CaseNodeWithOperator cmp(n, 0, OP_LOG_GT);
  AbstractQoreNode* lhs = new AbstractQoreNode(2.0);
  ExceptionSink xsink;
  bool b = cmp.matches(lhs, &xsink);
  assert(b);

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

