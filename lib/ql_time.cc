/*
  ql_time.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

// define qore_gettime() for various platforms to get time in nanoseconds
#ifdef HAVE_CLOCK_GETTIME
// define qore_gettime() for POSIX platforms
typedef struct timespec qore_timespec_t;
static inline void qore_gettime(qore_timespec_t *tp) {
   clock_gettime(CLOCK_REALTIME, tp);
}
#else
// use gettimeofday() to get microsecond resolution and multiply by 1000
struct qore_timespec_t {
      unsigned tv_sec;
      unsigned tv_nsec;
};
static inline void qore_gettime(qore_timespec_t *tp) {
   struct timeval tv;
   gettimeofday(&tv, 0);
   tp->tv_sec = tv.tv_sec;
   tp->tv_nsec = tv.tv_usec * 1000;
}
#endif

// returns the current date and time with a resolution to the second
static AbstractQoreNode *f_now(const QoreListNode *params, ExceptionSink *xsink) {
   time_t ct;

   ct = time(0);
   //printf("f_now() %d\n", ct);
   struct tm tms;
   return new DateTimeNode(q_localtime(&ct, &tms));
}

// returns the current date and time with a resolution to the millisecond
static AbstractQoreNode *f_now_ms(const QoreListNode *params, ExceptionSink *xsink) {
   struct timeval tv;
   struct timezone tz;
   gettimeofday(&tv, &tz);

   // get difference to local time zone (with DST)
   int delta = -tz.tz_minuteswest * 60 + 3600 * tz.tz_dsttime;

   return new DateTimeNode(tv.tv_sec + delta, tv.tv_usec / 1000);
}

static AbstractQoreNode *f_format_date(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   const AbstractQoreNode *p1;

   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = get_param(params, 1)))
      return 0;
   
   DateTimeValueHelper temp(p1);
   QoreStringNode *rv = new QoreStringNode();
   temp->format(*rv, p0->getBuffer());
   
   printd(5, "format_date() returning \"%s\"\n", rv->getBuffer());
   return rv;
}

static AbstractQoreNode *f_localtime(const QoreListNode *params, ExceptionSink *xsink) {
   time_t t;

   // if no parameters are passed, then return the current local time (like now())
   const AbstractQoreNode *p0 = get_param(params, 0);
   t = is_nothing(p0) ? time(0) : p0->getAsInt();

   struct tm tms;
   return new DateTimeNode(q_localtime(&t, &tms));
}

static AbstractQoreNode *f_gmtime(const QoreListNode *params, ExceptionSink *xsink) {
   time_t t;
   // if no parameters are passed, then return the current GMT time
   if (!num_params(params))
      t = time(0);
   else {
      const AbstractQoreNode *p0 = get_param(params, 0);
      t = p0 ? p0->getAsInt() : 0;
   }

   struct tm tms;
   return new DateTimeNode(q_gmtime(&t, &tms));
}

static AbstractQoreNode *f_mktime(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0;
   if (!(p0 = get_param(params, 0)))
      return 0;

   DateTimeValueHelper temp(p0);
   struct tm nt;
   time_t t;

   temp->getTM(&nt);
   t = mktime(&nt);

   return new QoreBigIntNode(t);
}

static AbstractQoreNode *f_timegm(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_TIMEGM
   const AbstractQoreNode *p0;
   if (!(p0 = get_param(params, 0)))
      return 0;

   DateTimeValueHelper temp(p0);
   struct tm nt;
   time_t t;

   temp->getTM(&nt);
   t = timegm(&nt);

   return new QoreBigIntNode(t);
#else
   xsink->raiseException("TIMEGM-ERROR", "this system does not implement timegm(); use the constant Qore::HAVE_TIMEGM to check if this function is implemented before calling");
   return 0;
#endif
}

static AbstractQoreNode *f_get_epoch_seconds(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;

   return new QoreBigIntNode(p0->getEpochSeconds());
}

static AbstractQoreNode *f_years(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   return new DateTimeNode(p0 ? p0->getAsInt() : 0, 0, 0, 0, 0, 0, 0, true);
}

static AbstractQoreNode *f_months(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   return new DateTimeNode(0, p0 ? p0->getAsInt() : 0, 0, 0, 0, 0, 0, true);
}

static AbstractQoreNode *f_days(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   return new DateTimeNode(0, 0, p0 ? p0->getAsInt() : 0, 0, 0, 0, 0, true);
}

static AbstractQoreNode *f_hours(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   return new DateTimeNode(0, 0, 0, p0 ? p0->getAsInt() : 0, 0, 0, 0, true);
}

static AbstractQoreNode *f_minutes(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   return new DateTimeNode(0, 0, 0, 0, p0 ? p0->getAsInt() : 0, 0, 0, true);
}

static AbstractQoreNode *f_seconds(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   return new DateTimeNode(0, 0, 0, 0, 0, p0 ? p0->getAsInt() : 0, 0, true);
}

static AbstractQoreNode *f_milliseconds(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   return new DateTimeNode(0, 0, 0, 0, 0, 0, p0 ? p0->getAsInt() : 0, true);
}

// returns an integer corresponding to the year value in the date
static AbstractQoreNode *f_get_years(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   return new QoreBigIntNode(p0->getYear());
}

// returns an integer corresponding to the month value in the date
static AbstractQoreNode *f_get_months(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   return new QoreBigIntNode(p0->getMonth());
}

// returns an integer corresponding to the day value in the date
static AbstractQoreNode *f_get_days(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   return new QoreBigIntNode(p0->getDay());
}

// returns an integer corresponding to the hour value in the date
static AbstractQoreNode *f_get_hours(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   return new QoreBigIntNode(p0->getHour());
}

// returns an integer corresponding to the minute value in the date
static AbstractQoreNode *f_get_minutes(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   return new QoreBigIntNode(p0->getMinute());
}

// returns an integer corresponding to the second value in the date
static AbstractQoreNode *f_get_seconds(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   return new QoreBigIntNode(p0->getSecond());
}

// returns an integer corresponding to the millisecond value in the date
static AbstractQoreNode *f_get_milliseconds(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   return new QoreBigIntNode(p0->getMillisecond());
}

// returns midnight on the date passed (strips the time component on the new value)
static AbstractQoreNode *f_get_midnight(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;

   class DateTimeNode *dt = new DateTimeNode(*p0);
   dt->setTime(0, 0, 0);
   return dt;
}

// returns an integer corresponding to the number of the day in the year
static AbstractQoreNode *f_getDayNumber(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   return new QoreBigIntNode(p0->getDayNumber());
}

// returns an integer between 0-6 corresponding to the number of the day in the week: 0 = sun, 6 = sat
static AbstractQoreNode *f_getDayOfWeek(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;

   int d = p0->getDayOfWeek();
   return new QoreBigIntNode(d);
}

// returns an integer between 1-7 corresponding to the number of the day in the week according to IS-8601: 1 = mon, 7 = sun
static AbstractQoreNode *f_getISODayOfWeek(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
   int d = p0->getDayOfWeek();
   return new QoreBigIntNode((!d ? 7 : d));
}

// returns a hash giving the ISO-8601 values for the year and calendar week for the date passed
static AbstractQoreNode *f_getISOWeekHash(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;

   int year, week, day;
   p0->getISOWeek(year, week, day);
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("year", new QoreBigIntNode(year), 0);
   h->setKeyValue("week", new QoreBigIntNode(week), 0);
   h->setKeyValue("day", new QoreBigIntNode(day), 0);
   
   return h;
}

// returns a string corresponding to the ISO-8601 year and calendar week for the date passed
static AbstractQoreNode *f_getISOWeekString(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *p0 = test_date_param(params, 0);
   if (!p0)
      return 0;
   
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
   const AbstractQoreNode *pt = get_param(params, 0);
   int year = pt ? pt->getAsInt() : 0;

   pt = get_param(params, 1);
   int week = pt ? pt->getAsInt() : 0;

   // day number defaults to 1 = Monday, start of the week (7 = Sun)
   pt = get_param(params, 2);
   int day = pt ? pt->getAsInt() : 1;

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
   struct timeval tv;
   gettimeofday(&tv, 0);

   return new QoreBigIntNode(((int64)tv.tv_sec*(int64)1000 + tv.tv_usec/1000));
}

/*
 * qore: cock_getnanos()
 * returns the current system clock time value as nanoseconds since Jan 1, 1970
 */
static AbstractQoreNode *f_clock_getnanos(const QoreListNode *params, ExceptionSink *xsink) 
{
   qore_timespec_t tp;
   qore_gettime(&tp);

   return new QoreBigIntNode(((int64)tp.tv_sec * (int64)1000000000 + tp.tv_nsec)); 
}

/* qore: clock_getmicros()
 * returns the current system clock time value as microseconds since Jan 1, 1970
 */
static AbstractQoreNode *f_clock_getmicros(const QoreListNode *params, ExceptionSink *xsink) {
   struct timeval tv;
   gettimeofday(&tv, 0);

   return new QoreBigIntNode(((int64)tv.tv_sec * (int64)1000000 + tv.tv_usec));
}

static AbstractQoreNode *f_date_ms(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   int64 ms = p ? p->getAsBigInt() : 0;
   int64 secs = ms / 1000;
   return new DateTimeNode(secs, (int)(ms - secs * 1000));
}

void init_time_functions() {
   builtinFunctions.add("now", f_now);
   builtinFunctions.add("now_ms", f_now_ms); 
   builtinFunctions.add("format_date", f_format_date);
   builtinFunctions.add("localtime", f_localtime);
   builtinFunctions.add("gmtime", f_gmtime);
   builtinFunctions.add("mktime", f_mktime);
   builtinFunctions.add("timegm", f_timegm);

   builtinFunctions.add("get_epoch_seconds",   f_get_epoch_seconds);

   builtinFunctions.add("years",               f_years);
   builtinFunctions.add("months",              f_months);
   builtinFunctions.add("days",                f_days);
   builtinFunctions.add("hours",               f_hours);
   builtinFunctions.add("minutes",             f_minutes);
   builtinFunctions.add("seconds",             f_seconds);
   builtinFunctions.add("milliseconds",        f_milliseconds);

   builtinFunctions.add("get_years",           f_get_years);
   builtinFunctions.add("get_months",          f_get_months);
   builtinFunctions.add("get_days",            f_get_days);
   builtinFunctions.add("get_hours",           f_get_hours);
   builtinFunctions.add("get_minutes",         f_get_minutes);
   builtinFunctions.add("get_seconds",         f_get_seconds);
   builtinFunctions.add("get_milliseconds",    f_get_milliseconds);
   builtinFunctions.add("get_midnight",        f_get_midnight);
   
   builtinFunctions.add("getDayNumber",        f_getDayNumber);
   builtinFunctions.add("getDayOfWeek",        f_getDayOfWeek);
   builtinFunctions.add("getISODayOfWeek",        f_getISODayOfWeek);
   builtinFunctions.add("getISOWeekHash",      f_getISOWeekHash);
   builtinFunctions.add("getISOWeekString",    f_getISOWeekString);
   builtinFunctions.add("getDateFromISOWeek",  f_getDateFromISOWeek);
   
   builtinFunctions.add("clock_getmillis",     f_clock_getmillis);
   builtinFunctions.add("clock_getnanos",      f_clock_getnanos);
   builtinFunctions.add("clock_getmicros",     f_clock_getmicros);

   builtinFunctions.add("date_ms",             f_date_ms);
}
