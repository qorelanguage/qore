#ifdef DEBUG

// Tests for inheriting from builtin classes.

#include <qore/AbstractPrivateData.h>
#include <qore/ReferenceHolder.h>

//-----------------------------------------------------------------------------
class BuiltinInheritanceTestBase : public AbstractPrivateData
{
public:
  int a_value;
  BuiltinInheritanceTestBase() : a_value(1024) {}
  ~BuiltinInheritanceTestBase() { a_value = 0; }
};

class BuiltinInheritanceTestBase2 : public AbstractPrivateData
{
public:
  int a_value;
  BuiltinInheritanceTestBase2() : a_value(2048) {}
  ~BuiltinInheritanceTestBase2() { a_value = 0; }
};

class BuiltinInheritanceTestDescendant2 : public AbstractPrivateData
{
public:
  double dummy[1000];
  BuiltinInheritanceTestDescendant2() { 
    for (unsigned i = 0; i < 1000; ++i) dummy[i] = (double)i;
  }
  ~BuiltinInheritanceTestDescendant2() { memset(dummy, 'a', sizeof(dummy)); }
};

//-----------------------------------------------------------------------------
extern int CID_BUILTININHERITANCETESTBASE;   
extern int CID_BUILTININHERITANCETESTBASE2;
extern int CID_BUILTININHERITANCETESTDESCENDANT1;
extern int CID_BUILTININHERITANCETESTDESCENDANT2;
extern int CID_BUILTININHERITANCETESTDESCENDANT3;
extern int CID_BUILTININHERITANCETESTDESCENDANT4;
extern int CID_BUILTININHERITANCETESTDESCENDANT_MULTI;


extern QoreClass* initBuiltinInheritanceTestBaseClass();
extern QoreClass* initBuiltinInheritanceTestBase2Class();
extern QoreClass* initBuiltinInheritanceTestDescendant1(QoreClass* base);
extern QoreClass* initBuiltinInheritanceTestDescendant2(QoreClass* base);
extern QoreClass* initBuiltinInheritanceTestDescendant3(QoreClass* base);
extern QoreClass* initBuiltinInheritanceTestDescendant4(QoreClass* base);
extern QoreClass* initBuiltinInheritanceTestDescendantMulti(QoreClass* base, AbstractQoreNode* base2);


//-----------------------------------------------------------------------------
int CID_BUILTININHERITANCETESTBASE;
int CID_BUILTININHERITANCETESTBASE2;
int CID_BUILTININHERITANCETESTDESCENDANT1;
int CID_BUILTININHERITANCETESTDESCENDANT2;
int CID_BUILTININHERITANCETESTDESCENDANT3;
int CID_BUILTININHERITANCETESTDESCENDANT4;
int CID_BUILTININHERITANCETESTDESCENDANT_MULTI;

// base class -----------------------------------------------------------------
static void BUILTININHERITANCETESTBASE_constructor(QoreObject *self, AbstractQoreNode *params, ExceptionSink *xsink)
{
  BuiltinInheritanceTestBase* tst = new BuiltinInheritanceTestBase;
  self->setPrivate(CID_BUILTININHERITANCETESTBASE, tst);
}

static void BUILTININHERITANCETESTBASE_destructor(QoreObject *self, BuiltinInheritanceTestBase* test, ExceptionSink *xsink)
{
  test->deref();
}

static AbstractQoreNode* BUILTININHERITANCETESTBASE_getnum(QoreObject *self, class AbstractQoreNode *params, ExceptionSink *xsink)
{
  return new QoreBigIntNode(1);
}

//-----------------------------------------------------------------------------
QoreClass* initBuiltinInheritanceTestBaseClass()
{
  QoreClass* tst = new QoreClass("BuiltinInheritanceTestBase", QDOM_PROCESS);
  CID_BUILTININHERITANCETESTBASE = tst->getID();

  tst->setConstructor((q_constructor_t)BUILTININHERITANCETESTBASE_constructor);
  tst->setDestructor((q_destructor_t)BUILTININHERITANCETESTBASE_destructor);
  tst->addMethod("getnum", (q_method_t)BUILTININHERITANCETESTBASE_getnum);
  return tst;
}

// base2 class -----------------------------------------------------------------
static void BUILTININHERITANCETESTBASE2_constructor(QoreObject *self, AbstractQoreNode *params, ExceptionSink *xsink)
{
  BuiltinInheritanceTestBase2* tst = new BuiltinInheritanceTestBase2;
  self->setPrivate(CID_BUILTININHERITANCETESTBASE2, tst);
}

static void BUILTININHERITANCETESTBASE2_destructor(QoreObject *self, BuiltinInheritanceTestBase2* test, ExceptionSink *xsink)
{
  test->deref();
}

static AbstractQoreNode* BUILTININHERITANCETESTBASE2_getnum(QoreObject *self, class AbstractQoreNode *params, ExceptionSink *xsink)
{
  return new QoreBigIntNode(11);
}

//-----------------------------------------------------------------------------
QoreClass* initBuiltinInheritanceTestBase2Class()
{
  QoreClass* tst = new QoreClass("BuiltinInheritanceTestBase2", QDOM_PROCESS);
  CID_BUILTININHERITANCETESTBASE2 = tst->getID();

  tst->setConstructor((q_constructor_t)BUILTININHERITANCETESTBASE2_constructor);
  tst->setDestructor((q_destructor_t)BUILTININHERITANCETESTBASE2_destructor);
  tst->addMethod("getnum", (q_method_t)BUILTININHERITANCETESTBASE2_getnum);
  return tst;
}

//-----------------------------------------------------------------------------
// Just inherit, no methods, no constructor/destructor
QoreClass* initBuiltinInheritanceTestDescendant1(QoreClass* base)
{
  QoreClass* tst = new QoreClass("BuiltinInheritanceTestDescendant1", QDOM_PROCESS);
  CID_BUILTININHERITANCETESTDESCENDANT1 = tst->getID();
  tst->addDefaultBuiltinBaseClass(base);

  return tst;
}

// descendant2  ---------------------------------------------------------------
static void BUILTININHERITANCETESTDESCENDANT2_constructor(QoreObject *self, AbstractQoreNode *params, ExceptionSink *xsink)
{
  BuiltinInheritanceTestDescendant2* tst = new BuiltinInheritanceTestDescendant2;
  self->setPrivate(CID_BUILTININHERITANCETESTDESCENDANT2, tst);
}

static void BUILTININHERITANCETESTDESCENDANT2_destructor(QoreObject *self, BuiltinInheritanceTestDescendant2* test, ExceptionSink *xsink)
{
  test->deref();
}

static AbstractQoreNode* BUILTININHERITANCETESTDESCENDANT2_getnum(QoreObject *self, class AbstractQoreNode *params, ExceptionSink *xsink)
{
  return new QoreBigIntNode(2);
}

//-----------------------------------------------------------------------------
// override constructor, destructor, getnum()
QoreClass* initBuiltinInheritanceTestDescendant2(QoreClass* base)
{
  QoreClass* tst = new QoreClass("BuiltinInheritanceTestDescendant2", QDOM_PROCESS);
  CID_BUILTININHERITANCETESTDESCENDANT2 = tst->getID();
  tst->addDefaultBuiltinBaseClass(base);

  tst->setConstructor((q_constructor_t)BUILTININHERITANCETESTDESCENDANT2_constructor);
  tst->setDestructor((q_destructor_t)BUILTININHERITANCETESTDESCENDANT2_destructor);
  tst->addMethod("getnum", (q_method_t)BUILTININHERITANCETESTDESCENDANT2_getnum);

  return tst;
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* BUILTININHERITANCETESTDESCENDANT3_getnum(QoreObject *self, class AbstractQoreNode *params, ExceptionSink *xsink)
{
  // private data could be accessed

  // get direct parent
  BuiltinInheritanceTestDescendant2* d2 = (BuiltinInheritanceTestDescendant2*)
    self->getReferencedPrivateData(CID_BUILTININHERITANCETESTDESCENDANT2, xsink);
  assert(!*xsink);
  assert(d2);
  ReferenceHolder<BuiltinInheritanceTestDescendant2> d2_holder(d2, xsink);
  assert(d2->dummy[36] == 36.0);

  // get parent of parent
  BuiltinInheritanceTestBase* base = (BuiltinInheritanceTestBase*)
    self->getReferencedPrivateData(CID_BUILTININHERITANCETESTBASE, xsink);
  assert(!*xsink);
  assert(base);
  ReferenceHolder<BuiltinInheritanceTestBase> base_holder(base, xsink);
  assert(base->a_value == 1024);

  return new QoreBigIntNode(3);
}

//-----------------------------------------------------------------------------
// override getnum()
QoreClass* initBuiltinInheritanceTestDescendant3(QoreClass* base)
{
  QoreClass* tst = new QoreClass("BuiltinInheritanceTestDescendant3", QDOM_PROCESS);
  CID_BUILTININHERITANCETESTDESCENDANT3 = tst->getID();
  tst->addDefaultBuiltinBaseClass(base);

  tst->addMethod("getnum", (q_method_t)BUILTININHERITANCETESTDESCENDANT3_getnum);

  return tst;
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* BUILTININHERITANCETESTDESCENDANT4_getnum(QoreObject *self, class AbstractQoreNode *params, ExceptionSink *xsink)
{
  return new QoreBigIntNode(4);
}

//-----------------------------------------------------------------------------
// override getnum()
QoreClass* initBuiltinInheritanceTestDescendant4(QoreClass* base)
{
  QoreClass* tst = new QoreClass("BuiltinInheritanceTestDescendant4", QDOM_PROCESS);
  CID_BUILTININHERITANCETESTDESCENDANT4 = tst->getID();
  tst->addDefaultBuiltinBaseClass(base);

  tst->addMethod("getnum", (q_method_t)BUILTININHERITANCETESTDESCENDANT4_getnum);

  return tst;
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* BUILTININHERITANCETESTDESCENDANT_MULTI_getnum(QoreObject *self, class AbstractQoreNode *params, ExceptionSink *xsink)
{
  return new QoreBigIntNode(100);
}

//-----------------------------------------------------------------------------
QoreClass* initBuiltinInheritanceTestDescendantMulti(QoreClass* base, QoreClass* base2)
{
  QoreClass* tst = new QoreClass("BuiltinInheritanceTestDescendantMulti", QDOM_PROCESS);
  CID_BUILTININHERITANCETESTDESCENDANT_MULTI = tst->getID();

  // we have a method (getnum) below, but no constructor to set our own private data,
  // therefore we have to call addDefaultBuiltinBaseClass() on one of the base classes
  // to have some private data...
  //tst->addBuiltinBaseClass(base);
  tst->addBuiltinBaseClass(base2);

  // OTOH simple inheritance works
  tst->addDefaultBuiltinBaseClass(base);

  tst->addMethod("getnum", (q_method_t)BUILTININHERITANCETESTDESCENDANT_MULTI_getnum);
  return tst;
}

//-----------------------------------------------------------------------------
namespace builtin_inheritance_tests {

TEST()
{
  printf("test BuiltinInheritanceTestBase\n");
  QoreString str(
    "sub test() {"
    "$a = new BuiltinInheritanceTestBase();"
    "$num = $a.getnum();"
    "if ($num != 1) return False;"
    "delete $a;"
    "return True; }");
  
  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("test BuiltinInheritanceTestDescendant1\n");
  QoreString str(
    "sub test() {"
    "$a = new BuiltinInheritanceTestDescendant1();"
    "$num = $a.getnum();"
    "if ($num != 1) return False;"
    "delete $a;"
    "return True; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("test BuiltinInheritanceTestDescendant2\n");
  QoreString str(
    "sub test() {"
    "$a = new BuiltinInheritanceTestDescendant2();"
    "$num = $a.getnum();"
    "if ($num != 2) return False;"
    "delete $a;"
    "return True; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
/*
  printf("test BuiltinInheritanceTestDescendant3\n");
  QoreString str(
    "sub test() {"
    "$a = new BuiltinInheritanceTestDescendant3();"
    "$num = $a.getnum();"
    "if ($num != 3) return False;"
    "delete $a;"
    "return True; }");

  run_Qore_test(str, __FILE__, __LINE__);
*/
}

/* BUGBUG - base -> descA -> descB -> descC crashed, 3 levels work
TEST()
{
  printf("test BuiltinInheritanceTestDescendant4\n");
  QoreString str(
    "sub test() {"
    "$a = new BuiltinInheritanceTestDescendant4();"
    "$num = $a.getnum();"
    "if ($num != 4) return False;"
    "delete $a;"
    "return True; }");

  run_Qore_test(str, __FILE__, __LINE__);
}
*/

TEST()
{
  printf("test BuiltinInheritanceTestBase2\n");
  QoreString str(
    "sub test() {"
    "$a = new BuiltinInheritanceTestBase2();"
    "$num = $a.getnum();"
    "if ($num != 11) return False;"
    "delete $a;"
    "return True; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("test BuiltinInheritanceTestDescendantMulti\n");
  QoreString str(
    "sub test() {"
    "$a = new BuiltinInheritanceTestDescendantMulti();"
    "$num = $a.getnum();"
    "if ($num != 100) return False;"
    "delete $a;"
    "return True; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

} // namespace
#endif

// EOF

