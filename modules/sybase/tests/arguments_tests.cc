#ifdef DEBUG

#include "common.h"
#include <cstypes.h>

namespace sybase_tests_96707125781 {

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  QoreListNode* args = 0;

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
  QoreListNode* args = 0;

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false)); // error

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('d');
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false)); // error - not int

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

//------------------------------------------------------------------------------
TEST()
{
  ExceptionSink xsink;
  std::vector<char> arg_types;
  arg_types.push_back('s');
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));

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
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));
  args->push(new QoreBigIntNode(12));
  args->push(new QoreStringNode("aaa"));

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
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));
  args->push(new QoreBigIntNode(12));
  args->push(new QoreStringNode("aaa"));
  
  // types
  args->push(new QoreBigIntNode(CS_BIT_TYPE));
  args->push(new QoreBigIntNode(CS_SMALLINT_TYPE));
  args->push(new QoreBigIntNode(CS_CHAR_TYPE));

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
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));
  args->push(new QoreBigIntNode(12));
  args->push(new QoreStringNode("aaa"));

  // types
  args->push(new QoreBigIntNode(CS_BIT_TYPE));
  args->push(new QoreBigIntNode(CS_SMALLINT_TYPE));
  args->push(new QoreBigIntNode(CS_CHAR_TYPE));

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
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));
  args->push(new QoreBigIntNode(12));
  args->push(new QoreStringNode("aaa"));

  // types
  args->push(new QoreBigIntNode(CS_BIT_TYPE));
  args->push(new QoreBigIntNode(CS_SMALLINT_TYPE));
  args->push(new QoreBigIntNode(CS_CHAR_TYPE));
  args->push(new QoreBigIntNode(CS_CHAR_TYPE)); // error

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
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));
  args->push(new QoreBigIntNode(12));
  args->push(new QoreStringNode("aaa"));

  // types
  args->push(new QoreBigIntNode(CS_BIT_TYPE));
  args->push(new QoreBigIntNode(CS_SMALLINT_TYPE));
  args->push(new QoreBigIntNode(CS_CHAR_TYPE));

  std::vector<argument_t> res = extract_language_command_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.size() == 3);
}

//------------------------------------------------------------------------------
// procedure parameters
/*###
TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  QoreListNode* args = 0;

  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.empty());
}

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));

  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  arg_types.push_back(std::make_pair(false, std::string("s")));
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));
  args->push(new QoreBigIntNode(CS_BIT_TYPE));

  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(!xsink.isException());
  assert(res.size() == 1);
}

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  arg_types.push_back(std::make_pair(false, std::string("d")));
  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false)); // error
  args->push(new QoreBigIntNode(CS_BIT_TYPE));

  std::vector<argument_t> res = extract_procedure_call_arguments(args, arg_types, &xsink);
  assert(xsink.isException());
}

TEST()
{
  ExceptionSink xsink;
  std::vector<processed_procedure_call_t::parameter_t> arg_types;
  arg_types.push_back(std::make_pair(false, std::string("s")));
  arg_types.push_back(std::make_pair(true, std::string("placeholder1")));
  arg_types.push_back(std::make_pair(true, std::string("placeholder2")));
  arg_types.push_back(std::make_pair(false, std::string("d")));
  arg_types.push_back(std::make_pair(false, std::string("s")));


  QoreListNode* args = new List;
  args->push(new QoreBoolNode(false));
  args->push(new QoreBigIntNode(123));
  args->push(new QoreStringNode("aaa"));

  args->push(new QoreBigIntNode(CS_BIT_TYPE));
  args->push(new QoreBigIntNode(CS_CHAR_TYPE)); // placeholder
  args->push(new QoreBigIntNode(CS_INT_TYPE));  // placeholder
  args->push(new QoreBigIntNode(CS_SMALLINT_TYPE));
  args->push(new QoreBigIntNode(CS_CHAR_TYPE));


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

  assert(res[0].m_node->getType() == NT_BOOLEAN);
  assert(res[1].m_node == 0);
  assert(res[2].m_node == 0);
  assert(res[3].m_node->getType() == NT_INT);
  assert(res[4].m_node->getType() == NT_STRING);
}
*/

} // namespace
#endif

// EOF

