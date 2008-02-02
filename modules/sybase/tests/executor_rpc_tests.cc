#ifdef DEBUG

#include "common.h"
#include <qore/ScopeGuard.h>
#include <cstypes.h>

namespace sybase_tests_6820236 {

//-------------------------------------------------------------------------------
static void create_test_procedure1()
{
  const char* cmd =
    "create proc my_sample_rpc1 ("
       "@intparam int, "
       "@sintparam smallint output, "
       "@floatparam float output, "
       "@moneyparam money output, "
       "@dateparam datetime output, "
       "@charparam char(20) output, "
       "@binaryparam  binary(20) output) as "

       "select @intparam, @sintparam, @floatparam, @moneyparam, "
              "@dateparam, @charparam, @binaryparam "
              "select @sintparam = @sintparam + @intparam "
              "select @floatparam = @floatparam + @intparam "
              "select @moneyparam = @moneyparam + convert(money, @intparam) "
              "select @dateparam = getdate() "
              "select @charparam = \"The char parameters\" "
              "select @binaryparam = @binaryparam "
              "print \"This is the message printed out by sample_rpc1.\"";

  connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//-------------------------------------------------------------------------------
static void create_test_procedure2()
{
  const char* cmd =
    "create proc my_sample_rpc2 ("
      "@intparam int) as "
      "print \"This is the message printed out by sample_rpc2.\"";

  connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//-------------------------------------------------------------------------------
static void create_test_procedure3()
{
  const char* cmd =
    "create proc my_sample_rpc3 ("
       "@intparam int, "
       "@ointparam int output) as "
         "select @ointparam = @intparam + @intparam "
         "print \"This is the message printed out by sample_rpc3.\"";

  connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//-------------------------------------------------------------------------------
static void drop_test_procedure1(bool quiet = false)
{
  const char* cmd = "drop proc my_sample_rpc1";

  connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    if (quiet) {
      xsink.clear();
    } else {
      assert(false);
    }
  }
}

//-------------------------------------------------------------------------------
static void drop_test_procedure2(bool quiet = false)
{
  const char* cmd = "drop proc my_sample_rpc2";

  connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    if (quiet) {
      xsink.clear();
    } else {
      assert(false);
    }
  }
}

//-------------------------------------------------------------------------------
static void drop_test_procedure3(bool quiet = false)
{
  const char* cmd = "drop proc my_sample_rpc3";

  connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  c.direct_execute(cmd, &xsink);
  if (xsink.isException()) {
    if (quiet) {
      xsink.clear();
    } else {
      assert(false);
    }
  }
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  drop_test_procedure1(true);

  create_test_procedure1();
  drop_test_procedure1();
  create_test_procedure1();
  drop_test_procedure1();
  create_test_procedure1();
  drop_test_procedure1();
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  drop_test_procedure2(true);

  create_test_procedure2();
  drop_test_procedure2();
  create_test_procedure2();
  drop_test_procedure2();
  create_test_procedure2();
  drop_test_procedure2();
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  drop_test_procedure3(true);

  create_test_procedure3();
  drop_test_procedure3();
  create_test_procedure3();
  drop_test_procedure3();
  create_test_procedure3();
  drop_test_procedure3();
}

/*
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  drop_test_procedure2(true);
  create_test_procedure2();
  ON_BLOCK_EXIT(drop_test_procedure2, false);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreString cmd("exec my_sample_rpc2 (%d)");
  QoreListNode* args = new List;
  args->push(new QoreBigIntNode(100));
  args->push(new QoreBigIntNode(CS_INT_TYPE));

  AbstractQoreNode* res = conn.exec_rpc(&cmd, args, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  if (res) res->deref(&xsink);
}
*/

/*
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  drop_test_procedure3(true);
  create_test_procedure3();
  ON_BLOCK_EXIT(drop_test_procedure3, false);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreString cmd("exec my_sample_rpc3 (%d, :sintparam)");
  QoreListNode* args = new List;
  args->push(new QoreBigIntNode(100));
  args->push(new QoreBigIntNode(CS_INT_TYPE));
  args->push(new QoreBigIntNode(CS_INT_TYPE));

  AbstractQoreNode* res = conn.exec_rpc(&cmd, args, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res);
  assert(res->type == NT_HASH);
  assert(res->val.hash->size() == 1);
  AbstractQoreNode* n = res->val.hash->getKeyValue("sintparam");
  assert(n);
  assert(n->type == NT_INT);
  assert(n->val.intval == 200);

  res->deref(&xsink);
}
*/

} // namespace
#endif

// EOF



