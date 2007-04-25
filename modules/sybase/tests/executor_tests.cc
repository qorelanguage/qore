#ifdef DEBUG

#include "common.h"
#include "connection.h"
#include <qore/QoreType.h>

namespace sybase_tests_51256220022 {

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  conn.set_charset("utf8", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  QoreEncoding* enc = QCS_DEFAULT;//###name_to_QoreEncoding("utf8");

  QoreString cmd("select count(*) from syskeys");
  
  QoreNode* res = execute_command_impl(conn, &cmd, 0, enc, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  assert(res);
  assert(res->type == NT_HASH);
  Hash* h = res->val.hash;
  assert(h->size() == 1);

  QoreNode* val = h->getKeyValue("column1");
  assert(val);
  assert(val->type == NT_INT);
  assert(val->val.intval >= 10 && val->val.intval < 80); // 48 on my machine
  
  if (res) res->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  conn.set_charset("utf8", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  QoreEncoding* enc = QCS_DEFAULT;//###name_to_QoreEncoding("utf8");

  QoreString cmd("select * from syskeys");

  QoreNode* res = execute_command_impl(conn, &cmd, 0, enc, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  assert(res);
  assert(res->type == NT_LIST);
printf("##### %s\n", res->type->getAsString(res, 0, 0)->getBuffer());

  if (res) res->deref(&xsink);
}

} // namespace
#endif

// EOF

