#ifdef DEBUG

#include "sybase_tests_common.h"
#include <qore/Hash.h>
#include <qore/List.h>
#include <qore/QoreNode.h>

namespace sybase_tests_710933832 {

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_executor::selectRows()
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select count(*) from syskeys";
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
  assert(l2->size() == 1);
  QoreNode* n2 = l2->retrieve_entry(0);
  assert(n2);
  assert(n2->type == NT_HASH);
  Hash* h2 = n2->val.hash;
  assert(h2->size() == 1);

  QoreNode* val = h2->getKeyValue("column1");
  assert(val);
  assert(val->type == NT_INT);
  assert(val->val.intval == 48); // on my machine

  n->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_executor::select()
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select count(*) from syskeys";
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

  QoreNode* n = executor.select(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);

  assert(n->type == NT_HASH);
  Hash* h2 = n->val.hash;
  assert(h2->size() == 1);

  QoreNode* val = h2->getKeyValue("column1");
  assert(val);
  assert(val->type == NT_INT);
  assert(val->val.intval == 48); // on my machine

  n->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_executor::selectRows()
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select * from syskeys";
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
  if (n->type != NT_LIST) {
    assert(false);
  }
  assert(n->val.list->size() == 48);
  
  QoreNode* row1 = n->val.list->retrieve_entry(0);
  assert(row1);
  assert(row1->type == NT_HASH);
  assert(row1->val.hash->size() == 22); // on my machine
  QoreNode* col1 = row1->val.hash->getKeyValue("column1");
  assert(col1);
  assert(col1->type == NT_INT);

  n->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_executor::selectRows()
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select count(*) from syskeys where id >= ? and id < ?";
  executor.m_parsed_query.m_is_procedure = false;
  executor.m_parsed_query.m_parameters.push_back(sybase_query_parameter());
  executor.m_parsed_query.m_parameters.push_back(sybase_query_parameter());

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
  l->push(new QoreNode((int64)0));
  l->push(new QoreNode((int64)100000000));
  executor.m_args = l;

  QoreNode* n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  assert(n->val.list->size() > 0);

  n->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_executor::selectRows()
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select id, id from syskeys";
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
  assert(n->val.list->size() > 0);

  QoreNode* row1 = n->val.list->retrieve_entry(0);
  assert(row1);
  assert(row1->type == NT_HASH);
  assert(row1->val.hash->size() == 2);

  n->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_executor::selectRows()
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  sybase_executor executor;
  executor.m_parsed_query.m_result_query_text = "select id, id from syskeys where id >= ? and id < ?";
  executor.m_parsed_query.m_is_procedure = false;
  executor.m_parsed_query.m_parameters.push_back(sybase_query_parameter());
  executor.m_parsed_query.m_parameters.push_back(sybase_query_parameter());

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
  l->push(new QoreNode((int64)0));
  l->push(new QoreNode((int64)100000000));
  executor.m_args = l;

  QoreNode* n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);

  assert(n->type == NT_LIST);
  assert(n->val.list->size() > 0);

  QoreNode* row1 = n->val.list->retrieve_entry(0);
  assert(row1);
  assert(row1->type == NT_HASH);
  assert(row1->val.hash->size() == 2);

  n->deref(&xsink);
}

} // namespace
#endif

// EOF



