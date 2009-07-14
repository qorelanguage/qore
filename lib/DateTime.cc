/*
  DateTime.cc

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
#include <qore/intern/qore_date_private.h>

#include <time.h>
#include <stdlib.h>
#include <string.h>

struct date_s {
   const char *long_name;
   const char *abbr;
};

const int DateTime::month_lengths[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int DateTime::positive_months[] = { 0,  31,  59,  90,  120,  151,  181,  212,  243,  273,  304,  334,  365 };
const int DateTime::negative_months[] = { 0, -31, -61, -92, -122, -153, -184, -214, -245, -275, -306, -334, -365 };

// month names (in English)
const struct date_s months[] = {
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
const struct date_s days[] = {
   { "Sunday", "Sun" },
   { "Monday", "Mon" },
   { "Tuesday", "Tue" },
   { "Wednesday", "Wed" },
   { "Thursday", "Thu" },
   { "Friday", "Fri" },
   { "Saturday", "Sat" }
};

static inline int ampm(int hour)
{
   int i;

   if (!(i = (hour % 12))) return 12;
   return i;
}

DateTime::DateTime(bool r) : priv(new qore_dt_private)
{
   if (r)
   {
      priv->year = 0;
      priv->month = 0;
      priv->day = 0;
   }
   else
   {
      priv->year = 1970;
      priv->month = 1;
      priv->day = 1;
   }
   priv->hour = 0;
   priv->minute = 0;
   priv->second = 0;
   priv->millisecond = 0;
   priv->relative = r;
}

DateTime::DateTime(const DateTime &dt) : priv(new qore_dt_private(*dt.priv))
{
}

DateTime::DateTime(int y, int mo, int d, int h, int mi, int s, short ms, bool r) : priv(new qore_dt_private)
{
   if (!r && !y && !mo && !d)
   {
      priv->year = 1970;
      priv->month = 1;
      priv->day = 1;
   }
   else
   {
      priv->year = y;
      priv->month = mo;
      priv->day = d;
   }
   priv->hour = h;
   priv->minute = mi;
   priv->second = s;
   priv->millisecond = ms;
   priv->relative = r;
}

DateTime::~DateTime()
{
   delete priv;
}

bool DateTime::isRelative() const
{
   return priv->relative;
}

bool DateTime::isAbsolute() const
{
   return !priv->relative;
}

short DateTime::getYear() const
{
   return priv->year;
}

int DateTime::getMonth() const
{
   return priv->month;
}

int DateTime::getDay() const
{
   return priv->day;
}

int DateTime::getHour() const
{
   return priv->hour;
}

int DateTime::getMinute() const
{
   return priv->minute;
}

int DateTime::getSecond() const
{
   return priv->second;
}

int DateTime::getMillisecond() const
{
   return priv->millisecond;
}

void DateTime::setTime(int h, int m, int s, short ms)
{
   priv->hour = h;
   priv->minute = m;
   priv->second = s;
   priv->millisecond = ms;
}

int DateTime::getDayNumber() const
{
   return positive_months[(priv->month < 13 ? priv->month : 12) - 1] + priv->day + (priv->month > 2 && isLeapYear(priv->year) ? 1 : 0);
}

// static method
int DateTime::getDayOfWeek(int year, int month, int day)
{
   int a = (14 - month) / 12;
   int y = year - a;
   int m = month + 12 * a - 2;
   return (day + y + y / 4 - y / 100 + y / 400 + (31 * m / 12)) % 7;
}

int DateTime::getDayOfWeek() const
{
   return getDayOfWeek(priv->year, priv->month, priv->day);
}

void DateTime::format(QoreString &str, const char *fmt) const
{
   struct tm nt;

   QORE_TRACE("DateTime::format()");

   const char *s = fmt;
   while (*s)
   {
      switch (*s)
      {
         case 'Y':
            if (s[1] != 'Y')
            {
               str.concat('Y');
               break;
            }
            s++;
            if ((s[1] == 'Y') && (s[2] == 'Y'))
            {
               str.sprintf("%04d", priv->year);
               s += 2;
            }
            else
               str.sprintf("%02d", priv->year - (priv->year / 100) * 100);
            break;
         case 'M':
            if (s[1] == 'M')
            {
               str.sprintf("%02d", priv->month);
               s++;
               break;
            }
            if ((s[1] == 'o') && (s[2] == 'n'))
            {
               s += 2;
               if ((s[1] == 't') && (s[2] == 'h'))
               {
                  s += 2;
                  if (priv->month && (priv->month <= 12))
                     str.sprintf("%s", months[(int)priv->month - 1].long_name);
                  else
                     str.sprintf("Month%d", priv->month - 1);
                  break;
               }
               if (priv->month && (priv->month <= 12))
                  str.sprintf("%s", months[(int)priv->month - 1].abbr);
               else
                  str.sprintf("M%02d", priv->month);
               break;
            }
            if ((s[1] == 'O') && (s[2] == 'N'))
            {
               s += 2;
               if ((s[1] == 'T') && (s[2] == 'H'))
               {
                  s += 2;
                  if (priv->month && (priv->month <= 12))
                  {
                     char *t = (char *)str.getBuffer() + str.strlen();
                     str.sprintf("%s", months[(int)priv->month - 1].long_name);
                     strtoupper(t);
                  }
                  else
                     str.sprintf("MONTH%d", priv->month);
                  break;
               }
               if (priv->month && (priv->month <= 12))
               {
                  char *t = (char *)str.getBuffer() + str.strlen();
                  str.sprintf("%s", months[(int)priv->month - 1].abbr);
                  strtoupper(t);
               }
               else
                  str.sprintf("M%02d", priv->month);
               break;
            }
            str.sprintf("%d", priv->month);
            break;
         case 'D':
            if (s[1] == 'D')
            {
               str.sprintf("%02d", priv->day);
               s++;
               break;
            }
            if ((s[1] == 'a') && (s[2] == 'y'))
            {
               s += 2;
               getTM(&nt);
               if (mktime(&nt) == -1) // invalid time
                  str.sprintf("Day%d", priv->day);
               else
                  str.sprintf("%s", days[nt.tm_wday].long_name);
               break;
            }
            if ((s[1] == 'A') && (s[2] == 'Y'))
            {
               s += 2;
               getTM(&nt);
               if (mktime(&nt) == -1) // invalid time
                  str.sprintf("DAY%d", priv->day);
               else
               {
                  char *t = (char *)str.getBuffer() + str.strlen();
                  str.sprintf("%s", days[nt.tm_wday].long_name);
                  strtoupper(t);
               }
               break;
            }
            if ((s[1] == 'y') || (s[1] == 'Y'))
            {
               s++;;
               getTM(&nt);
               if (mktime(&nt) == -1) // invalid time
                  str.sprintf("D%02d", priv->day);
               else
               {
                  char *t = (char *)str.getBuffer() + str.strlen();
                  str.sprintf("%s", days[nt.tm_wday].abbr);
                  if (*s == 'Y')
                     strtoupper(t);
               }
               break;
            }
            str.sprintf("%d", priv->day);
            break;
         case 'H':
            if (s[1] == 'H')
            {
               str.sprintf("%02d", priv->hour);
               s++;
            }
            else
               str.sprintf("%d", priv->hour);
            break;
         case 'h':
            if (s[1] == 'h')
            {
               str.sprintf("%02d", ampm(priv->hour));
               s++;
            }
            else
               str.sprintf("%d", ampm(priv->hour));
            break;
         case 'P':
            if (priv->hour > 11)
               str.sprintf("PM");
            else
               str.sprintf("AM");
            break;
         case 'p':
            if (priv->hour > 11)
               str.sprintf("pm");
            else
               str.sprintf("am");
            break;
         case 'm':
            if (s[1] == 'm')
            {
               str.sprintf("%02d", priv->minute);
               s++;
            }
            else if (s[1] == 's')
            {
               str.sprintf("%03d", priv->millisecond);
               s++;
            }
            else
               str.sprintf("%d", priv->minute);
            break;
         case 'S':
            if (s[1] == 'S') {
               str.sprintf("%02d", priv->second);
               s++;
            }
            else
               str.sprintf("%d", priv->second);
            break;
         case 'u':
            if (s[1] == 'u') {
               str.sprintf("%03d", priv->millisecond);
               s++;
            }
            else
               str.sprintf("%d", priv->millisecond);
            break;
         default:
	    str.concat(*s);
            break;
      }
      s++;
   }

   printd(5, "DateTime::format() returning \"%s\"\n", str.getBuffer());

}

// set the date from the number of seconds since January 1, 1970 (UNIX epoch)
void DateTime::setDate(int64 seconds, int ms)
{
   if (ms >= 1000 || ms <= -1000)
   {
      int ds = ms / 1000;
      seconds += ds;
      ms -= ds * 1000;
   }
   setDate(seconds);
   priv->millisecond = ms;
}

// set the date from the number of seconds since January 1, 1970 (UNIX epoch)
void DateTime::setDate(int64 seconds)
{
   priv->relative = false;
   priv->millisecond = priv->year = 0;
   // there are 97 leap days every 400 years (12622780800 seconds)
   int64 ty = seconds/12622780800ll;
   if (ty)
   {
      priv->year += ty * 400;
      seconds -= ty * 12622780800ll;
   }
   // there are 24 leap days every 100 years (3155673600 seconds)
   ty = seconds/3155673600ll;
   if (ty)
   {
      priv->year += ty * 100;
      seconds -= ty * 3155673600ll;
   }
   // then there are leap days every 4 years (126230400 seconds)
   ty = seconds/126230400;
   if (ty)
   {
      priv->year += ty * 4;
      seconds -= ty * 126230400;
   }
   //printd(0, "seconds: %lld year: %d\n", seconds, year);
   if (seconds >= 0)
   {
      // now we have taken care of all the seconds after 1974 - we now have
      // to deal with the time period from 1970 - 1974
      
      // there is a leap day on 1972.02.29 (ends 68256000 seconds)
      // to calculate the year with accuracy, we subtract this leap day from 
      // the second total if the date is after (or exactly equal to) Dec 31
      // 1972 (at 94608000 seconds) - this way we can assume years of equal
      // length = 31536000 seconds

      if (seconds >= 94608000)
	 ty = (seconds - 86400) / 31536000;
      else
	 ty = seconds / 31536000;
      // now that we have the year, we take out the seconds for the years we just calculated
      seconds -= ty * 31536000;
      // if we are after 1972, then we take the leap day in 1972 out so it doesn't mess up the day calculations
      if (ty > 2)
	 seconds -= 86400;

      // now we calculate the actual year of our date
      priv->year += ty + 1970;
      
      priv->day = seconds / 86400;
      seconds -= priv->day * 86400;
      //printd(5, "seconds=%lld year=%d day=%d\n", seconds, year, day);
      bool ly = isLeapYear(priv->year);
      // FIXME: this is inefficient - needs to be optimized
      for (int i = 1; ;i++)
      {
	 //printd(5, "day=%d, positive_months[%d]=%d, adjusted=%d\n", day, i, positive_months[i], (positive_months[i] + (ly && i > 1 ? 1 : 0)));
         if ((positive_months[i] + (ly && i > 1 ? 1 : 0)) > priv->day)
	 {
	    //printd(5, "pm[%d]=%d pm[%d]=%d month=%d day=%d (%d)\n", i, positive_months[i], i - 1, positive_months[i - 1], i, day, day - positive_months[i - 1] + 1);
	    priv->month = i;
	    priv->day = priv->day - positive_months[i - 1] + (ly && i > 2 ? 0 : 1);
	    break;
	 }
      }

      priv->hour = seconds / 3600;
      seconds -= priv->hour * 3600;
      priv->minute = seconds / 60;
      priv->second = seconds - priv->minute * 60;
      return;
   }

   // now we have taken care of all the seconds before 1966 - we now have
   // to deal with the time period from 1970 - 1966
   
   // there is a leap day on 1968-02-29 (ends -58060800 seconds)
   // to calculate the year with accuracy, we subtract this leap day from 
   // the second total if the date is before (or exactly equal to) Jan 2
   // 1968 (at -63072000 seconds) - this way we can assume years of equal
   // length = 31536000 seconds
   
   if (seconds <= -63072000)
      ty = (seconds + 86400) / 31536000;
   else
      ty = seconds / 31536000;
   // now that we have the year, we take out the seconds for the years we just calculated
   seconds -= ty * 31536000;
   // if we are before 1968, then we take the leap day in 1968 out so it doesn't mess up the day calculations
   if (ty < -1)
      seconds += 86400;

   //printd(5, "ty=%lld\n", ty);
   // now we calculate the actual year of our date (unless there are no seconds to take us further back,
   // in this case we are one off, which we correct below 
   priv->year += ty + 1969;

   //printf("year:%d\n", year);
   if (!seconds)
   {
      // calculate actual year
      priv->year++;
      priv->month = 1;
      priv->day = 1;
      priv->hour = 00;
      priv->minute = 00;
      priv->second = 00;
      return;
   }

   //printd(5, "1: year=%d seconds=%lld\n", year, seconds);
   priv->day = seconds / 86400;
   seconds -= priv->day * 86400;
   // move back a day if there is further time to subtract
   if (seconds)
      priv->day--;
   bool ly = isLeapYear(priv->year);
   //printd(5, "1.1: seconds=%lld day=%d ly=%s\n", seconds, day, ly ? "true" : "false");
   // FIXME: this is inefficient - needs to be optimized
   for (int i = 1; ; i++)
   {
#if 0
      printd(5, "nm[%d]=%d nm[%d]=%d adj=%d len=%d adj=%d day=%d (mon=%d day=%d)\n", i, negative_months[i], i - 1, 
	     negative_months[i - 1], (negative_months[i - 1] - (ly && i == 12 ? 1 : 0)),
	     month_lengths[13 - i], month_lengths[13 - i] + (ly && (13 - i) == 2 ? 1 : 0),
	     priv->day, 13 - i,
	     month_lengths[13 - i] + (ly && (13 - i) == 2) + priv->day - negative_months[i - 1] + (ly && i == 12 ? 0 : 1));
#endif
      // check to find out what month we're in - add extra days for jan & feb if it's a leap year
      if ((negative_months[i] - (ly && i > 10 ? 1 : 0)) <= priv->day)
      {
	 priv->month = 13 - i;
	 priv->day = month_lengths[priv->month] + (ly && priv->month == 2 ? 1 : 0) + priv->day - negative_months[i - 1] + (ly && i == 12 ? 2 : 1);
	 break;
      }
   }
   //printd(5, "2: seconds=%lld\n", seconds);
   priv->hour = seconds / 3600;
   seconds -= priv->hour * 3600;
   priv->minute = seconds / 60;
   seconds -= priv->minute * 60;
   if ((priv->second = seconds))
   {
      priv->second += 60;
      priv->minute--;
   }
   if (priv->minute) 
   {
      priv->minute += 60;
      priv->hour--;
   }
   if (priv->hour) priv->hour += 24;
}

void DateTime::setDate(const DateTime &date)
{
   priv->setDate(date.priv);
}

// static method
int DateTime::negative_leap_years(int year)
{
   year = 1970 - year - 1;
   
   if (year <= 0)
      return 0;
   
   year += 2;
   
   return -year/4 + year/100 - year/400;
}

// static method
int DateTime::positive_leap_years(int year, int month)
{
   if (month < 3 && isLeapYear(year))
      year--;

   year -= 1970;

   if (year <= 0)
      return 0;

   year += 2;
   
   return year/4 - year/100 + year/400;
}

// get the number of seconds before or after January 1, 1970 (UNIX epoch) for a particular date
// private static method
int64 DateTime::getEpochSeconds(int year, int month, int day)
{
   //printd(0, "%04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
   int tm = month;
   if (month < 0) tm = 1;
   else if (month > 12) tm = 12;
   
   if (year >= 1970)
      return ((int64)year - 1970) * 31536000ll
	 + (positive_months[tm - 1] + day - 1 + positive_leap_years(year, month)) * 86400;

   bool ly = isLeapYear(year);
   //printd(5, "DBG: %d %lld\n", year, ((int64)year - 1969) * 31536000ll);
   return ((int64)year - 1969) * 31536000ll
      + (negative_months[12 - tm] - (ly && tm < 2 ? 1 : 0) + (day - (month_lengths[tm] + (ly && tm == 2 ? 1 : 0))) 
      + negative_leap_years(year) - 1) * 86400;
}

// get the number of seconds before or after January 1, 1970 (UNIX epoch)
int64 DateTime::getEpochSeconds() const
{
   //printd(0, "%04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
   int tm = priv->month;
   if (priv->month < 0) tm = 1;
   else if (priv->month > 12) tm = 12;

   if (priv->year >= 1970)
      return ((int64)priv->year - 1970) * 31536000ll
	 + (positive_months[tm - 1] + priv->day - 1 + positive_leap_years(priv->year, priv->month)) * 86400
	 + priv->hour * 3600
	 + priv->minute * 60
	 + priv->second;

   bool ly = isLeapYear(priv->year);
   //printd(5, "DBG: %d %lld\n", year, ((int64)year - 1969) * 31536000ll);

#if 0
   printd(5, "tm=%d neg_mon[%d]=%d adj=%d day=%d mon_len[%d]=%d adj=%d nly=%d (%d) day=%d\n", tm, 12 - tm, negative_months[12 - tm], 
	  negative_months[12 - tm] - (ly && tm < 3 ? 1 : 0), priv->day, tm, month_lengths[tm],
	  month_lengths[tm] + (ly && tm == 2 ? 1 : 0), negative_leap_years(priv->year), negative_leap_years(priv->year) * 86400,
	  (negative_months[12 - tm] - (ly && tm < 2 ? 1 : 0) + (priv->day - (month_lengths[tm] + (ly && tm == 2 ? 1 : 0))) 
	   + negative_leap_years(priv->year)) - 1
	  );
#endif
   return ((int64)priv->year - 1969) * 31536000ll
      + (negative_months[12 - tm] - (ly && tm < 2 ? 1 : 0) + (priv->day - (month_lengths[tm] + (ly && tm == 2 ? 1 : 0))) 
      + negative_leap_years(priv->year)) * 86400
      + (priv->hour - 23) * 3600
      + (priv->minute - 59) * 60
      + (priv->second - 60);	 
}

void DateTime::setDateLiteral(int64 date)
{
   priv->year = date / 10000000000ll;
   date -= priv->year * 10000000000ll;
   priv->month = date / 100000000ll;
   date -= priv->month * 100000000ll;
   priv->day = date / 1000000ll;
   date -= priv->day * 1000000ll;
   priv->hour = date / 10000ll; 
   date -= priv->hour * 10000ll;
   priv->minute = date / 100ll;
   priv->second = date - priv->minute * 100ll;
   priv->millisecond = 0;

   if (priv->second > 59)
   {
      priv->minute += (priv->second / 60);
      priv->second %= 60;
   }
   if (priv->minute > 59)
   {
      priv->hour += (priv->minute / 60);
      priv->minute %= 60;
   }
   if (priv->hour > 23)
   {
      priv->day += (priv->hour / 24);
      priv->hour %= 24;
   }
   // adjust month and year
   if (priv->month > 12)
   {
      priv->year += ((priv->month - 1)/ 12);
      priv->month = ((priv->month - 1) % 12) + 1;
   }
   // now check day
   if (priv->day)
   {
      int i;
      while (priv->day > (i = getLastDayOfMonth(priv->month, priv->year)))
      {
	 priv->day -= i;
	 priv->month++;
	 if (priv->month == 13)
	 {
	    priv->month = 1;
	    priv->year++;
	 }
      }
   }
   priv->relative = false;
}

void DateTime::setRelativeDateLiteral(int64 date)
{
   priv->year = date / 10000000000ll;
   date -= priv->year * 10000000000ll;
   priv->month = date / 100000000ll;
   date -= priv->month * 100000000ll;
   priv->day = date / 1000000ll;
   date -= priv->day * 1000000ll;
   priv->hour = date / 10000ll; 
   date -= priv->hour * 10000ll;
   priv->minute = date / 100ll;
   priv->second = date - priv->minute * 100ll;
   priv->millisecond = 0;   
   priv->relative = true;
}

// return the ISO-8601 calendar week information - note that the ISO-8601 calendar year may be different than the actual year
void DateTime::getISOWeek(int &yr, int &week, int &wday) const
{
   // get day of week of jan 1 of this year
   int jan1 = getDayOfWeek(priv->year, 1, 1);

   // calculate day in calendar week
   int dn = getDayNumber();
   int dow = (dn + jan1 - 1) % 7;
   wday = !dow ? 7 : dow;
   
   //printd(5, "getISOWeek() year=%d, start=%d, daw=%d dn=%d offset=%d\n", year, jan1, dow, dn, (jan1 > 4 ? 9 - jan1 : 2 - jan1));
   if ((!jan1 && dn == 1) || (jan1 == 5 && dn < 4) || (jan1 == 6 && dn < 3))
   {
      yr = priv->year - 1;
      jan1 = getDayOfWeek(yr, 1, 1);
      //printd(5, "getISOWeek() previous year=%d, start=%d, leap=%d\n", yr, jan1, isLeapYear(yr));
      if ((jan1 == 4 && !isLeapYear(yr)) || (jan1 == 3 && isLeapYear(yr)))
	 week = 53;
      else
	 week = 52;
      return;
   }
   yr = priv->year;
   
   int offset = jan1 > 4 ? jan1 - 9 : jan1 - 2;
   week = ((dn + offset) / 7) + 1;
   if (week == 53) {
      if ((jan1 == 4 && !isLeapYear(yr)) || (jan1 == 3 && isLeapYear(yr)))
	 return;
      else {
	 yr++;
	 week = 1;
      }
   }
}

// note that ISO-8601 week days go from 1 - 7 = Mon - Sun
// a NULL return value means an exception was raised
// static method
DateTime *DateTime::getDateFromISOWeek(int year, int week, int day, ExceptionSink *xsink)
{
   std::auto_ptr<DateTime> rv(new DateTime());
   if (getDateFromISOWeekIntern(*rv, year, week, day, xsink))
      return 0;
   return rv.release();
}

// static method
// note that ISO-8601 week days go from 1 - 7 = Mon - Sun
// return value: -1 = an exception was raised, 0 = OK
int DateTime::getDateFromISOWeekIntern(DateTime &result, int year, int week, int day, ExceptionSink *xsink)
{
   if (week <= 0)
   {
      xsink->raiseException("ISO-8601-INVALID-WEEK", "week numbers must be positive (value passed: %d)", week);
      return -1;
   }

   // get day of week of jan 1 of this year
   int jan1 = getDayOfWeek(year, 1, 1);

   if (week > 52)
   {
      // get maximum week number in this year
      int mw = 52 + ((jan1 == 4 && !isLeapYear(year)) || (jan1 == 3 && isLeapYear(year)));
      if (week > mw)
      {
	 xsink->raiseException("ISO-8601-INVALID-WEEK", "there are only %d calendar weeks in year %d (week value passed: %d)", mw, year, week);
	 return -1;
      }
   }
   
   if (day < 1 || day > 7)
   {
      xsink->raiseException("ISO-8601-INVALID-DAY", "calendar week days must be between 1 and 7 for Mon - Sun (day value passed: %f)", day);
      return -1;
   }

   // get year, month, day for start of iso-8601 calendar year
   int y, m, d;
   // if jan1 is mon, then the iso-8601 year starts with the normal year
   if (jan1 == 1)
   {
      y = year;
      m = 1;
      d = 1;
   }
   // if jan1 is tue - thurs, iso-8601 year starts in dec of previous real year
   else if (jan1 > 1 && jan1 < 5)
   {
      y = year - 1;
      m = 12;
      d = 33 - jan1;
   }
   else
   {
      y = year;
      m = 1;
      // jan1 is fri or saturday
      if (jan1)
	 d = 9 - jan1;
      else // jan1 is sunday
	 d = 2;
   }
   
   // get seconds for date of start of iso-8601 calendar year, add seconds for day offset and create new time
   result.setDate(getEpochSeconds(y, m, d) + ((week - 1) * 7 + (day - 1)) * 86400);
   return 0;
}

int DateTime::compareDates(const DateTime *left, const DateTime *right)
{
   return qore_dt_private::compareDates(left->priv, right->priv);
}

// FIXME: implement and use
bool DateTime::checkValidity() const
{
   return true;
}

int64 DateTime::getRelativeSeconds() const
{
   if (priv->relative)
      return priv->millisecond/1000 + priv->second + priv->minute * 60 + priv->hour * 3600ll + priv->day * 86400ll 
	 + (priv->month ? priv->month * 2592000ll : 0)
	 + (priv->year ? priv->year * 31536000ll : 0);
   
   // find the difference between localtime and now
   time_t ct = time(0);
   struct tm tms;
   DateTime od(q_localtime(&ct, &tms));
   int64 diff = od.getEpochSeconds() - getEpochSeconds();
   if (diff < 0)
      return 0;
   return diff;
}

int64 DateTime::getRelativeMilliseconds() const
{
   if (priv->relative)
      return priv->millisecond + priv->second * 1000ll + priv->minute * 60000ll + priv->hour * 3600000ll + priv->day * 86400000ll 
	 + (priv->month ? priv->month * 2592000000ll : 0)
	 + (priv->year ? priv->year * 31536000000ll : 0);
   
   // find the difference between localtime and now
   time_t ct = time(0);
   struct tm tms;
   DateTime od(q_localtime(&ct, &tms));
   int64 diff = (od.getEpochSeconds() - getEpochSeconds()) * 1000 + od.priv->millisecond - priv->millisecond;
   if (diff < 0)
      return 0;
   return diff;
}

// static methods
bool DateTime::isLeapYear(int year)
{
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

// static function
int DateTime::getLastDayOfMonth(int month, int year)
{
   if (month != 2)
      return month_lengths[month];
   return isLeapYear(year) ? 29 : 28;
}

DateTime::DateTime(const struct tm *tms) : priv(new qore_dt_private)
{
   setDate(tms);
}

DateTime::DateTime(int64 seconds) : priv(new qore_dt_private)
{
   setDate(seconds);
}

DateTime::DateTime(int64 seconds, int ms) : priv(new qore_dt_private)
{
   setDate(seconds, ms);
}

DateTime::DateTime(const char *str) : priv(new qore_dt_private)
{
   setDate(str);
}

void DateTime::getTM(struct tm *tms) const
{
   tms->tm_year = priv->year - 1900;
   tms->tm_mon = priv->month - 1;
   tms->tm_mday = priv->day;
   tms->tm_hour = priv->hour;
   tms->tm_min = priv->minute;
   tms->tm_sec = priv->second;
   tms->tm_isdst = 0;
   tms->tm_wday = 0;
   tms->tm_yday = 0;
   tms->tm_isdst = -1;
}

void DateTime::setDate(const struct tm *tms, short ms)
{
   priv->year = 1900 + tms->tm_year;
   priv->month = tms->tm_mon + 1;
   priv->day = tms->tm_mday;
   priv->hour = tms->tm_hour;
   priv->minute = tms->tm_min;
   priv->second = tms->tm_sec;
   priv->millisecond = ms;
   priv->relative = false;
}

void DateTime::setDate(const char *str)
{
#ifdef HAVE_STRTOLL
   int64 date = strtoll(str, 0, 10);
#else
   int64 date = atoll(str);
#endif
   int l = strlen(str);
   // for date-only strings, move the date up to the right position
   if (l == 8)
      date *= 1000000;
   else if (l == 6 || l == 10) // for time-only strings
      date += 19700101000000LL;
   setDateLiteral(date);
   // check for ms
   const char *p = strchr(str, '.');
   if (!p)
      return;
   priv->millisecond = atoi(p + 1);
   priv->relative = false;
}

void DateTime::setRelativeDate(const char *str)
{
#ifdef HAVE_STRTOLL
   int64 date = strtoll(str, 0, 10);
#else
   int64 date = atoll(str);
#endif
   // for date-only strings, move the date up to the right position
   if (strlen(str) == 8)
      date *= 1000000;
   setRelativeDateLiteral(date);
   // check for ms
   const char *p = strchr(str, '.');
   if (!p)
      return;
   priv->millisecond = atoi(p + 1);
}

bool DateTime::isEqual(const DateTime *dt) const
{
   if (priv->year != dt->priv->year)
      return false;
   if (priv->month != dt->priv->month)
      return false;
   if (priv->day != dt->priv->day)
      return false;
   if (priv->hour != dt->priv->hour)
      return false;
   if (priv->minute != dt->priv->minute)
      return false;
   if (priv->second != dt->priv->second)
      return false;
   if (priv->millisecond != dt->priv->millisecond)
      return false;
   return true;
}

DateTime *DateTime::add(const DateTime *dt) const {
   DateTime *rv;
   if (!priv->relative) {
      rv = new DateTime(*this);
      addAbsoluteToRelative(*rv, dt);
      return rv;
   }
   if (!dt->priv->relative) {
      rv = new DateTime(*this);
      dt->addAbsoluteToRelative(*rv, this);
      return rv;
   }
   rv = new DateTime();
   addRelativeToRelative(*rv, dt);
   return rv;
}

DateTime *DateTime::subtractBy(const DateTime *dt) const {
   DateTime *rv;
   if (!priv->relative) {
      if (dt->priv->relative) {
	 rv = new DateTime(*this);
	 subtractAbsoluteByRelative(*rv, dt);
	 return rv;					     
      }
      rv = new DateTime(true);
      calcDifference(*rv, dt);
      return rv;
   }
   if (!dt->priv->relative) {
      rv = new DateTime(*this);
      dt->subtractAbsoluteByRelative(*rv, this);
      return rv;
   }
   rv = new DateTime();
   subtractRelativeByRelative(*rv, dt);
   return rv;
}

// "result" must be a copy of "this"
void DateTime::addAbsoluteToRelative(DateTime &result, const DateTime *dt) const {
   // add years
   result.priv->year += dt->priv->year;

   // add months
   if (dt->priv->month >= 12) {
      result.priv->year += (dt->priv->month / 12);
      result.priv->month += (dt->priv->month % 12);
   }
   else
      result.priv->month += dt->priv->month;

   if (result.priv->month < 1) {
      result.priv->year--;
      result.priv->month += 12;
   }
   else if (result.priv->month > 12) {
      result.priv->year++;
      result.priv->month -= 12;
   }

   // fix days if necessary
   // check for leap years
   if (result.priv->month == 2 && result.priv->day > 28)
      result.priv->day = isLeapYear(result.priv->year) ? 29 : 28;
   // otherwise set day to last day of month if necessary
   else if (result.priv->day > month_lengths[result.priv->month])
      result.priv->day = month_lengths[result.priv->month];

#ifdef QORE_DST_AWARE
   // first add days
   if (dt->priv->day) {
      // set the time to 12 noon to avoid problems with dst
      result.priv->hour = 12;
      result.priv->setDate(result.priv->getEpochSeconds() + 86400 * dt->priv->day);
      result.priv->hour = hour;
   }
#endif
   
   int ms = priv->millisecond + dt->priv->millisecond;
   int sec = dt->priv->second;
   // calculate milliseconds and additional seconds to add
   if (ms < 0) {
      // ensure ms is greater than 0; add seconds to sec
      int sd = (ms / 1000) - 1;
      // increase seconds (subtract a negative number)
      sec -= sd;
      ms -= sd * 1000;
   }
   else if (ms >= 1000) {
      sec += (ms / 1000);
      ms %= 1000;
   }
   
   if (dt->priv->hour || dt->priv->minute || sec
#ifndef QORE_DST_AWARE
       || dt->priv->day
#endif
       )
      result.setDate(result.getEpochSeconds() 
#ifndef QORE_DST_AWARE
		  + (86400 * dt->priv->day)
#endif
		  + (3600 * dt->priv->hour) + (60 * dt->priv->minute) + sec);
   result.priv->millisecond = ms;
}

void DateTime::addRelativeToRelative(DateTime &result, const DateTime *dt) const {
   result.priv->relative = true;
   
   result.priv->year = priv->year + dt->priv->year;
   result.priv->month = priv->month + dt->priv->month;
   result.priv->day = priv->day + dt->priv->day;
   result.priv->hour = priv->hour + dt->priv->hour;
   result.priv->minute = priv->minute + dt->priv->minute;
   result.priv->second = priv->second + dt->priv->second;
   result.priv->millisecond = priv->millisecond + dt->priv->millisecond;
}

void DateTime::subtractAbsoluteByRelative(DateTime &result, const DateTime *dt) const {
   // subtract years
   result.priv->year -= dt->priv->year;

   // subtract months
   if (dt->priv->month >= 12) {
      result.priv->year -= (dt->priv->month / 12);
      result.priv->month -= (dt->priv->month % 12);
   }
   else
      result.priv->month -= dt->priv->month;

   if (result.priv->month < 1) {
      result.priv->year--;
      result.priv->month += 12;
   }
   else if (result.priv->month > 12) {
      result.priv->year++;
      result.priv->month -= 12;
   }

   // fix days if necessary
   // check for leap years
   if (result.priv->month == 2 && result.priv->day > 28)
      result.priv->day = isLeapYear(result.priv->year) ? 29 : 28;
   // otherwise set day to last day of month if necessary
   else if (result.priv->day > month_lengths[result.priv->month])
      result.priv->day = month_lengths[result.priv->month];

#ifdef QORE_DST_AWARE
   // subtract days aresult time
   if (dt->priv->day) {
      // set the time to 12 noon to avoid problems with dst
      result.priv->hour = 12;
      // there are 86400 seconds in an average day
      result.priv->setDate(result.priv->getEpochSeconds() - (86400 * dt->priv->day));
      result.priv->hour = hour;
   }
#endif
   
   // now subtract time
   int ms = priv->millisecond - dt->priv->millisecond;
   // calculate milliseconds a result additional seconds to subtract
   int sec = dt->priv->second;
   if (ms < 0) {
      // ensure ms is greater than 0; add seconds to sec
      int sd = (ms / 1000) - 1;
      // increase seconds (subtract a negative number)
      sec -= sd;
      ms -= sd * 1000;
   }
   else if (ms >= 1000) {
      sec += (ms / 1000);
      ms %= 1000;
   }

   if (dt->priv->hour || dt->priv->minute || sec
#ifndef QORE_DST_AWARE
       || dt->priv->day
#endif
       )
      result.setDate(result.getEpochSeconds() 
#ifndef QORE_DST_AWARE
		  - (86400 * dt->priv->day)
#endif
		  - (3600 * dt->priv->hour) - (60 * dt->priv->minute) - sec);

   result.priv->millisecond = ms;
}

void DateTime::subtractRelativeByRelative(DateTime &result, const DateTime *dt) const {
   result.priv->year = priv->year - dt->priv->year;
   result.priv->month = priv->month - dt->priv->month;
   result.priv->day = priv->day - dt->priv->day;
   result.priv->hour = priv->hour - dt->priv->hour;
   result.priv->minute = priv->minute - dt->priv->minute;
   result.priv->second = priv->second - dt->priv->second;
   result.priv->millisecond = priv->millisecond - dt->priv->millisecond;
}

// returns a relative date value in days, hours, minutes, seconds, and milliseconds
void DateTime::calcDifference(DateTime &result, const DateTime *dt) const {
   int64 sec = getEpochSeconds() - dt->getEpochSeconds();
   int ms = priv->millisecond - dt->priv->millisecond;
   //printd(5, "DT:cD() sec=%lld ms=%d\n", sec, ms);

   // normalize milliseconds   
   if (ms <= -1000 || ms >= 1000) {
      int ns = ms / 1000;
      sec += ns;
      ms -= ns * 1000;
   }
   // further normalize ms
   if (sec >= 0) {
      if (ms < 0) {
	 ms += 1000;
	 sec--;
      }
   }
   else if (ms > 0) {
      ms -= 1000;
      sec++;
   }
   
   result.priv->millisecond = ms;
   result.priv->relative = true;

   // first extract days
   if (sec <= -86400 || sec >= 86400) {
      int nv = sec / 86400;
      result.priv->day = nv;
      sec -= nv * 86400LL;
   }

   // now extract hours
   if (sec <= 3600 || sec >= 3600) {
      int nh = sec / 3600;
      result.priv->hour = nh;
      sec -= nh * 3600;
   }

   // extract minutes
   if (sec <= 60 || sec >= 60) {
      int nm = sec / 60;
      result.priv->minute = nm;
      sec -= nm * 60;
   }
   result.priv->second = sec;
}
