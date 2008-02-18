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

//! holds absolute and relative date/time values in Qore
/** conversion to and from integers is based on a 64-bit offset in seconds since January 1, 1970
    (the start of the UNIX epoch).
 */
class DateTime {
      friend class DateTimeNode;

   private:
      // static constants
      static const int month_lengths[];
      // for calculating the days passed in a year
      static const int positive_months[];
      static const int negative_months[];
      // month and day names (in English)
      static const struct date_s months[];
      static const struct date_s days[];

      //! private date data - most are ints so relative dates can hold a lot of data
      struct qore_dt_private *priv;
      
      //! adds an absolute date ("this" must be absolute) to relative date "dt"
      /** must be called with "result" being already a copy of "this"
	  @param result the resulting value, must be already initialized as a copy of "this"
	  @param dt the relative date to add
       */
      DLLLOCAL void addAbsoluteToRelative(DateTime &result, const class DateTime *dt) const;

      //! adds a relative date ("this" must be relative) to relative date "dt"
      /** "result" must be empty 
	  @param result the resulting value, must be an empty relative date
	  @param dt the relative date to add
       */
      DLLLOCAL void addRelativeToRelative(DateTime &result, const class DateTime *dt) const;

      //! subtracts relative date "dt" from an absolute date ("this" must be absolute)
      /** must be called with "result" being already a copy of "this"
	  @param result the resulting value, must be already initialized as a copy of "this"
	  @param dt the relative date to subtract
       */
      DLLLOCAL void subtractAbsoluteByRelative(DateTime &result, const class DateTime *dt) const;

      //! subtracts relative date "dt" from a relative date ("this" must be relative)
      /** "result" must be empty 
	  @param result the resulting value, must be an empty relative date
	  @param dt the relative date to subtract
       */
      DLLLOCAL void subtractRelativeByRelative(DateTime &result, const class DateTime *dt) const;

      //! "result" must be a blank relative date
      DLLLOCAL void calcDifference(DateTime &result, const class DateTime *dt) const;
      DLLLOCAL void setDateLiteral(int64 date);
      DLLLOCAL void setRelativeDateLiteral(int64 date);

      // static private methods
      DLLLOCAL static int positive_leap_years(int year, int month);
      DLLLOCAL static int negative_leap_years(int year);

      // returns 0 - 6, 0 = Sunday
      DLLLOCAL static int getDayOfWeek(int year, int month, int day);
      DLLLOCAL static int64 getEpochSeconds(int year, int month, int day);

      // result should be empty
      DLLLOCAL static int getDateFromISOWeekIntern(DateTime &result, int year, int week, int day, class ExceptionSink *xsink);

      // not implemented
      DLLLOCAL DateTime& operator=(const DateTime&);
      
   public:
      //! constructor for an empty object
      /**
	 @param r sets the "relative" flag for the object
       */
      DLLEXPORT DateTime(bool r = false);

      //! constructor for setting all parameters
      /**
	 @param y the year value
	 @param mo the months value
	 @param d the days value
	 @param h the hours value
	 @param mi the minutes value
	 @param s the seconds value
	 @param ms the milliseconds value
	 @param r the relative flag
       */
      DLLEXPORT DateTime(int y, int mo, int d, int h = 0, int mi = 0, int s = 0, short ms = 0, bool r = false);

      //! constructor for setting an absolute date based on the number of seconds from January 1, 1970
      /**
	 @param seconds the number of seconds from January 1, 1970
       */
      DLLEXPORT DateTime(int64 seconds);

      //! constructor for setting an absolute date based on the number of seconds from January 1, 1970 (plus milliseconds)
      /**
	 @param seconds the number of seconds from January 1, 1970
	 @param ms the milliseconds portion of the time	 
       */
      DLLEXPORT DateTime(int64 seconds, int ms);

      //! constructor for setting the date from a string, the string is converted to an integer and then setDate(int64) is called on it
      /**
	 @param date the string is converted to a 64-bit integer to be used for setting the date
       */
      DLLEXPORT DateTime(const char *date);

      //! constructor for setting an absolute date based on a "struct tm"
      /**
	 @param tms a structure giving the absolute date to set 
      */
      DLLEXPORT DateTime(const struct tm *tms);

      DLLEXPORT DateTime(const DateTime &dt);

      //! destroys the object and frees all memory
      DLLEXPORT ~DateTime();

      DLLEXPORT void getTM(struct tm *tms) const;

      DLLEXPORT void setDate(int64 seconds);
      DLLEXPORT void setDate(int64 seconds, int ms);
      DLLEXPORT void setDate(const char *str);
      DLLEXPORT void setRelativeDate(const char *str);
      DLLEXPORT void setDate(const struct tm *tms, short ms = 0);
      DLLEXPORT void setDate(const DateTime &date);
      DLLEXPORT void setTime(int h, int m, int s, short ms = 0);
      DLLEXPORT bool checkValidity() const;
      DLLEXPORT bool isEqual(const class DateTime *dt) const;
      DLLEXPORT class DateTime *add(const class DateTime *dt) const;
      DLLEXPORT class DateTime *subtractBy(const class DateTime *dt) const;

      //! gets the number of seconds since January 1, 1970 for the current date
      /**
	 @return the number of seconds since January 1, 1970
       */
      DLLEXPORT int64 getEpochSeconds() const;

      DLLEXPORT int getDayNumber() const;

      //! returns the day of week for the current date (0-6, Sun-Sat)
      /**
	 @return the day of week for the current date (0-6, Sun-Sat)
      */
      DLLEXPORT int getDayOfWeek() const;
      
      //! returns the ISO-8601 week information
      /** NOTE: the year may be different than the actual year
	  @param year the year portion of the ISO-9601 week information
	  @param week the ISO-9601 week number
	  @param day the day offset in the week (1-7 = Mon-Sun)
       */
      DLLEXPORT void getISOWeek(int &year, int &week, int &day) const;

      DLLEXPORT void format(class QoreString *str, const char *fmt) const;
      DLLEXPORT void getString(class QoreString *str) const;

      //! returns true if the value is a relative date-time value
      DLLEXPORT bool isRelative() const;

      //! returns true if the value is an absoluter date-time value
      DLLEXPORT bool isAbsolute() const;

      //! returns the year portion of the date-time value
      DLLEXPORT short getYear() const;

      //! returns the month portion of the date-time value
      DLLEXPORT int getMonth() const;

      //! returns the day portion of the date-time value
      DLLEXPORT int getDay() const;

      //! returns the hour portion of the date-time value
      DLLEXPORT int getHour() const;

      //! returns the minute portion of the date-time value
      DLLEXPORT int getMinute() const;

      //! returns the second portion of the date-time value
      DLLEXPORT int getSecond() const;

      //! returns the millisecond portion of the date-time value
      DLLEXPORT int getMillisecond() const;

      DLLEXPORT int64 getRelativeSeconds() const;
      DLLEXPORT int64 getRelativeMilliseconds() const;
      
      // static methods
      //! returns true if the year passed is a leap year according to a proleptic gregorian calendar
      DLLEXPORT static bool isLeapYear(int year);

      //! returns the number of days in the month given according to a proleptic gregorian calendar
      DLLEXPORT static int getLastDayOfMonth(int month, int year);

      //! returns a DateTime value from ISO-8601 week and day offsets
      /** note that ISO-8601 week days go from 1 - 7 = Mon - Sun
	  a 0 return value means an exception was raised
	  @param year the year portion of the date in which the ISO-8601 week is found in
	  @param week the ISO-8601 week number
	  @param day the day offset in the week (1-7 = Mon-Sun)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the DateTime value corresponding to the ISO-8601 week information (or 0 if an error occured)
      */
      DLLEXPORT static class DateTime *getDateFromISOWeek(int year, int week, int day, class ExceptionSink *xsink);

      //! returns -1, 0, or 1 if the left date is less than, equal, or greater than the right date
      DLLEXPORT static int compareDates(const class DateTime *left, const class DateTime *right);
};

#endif
