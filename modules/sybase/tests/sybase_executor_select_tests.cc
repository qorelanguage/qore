#ifdef DEBUG

#include "sybase_tests_common.h"

namespace sybase_tests_8272972100 {

//------------------------------------------------------------------------------
static void create_tinyint_table()
{
  const char* cmd =  "create table tinyint_table (tinyint_col tinyint)";

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
  cmd = "insert into tinyint_table values(111)";
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  cmd = "insert into tinyint_table values(22)";
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
static void delete_tinyint_table(bool quiet = false)
{
  const char* cmd =  "drop table tinyint_table";

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
  // testing tinyint result
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_tinyint_table(true);
  create_tinyint_table();
  ON_BLOCK_EXIT(delete_tinyint_table);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from tinyint_table where tinyint_col = ?";
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
  l->push(new QoreNode((int64)22));
  executor.m_args = l;

  QoreNode* n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  List* l2 = n->val.list;
  assert(l2->size() == 3);
  QoreNode* row = l2->retrieve_entry(0);
  assert(row->type == NT_HASH);
  Hash* h = row->val.hash;
  assert(h->size() == 1);
  QoreNode* x = h->getKeyValue("column1");
  assert(x->type == NT_INT);
  assert(x->val.intval == 22);

  if (n) n->deref(&xsink);
  QoreNode* aux = new QoreNode(l);
  aux->deref(&xsink);
}

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
  // testing smallint result
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
  cmd = "insert into int_table values(111)";
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  cmd = "insert into int_table values(222)";
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
  // testing int result 
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_int_table(true);
  create_int_table();
  ON_BLOCK_EXIT(delete_int_table);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from int_table";
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

  QoreNode* n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  List* l2 = n->val.list;
  assert(l2->size() == 2);
  QoreNode* row = l2->retrieve_entry(0);
  assert(row->type == NT_HASH);
  Hash* h = row->val.hash;
  assert(h->size() == 1);
  QoreNode* x = h->getKeyValue("column1");
  assert(x->type == NT_INT);
  assert(x->val.intval == 111 || x->val.intval == 222);

  if (n) n->deref(&xsink);
  QoreNode* aux = new QoreNode(l);
  aux->deref(&xsink);
}

//------------------------------------------------------------------------------
static void create_string_table2()
{
  const char* cmd =  "create table string_table2 (char_col CHAR(30))";

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
  cmd = "insert into string_table2 values('aaa')";
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  cmd = "insert into string_table2 values('bbb')";
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
static void delete_string_table2(bool quiet = false)
{
  const char* cmd =  "drop table string_table2";

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
/* ### - trimming
  // testing char parameter
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_string_table2(true);
  create_string_table2();
  ON_BLOCK_EXIT(delete_string_table2);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from string_table2";
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
  executor.m_args = l;

  QoreNode* n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  List* l2 = n->val.list;
  assert(l2->size() == 2);
  QoreNode* row = l2->retrieve_entry(0);
  assert(row->type == NT_HASH);
  QoreNode* x = row->val.hash->getKeyValue("column1");
  assert(x->type == NT_STRING);
  const char* s = x->val.String->getBuffer();
printf("##### READ STRING of length %d= [%s]\n", strlen(s), s);
  if (strcmp(s, "aaa") && strcmp(s, "bbb")) {
    assert(false);
  }

  if (n) n->deref(&xsink);
  QoreNode* aux = new QoreNode(l);
  aux->deref(&xsink);
*/
}

} // namespace
#endif

// EOF



