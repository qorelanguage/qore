#ifdef DEBUG

#include "common.h"
#include <qore/QoreNode.h>
#include <math.h>
#include <memory>

namespace sybase_tests_7527024186 {

//------------------------------------------------------------------------------
TEST()
{
  // testing DATETIME <-> Qore Datetime conversion
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::auto_ptr<DateTime> d(new DateTime);

  CS_DATETIME dt1;
  DateTime_to_DATETIME(conn, d.get(), dt1, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
 
  std::auto_ptr<DateTime> d2(DATETIME_to_DateTime(conn, dt1, &xsink));
  if (xsink.isException()) {
    assert(false);
  }

  if (d->getEpochSeconds() != d2->getEpochSeconds()) {
    assert(false);
  }
  printf("DateTime <-> DATETIME conversion works\n");
}

//------------------------------------------------------------------------------
TEST()
{
  // testing DATETIME4 <-> Qore Datetime conversion
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::auto_ptr<DateTime> d(new DateTime);

  CS_DATETIME4 dt1;
  DateTime_to_DATETIME4(conn, d.get(), dt1, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  std::auto_ptr<DateTime> d2(DATETIME4_to_DateTime(conn, dt1, &xsink));
  if (xsink.isException()) {
    assert(false);
  }

  if (d->getEpochSeconds() != d2->getEpochSeconds()) {
    assert(false);
  }
  printf("conversion DateTime <-> DATETIME4 works\n");
}

//------------------------------------------------------------------------------
#ifdef SYBASE
// fails with FreeTDS and Sybase 15.0

TEST()
{
  // testing MONEY <-> float conversion
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  CS_MONEY m;
  double_to_MONEY(conn, 1.2, m, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  double d = MONEY_to_double(conn, m, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(d == 1.2);  
  printf("conversion double <-> MONEY works\n");
}
#endif

//------------------------------------------------------------------------------
#ifdef SYBASE
// fails with FreeTDS and Sybase 15.0

TEST()
{
  // testing MONEY4 <-> float conversion
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  CS_MONEY4 m;
  double_to_MONEY4(conn, 6.2, m, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  double d = MONEY4_to_double(conn, m, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(d == 6.2);
  printf("conversion double <-> MONEY4 works\n");
}
#endif

//------------------------------------------------------------------------------
TEST()
{
  // testing DECIMAL <-> float conversion
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  CS_DECIMAL dc;  
  double_to_DECIMAL(conn, 6.22, dc, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  double d = DECIMAL_to_double(conn, dc, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(d == 6.22);
  printf("conversion double <-> DECIMAL works\n");
}

//------------------------------------------------------------------------------
TEST()
{
  // testing NUMERIC <-> float conversion
  printf("running test %s[%d]\n", __FILE__, __LINE__);

  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  CS_DECIMAL dc;
  double_to_NUMERIC(conn, 16.22, dc, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  double d = NUMERIC_to_double(conn, dc, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(d == 16.22);
  printf("conversion double <-> NUMERIC works\n");
}

} // namespace
#endif

// EOF

