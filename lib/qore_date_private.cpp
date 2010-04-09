/*
  qore_date_private.cpp

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
#include <qore/intern/qore_date_private.h>

#include <sys/time.h>
#include <errno.h>
#include <string.h>

const char *STATIC_UTC = "UTC";

const int qore_date_info::month_lengths[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int qore_date_info::positive_months[] = { 0,  31,  59,  90,  120,  151,  181,  212,  243,  273,  304,  334,  365 };
const int qore_date_info::negative_months[] = { 0, -31, -61, -92, -122, -153, -184, -214, -245, -275, -306, -334, -365 };

struct date_s {
   const char *long_name;
   const char *abbr;
};

// month names (in English)
static const struct date_s months[] = {
   { "January", "Jan" },
   { "February", "Feb" },
   { "March", "Mar" },
   { "April", "Apr" },
   { "May", "May" },
   { "June", "Jun" },
   { "July", "Jul" },
   { "August", "Aug" },
   { "September", "Sep" },
   { "October", "Oct" },
   { "November", "Nov" },
   { "December", "Dec" }
};

// day names (in English!) FIXME: add locale-awareness!
static const struct date_s days[] = {
   { "Sunday", "Sun" },
   { "Monday", "Mon" },
   { "Tuesday", "Tue" },
   { "Wednesday", "Wed" },
   { "Thursday", "Thu" },
   { "Friday", "Fri" },
   { "Saturday", "Sat" }
};

static int ampm(int hour) {
   int i = hour % 12;
   return i ? i : 12;
}

bool qore_date_info::isLeapYear(int year) {
#if NO_PROLEPTIC_GREGORIAN_CALENDAR
   // in 45 BC Julius Ceasar initiated the Julian calendar
   if (year <= -45)
      return false;
   // in 1582 AD Pope Gregory initiated the Gregorian calendar
   // although it was not universally adopted in Europe at that time
   if (year < 1582)
      return (year % 4) ? false : true;
#endif
   if (!(year % 100)) {
      if (!(year % 400))
	 return true;
      else
	 return false;
   }
   return (year % 4) ? false : true;
}

int64 qore_absolute_time::getRelativeMicroseconds() const {
   // find the difference between gmtime and our time
   struct timeval tv;
   if (gettimeofday(&tv, 0)) {
      printd(0, "qore_absolute_time::getRelativeMicroseconds() gettimeofday() failed: %s\n", strerror(errno));
      return 0;
   }

   int64 ds = (epoch - tv.tv_sec) * 1000000 + (us - tv.tv_usec);
   return ds < 0 ? 0 : ds;
}

void qore_absolute_time::getAsString(QoreString &str) const {
      qore_time_info i;
      get(i);

      str.sprintf("%04d-%02d-%02d %02d:%02d:%02d", i.year, i.month, i.day, i.hour, i.minute, i.second);
      if (us) {
         if (us / 1000 * 1000 == us)
            str.sprintf(".%03d", us / 1000);
         else
            str.sprintf(".%06d", us);
      }
      const char *wday = days[qore_date_info::getDayOfWeek(i.year, i.month, i.day)].abbr;
      str.sprintf(" %s ", wday);
      concatOffset(i.gmtoffset, str);
      str.sprintf(" (%s)", i.zname);
   }

qore_absolute_time &qore_absolute_time::operator+=(const qore_relative_time &dt) {
   int usecs;

   // break down date and do day, month, and year math
   if (dt.year || dt.month || dt.day) {
      // get the broken-down date values for the date in local time
      qore_simple_tm2 tm(epoch + zone->getGMTOffset(epoch), us);

#ifdef DEBUG
      // only needed by the debugging statement at the bottom
      //int64 oe=epoch;
#endif
      //printd(5, "absolute_time::operator+= this=%p %lld.%06d (%d) %04d-%02d-%02d %02d:%02d:%02d.%06d (+%dY %dM %dD %dh %dm %ds %dus)\n", this, epoch, us, zone ? zone->getGMTOffset(epoch) : 0, tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, tm.us, dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.us);
      
      // add years, months, and days
      tm.year += dt.year;
      tm.month += dt.month;
      // normalize to the end of the month
      normalize_dm(tm.year, tm.month, tm.day);

      tm.day += dt.day;
      // normalize to the correct day, month, and year
      normalize_day(tm.year, tm.month, tm.day);

      // get epoch offset for same time on the target day
      epoch = qore_date_info::getEpochSeconds(tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);

      // adjust for new GMT offset for target day at the original time
      epoch -= zone->getGMTOffset(epoch);

      usecs = tm.us;
   }
   else
      usecs = us;

   // get resulting microseconds
   usecs += dt.us;

   // add time component
   epoch += (3600 * dt.hour) + (60 * dt.minute) + dt.second;

   // normalize epoch and microseconds
   normalize_units2<int64, int>(epoch, usecs, 1000000);
   us = usecs;

   //printd(5, "absolute_time::operator+= new epoch=%lld.%06d (diff=%lld)\n", epoch, tm.us, epoch - oe);
   return *this;
}

qore_absolute_time &qore_absolute_time::operator-=(const qore_relative_time &dt) {
   qore_relative_time t(dt);
   t.unaryMinus();
   return *this += t;
}

qore_relative_time &qore_relative_time::operator+=(const qore_relative_time &dt) {
   year += dt.year;
   month += dt.month;
   day += dt.day;
   hour += dt.hour;
   minute += dt.minute;
   second += dt.second;
   us += dt.us;

   normalize();

   return *this;
}

qore_relative_time &qore_relative_time::operator-=(const qore_relative_time &dt) {
   year -= dt.year;
   month -= dt.month;
   day -= dt.day;
   hour -= dt.hour;
   minute -= dt.minute;
   second -= dt.second;
   us -= dt.us;

   normalize();

   return *this;
}

void concatOffset(int gmtoffset, QoreString &str) {
   if (!gmtoffset) {
      str.concat('Z');
      return;
   }

   str.concat(gmtoffset < 0 ? '-' : '+');
   int h = gmtoffset / 3600;
   // the remaining seconds after hours
   int r = gmtoffset - h * 3600;
   // minutes
   int m = r / 60;
   // we have already output the hour sign above
   str.sprintf("%02d:%02d", h < 0 ? -h : h, m);
   // see if there are any seconds
   int s = gmtoffset - h * 3600 - m * 60;
   if (s)
      str.sprintf(":%02d", s);
}

void qore_date_private::format(QoreString &str, const char *fmt) const {
   qore_time_info i;
   get(i);

   QORE_TRACE("qore_date_private::format()");

   const char *s = fmt;
   while (*s) {
      switch (*s) {
         case 'Y':
            if (s[1] != 'Y') {
               str.concat('Y');
               break;
            }
            s++;
            if ((s[1] == 'Y') && (s[2] == 'Y')) {
               str.sprintf("%04d", i.year);
               s += 2;
            }
            else
               str.sprintf("%02d", i.year - (i.year / 100) * 100);
            break;
         case 'M':
            if (s[1] == 'M') {
               str.sprintf("%02d", i.month);
               s++;
               break;
            }
            if ((s[1] == 'o') && (s[2] == 'n')) {
               s += 2;
               if ((s[1] == 't') && (s[2] == 'h')) {
                  s += 2;
                  if (i.month && (i.month <= 12))
                     str.sprintf("%s", months[(int)i.month - 1].long_name);
                  else
                     str.sprintf("Month%d", i.month - 1);
                  break;
               }
               if (i.month && (i.month <= 12))
                  str.sprintf("%s", months[(int)i.month - 1].abbr);
               else
                  str.sprintf("M%02d", i.month);
               break;
            }
            if ((s[1] == 'O') && (s[2] == 'N')) {
               s += 2;
               if ((s[1] == 'T') && (s[2] == 'H')) {
                  s += 2;
                  if (i.month && (i.month <= 12)) {
                     char *t = (char *)str.getBuffer() + str.strlen();
                     str.sprintf("%s", months[(int)i.month - 1].long_name);
                     strtoupper(t);
                  }
                  else
                     str.sprintf("MONTH%d", i.month);
                  break;
               }
               if (i.month && (i.month <= 12)) {
                  char *t = (char *)str.getBuffer() + str.strlen();
                  str.sprintf("%s", months[(int)i.month - 1].abbr);
                  strtoupper(t);
               }
               else
                  str.sprintf("M%02d", i.month);
               break;
            }
            str.sprintf("%d", i.month);
            break;
         case 'D':
            if (s[1] == 'D') {
               str.sprintf("%02d", i.day);
               s++;
               break;
            }
            if ((s[1] == 'a') && (s[2] == 'y')) {
               s += 2;
	       int wday = qore_date_info::getDayOfWeek(i.year, i.month, i.day);
	       str.sprintf("%s", days[wday].long_name);
               break;
            }
            if ((s[1] == 'A') && (s[2] == 'Y')) {
               s += 2;
	       int wday = qore_date_info::getDayOfWeek(i.year, i.month, i.day);
	       char *t = (char *)str.getBuffer() + str.strlen();
	       str.sprintf("%s", days[wday].long_name);
	       strtoupper(t);
               break;
            }
            if ((s[1] == 'y') || (s[1] == 'Y')) {
               s++;;
	       int wday = qore_date_info::getDayOfWeek(i.year, i.month, i.day);
	       char *t = (char *)str.getBuffer() + str.strlen();
	       str.sprintf("%s", days[wday].abbr);
	       if (*s == 'Y')
		  strtoupper(t);
               break;
            }
            str.sprintf("%d", i.day);
            break;
         case 'H':
            if (s[1] == 'H') {
               str.sprintf("%02d", i.hour);
               s++;
            }
            else
               str.sprintf("%d", i.hour);
            break;
         case 'h':
            if (s[1] == 'h') {
               str.sprintf("%02d", ampm(i.hour));
               s++;
            }
            else
               str.sprintf("%d", ampm(i.hour));
            break;
         case 'P':
            if (i.hour > 11)
               str.sprintf("PM");
            else
               str.sprintf("AM");
            break;
         case 'p':
            if (i.hour > 11)
               str.sprintf("pm");
            else
               str.sprintf("am");
            break;
         case 'm':
            if (s[1] == 'm') {
               str.sprintf("%02d", i.minute);
               s++;
            }
            else if (s[1] == 's') {
               str.sprintf("%03d", i.us / 1000);
               s++;
            }
            else
               str.sprintf("%d", i.minute);
            break;
         case 'S':
            if (s[1] == 'S') {
               str.sprintf("%02d", i.second);
               s++;
            }
            else
               str.sprintf("%d", i.second);
            break;
         case 'u':
            if (s[1] == 'u') {
               str.sprintf("%03d", i.us / 1000);
               s++;
            }
            else
               str.sprintf("%d", i.us / 1000);
            break;
         case 'x':
            if (s[1] == 'x') {
               str.sprintf("%06d", i.us);
               s++;
            }
            else
               str.sprintf("%d", i.us);
            break;
         case 'z':
	    str.sprintf("%s", i.zname);
            break;
	    // add iso8601 GMT offset
	 case 'Z':
	    concatOffset(i.gmtoffset, str);
	    break;
         default:
	    str.concat(*s);
            break;
      }
      s++;
   }

   printd(5, "qore_date_private::format() returning \"%s\"\n", str.getBuffer());
}

void qore_simple_tm2::getISOWeek(int &yr, int &week, int &wday) const {
   // get day of week of jan 1 of this year
   int jan1 = qore_date_info::getDayOfWeek(year, 1, 1);

   // calculate day in calendar week
   int dn = qore_date_info::getDayNumber(year, month, day);
   int dow = (dn + jan1 - 1) % 7;
   wday = !dow ? 7 : dow;
   
   //printd(5, "qore_simple_tm2::getISOWeek() year=%d, start=%d, daw=%d dn=%d offset=%d\n", year, jan1, dow, dn, (jan1 > 4 ? 9 - jan1 : 2 - jan1));
   if ((!jan1 && dn == 1) || (jan1 == 5 && dn < 4) || (jan1 == 6 && dn < 3)) {
      yr = year - 1;
      jan1 = qore_date_info::getDayOfWeek(yr, 1, 1);
      //printd(5, "qore_simple_tm2::getISOWeek() previous year=%d, start=%d, leap=%d\n", yr, jan1, qore_date_info::isLeapYear(yr));
      if ((jan1 == 4 && !qore_date_info::isLeapYear(yr)) || (jan1 == 3 && qore_date_info::isLeapYear(yr)))
	 week = 53;
      else
	 week = 52;
      return;
   }
   yr = year;
   
   int offset = jan1 > 4 ? jan1 - 9 : jan1 - 2;
   week = ((dn + offset) / 7) + 1;
   if (week == 53) {
      if ((jan1 == 4 && !qore_date_info::isLeapYear(yr)) || (jan1 == 3 && qore_date_info::isLeapYear(yr)))
	 return;
      else {
	 ++yr;
	 week = 1;
      }
   }
}

