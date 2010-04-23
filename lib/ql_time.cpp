/*
  ql_time.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/ql_time.h>

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// returns the current date and time with a resolution to the millisecond
static AbstractQoreNode *f_now_ms(const QoreListNode *params, ExceptionSink *xsink) {
   int us;
   int64 seconds = q_epoch_us(us);

   return DateTimeNode::makeAbsolute(currentTZ(), seconds, (us / 1000) * 1000);
}

// returns the current date and time with a resolution to the microsecond
static AbstractQoreNode *f_now_us(const QoreListNode *params, ExceptionSink *xsink) {
   int us;
   int64 seconds = q_epoch_us(us);

   return DateTimeNode::makeAbsolute(currentTZ(), seconds, us);
}

// returns the current UTC date and time with a resolution to the millisecond
static AbstractQoreNode *f_now_utc_ms(const QoreListNode *params, ExceptionSink *xsink) {
   int us;
   int64 seconds = q_epoch_us(us);

   return DateTimeNode::makeAbsolute(NULL, seconds, (us / 1000) * 1000);
}

// returns the current date and time with a resolution to the microsecond
static AbstractQoreNode *f_now_utc_us(const QoreListNode *params, ExceptionSink *xsink) {
   int us;
   int64 seconds = q_epoch_us(us);

   return DateTimeNode::makeAbsolute(NULL, seconds, us);
}

static AbstractQoreNode *f_format_date(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const DateTimeNode *p1 = HARD_QORE_DATE(params, 1);

   QoreStringNode *rv = new QoreStringNode;
   p1->format(*rv, p0->getBuffer());

   printd(5, "format_date() returning \"%s\"\n", rv->getBuffer());
   return rv;
}

// returns the current date and time with a resolution to the second
static AbstractQoreNode *f_localtime(const QoreListNode *params, ExceptionSink *xsink) {
   return DateTimeNode::makeAbsolute(currentTZ(), (int64)time(0), 0);
}

static AbstractQoreNode *f_localtime_int_int(const QoreListNode *params, ExceptionSink *xsink) {
   return DateTimeNode::makeAbsolute(currentTZ(), HARD_QORE_INT(params, 0), HARD_QORE_INT(params, 1));
}

static AbstractQoreNode *f_localtime_date(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *d = HARD_QORE_DATE(params, 0);
   return DateTimeNode::makeAbsolute(currentTZ(), d->getEpochSecondsUTC(), d->getMicrosecond());
}

static AbstractQoreNode *f_gmtime(const QoreListNode *params, ExceptionSink *xsink) {
   return DateTimeNode::makeAbsolute(NULL, (int64)time(0), 0);
}

static AbstractQoreNode *f_gmtime_int_int(const QoreListNode *params, ExceptionSink *xsink) {
   return DateTimeNode::makeAbsolute(NULL, HARD_QORE_INT(params, 0), HARD_QORE_INT(params, 1));
}

static AbstractQoreNode *f_gmtime_date(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *d = HARD_QORE_DATE(params, 0);
   return DateTimeNode::makeAbsolute(NULL, d->getEpochSecondsUTC(), d->getMicrosecond());
}

static AbstractQoreNode *f_mktime(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);

   struct tm nt;
   time_t t;

   p0->getTM(&nt);
   t = mktime(&nt);

   return new QoreBigIntNode(t);
}

static AbstractQoreNode *f_timegm(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_TIMEGM
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);

   struct tm nt;
   time_t t;

   p0->getTM(&nt);
   t = timegm(&nt);

   return new QoreBigIntNode(t);
#else
   xsink->raiseException("TIMEGM-ERROR", "this system does not implement timegm(); use the constant Qore::HAVE_TIMEGM to check if this function is implemented before calling");
   return 0;
#endif
}

static AbstractQoreNode *f_get_epoch_seconds(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getEpochSeconds());
}

static AbstractQoreNode *f_years(const QoreListNode *params, ExceptionSink *xsink) {
   return new DateTimeNode(HARD_QORE_INT(params, 0), 0, 0, 0, 0, 0, 0, true);
}

static AbstractQoreNode *f_months(const QoreListNode *params, ExceptionSink *xsink) {
   return new DateTimeNode(0, HARD_QORE_INT(params, 0), 0, 0, 0, 0, 0, true);
}

static AbstractQoreNode *f_days(const QoreListNode *params, ExceptionSink *xsink) {
   return new DateTimeNode(0, 0, HARD_QORE_INT(params, 0), 0, 0, 0, 0, true);
}

static AbstractQoreNode *f_hours(const QoreListNode *params, ExceptionSink *xsink) {
   return new DateTimeNode(0, 0, 0, HARD_QORE_INT(params, 0), 0, 0, 0, true);
}

static AbstractQoreNode *f_minutes(const QoreListNode *params, ExceptionSink *xsink) {
   return new DateTimeNode(0, 0, 0, 0, HARD_QORE_INT(params, 0), 0, 0, true);
}

static AbstractQoreNode *f_seconds(const QoreListNode *params, ExceptionSink *xsink) {
   return new DateTimeNode(0, 0, 0, 0, 0, HARD_QORE_INT(params, 0), 0, true);
}

static AbstractQoreNode *f_milliseconds(const QoreListNode *params, ExceptionSink *xsink) {
   return new DateTimeNode(0, 0, 0, 0, 0, 0, HARD_QORE_INT(params, 0), true);
}

static AbstractQoreNode *f_microseconds(const QoreListNode *params, ExceptionSink *xsink) {
   return DateTimeNode::makeRelative(0, 0, 0, 0, 0, 0, HARD_QORE_INT(params, 0));
}

// returns an integer corresponding to the year value in the date
static AbstractQoreNode *f_get_years(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getYear());
}

// returns an integer corresponding to the month value in the date
static AbstractQoreNode *f_get_months(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getMonth());
}

// returns an integer corresponding to the day value in the date
static AbstractQoreNode *f_get_days(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getDay());
}

// returns an integer corresponding to the hour value in the date
static AbstractQoreNode *f_get_hours(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getHour());
}

// returns an integer corresponding to the minute value in the date
static AbstractQoreNode *f_get_minutes(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getMinute());
}

// returns an integer corresponding to the second value in the date
static AbstractQoreNode *f_get_seconds(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getSecond());
}

// returns an integer corresponding to the millisecond value in the date
static AbstractQoreNode *f_get_milliseconds(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getMillisecond());
}

// returns an integer corresponding to the microsecond value in the date
static AbstractQoreNode *f_get_microseconds(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getMicrosecond());
}

// returns midnight on the date passed (strips the time component on the new value)
static AbstractQoreNode *f_get_midnight(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   DateTimeNode *dt = new DateTimeNode(*p0);
   dt->setTime(0, 0, 0);
   return dt;
}

// returns an integer corresponding to the number of the day in the year
static AbstractQoreNode *f_getDayNumber(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   return new QoreBigIntNode(p0->getDayNumber());
}

// returns an integer between 0-6 corresponding to the number of the day in the week: 0 = sun, 6 = sat
static AbstractQoreNode *f_getDayOfWeek(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   int d = p0->getDayOfWeek();
   return new QoreBigIntNode(d);
}

// returns an integer between 1-7 corresponding to the number of the day in the week according to IS-8601: 1 = mon, 7 = sun
static AbstractQoreNode *f_getISODayOfWeek(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);
   int d = p0->getDayOfWeek();
   return new QoreBigIntNode((!d ? 7 : d));
}

// returns a hash giving the ISO-8601 values for the year and calendar week for the date passed
static AbstractQoreNode *f_getISOWeekHash(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);

   int year, week, day;
   p0->getISOWeek(year, week, day);
   QoreHashNode *h = new QoreHashNode;
   h->setKeyValue("year", new QoreBigIntNode(year), 0);
   h->setKeyValue("week", new QoreBigIntNode(week), 0);
   h->setKeyValue("day", new QoreBigIntNode(day), 0);

   return h;
}

// returns a string corresponding to the ISO-8601 year and calendar week for the date passed
static AbstractQoreNode *f_getISOWeekString(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = HARD_QORE_DATE(params, 0);

   int year, week, day;
   p0->getISOWeek(year, week, day);
   QoreStringNode *str = new QoreStringNode();
   str->sprintf("%04d-W%02d-%d", year, week, day);

   return str;
}

// returns a date corresponding to the ISO-8601 calendar week information passed
// args: year, week #, [day #]
// note that ISO-8601 week days go from 1 - 7 = Mon - Sun
static AbstractQoreNode *f_getDateFromISOWeek(const QoreListNode *params, ExceptionSink *xsink) {
   int year = HARD_QORE_INT(params, 0);
   int week = HARD_QORE_INT(params, 1);
   // day number defaults to 1 = Monday, start of the week (7 = Sun)
   int day = HARD_QORE_INT(params, 2);

   return DateTimeNode::getDateFromISOWeek(year, week, day, xsink);
}

/**
 * f_clock_getmillis(const QoreListNode *params, ExceptionSink *xsink)
 *
 * returns the current system clock time value as milliseconds
 * 20051114 aargon
 * updates for Darwin
 * 20051116 david_nichols
 */
static AbstractQoreNode *f_clock_getmillis(const QoreListNode *params, ExceptionSink *xsink) {
   int us;
   int64 seconds = q_epoch_us(us);

   return new QoreBigIntNode(seconds * 1000 + us / 1000);
}

/*
 * qore: cock_getnanos()
 * returns the current system clock time value as nanoseconds since Jan 1, 1970
 */
static AbstractQoreNode *f_clock_getnanos(const QoreListNode *params, ExceptionSink *xsink) {
   int ns;
   int64 seconds = q_epoch_ns(ns);

   return new QoreBigIntNode(seconds * 1000000000ll + ns);
}

/* qore: clock_getmicros()
 * returns the current system clock time value as microseconds since Jan 1, 1970
 */
static AbstractQoreNode *f_clock_getmicros(const QoreListNode *params, ExceptionSink *xsink) {
   int us;
   int64 seconds = q_epoch_us(us);

   return new QoreBigIntNode(seconds * 1000000ll + us);
}

static AbstractQoreNode *f_date_ms(const QoreListNode *params, ExceptionSink *xsink) {
   int64 ms = HARD_QORE_INT(params, 0);
   return new DateTimeNode(ms / 1000, (int)(ms % 1000));
}

static AbstractQoreNode *f_date_us(const QoreListNode *params, ExceptionSink *xsink) {
   int64 us = HARD_QORE_INT(params, 0);
   return DateTimeNode::makeAbsolute(currentTZ(), us / 1000000, (int)(us % 1000000));
}

void init_time_functions() {
   builtinFunctions.add2("now", f_localtime, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("now_ms", f_now_ms, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("now_us", f_now_us, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("now_utc_ms", f_now_utc_ms, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("now_utc_us", f_now_utc_us, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);

   builtinFunctions.add2("format_date", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("format_date", f_format_date, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("localtime", f_localtime, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("localtime", f_localtime_int_int, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 2, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("localtime", f_localtime_date, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("gmtime", f_gmtime, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("gmtime", f_gmtime_int_int, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 2, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("gmtime", f_gmtime_date, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("mktime", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("mktime", f_mktime, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("timegm", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("timegm", f_timegm, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_epoch_seconds", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_epoch_seconds", f_get_epoch_seconds, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("years", f_reldate_noop, QC_NOOP, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("years", f_years, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("months", f_reldate_noop, QC_NOOP, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("months", f_months, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("days", f_reldate_noop, QC_NOOP, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("days", f_days, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("hours", f_reldate_noop, QC_NOOP, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("hours", f_hours, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("minutes", f_reldate_noop, QC_NOOP, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("minutes", f_minutes, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("seconds", f_reldate_noop, QC_NOOP, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("seconds", f_seconds, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("milliseconds", f_reldate_noop, QC_NOOP, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("milliseconds", f_milliseconds, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("microseconds", f_microseconds, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_years", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_years", f_get_years, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_months", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_months", f_get_months, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_days", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_days", f_get_days, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_hours", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_hours", f_get_hours, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_minutes", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_minutes", f_get_minutes, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_seconds", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_seconds", f_get_seconds, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_milliseconds", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_milliseconds", f_get_milliseconds, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_microseconds", f_get_microseconds, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_midnight", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_midnight", f_get_midnight, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getDayNumber", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("getDayNumber", f_getDayNumber, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getDayOfWeek", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("getDayOfWeek", f_getDayOfWeek, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getISODayOfWeek", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("getISODayOfWeek", f_getISODayOfWeek, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getISOWeekHash", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("getISOWeekHash", f_getISOWeekHash, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getISOWeekString", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("getISOWeekString", f_getISOWeekString, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getDateFromISOWeek", f_getDateFromISOWeek, QC_NO_FLAGS, QDOM_DEFAULT, dateTypeInfo, 3, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("clock_getmillis",     f_clock_getmillis, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("clock_getnanos",      f_clock_getnanos, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("clock_getmicros",     f_clock_getmicros, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   builtinFunctions.add2("date_ms", f_date_noop, QC_NOOP, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("date_ms", f_date_ms, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("date_us", f_date_us, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
}
