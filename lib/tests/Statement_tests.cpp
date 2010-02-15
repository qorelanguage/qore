// Unit tests for Statement.cc

#ifdef DEBUG
namespace Statement_tests {

TEST()
{
  printf("test standard switch 1\n");

  QoreString str(
    "sub test() {"
    "$a = 1;"
    "switch ($a) {"
    "case 2: return False;"
    "case 0 : return False;"
    "case 1: return True;"
    "default: return False;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("test standard switch 2\n");
  
  QoreString str(
    "sub test() {"
    "$a = 30;"
    "switch ($a) {"
    "case 2: return False;"
    "case 0: return False;"
    "case 1: return False;"
    "}"
    "return True; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("test standard switch 3\n");

  QoreString str(  
    "sub test() {"
    "$a = \"aa\";"
    "switch ($a) {"
    "case \"ab\": return False;"
    "case \"a\" : return False;"
    "case \"aa\": return True;"
    "default: return False;"
    "}"
    "return False; }");

  run_Qore_test(str, __FILE__, __LINE__);
}

TEST()
{
  printf("test standard switch 4\n");

  QoreString str(
    "sub test() {"
    "$a = \"abcd\";"
    "switch ($a) {"
    "case \"aa\": return False;"
    "case \"a\": return False;"
    "case \"abcdef\": return False;"
    "}"
    "return True; }");
 
  run_Qore_test(str, __FILE__, __LINE__);
}

} // namespace
#endif // DEBUG

// EOF

