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

} // namespace
#endif // DEBUG

// EOF

