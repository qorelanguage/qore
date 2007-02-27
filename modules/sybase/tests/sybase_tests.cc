#ifdef DEBUG
namespace sybase_tests_962876 {

//------------------------------------------------------------------------------
TEST()
{
  // basic test if we can connect
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // test whether parseQuery() works
  ExceptionSink xsink;
  QoreString str("select cound(*) from a_table where num = %v and value = %v");
  SybaseBindGroup grp(&str);

  List* lst = new List;
  lst->push(new QoreNode("aaa"));
  lst->push(new QoreNode("bbb"));

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  unsigned size = grp.m_input_parameters.size();
  assert(size == 2);

  char* new_str = grp.m_cmd->getBuffer();
  if (strcmp(new_str, "select cound(*) from a_table where num =  ? and value =  ?") != 0) {
    assert(false); // it needs to be formatted acc to Sybase
  }

  QoreNode* n = new QoreNode(lst);
  n->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // parseQuery() test with more arguments than placeholders 
  // (perhaps this should fail too but this is how Oracle module behaves)
  ExceptionSink xsink;
  QoreString str("select count(*) from a_table where num = %v and value = %v");
  SybaseBindGroup grp(&str);

  List* lst = new List;
  lst->push(new QoreNode("aaa"));
  lst->push(new QoreNode("bbb"));
  lst->push(new QoreNode("ccc"));

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  unsigned size = grp.m_input_parameters.size();
  assert(size == 2);

  char* new_str = grp.m_cmd->getBuffer();
  if (strcmp(new_str, "select count(*) from a_table where num =  ? and value =  ?") != 0) {
    assert(false); // it needs to be formatted acc to Sybase
  }

  QoreNode* n = new QoreNode(lst);
  n->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // parseQuery() test that fails
  ExceptionSink xsink;
  QoreString str("select count(*) from a_table where num = %v and value = %v");
  SybaseBindGroup grp(&str);

  List* lst = new List;
  lst->push(new QoreNode("aaa"));

  grp.parseQuery(lst, &xsink);
  if (!xsink.isException()) { // not enought arguments
    assert(false);
  }
  xsink.clear();

  QoreNode* n = new QoreNode(lst);
  n->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_commit()
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  int res = sybase_commit_impl(&conn, &xsink);
  if (res != 1) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // test sybase_rollback()
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  int res = sybase_rollback_impl(&conn, &xsink);
  if (res != 1) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // several commit/rollbacks
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  int res = sybase_commit_impl(&conn, &xsink);
  if (res != 1) {
    assert(false);
  }
  res = sybase_commit_impl(&conn, &xsink);
  assert(res == 1);
  res = sybase_rollback_impl(&conn, &xsink);
  assert(res == 1);
  res = sybase_commit_impl(&conn, &xsink);
  assert(res == 1);
  res = sybase_rollback_impl(&conn, &xsink);
  assert(res == 1);
  res = sybase_commit_impl(&conn, &xsink);
  assert(res == 1);
  res = sybase_rollback_impl(&conn, &xsink);
  assert(res == 1); 
}

//------------------------------------------------------------------------------
TEST()
{
  // test for prepare_command()
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreString str("select count(*) from syskeys");
  SybaseBindGroup grp(&str);
  grp.m_connection = conn.getConnection();

  grp.prepare_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
static void test1()
{
  // test used during the development
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreString str("select count(*) from syskeys where id > %v and id < %v");
  SybaseBindGroup grp(&str);
  grp.m_connection = conn.getConnection();

  List* lst = new List;
  lst->push(new QoreNode((int64)0));
  lst->push(new QoreNode((int64)1000));

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  CS_COMMAND* cmd = grp.prepare_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  std::vector<SybaseBindGroup::column_info_t> inputs = grp.extract_input_parameters_info(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 2);

  std::vector<SybaseBindGroup::column_info_t> outputs = grp.extract_output_parameters_info(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(outputs.size() == 1);

  grp.bind_input_parameters(cmd, inputs, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* res = grp.read_output(cmd, outputs, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res);
  // row "column1" with value 48 (on test machine)
  if (res->type != NT_HASH) {
    assert(false);
  }
  QoreNode* n = res->val.hash->getKeyValue("column1");
  assert(n);
  assert(n->type == NT_INT);
  assert(n->val.intval == 48);

  CS_RETCODE err = ct_cmd_drop(cmd);
  assert(err = CS_SUCCEED);

  res->deref(&xsink);

  QoreNode* aux = new QoreNode(lst);
  aux->deref(&xsink);
}

TEST()
{
  test1();
}

//------------------------------------------------------------------------------
static void test2()
{
  // as above but using the execute_command()
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreString str("select count(*) from syskeys where id > %v and id < %v");
  SybaseBindGroup grp(&str);
  grp.m_connection = conn.getConnection();

  List* lst = new List;
  lst->push(new QoreNode((int64)0));
  lst->push(new QoreNode((int64)1000));

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  QoreNode* n = grp.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);  
  }
  if (n->type != NT_HASH) {
    assert(false);
  }
  QoreNode* val = n->val.hash->getKeyValue("column1");
  assert(val);
  assert(val->type == NT_INT);
  assert(val->val.intval == 48);

  n->deref(&xsink);
}

TEST()
{
  test2();
}

//------------------------------------------------------------------------------
static void test3()
{
  // select all
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreString str("select * from syskeys where id > %v and id < %v");
  SybaseBindGroup grp(&str);
  grp.m_connection = conn.getConnection();

  List* lst = new List;
  lst->push(new QoreNode((int64)0));
  lst->push(new QoreNode((int64)1000));

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  CS_COMMAND* cmd = grp.prepare_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::vector<SybaseBindGroup::column_info_t> inputs = grp.extract_input_parameters_info(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(inputs.size() == 2); 

  std::vector<SybaseBindGroup::column_info_t> outputs = grp.extract_output_parameters_info(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(outputs.size() == 22); // 22 on my machine

  grp.bind_input_parameters(cmd, inputs, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* res = grp.read_output(cmd, outputs, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res);
  if (res->type != NT_LIST) {
    assert(false);
  }
  assert(res->val.list->size() == 48); // 48 on my machine

  if (res) res->deref(&xsink);

  CS_RETCODE err = ct_cmd_drop(cmd);
  assert(err = CS_SUCCEED);

  QoreNode* aux = new QoreNode(lst);
  aux->deref(&xsink);
}

TEST()
{
  test3();
}

//------------------------------------------------------------------------------
static void test4()
{
  // as above but using execute_command()
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreString str("select * from syskeys where id > %v and id < %v");
  SybaseBindGroup grp(&str);
  grp.m_connection = conn.getConnection();

  List* lst = new List;
  lst->push(new QoreNode((int64)0));
  lst->push(new QoreNode((int64)1000));

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n = grp.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  if (n->type != NT_LIST) {
    assert(false);
  }
  assert(n->val.list->size() == 48);

  n->deref(&xsink);
}

TEST()
{
  test4();
}

//------------------------------------------------------------------------------
TEST()
{
/*
  // test whether it leaks - check memory usage
  for (;;) {
    test3();
  }
*/
}

//------------------------------------------------------------------------------
static void prepare_testing(sybase_connection* conn)
{
  ExceptionSink xsink;
  {
  QoreString str1("drop table my_test"); 
  SybaseBindGroup grp1(&str1);
  grp1.m_connection = conn->getConnection();

  grp1.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n1 = grp1.execute_command(&xsink);
  assert(!n1);
  xsink.clear();
  }
  //----------
  {
  QoreString str2("create table my_test ("
    "int_col INTEGER DEFAULT NULL, "
    "varchar_col VARCHAR(30) DEFAULT NULL "
    ")");
  SybaseBindGroup grp2(&str2);
  grp2.m_connection = conn->getConnection();

  grp2.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n2 = grp2.execute_command(&xsink);
  assert(!n2);
  xsink.clear();
  }
  //--------
  {
  QoreString str3("insert into my_test values(11, 'aaa')");
  SybaseBindGroup grp3(&str3);
  grp3.m_connection = conn->getConnection();

  grp3.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n3 = grp3.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n3);

  if (!sybase_commit_impl(conn, &xsink)) {
    assert(false);    
  }
  if (xsink.isException()) {
    assert(false);
  }
  }
  //--------
  {
  QoreString str4("select count(*) from my_test");
  SybaseBindGroup grp4(&str4);
  grp4.m_connection = conn->getConnection();

  grp4.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n4 = grp4.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n4);
  assert(n4->type == NT_HASH);
  assert(n4->val.hash->size() == 1);
  n4->deref(&xsink);
  }
  //-------- once more
  {
  QoreString str5("select count(*) from my_test");
  SybaseBindGroup grp5(&str5);
  grp5.m_connection = conn->getConnection();

  grp5.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n5 = grp5.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n5);
  assert(n5->type == NT_HASH);
  assert(n5->val.hash->size() == 1);
  n5->deref(&xsink);
  }
  //--------
  {
  // new transaction may be started explicitly
  QoreString str6a("begin transaction");
  SybaseBindGroup grp6a(&str6a);
  grp6a.m_connection = conn->getConnection();

  grp6a.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n6a = grp6a.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n6a);

  QoreString str6("insert into my_test values(22, 'bbb')");
  SybaseBindGroup grp6(&str6);
  grp6.m_connection = conn->getConnection();

  grp6.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n6 = grp6.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n6);

  if (!sybase_commit_impl(conn, &xsink)) {
    assert(false);
  }
  if (xsink.isException()) {
    assert(false);
  }
  }
  //--------
  { // no explicit transaction start
  QoreString str6("insert into my_test values(33, 'ccc')");
  SybaseBindGroup grp6(&str6);
  grp6.m_connection = conn->getConnection();

  grp6.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n6 = grp6.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n6);
  if (!sybase_commit_impl(conn, &xsink)) {
    assert(false);
  }
  if (xsink.isException()) {
    assert(false);
  }
  }
  //--------
  {
  QoreString str4("select int_col from my_test");
  SybaseBindGroup grp4(&str4);
  grp4.m_connection = conn->getConnection();

  grp4.parseQuery(0, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n4 = grp4.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n4);
  assert(n4->type == NT_LIST);
  assert(n4->val.list->size() == 3);
  n4->deref(&xsink);
  }
  //--------
  {
  QoreString str("select int_col from my_test where int_col = %v");
  SybaseBindGroup grp(&str);
  grp.m_connection = conn->getConnection();

  List* lst = new List;
  lst->push(new QoreNode((int64)22));

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n = grp.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  if (n->type != NT_HASH) {
    assert(false);
  }
  assert(n->val.hash->size() == 1);

  n->deref(&xsink);
  }
  //--------
  {
  QoreString str("select int_col from my_test where int_col = %v");
  SybaseBindGroup grp(&str);
  grp.m_connection = conn->getConnection();

  List* lst = new List;
  lst->push(new QoreNode((int64)22222)); // nonexistent

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n = grp.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n);
  }

  //--------
  {
  QoreString str("select int_col from my_test where varchar_col = %v");
  SybaseBindGroup grp(&str);
  grp.m_connection = conn->getConnection();

  List* lst = new List;
  lst->push(new QoreNode("aaa"));

  grp.parseQuery(lst, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  QoreNode* n = grp.execute_command(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  if (n->type != NT_HASH) {
    assert(false);
  }
  assert(n->val.hash->size() == 1);

  n->deref(&xsink);
  }
}

//------------------------------------------------------------------------------
TEST()
{
  // as above but using execute_command()
  sybase_connection conn;
  ExceptionSink xsink;
  conn.init("sa", 0, "pavel", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  prepare_testing(&conn);
}

} // namespace
#endif

// EOF

