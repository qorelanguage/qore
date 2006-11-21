// Unit tests for Statement.cc

#ifdef DEBUG
namespace Statement_tests {

TEST()
{
  printf("test standard switch 1\n");

  char* s = "qore -e '\n"
    "$a = 1;\n"
    "switch ($a) {\n"
    "case 2: exit(11); break;\n"
    "case 0 : exit(11); break;\n"
    "case 1: exit(10);\n"
    "default: exit(11); break;\n"
    "}\n"
    "exit(11);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("test standard switch 2\n");

  char* s = "qore -e '\n"
    "$a = 30;\n"
    "switch ($a) {\n"
    "case 2: exit(11); break;\n"
    "case 0: exit(11); break;\n"
    "case 1: exit(11); break;\n"
    "}\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("test standard switch 3\n");

  char* s = "qore -e '\n"
    "$a = \"aa\";\n"
    "switch ($a) {\n"
    "case \"ab\": exit(11); break;\n"
    "case \"a\" : exit(11); break;\n"
    "case \"aa\": exit(10);\n"
    "default: exit(11); break;\n"
    "}\n"
    "exit(11);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("test standard switch 4\n");

  char* s = "qore -e '\n"
    "$a = \"abcd\";\n"
    "switch ($a) {\n"
    "case \"aa\": exit(11); break;\n"
    "case \"a\": exit(11); break;\n"
    "case \"abcdef\": exit(11); break;\n"
    "}\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("test standard switch - invalid switch with multiple equal case value\n");

  char* s = "qore -e '\n"
    "$a = a;\n"
    "switch ($a) {\n"
    "case 1: exit(10); break;\n"
    "case 1: exit(10); break;\n"
    "}\n"
    "exit(10);' > /dev/null 2>&1\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res != 10); // expected parse error code
}

} // namespace
#endif // DEBUG

// EOF

