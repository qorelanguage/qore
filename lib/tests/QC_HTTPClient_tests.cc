// Unit tests for QC_HTTPClient.cc

#ifdef DEBUG
namespace QC_HTTPClient_tests {

TEST()
{
  printf("testing HTTPClient 1\n");
  // test float
  char* s = "qore -e '\n"
    "$a = new HTTPClient2();\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing HTTPClient DEFAULT_METHODS constants\n");
  char* s = "qore -e '\n"
    "$a = HTTPClient3::DEFAULT_METHODS;\n"
    "if (type($a) != Qore::Type::List) exit(11);\n"
    "$size = elements $a;\n"
    "if ($size != 8) { printf(\"elements = %d\n\", $size); exit(12); }\n"
    "$b = $a[2];\n"
    "if ($b != \"HEAD\") exit(13);\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing HTTPClient DEFAULT_HEADERS constants\n");
  char* s = "qore -e '\n"
    "$a = HTTPClient3::DEFAULT_HEADERS;\n"
    "if (type($a) != Qore::Type::Hash) exit(11);\n"
    "$x = $a.Accept;\n"
    "if ($x != \"text/html\") exit(11);\n"
    "$x = $a.\"Content-Type\";\n"
    "if ($x != \"text/html\") exit(12);\n"
    "$x = $a.\"User-Agent\";\n"
    "if ($x == \"\") exit(13);\n"
    "$x = $a.Connection;\n"
    "if ($x != \"Keep-Alive\") exit(14);\n"
    "$size = element keys $a;\n"
    "if ($size != 4) exit(15);\n" 
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

} // namespace
#endif // DEBUG

// EOF

