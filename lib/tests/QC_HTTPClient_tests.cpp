// Unit tests for QC_HTTPClient.cc

#ifdef DEBUG
namespace QC_HTTPClient_tests {
/*
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
    "$a = HTTPClient::DEFAULT_METHODS;\n"
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
    "$a = HTTPClient::DEFAULT_HEADERS;\n"
    "if (type($a) != Qore::Type::Hash) exit(11);\n"
    "$x = $a.Accept;\n"
    "if ($x != \"text/html\") exit(11);\n"
    "$x = $a.\"Content-Type\";\n"
    "if ($x != \"text/html\") exit(12);\n"
    "$x = $a.\"User-Agent\";\n"
    "if ($x == \"\") exit(13);\n"
    "$x = $a.Connection;\n"
    "if ($x != \"Keep-Alive\") exit(14);\n"
    "$list = keys $a;\n"
    "$size = elements $list;\n"
    "if ($size != 4) exit(15);\n" 
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing HTTPClient DEFAULT_HEADER_IGNORE constants\n");
  char* s = "qore -e '\n"
    "$a = HTTPClient::DEFAULT_HEADER_IGNORE;\n"
    "if (type($a) != Qore::Type::List) exit(11);\n"
    "$size = elements $a;\n"
    "if ($size != 3) { printf(\"elements = %d\n\", $size); exit(12); }\n"
    "$b = $a[2];\n"
    "if ($b != \"Content-Length\") exit(13);\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing HTTPClient DEFAULT_PROTOCOL constants\n");
  char* s = "qore -e '\n"
    "$a = HTTPClient::DEFAULT_PROTOCOLS;\n"
    "if (type($a) != Qore::Type::Hash) { printf(\"not a hash\n\"); exit(11); }\n"
    "$x = $a.http;\n"
    "if (type($x) != Qore::Type::Hash) { printf(\"not a hash\n\"); exit(12); }\n"
    "if (elements $x != 2) { printf(\"size = %d\n\", elements $x); exit(13); }\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing HTTPClient DEFAULT_HTTP_VERSION constants\n");
  char* s = "qore -e '\n"
    "$a = HTTPClient::DEFAULT_HTTP_VERSION;\n"
    "if ($a != \"1.1\") exit(11);\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing HTTPClient alloved_versions constants\n");
  char* s = "qore -e '\n"
    "$a = HTTPClient::allowed_versions;\n"
    "if (type($a) != Qore::Type::List) exit(11);\n"
    "$size = elements $a;\n"
    "if ($size != 2) { printf(\"elements = %d\n\", $size); exit(12); }\n"
    "$b = $a[1];\n"
    "if ($b != \"1.1\") exit(13);\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing HTTPClient Version constant\n");
  char* s = "qore -e '\n"
    "$a = HTTPClient::Version;\n"
    "if ($a != \"0.3\") exit(12);\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  printf("testing HTTPClient defaultTimeout constant\n");
  char* s = "qore -e '\n"
    "$a = HTTPClient::defaultTimeout;\n"
    "if ($a != 300000) exit(12);\n"
    "exit(10);'\n";

  int res = system(s);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
*/
} // namespace
#endif // DEBUG

// EOF

