#ifdef DEBUG

#include "sybase_tests_common.h"
#include <qore/Hash.h>

namespace sybase_tests_9112654251 {

//------------------------------------------------------------------------------
static void create_smallint_table()
{
  const char* cmd =  "create table smallint_table (smallint_col smallint)";

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
static void delete_smallint_table(bool quiet = false)
{
  const char* cmd =  "drop table smallint_table";

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
  delete_smallint_table(true);
  create_smallint_table();
  delete_smallint_table();
}

//------------------------------------------------------------------------------
TEST()
{
  // testing smallint parameter
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_smallint_table(true);
  create_smallint_table();
  ON_BLOCK_EXIT(delete_smallint_table);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from smallint_table where smallint_col = ?";
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
  l->push(new QoreNode((int64)1000));
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
  delete_smallint_table(true);
  create_smallint_table();
  ON_BLOCK_EXIT(delete_smallint_table);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "insert into smallint_table values (?)";
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
  l->push(new QoreNode((int64)12345));
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

  executor.m_parsed_query.m_result_query_text = "insert into smallint_table values (46)";
  executor.m_args = new List;
  n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_query_text = "select count(*) from smallint_table";
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

  executor.m_parsed_query.m_result_query_text = "select * from smallint_table";
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
  x = x->val.hash->getKeyValue("smallint_col");
  assert(x);
  assert(x->type == NT_INT);
  assert(x->val.intval == 12345 || x->val.intval == 46);

  executor.m_parsed_query.m_result_query_text = "delete from smallint_table";
  executor.m_args = new List;
  n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_query_text = "select count(*) from smallint_table";
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

namespace sybase_tests_6737363463700 {

//------------------------------------------------------------------------------
static void create_smallint_table()
{
  const char* cmd =  "create table smallint_table (smallint_col1 smallint, smallint_col2 smallint)";

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
  cmd = "insert into smallint_table values(111, 2222)";
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  cmd = "insert into smallint_table values(111, 4444)";
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_commit(&c, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
static void delete_smallint_table(bool quiet = false)
{
  const char* cmd =  "drop table smallint_table";

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
  // more complex smallint test
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_smallint_table(true);
  create_smallint_table();
  ON_BLOCK_EXIT(delete_smallint_table);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from smallint_table where smallint_col1 = ? and smallint_col2 = ?";
  executor.m_parsed_query.m_parameters.push_back(sybase_query_parameter());
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
  l->push(new QoreNode((int64)111));
  l->push(new QoreNode((int64)2222));
  executor.m_args = l;

  QoreNode* n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  List* l2 = n->val.list;
  assert(l2->size() == 1);

  if (n) n->deref(&xsink);
  QoreNode* aux = new QoreNode(l);
  aux->deref(&xsink);
}

} // namespace
#endif

// EOF

