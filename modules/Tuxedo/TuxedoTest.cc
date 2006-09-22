#include <qore/common.h>
#include <qore/support.h>
#include <qore/params.h>
#include <qore/ReferenceObject.h>

#include "TuxedoTest.h"
extern "C" {
#include "atmi.h"
}

int CID_TUXEDOTEST;

//------------------------------------------------------------------------------
class QoreTuxedoTest : public ReferenceObject
{
public:
  QoreTuxedoTest() { printf("constructor called\n"); }
  ~QoreTuxedoTest() { printf("destructor called\n"); }

  void doit() {	
    printf("##################### Tuxedo Test running\n");
    char* sendbuf = (char*)tpalloc("STRING", 0, 5);
    char* rcvbuf = (char*)tpalloc("STRING", 0, 5);
    strcpy(sendbuf, "test");
    long rcvlen;
    int ret = tpcall("TOUPPER", sendbuf, 0, &rcvbuf, &rcvlen, 0);
    if (ret == -1) {	
        printf("##### problem with Tuxedo\n");
    } else {
        printf("##### works, returned %s\n", rcvbuf);
    }
    tpfree(sendbuf);
    tpfree(rcvbuf);
  }

  void deref() {
    printf("DEREF called\n");
    if (ROdereference()) {
      delete this;
    }
  }
};

//-----------------------------------------------------------------------------
static void getTuxedoTest(void* obj)
{
printf("getTuxedoTest called\n");
  ((QoreTuxedoTest*)obj)->ROreference();
}

static void releaseTuxedoTest(void* obj)
{
printf("releaseTuxedoTest called\n");
  ((QoreTuxedoTest*)obj)->deref();
}

//-----------------------------------------------------------------------------
static void TUXEDOTEST_constructor(class Object* self, class QoreNode* params, class ExceptionSink* xsink)
{
printf("TUXEDOTEST_constructor called\n");
  tracein("TUXEDOTEST_constructor");
  QoreTuxedoTest* t = new QoreTuxedoTest();
  if (xsink->isException()) {
     t->deref();
  } else {
     self->setPrivate(CID_TUXEDOTEST, t, getTuxedoTest, releaseTuxedoTest);
  } 
  traceout("TUXEDOTEST_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOTEST_destructor(class Object* self, class QoreTuxedoTest* t, class ExceptionSink* xsink)
{
printf("TUXEDOTEST_destructor called\n");
  tracein("TUXEDOTEST_destructor");
  t->deref(); 
  tracein("TUXEDOTEST_destructor");
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOTEST_doit(class Object* self, class QoreTuxedoTest* t, QoreNode* params, ExceptionSink* xsink)
{
  t->doit();
  return NULL;
}

//------------------------------------------------------------------------------
static bool foo()
{
   printf("MODULE TUXEDO LOADED\n");
   return 0;
}
static bool dummy = foo();

class QoreClass* initTuxedoTestClass()
{
printf("ENTERING initTuxedoTestClass\n");
  tracein("iniTuxedoTestClass");
  class QoreClass* t = new QoreClass(QDOM_NETWORK, strdup("TuxedoTest"));
  CID_TUXEDOTEST = t->getID();
  t->setConstructor((q_constructor_t)TUXEDOTEST_constructor);
  t->setDestructor((q_destructor_t)TUXEDOTEST_destructor);
  t->addMethod("doit", (q_method_t)TUXEDOTEST_doit);
  traceout("iniTuxedoTest");
  return t;
}

// EOF

