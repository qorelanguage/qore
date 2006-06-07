/*
  DateTime.h

  Qore programming language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

// FIXME: implement absolute date subtraction

#ifndef QORE_DATETIME_H

#define QORE_DATETIME_H

#include <qore/common.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

extern const int month_lengths[];

class DateTime {
   private:
      class DateTime *addAbsoluteToRelative(class DateTime *dt);
      inline class DateTime *addRelativeToRelative(class DateTime *dt);

      class DateTime *subtractAbsoluteByRelative(class DateTime *dt);
      inline class DateTime *subtractRelativeByRelative(class DateTime *dt);

   public:
      short year;
      int month;
      int day;
      int hour;
      int minute;
      int second;
      int millisecond;
      bool relative;

      inline DateTime() 
      { 
	 year = 0; 
	 month = 0; 
	 day = 0; 
	 hour = 0; 
	 minute = 0; 
	 second = 0; 
	 millisecond = 0; 
	 relative = false;
      }
      inline DateTime(int y, int mo, int d, int h, int mi, int s, int ms = 0);
      inline DateTime(int64 date);
      inline DateTime(char *date);
      inline DateTime(struct tm *tms);

      inline void getTM(struct tm *tms);
      void setDate(int64 date);
      inline void setDate(char *str);
      inline void setDate(struct tm *tms);
      inline bool isEqual(class DateTime *dt);
      inline class DateTime *add(class DateTime *dt);
      inline class DateTime *subtractBy(class DateTime *dt);
      
      class QoreString *format(char *fmt);
};

int compareDates(class DateTime *left, class DateTime *right);

#include <qore/QoreLib.h>

inline DateTime::DateTime(int y, int mo, int d, int h, int mi, int s, int ms)
{
   year = y;
   month = mo;
   day = d;
   hour = h;
   minute = mi;
   second = s;
   millisecond = ms;
   relative = false;
}

inline DateTime::DateTime(struct tm *tms)
{
   setDate(tms);
}

inline DateTime::DateTime(int64 date)
{
   setDate(date);
}

inline DateTime::DateTime(char *str)
{
   setDate(str);
}

inline void DateTime::getTM(struct tm *tms)
{
   tms->tm_year = year - 1900;
   tms->tm_mon = month - 1;
   tms->tm_mday = day;
   tms->tm_hour = hour;
   tms->tm_min = minute;
   tms->tm_sec = second;
   tms->tm_isdst = 0;
   tms->tm_wday = 0;
   tms->tm_yday = 0;
   tms->tm_isdst = -1;
}

// in 45 BC Julius Ceasar initiated the Julian calendar
// in 1582 AD Pope Gregory initiated the Gregorian calendar
static inline int getLastDayOfFebrary(int year)
{
   if (year <= -45)
      return 28;
   if (year < 1582)
      return (year % 4) ? 28 : 29;
   if (!(year % 400))
      return 29;
   if (!(year % 100))
      return 28;
   return (year % 4) ? 28 : 29;
}

static inline int getLastDayOfMonth(int month, int year)
{
   if (month != 2)
      return month_lengths[month];
   return getLastDayOfFebrary(year);
}

inline void DateTime::setDate(struct tm *tms)
{
   year = 1900 + tms->tm_year;
   month = tms->tm_mon + 1;
   day = tms->tm_mday;
   hour = tms->tm_hour;
   minute = tms->tm_min;
   second = tms->tm_sec;
   millisecond = 0;
   relative = false;
}

inline void DateTime::setDate(char *str)
{
#ifdef HAVE_STRTOLL
   int64 date = strtoll(str, NULL, 10);
#else
   int64 date = atoll(str);
#endif
   if (strlen(str) == 8)
      date *= 1000000LL;
   setDate(date);
}

inline bool DateTime::isEqual(class DateTime *dt)
{
   if (year != dt->year)
      return false;
   if (month != dt->month)
      return false;
   if (day != dt->day)
      return false;
   if (hour != dt->hour)
      return false;
   if (minute != dt->minute)
      return false;
   if (second != dt->second)
      return false;
   if (millisecond != dt->millisecond)
      return false;
   return true;
}

inline class DateTime *DateTime::addRelativeToRelative(class DateTime *dt)
{
   class DateTime *nd = new DateTime();
   nd->relative = true;

   nd->year = year + dt->year;
   nd->month = month + dt->month;
   nd->day = day + dt->day;
   nd->hour = hour + dt->hour;
   nd->minute = minute + dt->minute;
   nd->second = second + dt->second;
   nd->millisecond = millisecond + dt->millisecond;
   return nd;
}

inline class DateTime *DateTime::add(class DateTime *dt)
{
   if (!relative)
      return addAbsoluteToRelative(dt);
   if (!dt->relative)
      return dt->addAbsoluteToRelative(this);
   return addRelativeToRelative(dt);
}

inline class DateTime *DateTime::subtractRelativeByRelative(class DateTime *dt)
{
   class DateTime *nd = new DateTime();

   nd->year = year - dt->year;
   nd->month = month - dt->month;
   nd->day = day - dt->day;
   nd->hour = hour - dt->hour;
   nd->minute = minute - dt->minute;
   nd->second = second - dt->second;
   nd->millisecond = millisecond - dt->millisecond;
   return nd;
}

inline class DateTime *DateTime::subtractBy(class DateTime *dt)
{
   if (!relative)
      return subtractAbsoluteByRelative(dt);
   if (!dt->relative)
      return dt->subtractAbsoluteByRelative(this);
   return subtractRelativeByRelative(dt);
}

#endif
