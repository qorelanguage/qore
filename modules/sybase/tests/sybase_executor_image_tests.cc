#ifdef DEBUG

#include "sybase_tests_common.h"
#include <qore/Hash.h>
#include <qore/BinaryObject.h>

namespace sybase_tests_85242614111 {

//------------------------------------------------------------------------------
static void create_image_table()
{
  const char* cmd =  "create table image_table (image_col image )";

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
static void delete_image_table(bool quiet = false)
{
  const char* cmd =  "drop table image_table";

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
  delete_image_table(true);
  create_image_table();
  delete_image_table();
}

//------------------------------------------------------------------------------
TEST()
{
  // testing insert, delete, drop etc using executor
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_image_table(true);
  create_image_table();
  ON_BLOCK_EXIT(delete_image_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "insert into image_table (image_col) values (?)";
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
  void* block = malloc(290);
  BinaryObject* bin = new BinaryObject(block, 290);
  l->push(new QoreNode(bin));
  executor.m_args = l;

  QoreNode* n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n);
/*
  n = executor.exec(&xsink); // once more
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_args = 0;
  executor.m_parsed_query.m_result_query_text = "select count(*) from image_table";
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

  executor.m_parsed_query.m_result_query_text = "select * from image_table";
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
  x = x->val.hash->getKeyValue("image_col");
  assert(x);
  assert(x->type == NT_BINARY);
  BinaryObject* bin2 = x->val.bin;
printf("##### size of read binary is %d\n", bin2->size() );
  assert(bin2->size() == 29);
  if (memcmp(bin2->getPtr(), block, 29)) {
    assert(false);
  }

  executor.m_parsed_query.m_result_query_text = "delete from image_table";
  executor.m_args = new List;
  n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_query_text = "select count(*) from varbinary_table";
  n = executor.select(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_HASH);
  x = n->val.hash->getKeyValue("column1");
  assert(x->type == NT_INT);
  assert(x->val.intval == 0);
*/
}

} // namespace
#endif

// EOF

