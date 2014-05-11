/* -*- indent-tabs-mode: nil -*- */
/*
  qore_date_private.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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
   const char *upper_long_name;
   const char *upper_abbr;
};

// month names (in English)
static const struct date_s months[] = {
   { "January", "Jan", "JANUARY", "JAN" },
   { "February", "Feb", "FEBRUARY", "FEB" },
   { "March", "Mar", "MARCH", "MAR" },
   { "April", "Apr", "APRIL", "APR" },
   { "May", "May", "MAY", "MAY" },
   { "June", "Jun", "JUNE", "JUN" },
   { "July", "Jul", "JULY", "JUL" },
   { "August", "Aug", "AUGUST", "AUG" },
   { "September", "Sep", "SEPTEMBER", "SEP" },
   { "October", "Oct", "OCTOBER", "OCT" },
   { "November", "Nov", "NOVEMBER", "NOV" },
   { "December", "Dec", "DECEMBER", "DEC" }
};

// day names (in English!) FIXME: add locale-awareness!
static const struct date_s days[] = {
   { "Sunday", "Sun", "SUNDAY", "SUN" },
   { "Monday", "Mon", "MONDAY", "MON" },
   { "Tuesday", "Tue", "TUESDAY", "TUE" },
   { "Wednesday", "Wed", "WEDNESDAY", "WED" },
   { "Thursday", "Thu", "THURSDAY", "THU" },
   { "Friday", "Fri", "FRIDAY", "FRI" },
   { "Saturday", "Sat", "SATURDAY", "SAT" }
};

static int ampm(int hour) {
   int i = hour % 12;
   return i ? i : 12;
}

bool qore_date_info::isLeapYear(int year) {
   if (!(year % 100))
      return !(year % 400) ? true : false;
   return (year % 4) ? false : true;
}

int qore_date_info::getMonthIxFromAbbr(const char* abbr) {
   for (int i = 0; i < 12; ++i) {
      if (!strcasecmp(months[i].abbr, abbr))
         return i;
   }
   // not found
   return -1;
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

   str.sprintf("%04d-%02d-%02d %02d:%02d:%02d.%06d", i.year, i.month, i.day, i.hour, i.minute, i.second, i.us);
   const char *wday = days[qore_date_info::getDayOfWeek(i.year, i.month, i.day)].abbr;
   str.sprintf(" %s ", wday);
   concatOffset(i.utcoffset, str);
   // only concat zone name if it's not the same as the offset just output
   if (*i.zname != '+' && *i.zname != '-')
      str.sprintf(" (%s)", i.zname);
}

qore_absolute_time &qore_absolute_time::operator+=(const qore_relative_time &dt) {
   int usecs;

   // break down date and do day, month, and year math
   if (dt.year || dt.month || dt.day) {
      // get the broken-down date values for the date in local time
      qore_simple_tm2 tm(epoch + zone->getUTCOffset(epoch), us);

#ifdef DEBUG
      // only needed by the debugging statement at the bottom
      //int64 oe=epoch;
#endif
      //printd(5, "absolute_time::operator+= this=%p %lld.%06d (%d) %04d-%02d-%02d %02d:%02d:%02d.%06d (+%dY %dM %dD %dh %dm %ds %dus)\n", this, epoch, us, zone ? zone->getUTCOffset(epoch) : 0, tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, tm.us, dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.us);
      
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

      // adjust for new UTC offset for target day at the original time
      epoch -= zone->getUTCOffset(epoch);

      usecs = tm.us;
   }
   else
      usecs = us;

   // get resulting microseconds
   usecs += dt.us;

   // add time component
   epoch += (SECS_PER_HOUR * dt.hour) + (SECS_PER_MINUTE * dt.minute) + dt.second;

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

void concatOffset(int utcoffset, QoreString &str) {
   //printd(0, "concatOffset(%d)", utcoffset);

   if (!utcoffset) {
      str.concat('Z');
      return;
   }
   
   str.concat(utcoffset < 0 ? '-' : '+');
   if (utcoffset < 0)
      utcoffset = -utcoffset;
   int h = utcoffset / SECS_PER_HOUR;
   // the remaining seconds after hours
   int r = utcoffset % SECS_PER_HOUR;
   // minutes
   int m = r / SECS_PER_MINUTE;
   // we have already output the hour sign above
   str.sprintf("%02d:%02d", h < 0 ? -h : h, m);
   // see if there are any seconds
   int s = utcoffset - h * SECS_PER_HOUR - m * SECS_PER_MINUTE;
   if (s)
      str.sprintf(":%02d", s);
}

void qore_date_private::format(QoreString &str, const char *fmt) const {
   qore_time_info i;
   get(i);

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
                     str.sprintf("%s", months[(int)i.month - 1].upper_long_name);
                  }
                  else
                     str.sprintf("MONTH%d", i.month);
                  break;
               }
               if (i.month && (i.month <= 12)) {
                  str.sprintf("%s", months[(int)i.month - 1].upper_abbr);
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
               str.sprintf("%s", days[wday].upper_long_name);
               break;
            }
            if ((s[1] == 'y') || (s[1] == 'Y')) {
               s++;
               int wday = qore_date_info::getDayOfWeek(i.year, i.month, i.day);
               str.sprintf("%s", *s == 'Y' ? days[wday].upper_abbr : days[wday].abbr);
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
            else if (s[1] == 's') {
               str.sprintf("%06d", i.us);
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
         case 'y':
            str.sprintf("%06d", i.us);
            str.trim_trailing('0');
            break;
         case 'z':
	    str.sprintf("%s", i.zname);
            break;
	    // add iso8601 UTC offset
	 case 'Z':
	    concatOffset(i.utcoffset, str);
	    break;
         default:
	    str.concat(*s);
            break;
      }
      s++;
   }
}

static int get_uint(const char *&p, int digits) {
   int rc = 0;
   for (int i = 0; i < digits; ++i) {
      if (*p < '0' || *p > '9')
         return -1;
      rc = (10 * rc) + ((*p) - '0');
      ++p;
   }
   return rc;
}

static int get_int(const char *&p, bool &err) {
   int rc = 0;

   int sign = 1;
   if (*p == '-') {
      sign = -1;
      ++p;
   }

   if (!isdigit(*p)) {
      err = true;
      return 0;
   }

   do {
      rc = (10 * rc) + ((*p) - '0');
      ++p;
   } while (isdigit(*p));

   return rc * sign;
}


void qore_date_private::setISO8601RelativeDate(const char *str) {
   const char *p = str;
   if (*p == 'P' || *p == 'p')
      ++p;

   bool time = false;
   d.rel.zero();

   bool err = false;
   while (true) {
      if (*p == 'T' || *p == 't') {
         time = true;
         ++p;
      }
      int val = get_int(p, err);
      if (err)
         break;

      switch (*p) {
         case 'Y':
         case 'y':
            d.rel.year += val;
            break;

         case 'M':
         case 'm':
            if (time)
               d.rel.minute += val;
            else
               d.rel.month += val;
            break;

         case 'D':
         case 'd':
            d.rel.day += val;
            break;

         case 'H':
         case 'h':
            if (!time)
               return;

            d.rel.hour += val;
            break;

         case 'S':
         case 's':
            if (!time)
               return;

            d.rel.second += val;
            break;

            // non-ISO-8601 extension: <int>u for microseconds
         case 'u':
            if (!time)
               return;

            d.rel.us += val;
            break;
         default:
            break;
      }
      ++p;
   }
}

void qore_date_private::setRelativeDate(const char *str) {
   relative = true;

   if (*str == 'P' || *str == 'p') {
      setISO8601RelativeDate(str);
      return;
   }

#ifdef HAVE_STRTOLL
   int64 date = strtoll(str, 0, 10);
#else
   int64 date = atoll(str);
#endif
   const char *p = strchr(str, '.');

   int l = p ? p - str : strlen(str);
   // for date-only strings, move the date up to the right position
   if (l == 8)
      date *= 1000000;

   int us = p ? atoi(p + 1) : 0;
   if (us) {
      l = strlen(p + 1);
      assert(l < 7);
      us *= (int)pow((double)10, 6 - l);
   }         

   d.rel.setLiteral(date, us);
}

void qore_date_private::setAbsoluteDate(const char *str, const AbstractQoreZoneInfo *zone) {
   relative = false;

   size_t len = strlen(str);

   // we need at last YYYYMMDD
   if (len < 8) {
      d.abs.set(zone, 0, 0);
      return;
   }
      
   const char *p = str;

   int year = get_uint(p, 4);
   if (year < 0) {
      d.abs.set(zone, 0, 0);
      return;
   }

   bool needs_sep = false;
   if (*p == '-') {
      needs_sep = true;
      ++p;
   }
   
   int month = get_uint(p, 2);
   if (month < 0) {
      d.abs.set(zone, year, 1, 1, 0, 0, 0, 0);
      return;
   }

   if (needs_sep) {
      if (*p == '-')
         ++p;
      else {
         d.abs.set(zone, year, month, 1, 0, 0, 0, 0);
         return;
      }
   }

   int day = get_uint(p, 2);
   if (day < 0) {
      d.abs.set(zone, year, month, 1, 0, 0, 0, 0);
      return;
   }

   //printd(5, "date: %04d-%02d-%02d\n", year, month, day);

   if (*p == 'Z' || (*p == ' ' && *(p + 1) == 'Z')) {
      d.abs.set(0, year, month, day, 0, 0, 0, 0);
      return;
   }

   if (*p == ' ' || *p == 't' || *p == 'T' || *p == '-')
      ++p;

   int hour = get_uint(p, 2);
   if (hour < 0) {
      d.abs.set(zone, year, month, day, 0, 0, 0, 0);
      return;
   }

   needs_sep = false;
   if (*p == ':') {
      needs_sep = true;
      ++p;
   }

   int minute = get_uint(p, 2);
   if (minute < 0) {
      d.abs.set(zone, year, month, day, hour, 0, 0, 0);
      return;
   }

   if (needs_sep) {
      if (*p == ':')
         ++p;
      else {
         d.abs.set(zone, year, month, day, hour, minute, 0, 0);
         return;
      }
   }

   int second = get_uint(p, 2);
   if (second < 0) {
      d.abs.set(zone, year, month, day, hour, minute, 0, 0);
      return;
   }

   if (!*p) {
      d.abs.set(zone, year, month, day, hour, minute, second, 0);
      return;
   }
	 
   int us = 0;
   if (*p == '.') {
      ++p;
      if (!isdigit(*p)) {
         d.abs.set(zone, year, month, day, hour, minute, second, 0);
         return;
      }
	 
      // read all digits
      int dlen = 0;
      while (isdigit(*p)) {
         // ignore excess digits beyond microsecond resolution
         if (dlen < 6) {
            us *= 10;
            us += *p - '0';
         }
	 ++dlen;
	 ++p;
      }
	 
      // adjust to microseconds
      while (dlen < 6) {
	 us *= 10;
	 ++dlen;
      }

      if (!*p) {
         d.abs.set(zone, year, month, day, hour, minute, second, us);
         return;
      }
   }

   // get time zone offset

   // read 
   if (*p == ' ')
      ++p;

   if (*p == 'Z') {
      zone = 0;
      ++p;
   }
   else if (*p == '+' || *p == '-') {
      int mult = *p == '-' ? -1 : 1;
	 
      ++p;
      if (!isdigit(*p)) {
         d.abs.set(zone, year, month, day, hour, minute, second, us);
         return;

      }
	 
      int utc_h = *p - '0';
      ++p;
      if (isdigit(*p)) {
	 utc_h = utc_h * 10 + (*p - '0');
	 ++p;
      }
	 
      int offset = utc_h * 3600;
	 
      if (*p) {
	 if (*p == ':')
            ++p;
	    
	 if (!isdigit(*p)) {
            zone = findCreateOffsetZone(offset * mult);
            d.abs.set(zone, year, month, day, hour, minute, second, us);
            return;
         }
	    
	 int utc_m = *p - '0';
	 ++p;
	 if (isdigit(*p)) {
	    utc_m = utc_m * 10 + (*p - '0');
	    ++p;
	 }
	    
	 offset += utc_m * 60;
	    
	 if (*p) {
	    if (*p == ':')
               ++p;
	       
	    if (!isdigit(*p)) {
               zone = findCreateOffsetZone(offset * mult);
               d.abs.set(zone, year, month, day, hour, minute, second, us);
               return;
            }
	       
	    int utc_s = *p - '0';
	    ++p;
	    if (isdigit(*p)) {
	       utc_s = utc_s * 10 + (*p - '0');
	       ++p;
	    }
	       
	    offset += utc_s;		    
	 }
      }
	 
      zone = findCreateOffsetZone(offset * mult);
   }

   d.abs.set(zone, year, month, day, hour, minute, second, us); 
}

void qore_date_private::setDate(const char *str) {
   assert(str);
   if (*str == 'P' || *str == 'p')
      setRelativeDate(str);
   else
      setAbsoluteDate(str);
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

// normalize the given date to the last day of the month
void normalize_dm(int &year, int &month, int &day) {
   // normalize months and years
   if (month > 12 || month < 1) {
      --month;
      normalize_units2<int, int>(year, month, 12);
      ++month;
   }

   // fix resulting day of month; check for leap years
   if (month == 2 && day > 28)
      day = qore_date_info::isLeapYear(year) ? 29 : 28;
   else // otherwise set day to last day of month if necessary
      if (day > qore_date_info::month_lengths[month])
         day = qore_date_info::month_lengths[month];
}

// normalize to the correct day, month, and year
void normalize_day(int &year, int &month, int &day) {
   // assumes month is already normalized
   assert(month > 0 && month < 13);

   if (day > 0) {
      int i;
      while (day > (i = qore_date_info::getLastDayOfMonth(month, year))) {
         day -= i;
         ++month;
         if (month == 13) {
            month = 1;
            ++year;
         }
      }
      return;
   }

   while (day < 1) {
      --month;
      if (!month) {
         month = 12;
         --year;
      }
      day += qore_date_info::getLastDayOfMonth(month, year);
   }
}
