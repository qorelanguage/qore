/*
  DateTime.cc

  Qore Programming Language

  Copyright (C) 2005 David Nichols

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

#include <qore/DateTime.h>
#include <qore/QoreString.h>

#include <time.h>

/*
 * date format codes: (NI = not implemented yet)
 * YY: last two-digits of year
 * YYYY: four-digit year
 * M: month number (1-12)
 * MM: month number (1-12), zero padded
 * Month: long month string (ex: January)
 * MONTH: long month string capitolised (ex: JANUARY)
 * Mon: abbreviated month (ex: Jan)
 * MON: abbreviated month capitolised (ex: JAN)
 * D: day number (1 - 31)
 * DD: day number (1 - 31), zero padded
 * Day: long day of week string (ex: Monday)
 * DAY: long day of week string capitolised (ex: MONDAY)
 * Dy: abbreviated day of week string (ex: Mon)
 * DY: abbreviated day of week string capitolised (ex: MON)
 * H: hour number (0 - 23)
 * HH: hour number (00 - 23), zero padded
 * h: hour number (1 - 12)
 * hh: hour number (01 - 12), zero padded
 * m: minute number (0 - 59)
 * mm: minute number (00 - 59), zero padded
 * S: second number (0 - 59)
 * SS: second number (00 - 59), zero padded
 * ms: milliseconds (000 - 999), zero padded
 * P: AM or PM
 * p: am or pm
 */

const int DateTime::month_lengths[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const int DateTime::positive_months[] = { 0,  31,  59,  90,  120,  151,  181,  212,  243,  273,  304,  334,  365 };
const int DateTime::negative_months[] = { 0, -31, -61, -92, -122, -153, -184, -214, -245, -275, -306, -334, -365 };

const struct date_s DateTime::months[] = {
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

const struct date_s DateTime::days[] = {
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

class QoreString *DateTime::format(char *fmt)
{
   struct tm nt;

   tracein("DateTime::format()");

   class QoreString *str = new QoreString();
   char *s = fmt;
   while (*s)
   {
      switch (*s)
      {
         case 'Y':
            if (s[1] != 'Y')
            {
               str->concat('Y');
               break;
            }
            s++;
            if ((s[1] == 'Y') && (s[2] == 'Y'))
            {
               str->sprintf("%04d", year);
               s += 2;
            }
            else
               str->sprintf("%02d", year - (year / 100) * 100);
            break;
         case 'M':
            if (s[1] == 'M')
            {
               str->sprintf("%02d", month);
               s++;
               break;
            }
            if ((s[1] == 'o') && (s[2] == 'n'))
            {
               s += 2;
               if ((s[1] == 't') && (s[2] == 'h'))
               {
                  s += 2;
                  if (month && (month <= 12))
                     str->sprintf("%s", months[(int)month - 1].long_name);
                  else
                     str->sprintf("Month%d", month - 1);
                  break;
               }
               if (month && (month <= 12))
                  str->sprintf("%s", months[(int)month - 1].abbr);
               else
                  str->sprintf("M%02d", month);
               break;
            }
            if ((s[1] == 'O') && (s[2] == 'N'))
            {
               s += 2;
               if ((s[1] == 'T') && (s[2] == 'H'))
               {
                  s += 2;
                  if (month && (month <= 12))
                  {
                     char *t = str->getBuffer() + str->strlen();
                     str->sprintf("%s", months[(int)month - 1].long_name);
                     strtoupper(t);
                  }
                  else
                     str->sprintf("MONTH%d", month);
                  break;
               }
               if (month && (month <= 12))
               {
                  char *t = str->getBuffer() + str->strlen();
                  str->sprintf("%s", months[(int)month - 1].abbr);
                  strtoupper(t);
               }
               else
                  str->sprintf("M%02d", month);
               break;
            }
            str->sprintf("%d", month);
            break;
         case 'D':
            if (s[1] == 'D')
            {
               str->sprintf("%02d", day);
               s++;
               break;
            }
            if ((s[1] == 'a') && (s[2] == 'y'))
            {
               s += 2;
               getTM(&nt);
               if (mktime(&nt) == -1) // invalid time
                  str->sprintf("Day%d", day);
               else
                  str->sprintf("%s", days[nt.tm_wday].long_name);
               break;
            }
            if ((s[1] == 'A') && (s[2] == 'Y'))
            {
               s += 2;
               getTM(&nt);
               if (mktime(&nt) == -1) // invalid time
                  str->sprintf("DAY%d", day);
               else
               {
                  char *t = str->getBuffer() + str->strlen();
                  str->sprintf("%s", days[nt.tm_wday].long_name);
                  strtoupper(t);
               }
               break;
            }
            if ((s[1] == 'y') || (s[1] == 'Y'))
            {
               s++;;
               getTM(&nt);
               if (mktime(&nt) == -1) // invalid time
                  str->sprintf("D%02d", day);
               else
               {
                  char *t = str->getBuffer() + str->strlen();
                  str->sprintf("%s", days[nt.tm_wday].abbr);
                  if (*s == 'Y')
                     strtoupper(t);
               }
               break;
            }
            str->sprintf("%d", day);
            break;
         case 'H':
            if (s[1] == 'H')
            {
               str->sprintf("%02d", hour);
               s++;
            }
            else
               str->sprintf("%d", hour);
            break;
         case 'h':
            if (s[1] == 'h')
            {
               str->sprintf("%02d", ampm(hour));
               s++;
            }
            else
               str->sprintf("%d", ampm(hour));
            break;
         case 'P':
            if (hour > 11)
               str->sprintf("PM");
            else
               str->sprintf("AM");
            break;
         case 'p':
            if (hour > 11)
               str->sprintf("pm");
            else
               str->sprintf("am");
            break;
         case 'm':
            if (s[1] == 'm')
            {
               str->sprintf("%02d", minute);
               s++;
            }
            else if (s[1] == 's')
            {
               str->sprintf("%03d", millisecond);
               s++;
            }
            else
               str->sprintf("%d", minute);
            break;
         case 'S':
            if (s[1] == 'S')
            {
               str->sprintf("%02d", second);
               s++;
            }
            else
               str->sprintf("%d", second);
            break;
         case 'u':
            if (s[1] == 'u')
            {
               str->sprintf("%02d", millisecond);
               s++;
            }
            else
               str->sprintf("%d", millisecond);
            break;
         default:
	    str->concat(*s);
            break;
      }
      s++;
   }

   printd(5, "DateTime::format() returning \"%s\"\n", str->getBuffer());
   traceout("DateTime::format()");
   return str;
}

// set the date from the number of seconds since January 1, 1970 (UNIX epoch)
void DateTime::setDate(int64 seconds)
{
   millisecond = year = 0;
   // there are 97 leap days every 400 years (12622780800 seconds)
   int64 ty = seconds/12622780800ll;
   if (ty)
   {
      year += ty * 400;
      seconds -= ty * 12622780800ll;
   }
   // there are 24 leap days every 100 years (3155673600 seconds)
   ty = seconds/3155673600ll;
   if (ty)
   {
      year += ty * 100;
      seconds -= ty * 3155673600ll;
   }
   // then there are leap days every 4 years (126230400 seconds)
   ty = seconds/126230400;
   if (ty)
   {
      year += ty * 4;
      seconds -= ty * 126230400;
   }
   //printd(0, "seconds: %lld year: %d\n", seconds, year);
   if (seconds >= 0)
   {
      // there is a leap day on 1972.02.29 (ends 68256000 seconds)
      // however we adjust if the year is greater than or equal to 1973 (starts 94694400 seconds)
      if (seconds >= 94694400)
	 seconds -= 86400;

      ty = seconds / 31536000;
      if (ty)
      {
	 year += ty;
	 seconds -= ty * 31536000;
      }
      year += 1970;

      day = seconds / 86400;
      seconds -= day * 86400;
      //printd(5, "seconds=%lld year=%d day=%d\n", seconds, year, day);
      bool ly = isLeapYear(year);
      for (int i = 1; ;i++)
      {
	 //printd(5, "day=%d, positive_months[%d]=%d, adjusted=%d\n", day, i, positive_months[i], (positive_months[i] + (ly && i > 1 ? 1 : 0)));
         if ((positive_months[i] + (ly && i > 1 ? 1 : 0)) > day)
	 {
	    //printd(5, "pm[%d]=%d pm[%d]=%d month=%d day=%d (%d)\n", i, positive_months[i], i - 1, positive_months[i - 1], i, day, day - positive_months[i - 1] + 1);
	    month = i;
	    day = day - positive_months[i - 1] + (ly && i > 2 ? 0 : 1);
	    break;
	 }
      }

      hour = seconds / 3600;
      seconds -= hour * 3600;
      minute = seconds / 60;
      second = seconds - minute * 60;
      return;
   }

   // there is a leap day on 1968.02.29 (ends -58060800 seconds)
   // however we adjust if the year is equal or less than 1968, starting -63158400 seconds
   if (seconds <= -63158400)
      //if (seconds <= -58060800)
      seconds += 86400;

   ty = seconds / 31536000;
   if (ty)
   {
      year += ty;
      seconds -= ty * 31536000;
   }
   year += 1969;
   if (!seconds)
   {
      year++;
      month = 1;
      day = 1;
      hour = 00;
      minute = 00;
      second = 00;
      return;
   }

   //printd(5, "1: seconds=%lld\n", seconds);
   day = seconds / 86400;
   seconds -= day * 86400;
   // move back a day if there is further time to subtract
   if (seconds)
      day--;
   //printd(5, "1.1: seconds=%lld day=%d\n", seconds, day);
   bool ly = isLeapYear(year);
   for (int i = 1; ; i++)
   {
#if 0
      printd(0, "nm[%d]=%d nm[%d]=%d adj=%d len=%d adj=%d day=%d (mon=%d day=%d)\n", i, negative_months[i], i - 1, 
	     negative_months[i - 1], (negative_months[i - 1] - (ly && i == 12 ? 1 : 0)),
	     month_lengths[13 - i], month_lengths[13 - i] + (ly && (13 - i) == 2 ? 1 : 0),
	     day, 13 - i,
	     month_lengths[13 - i] + (ly && (13 - i) == 2) + day - negative_months[i - 1] + (ly && i == 12 ? 0 : 1));
#endif
      //month_lengths[13 - i] + day - negative_months[i - 1] + 1);
      if ((negative_months[i] - (ly && i > 10 ? 1 : 0)) <= day)
      {
	 month = 13 - i;
	 day = month_lengths[month] + (ly && month == 2 ? 1 : 0) + day - negative_months[i - 1] + (ly && i == 12 ? 0 : 1);
	 break;
      }
   }
   //printd(5, "2: seconds=%lld\n", seconds);
   hour = seconds / 3600;
   seconds -= hour * 3600;
   minute = seconds / 60;
   seconds -= minute * 60;
   if ((second = seconds))
   {
      second += 60;
      minute--;
   }
   if (minute) 
   {
      minute += 60;
      hour--;
   }
   if (hour) hour += 24;
}

inline int DateTime::negative_leap_years(int year)
{
   year = 1970 - year - 1;
   
   if (year <= 0)
      return 0;
   
   year += 2;
   
   return -year/4 + year/100 - year/400;
}


inline int DateTime::positive_leap_years(int year, int month)
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
inline int64 DateTime::getEpochSeconds(int year, int month, int day)
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
int64 DateTime::getEpochSeconds()
{
   //printd(0, "%04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
   int tm = month;
   if (month < 0) tm = 1;
   else if (month > 12) tm = 12;

   if (year >= 1970)
      return ((int64)year - 1970) * 31536000ll
	 + (positive_months[tm - 1] + day - 1 + positive_leap_years(year, month)) * 86400
	 + hour * 3600
	 + minute * 60
	 + second;

   bool ly = isLeapYear(year);
   //printd(5, "DBG: %d %lld\n", year, ((int64)year - 1969) * 31536000ll);

#if 0
   printd(5, "tm=%d neg_mon[%d]=%d adj=%d day=%d mon_len[%d]=%d adj=%d nly=%d (%d) day=%d\n", tm, 12 - tm, negative_months[12 - tm], 
	  negative_months[12 - tm] - (ly && tm < 3 ? 1 : 0), day, tm, month_lengths[tm],
	  month_lengths[tm] + (ly && tm == 2 ? 1 : 0), negative_leap_years(year), negative_leap_years(year) * 86400,
	  (negative_months[12 - tm] - (ly && tm < 2 ? 1 : 0) + (day - (month_lengths[tm] + (ly && tm == 2 ? 1 : 0))) 
	   + negative_leap_years(year)) - 1
	  );
#endif
   return ((int64)year - 1969) * 31536000ll
      + (negative_months[12 - tm] - (ly && tm < 2 ? 1 : 0) + (day - (month_lengths[tm] + (ly && tm == 2 ? 1 : 0))) 
      + negative_leap_years(year)) * 86400
      + (hour - 23) * 3600
      + (minute - 59) * 60
      + (second - 60);	 
}

void DateTime::setDateLiteral(int64 date)
{
   year = date / 10000000000ll;
   date -= year * 10000000000ll;
   month = date / 100000000ll;
   date -= month * 100000000ll;
   day = date / 1000000ll;
   date -= day * 1000000ll;
   hour = date / 10000ll; 
   date -= hour * 10000ll;
   minute = date / 100ll;
   second = date - minute * 100ll;
   millisecond = 0;

   if (second > 59)
   {
      minute += (second / 60);
      second %= 60;
   }
   if (minute > 59)
   {
      hour += (minute / 60);
      minute %= 60;
   }
   if (hour > 23)
   {
      day += (hour / 24);
      hour %= 24;
   }
   // adjust month and year
   if (month > 12)
   {
      year += ((month - 1)/ 12);
      month = ((month - 1) % 12) + 1;
   }
   // now check day
   if (day)
   {
      int i;
      while (day > (i = getLastDayOfMonth(month, year)))
      {
	 day -= i;
	 month++;
	 if (month == 13)
	 {
	    month = 1;
	    year++;
	 }
      }
   }
   relative = false;
}

void DateTime::setRelativeDateLiteral(int64 date)
{
   year = date / 10000000000ll;
   date -= year * 10000000000ll;
   month = date / 100000000ll;
   date -= month * 100000000ll;
   day = date / 1000000ll;
   date -= day * 1000000ll;
   hour = date / 10000ll; 
   date -= hour * 10000ll;
   minute = date / 100ll;
   second = date - minute * 100ll;
   millisecond = 0;   
   relative = true;
}

class DateTime *DateTime::addAbsoluteToRelative(class DateTime *dt)
{
   class DateTime *nd = new DateTime();
   nd->relative = false;

   // add years
   nd->year = year + dt->year;

   // add months
   if (dt->month >= 12)
   {
      nd->year += (dt->month / 12);
      nd->month = month + (dt->month % 12);
   }
   else
      nd->month = month + dt->month;
   if (nd->month > 12)
   {
      nd->year++;
      nd->month -= 12;
   }
   // fix days if necessary
   nd->day = day;
   // check for leap years
   if (nd->month == 2 && nd->day > 28)
      nd->day = isLeapYear(nd->year) ? 29 : 28;
   // otherwise set day to last day of month if necessary
   else if (nd->day > month_lengths[nd->month])
      nd->day = month_lengths[nd->month];

#ifdef QORE_DST_AWARE
   // first add days
   if (dt->day)
   {
      // set the time to 12 noon to avoid problems with dst
      nd->hour = 12;
      nd->setDate(nd->getEpochSeconds() + 86400 * dt->day);
   }
#endif
   
   // now add time, first write over the time possibly set above
   nd->hour = hour;
   nd->minute = minute;
   nd->second = second;

   int ms = millisecond + dt->millisecond;
   // calculate milliseconds and additional seconds to add
   if (ms >= 1000)
   {
      nd->second += (ms / 1000);
      ms %= 1000;
   }
   
   if (dt->hour || dt->minute || dt->second
#ifndef QORE_DST_AWARE
       || dt->day
#endif
       )
      nd->setDate(nd->getEpochSeconds() 
#ifndef QORE_DST_AWARE
		  + (86400 * dt->day)
#endif
		  + (3600 * dt->hour) + (60 * dt->minute) + dt->second);
   nd->millisecond = ms;
   
   return nd;   
}

class DateTime *DateTime::subtractAbsoluteByRelative(class DateTime *dt)
{
   class DateTime *nd = new DateTime();
   
   // subtract years
   nd->year = year - dt->year;

   // subtract months
   if (dt->month >= 12)
   {
      nd->year -= (dt->month / 12);
      nd->month = month - (dt->month % 12);
   }
   else
      nd->month = month - dt->month;
   if (nd->month < 1)
   {
      nd->year--;
      nd->month += 12;
   }
   // fix days if necessary
   nd->day = day;
   // check for leap years
   if (nd->month == 2 && nd->day > 28)
      nd->day = isLeapYear(nd->year) ? 29 : 28;
   // otherwise set day to last day of month if necessary
   else if (nd->day > month_lengths[nd->month])
      nd->day = month_lengths[nd->month];

#ifdef QORE_DST_AWARE
   // subtract days and time
   if (dt->day)
   {
      // set the time to 12 noon to avoid problems with dst
      nd->hour = 12;
      // there are 86400 seconds in an average day
      nd->setDate(nd->getEpochSeconds() - (86400 * dt->day));
   }
#endif
   
   // now subtract time, first write over the time possibly set above
   nd->hour = hour;
   nd->minute = minute;
   nd->second = second;
   int ms;
   // calculate milliseconds and additional seconds to subtract
   if (dt->millisecond > 1000)
   {
      nd->second += (dt->millisecond / 1000);
      ms = dt->millisecond % 1000;
   }
   else
      ms = dt->millisecond;
   ms = millisecond - ms;
   int sec = dt->second;
   if (ms < 0)
   {
      ms += 1000;
      sec++;
   }
   if (dt->hour || dt->minute || sec
#ifndef QORE_DST_AWARE
       || dt->day
#endif
       )
      nd->setDate(nd->getEpochSeconds() 
#ifndef QORE_DST_AWARE
		  - (86400 * dt->day)
#endif
		  - (3600 * dt->hour) - (60 * dt->minute) - sec);

   nd->millisecond = ms;
   
   return nd;   
}

// return the ISO-8601 calendar week information - note that the ISO-8601 calendar year may be different than the actual year
void DateTime::getISOWeek(int &yr, int &week, int &day)
{
   // get day of week of jan 1 of this year
   int jan1 = getDayOfWeek(year, 1, 1);

   // calculate day in calendar week
   int dn = getDayNumber();
   int dow = (dn + jan1 - 1) % 7;
   day = !dow ? 7 : dow;
   
   //printd(5, "getISOWeek() year=%d, start=%d, daw=%d dn=%d offset=%d\n", year, jan1, dow, dn, (jan1 > 4 ? 9 - jan1 : 2 - jan1));
   if ((!jan1 && dn == 1) || (jan1 == 5 && dn < 4) || (jan1 == 6 && dn < 3))
   {
      yr = year - 1;
      jan1 = getDayOfWeek(yr, 1, 1);
      //printd(5, "getISOWeek() previous year=%d, start=%d, leap=%d\n", yr, jan1, isLeapYear(yr));
      if ((jan1 == 4 && !isLeapYear(yr)) || (jan1 == 3 && isLeapYear(yr)))
	 week = 53;
      else
	 week = 52;
      return;
   }
   yr = year;
   
   int offset = jan1 > 4 ? jan1 - 9 : jan1 - 2;
   week = ((dn + offset) / 7) + 1;
   if (week == 53)
      if ((jan1 == 4 && !isLeapYear(yr)) || (jan1 == 3 && isLeapYear(yr)))
	 return;
      else
      {
	 yr++;
	 week = 1;
      }
}

// note that ISO-8601 week days go from 1 - 7 = Mon - Sun
// a NULL return value means an exception was raised
// static method
class DateTime *DateTime::getDateFromISOWeek(int year, int week, int day, class ExceptionSink *xsink)
{
   if (week <= 0)
   {
      xsink->raiseException("ISO-8601-INVALID-WEEK", "week numbers must be positive (value passed: %d)", week);
      return NULL;
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
	 return NULL;
      }
   }
   
   if (day < 1 || day > 7)
   {
      xsink->raiseException("ISO-8601-INVALID-DAY", "calendar week days must be between 1 and 7 for Mon - Sun (day value passed: %f)", day);
      return NULL;
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
   return new DateTime(getEpochSeconds(y, m, d) + ((week - 1) * 7 + (day - 1)) * 86400);
}

int compareDates(class DateTime *left, class DateTime *right)
{
   if (left->year > right->year)
      return 1;
   if (left->year < right->year)
      return -1;
   if (left->month > right->month)
      return 1;
   if (left->month < right->month)
      return -1;
   if (left->day > right->day)
      return 1;
   if (left->day < right->day)
      return -1;
   if (left->hour > right->hour)
      return 1;
   if (left->hour < right->hour)
      return -1;
   if (left->minute > right->minute)
      return 1;
   if (left->minute < right->minute)
      return -1;
   if (left->second > right->second)
      return 1;
   if (left->second < right->second)
      return -1;
   if (left->millisecond > right->millisecond)
      return 1;
   if (left->millisecond < right->millisecond)
      return -1;
   return 0;
}

