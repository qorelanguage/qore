#ifdef DEBUG

#include "sybase_tests_common.h"
#include <qore/Hash.h>

namespace sybase_tests_51415257152520 {

//------------------------------------------------------------------------------
static void create_int_table()
{
  const char* cmd =  "create table int_table (int_col int)";

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
static void delete_int_table(bool quiet = false)
{
  const char* cmd =  "drop table int_table";

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
  delete_int_table(true);
  create_int_table();
  delete_int_table();
}

//------------------------------------------------------------------------------
TEST()
{
  // testing tinyint parameter
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_int_table(true);
  create_int_table();
  ON_BLOCK_EXIT(delete_int_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from int_table where int_col = ?";
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
  l->push(new QoreNode((int64)10));
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
  delete_int_table(true);
  create_int_table();
  ON_BLOCK_EXIT(delete_int_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "insert into int_table values (?)";
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
  l->push(new QoreNode((int64)123000));
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

  executor.m_parsed_query.m_result_query_text = "insert into int_table values (46)";
  executor.m_args = new List;
  n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_query_text = "select count(*) from int_table";
  n = executor.select(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_HASH);
  QoreNode* x = n->val.hash->getKeyValue("column1");
  assert(x);
  assert(x->type == NT_INT);
  assert(x->val.intval == 3);

  executor.m_parsed_query.m_result_query_text = "select * from int_table";
  n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  assert(n->val.list->size() == 3);
  x = n->val.list->retrieve_entry(0);
  assert(x);
  assert(x->type == NT_HASH);
  assert(x->val.hash->size() == 1);
  x = x->val.hash->getKeyValue("int_col");
  assert(x);
  assert(x->type == NT_INT);
  assert(x->val.intval == 123000 || x->val.intval == 46);

  executor.m_parsed_query.m_result_query_text = "delete from int_table";
  executor.m_args = new List;
  n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_query_text = "select count(*) from int_table";
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

//------------------------------------------------------------------------------
TEST()
{
  // testing generated name for calculated output
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_int_table(true);
  create_int_table();
  ON_BLOCK_EXIT(delete_int_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "insert into int_table values(1)";
  executor.m_parsed_query.m_is_procedure = false;

  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  executor.m_test_encoding = QCS_DEFAULT;
  executor.m_test_autocommit = true;
  executor.m_test_connection = &conn;
  List* l= new List;
  executor.m_args = l;
  QoreNode* n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n);

  executor.m_parsed_query.m_result_query_text = "select 2 * int_col  from int_table";
  n = executor.select(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_HASH);
  Hash* h = n->val.hash;
  assert(h->size() == 1);
  QoreNode* x = h->getKeyValue("column1"); // always generated!
  assert(x->type == NT_INT);
  assert(x->val.intval == 2);
}

} // namespace
#endif

// EOF

