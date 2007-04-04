#ifdef DEBUG

#include "sybase_tests_common.h"
#include <qore/Hash.h>
#include <qore/QoreType.h>

namespace sybase_tests_9876212 {

//------------------------------------------------------------------------------
static void create_money_table()
{
  const char* cmd =  "create table money_table (money_col money)";

  sybase_connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
static void delete_money_table(bool quiet = false)
{
  const char* cmd =  "drop table money_table";

  sybase_connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
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
  delete_money_table(true);
  create_money_table();
  delete_money_table();
}

//------------------------------------------------------------------------------
TEST()
{
  // testing money parameter
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_money_table(true);
  create_money_table();
  ON_BLOCK_EXIT(delete_money_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_dynamic_query_text = "select * from money_table where money_col = ?";
  executor.m_parsed_query.m_is_procedure = false;

  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  executor.m_test_encoding = QCS_DEFAULT;
  executor.m_test_autocommit = false;
  executor.m_test_connection = &conn;
  List* l= new List;
  l->push(new QoreNode(1.23));
  executor.m_args = l;

  QoreNode* n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n);
  if (n) n->deref(&xsink);

  QoreNode* aux = new QoreNode(l);
  aux->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // testing insert, delete, drop etc using executor
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_money_table(true);
  create_money_table();
  ON_BLOCK_EXIT(delete_money_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_dynamic_query_text = "insert into money_table values (?)";
  executor.m_parsed_query.m_is_procedure = false;

  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_test_encoding = QCS_DEFAULT;
  executor.m_test_autocommit = false;
  executor.m_test_connection = &conn;
  List* l= new List;
  l->push(new QoreNode((int64)100)); // could be float
  executor.m_args = l;

  QoreNode* n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n);

  n = executor.exec(&xsink); // once more
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_dynamic_query_text = "select count(*) from money_table";
  executor.m_args = new List;
  n = executor.select(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  assert(n);
  assert(n->type == NT_HASH);
  QoreNode* x = n->val.hash->getKeyValue("column1");
  assert(x);
  assert(x->type == NT_INT);
  assert(x->val.intval == 2);

  executor.m_parsed_query.m_result_dynamic_query_text = "select * from money_table";
  n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  assert(n->val.list->size() == 2);
  x = n->val.list->retrieve_entry(0);
  assert(x);
  assert(x->type == NT_HASH);
  assert(x->val.hash->size() == 1);
  x = x->val.hash->getKeyValue("money_col");
  assert(x);
  assert(x->type == NT_FLOAT);
  assert(x->val.floatval == 100.0);

  executor.m_parsed_query.m_result_dynamic_query_text = "delete from money_table";
  n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_dynamic_query_text = "select count(*) from money_table";
  n = executor.select(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_HASH);
  x = n->val.hash->getKeyValue("column1");
  assert(x->type == NT_INT);
  assert(x->val.intval == 0);
}

} // namespace
#endif

// EOF

