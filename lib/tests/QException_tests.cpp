#ifdef DEBUG

#include <assert.h>

namespace qexception_tests_912876 {

//------------------------------------------------------------------------------
static void foo()
{
  throw QException("MODULE", "a problem", "a cause");
}

TEST()
{
  printf("testing QException\n");
  try {
    foo();
  } catch (QException& e) {
    const char* s = e.getType();
    assert(strcmp(s, "MODULE") == 0);
    s = e.getProblem();
    assert(strcmp(s, "a problem") == 0);
    s = e.getCause();
    assert(strcmp(s, "a cause") == 0);
    s = e.getSolution();
    assert(!s);
    std::string str = e.getDetailedDecsription();
    assert(str == "PROBLEM: a problem\nCAUSE: a cause\n");
    str = e.getAllDetails();
    assert(str.empty());
  }
}

} // namespace
#endif

// EOF

