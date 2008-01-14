/*
  conversions.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007 Qore Technologies

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <qore/Qore.h>
#include <qore/minitest.hpp>

#include <cstypes.h>
#include <ctpublic.h>
#include <assert.h>

#include <string>

#include <math.h>

#include "connection.h"
#include "conversions.h"

// Sybase dates (so the "Sybase epoch") start from 1900-01-01
// 25567 days from 1900-01-01 to 1970-01-01, the start of the Qore 64-bit epoch
#define SYB_DAYS_TO_EPOCH 25567
#define SYB_SECS_TO_EPOCH (SYB_DAYS_TO_EPOCH * 86400LL)

DateTime *TIME_to_DateTime(CS_DATETIME &dt)
{
   int64 secs = dt.dttime / 300;

   // use floating point to get more accurate 1/3 s
   double ts = round((double)(dt.dttime - (secs * 300)) * 3.3333333);
   return new DateTime(secs, (int)ts);
}

int DateTime_to_DATETIME(DateTime* dt, CS_DATETIME &out, ExceptionSink* xsink)
{
   if (dt->isRelative())
   {
      xsink->raiseException("DBI:SYBASE:DATE-ERROR", "relative date passed for binding as absolute date");
      return -1;
   }
   int year = dt->getYear();
   if (year > 9999)
   {
      QoreString *desc = new QoreString();
      desc->sprintf("maximum sybase datetime value is 9999-12-31, date passed: ");
      dt->format(desc, "YYYY-DD-MM");
      xsink->raiseException("DBI:SYBASE:DATE-ERROR", desc);
      return -1;
   }
   if (year < 1753)
   {
      QoreString *desc = new QoreString();
      desc->sprintf("minumum sybase datetime value is 1753-01-01, date passed: ");
      dt->format(desc, "YYYY-DD-MM");
      xsink->raiseException("DBI:SYBASE:DATE-ERROR", desc);
      return -1;
   }
   int64 secs = dt->getEpochSeconds();
   int days = secs / 86400;
   out.dtdays = days + SYB_DAYS_TO_EPOCH;
   // use floating point to get more accurate 1/3 s
   double ts = round((double)dt->getMillisecond() / 3.3333333);
   out.dttime = (secs - (days * 86400)) * 300 + (int)ts;

   return 0;
}

DateTime* DATETIME_to_DateTime(CS_DATETIME& dt)
{
   int64 secs = dt.dttime / 300;
   // use floating point to get more accurate 1/3 s
   double ts = round((double)(dt.dttime - (secs * 300)) * 3.3333333);
   return new DateTime(secs + dt.dtdays * 86400ll - SYB_SECS_TO_EPOCH, (int)ts);
}

// maximum sybase small datetime value (June 6, 2079)
static DateTime dt4_max(2079, 6, 6);

DateTime *DATETIME4_to_DateTime(CS_DATETIME4 &dt, ExceptionSink *xsink)
{
   int64 secs = dt.minutes * 60LL + dt.days * 86400LL - SYB_SECS_TO_EPOCH;
   return new DateTime(secs);
}

/*
// currently unused
int DateTime_to_DATETIME4(DateTime *dt, CS_DATETIME4 &out, ExceptionSink *xsink)
{
   if (dt->isRelative())
   {
      xsink->raiseException("DBI:SYBASE:DATE-ERROR", "relative date passed for binding as absolute date");
      return -1;
   }
   if (DateTime::compareDates(dt, &dt4_max) > 0)
   {
      QoreString *desc = new QoreString();
      desc->sprintf("maximum sybase small datetime value is 2079-06-06, date passed: ");
      dt->format(desc, "YYYY-DD-MM");
      xsink->raiseException("DBI:SYBASE:DATE-ERROR", desc);
      return -1;
   }
   int year = dt->getYear();
   if (year < 1900)
   {
      QoreString *desc = new QoreString();
      desc->sprintf("minumum sybase small datetime value is 1900-01-01, date passed: ");
      dt->format(desc, "YYYY-DD-MM");
      xsink->raiseException("DBI:SYBASE:DATE-ERROR", desc);
      return -1;
   }

   int64 secs = dt->getEpochSeconds();
   unsigned short days = secs / 86400;
   out.days = days + SYB_DAYS_TO_EPOCH;
   out.minutes = (secs - (days * 86400)) / 60;

   return 0;
}

// currently unused
void double_to_MONEY(connection& conn, double val, CS_MONEY& out, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_FLOAT_TYPE;
  srcfmt.maxlength = sizeof(CS_FLOAT);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_MONEY_TYPE;
  destfmt.maxlength = sizeof(CS_MONEY);

  CS_INT outlen;
  CS_RETCODE err = cs_convert(conn.getContext(), &srcfmt, (CS_BYTE*)&val, &destfmt, (CS_BYTE*)&out, &outlen);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert a float value to Sybase MONEY, err %d", (int)err);
    return;
  }
}

void double_to_MONEY4(connection& conn, double val, CS_MONEY4& out, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_FLOAT_TYPE;
  srcfmt.maxlength = sizeof(CS_FLOAT);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_MONEY4_TYPE;
  destfmt.maxlength = sizeof(CS_MONEY4);

  CS_INT outlen;
  CS_RETCODE err = cs_convert(conn.getContext(), &srcfmt, (CS_BYTE*)&val, &destfmt, (CS_BYTE*)&out, &outlen);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert a float value to Sybase MONEY4, err %d", (int)err);
    return;
  }
}

double MONEY_to_double(connection& conn, CS_MONEY& m, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_MONEY_TYPE;
  srcfmt.maxlength = sizeof(CS_MONEY);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_FLOAT_TYPE;
  destfmt.maxlength = sizeof(CS_FLOAT);

  CS_INT outlen;
  CS_FLOAT result = 0.0;
  CS_RETCODE err = cs_convert(conn.getContext(), &srcfmt, (CS_BYTE*)&m, &destfmt, (CS_BYTE*)&result, &outlen);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert Sybase MONEY to FLOAT, err %d", (int)err);
  }

  return result;
}

double MONEY4_to_double(connection& conn, CS_MONEY4& m, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_MONEY4_TYPE;
  srcfmt.maxlength = sizeof(CS_MONEY4);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_FLOAT_TYPE;
  destfmt.maxlength = sizeof(CS_FLOAT);

  CS_INT outlen;
  CS_FLOAT result = 0.0;
  CS_RETCODE err = cs_convert(conn.getContext(), &srcfmt, (CS_BYTE*)&m, &destfmt, (CS_BYTE*)&result, &outlen);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert Sybase MONEY4 to FLOAT, err %d", (int)err);
  }

  return result;
}

void double_to_DECIMAL(connection& conn, double val, CS_DECIMAL& out, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_FLOAT_TYPE;
  srcfmt.maxlength = sizeof(CS_FLOAT);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_DECIMAL_TYPE;
  destfmt.maxlength = 35; // recommended by docs
  destfmt.scale = 15; // # of digits after decimal point, guess 
  destfmt.precision = 30; // total # of digits in number, also guess
 
  CS_INT outlen;
  CS_RETCODE err = cs_convert(conn.getContext(), &srcfmt, (CS_BYTE*)&val, &destfmt, (CS_BYTE*)&out, &outlen);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert a float value to Sybase decimal, err %d", (int)err);
    return;
  }
  assert(out.precision);
  assert(out.scale);
  assert(out.precision > out.scale);
}

#define SYB_DEC_STR_LEN 50
class QoreStringNode *DECIMAL_to_string(connection& conn, CS_DECIMAL& m, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_DECIMAL_TYPE;
  srcfmt.maxlength = 35; // recommended by docs
  srcfmt.scale = 15; // guess, keep the same as above
  srcfmt.precision = 30; // also guess

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_CHAR_TYPE;
  destfmt.format = CS_FMT_NULLTERM;
  destfmt.maxlength = SYB_DEC_STR_LEN;

  CS_INT outlen;
  CS_CHAR buf[SYB_DEC_STR_LEN + 1];
  CS_RETCODE err = cs_convert(conn.getContext(), &srcfmt, (CS_BYTE*)&m, &destfmt, buf, &outlen);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert Sybase decimal to string, err %d", (int)err);
    return 0;
  }
  //printd(5, "DECIMAL_to_string: '%s'\n", buf);
  return new QoreStringNode(buf);  
}
*/


/*
#ifdef DEBUG
#  include "tests/conversions_tests.cc"
#endif
*/

// EOF

