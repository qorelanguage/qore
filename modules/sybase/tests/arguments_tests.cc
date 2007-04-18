#ifdef DEBUG

#include "common.h"
#include <cstypes.h>

namespace sybase_tests_96707125781 {

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  List* args = 0;

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.empty());
}

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('d'); // error
  List* args = 0;

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  List* args = new List;
  args->push(new QoreNode(false)); // error

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('d');
  List* args = new List;
  args->push(new QoreNode(false)); // error - not int

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('s');
  List* args = new List;
  args->push(new QoreNode(false));

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.size() == 1);
}

TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('s');
  arg_types.push_back('d');
  arg_types.push_back('s');
  List* args = new List;
  args->push(new QoreNode(false));
  args->push(new QoreNode((int64)12));
  args->push(new QoreNode("aaa"));

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.size() == 3);
}

TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('s');
  arg_types.push_back('d');
  arg_types.push_back('s');
  List* args = new List;
  args->push(new QoreNode(false));
  args->push(new QoreNode((int64)12));
  args->push(new QoreNode("aaa"));
  
  // types
  args->push(new QoreNode((int64)CS_BIT_TYPE));
  args->push(new QoreNode((int64)CS_SMALLINT_TYPE));
  args->push(new QoreNode((int64)CS_CHAR_TYPE));

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.size() == 3);
}

TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('s');
  arg_types.push_back('d');
  arg_types.push_back('d'); // error
  List* args = new List;
  args->push(new QoreNode(false));
  args->push(new QoreNode((int64)12));
  args->push(new QoreNode("aaa"));

  // types
  args->push(new QoreNode((int64)CS_BIT_TYPE));
  args->push(new QoreNode((int64)CS_SMALLINT_TYPE));
  args->push(new QoreNode((int64)CS_CHAR_TYPE));

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('s');
  arg_types.push_back('d');
  arg_types.push_back('s');
  List* args = new List;
  args->push(new QoreNode(false));
  args->push(new QoreNode((int64)12));
  args->push(new QoreNode("aaa"));

  // types
  args->push(new QoreNode((int64)CS_BIT_TYPE));
  args->push(new QoreNode((int64)CS_SMALLINT_TYPE));
  args->push(new QoreNode((int64)CS_CHAR_TYPE));
  args->push(new QoreNode((int64)CS_CHAR_TYPE)); // error

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('s');
  arg_types.push_back('d');
  arg_types.push_back('s');
  List* args = new List;
  args->push(new QoreNode(false));
  args->push(new QoreNode((int64)12));
  args->push(new QoreNode("aaa"));

  // types
  args->push(new QoreNode((int64)CS_BIT_TYPE));
  args->push(new QoreNode((int64)CS_SMALLINT_TYPE));
  args->push(new QoreNode((int64)CS_CHAR_TYPE));

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.size() == 3);
}

//------------------------------------------------------------------------------
// procedure parameters

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  List* args = 0;

  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.empty());
}

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  List* args = new List;
  args->push(new QoreNode(false));

  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  arg_types.push_back(std::make_pair(false, "s"));
  List* args = new List;
  args->push(new QoreNode(false));
  args->push(new QoreNode((int64)CS_BIT_TYPE));

  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.size() == 1);
}

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  arg_types.push_back(std::make_pair(false, "d"));
  List* args = new List;
  args->push(new QoreNode(false)); // error
  args->push(new QoreNode((int64)CS_BIT_TYPE));

  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  arg_types.push_back(std::make_pair(false, "s"));
  arg_types.push_back(std::make_pair(true, "placeholder1"));
  arg_types.push_back(std::make_pair(true, "placeholder2"));
  arg_types.push_back(std::make_pair(false, "d"));
  arg_types.push_back(std::make_pair(false, "s"));


  List* args = new List;
  args->push(new QoreNode(false));
  args->push(new QoreNode((int64)123));
  args->push(new QoreNode("aaa"));

  args->push(new QoreNode((int64)CS_BIT_TYPE));
  args->push(new QoreNode((int64)CS_CHAR_TYPE)); // placeholder
  args->push(new QoreNode((int64)CS_INT_TYPE));  // placeholder
  args->push(new QoreNode((int64)CS_SMALLINT_TYPE));
  args->push(new QoreNode((int64)CS_CHAR_TYPE));


  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.size() == 5);

  assert(res[0].m_name == "");
  assert(res[1].m_name == "placeholder1");
  assert(res[2].m_name == "placeholder2");
  assert(res[3].m_name == "");
  assert(res[4].m_name == "");

  assert(res[0].m_type == CS_BIT_TYPE);
  assert(res[1].m_type == CS_CHAR_TYPE);
  assert(res[2].m_type == CS_INT_TYPE);
  assert(res[3].m_type == CS_SMALLINT_TYPE);
  assert(res[4].m_type == CS_CHAR_TYPE);

  assert(res[0].m_node->type == NT_BOOLEAN);
  assert(res[1].m_node == 0);
  assert(res[2].m_node == 0);
  assert(res[3].m_node->type == NT_INT);
  assert(res[4].m_node->type == NT_STRING);
}

} // namespace
#endif

// EOF

