#ifdef DEBUG

#include "sybase_tests_common.h"
#include <qore/Hash.h>
#include <qore/QoreType.h>

namespace sybase_tests_6168 {

//------------------------------------------------------------------------------
static void create_numeric1_table()
{
  // scale must be always used and at least 1, the scale 0 or default will cause a failure of the driver
  const char* cmd =  "create table numeric1_table (numeric1_col numeric(18, 1) )";

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
  cmd =  "insert into numeric1_table values (1.2)";
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  cmd =  "insert into numeric1_table values (1.2)";
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
static void delete_numeric1_table(bool quiet = false)
{
  const char* cmd =  "drop table numeric1_table";

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
  delete_numeric1_table(true);
  create_numeric1_table();
  delete_numeric1_table();
}

//------------------------------------------------------------------------------
TEST()
{
  // testing numeric parameter
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_numeric1_table(true);
  create_numeric1_table();
  ON_BLOCK_EXIT(delete_numeric1_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from numeric1_table where numeric1_col = ?";
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
  l->push(new QoreNode(1.2)); 
  executor.m_args = l;

  QoreNode* n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  assert(n->val.list->size() == 2);
  if (n) n->deref(&xsink);

  QoreNode* aux = new QoreNode(l);
  aux->deref(&xsink);
}

} // namespace

namespace sybase_tests_967876891190 {

//------------------------------------------------------------------------------
static void create_numeric_table()
{
  const char* cmd =  "create table numeric_table (numeric_col numeric(30, 15) )";

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
static void delete_numeric_table(bool quiet = false)
{
  const char* cmd =  "drop table numeric_table";

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
  delete_numeric_table(true);
  create_numeric_table();
  delete_numeric_table();
}

//------------------------------------------------------------------------------
TEST()
{
  // testing numeric parameter
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_numeric_table(true);
  create_numeric_table();
  ON_BLOCK_EXIT(delete_numeric_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from numeric_table where numeric_col = ?";
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
  l->push(new QoreNode(1.111));
  executor.m_args = l;

  QoreNode* n = executor.selectRows(&xsink);
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
  delete_numeric_table(true);
  create_numeric_table();
  ON_BLOCK_EXIT(delete_numeric_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "insert into numeric_table values (?)";
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

  executor.m_parsed_query.m_result_query_text = "select count(*) from numeric_table";
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

  executor.m_parsed_query.m_result_query_text = "select * from numeric_table";
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
  x = x->val.hash->getKeyValue("numeric_col");
  assert(x);
  assert(x->type == NT_FLOAT);
  assert(x->val.floatval == 100.0);

  executor.m_parsed_query.m_result_query_text = "delete from numeric_table";
  n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_query_text = "select count(*) from numeric_table";
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

