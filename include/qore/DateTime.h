/*
  DateTime.h

  Qore programming language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#ifndef QORE_DATETIME_H

#define QORE_DATETIME_H

#include <qore/common.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct date_s {
   char *long_name;
   char *abbr;
};

class DateTime {
   private:
      // static constants
      static const int month_lengths[];
      // for calculating the days passed in a year
      static const int positive_months[];
      static const int negative_months[];
      // month and day names (in English)
      static const struct date_s months[];
      static const struct date_s days[];

      // private date data - most are ints so relative dates can hold a lot of data
      int year;
      int month;
      int day;
      int hour;
      int minute;
      int second;
      int millisecond;
      bool relative;
      
      DLLLOCAL class DateTime *addAbsoluteToRelative(const class DateTime *dt) const;
      DLLLOCAL class DateTime *addRelativeToRelative(const class DateTime *dt) const;

      DLLLOCAL class DateTime *subtractAbsoluteByRelative(const class DateTime *dt) const;
      DLLLOCAL class DateTime *subtractRelativeByRelative(const class DateTime *dt) const;
      DLLLOCAL class DateTime *calcDifference(const class DateTime *dt) const;
      DLLLOCAL void setDateLiteral(int64 date);
      DLLLOCAL void setRelativeDateLiteral(int64 date);

      // static private methods
      DLLLOCAL static int positive_leap_years(int year, int month);
      DLLLOCAL static int negative_leap_years(int year);

      // returns 0 - 6, 0 = Sunday
      DLLLOCAL static int getDayOfWeek(int year, int month, int day);
      DLLLOCAL static int64 getEpochSeconds(int year, int month, int day);
      
   public:

      DLLEXPORT DateTime(bool r = false);
      DLLEXPORT DateTime(int y, int mo, int d, int h = 0, int mi = 0, int s = 0, short ms = 0, bool r = false);
      DLLEXPORT DateTime(int64 seconds);
      DLLEXPORT DateTime(int64 seconds, int ms);
      DLLEXPORT DateTime(char *date);
      DLLEXPORT DateTime(struct tm *tms);

      DLLEXPORT void getTM(struct tm *tms) const;
      DLLEXPORT void setDate(int64 seconds);
      DLLEXPORT void setDate(int64 seconds, int ms);
      DLLEXPORT void setDate(char *str);
      DLLEXPORT void setRelativeDate(char *str);
      DLLEXPORT void setDate(struct tm *tms, short ms = 0);
      DLLEXPORT void setTime(int h, int m, int s, short ms = 0);
      DLLEXPORT bool checkValidity() const;
      DLLEXPORT bool isEqual(const class DateTime *dt) const;
      DLLEXPORT class DateTime *add(const class DateTime *dt) const;
      DLLEXPORT class DateTime *subtractBy(const class DateTime *dt) const;
      DLLEXPORT int64 getEpochSeconds() const;
      DLLEXPORT int getDayNumber() const;
      DLLEXPORT int getDayOfWeek() const;
      
      // returns the ISO-8601 week number (year may be different)
      DLLEXPORT void getISOWeek(int &year, int &week, int &day) const;

      DLLEXPORT void format(class QoreString *str, char *fmt) const;
      DLLEXPORT void getString(class QoreString *str) const;
      
      DLLEXPORT bool isRelative() const;
      DLLEXPORT bool isAbsolute() const;
      DLLEXPORT short getYear() const;
      DLLEXPORT int getMonth() const;
      DLLEXPORT int getDay() const;
      DLLEXPORT int getHour() const;
      DLLEXPORT int getMinute() const;
      DLLEXPORT int getSecond() const;
      DLLEXPORT int getMillisecond() const;
      DLLEXPORT int64 getRelativeSeconds() const;
      DLLEXPORT int64 getRelativeMilliseconds() const;
      
      // static methods
      DLLEXPORT static bool isLeapYear(int year);
      DLLEXPORT static int getLastDayOfMonth(int month, int year);
      // note that ISO-8601 week days go from 1 - 7 = Mon - Sun
      // a NULL return value means an exception was raised
      DLLEXPORT static class DateTime *getDateFromISOWeek(int year, int week, int day, class ExceptionSink *xsink);
      DLLEXPORT static int compareDates(class DateTime *left, class DateTime *right);
};

#endif
