/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  DateTime.h

  Qore programming language

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

#ifndef QORE_DATETIME_H

#define QORE_DATETIME_H

#include <time.h>

class AbstractQoreZoneInfo;

//! for returning broken-down time information
struct qore_tm {
   int year, month, day, hour, minute, second, us, utc_secs_east;
   bool dst;
   const char *zone_name;
   const AbstractQoreZoneInfo *zone;

   DLLLOCAL bool isTimeNull() const {
      return !hour && !minute && !second && !us;
   }

   //! returns seconds east of UTC for zone
   DLLEXPORT int secsEast() const;

   //! returns the region name of the zone
   DLLEXPORT const char *regionName() const;
};

//! Holds absolute and relative date/time values in Qore with precision to the microsecond
/** Date arithmetic and date formatting is supported by this class.
    As of qore 0.8.0, the internal representation for absolute date/time values has changed.
    Now, absolute date/time values are stored internally as a 64-bit offset in seconds since
    the UNIX epoch (January 1, 1970 UTC), plus a 4-byte integer microseconds offset, plus a
    pointer to an AbstractQoreZoneInfo object, which gives the time zone information (UTC
    offset, daylights savings time transitions, if any, etc).

    Therefore, for absolute date/time values, it is expensive to functions that get discrete
    values for the date (such as getYear(), getMonth(), etc), because to return the value, the
    entire date must be calculated for each call.  Instead DateTime::getInfo() should be used.

    Relative date/time values are stored with discrete values for years, months, days, hours,
    minutes, seconds, and microseconds.

    This is a "normal" (i.e. not reference counted) class, for the equivalent Qore value
    type, see DateTimeNode
    @see DateTimeNode
 */
class DateTime {
   friend class DateTimeNode;

protected:
   //! private date data - most are ints so relative dates can hold a lot of data
   struct qore_date_private *priv;

   DLLLOCAL void setDateLiteral(int64 date);
   DLLLOCAL void setRelativeDateLiteral(int64 date);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL DateTime& operator=(const DateTime&);
   
   //! this constructor is not exported in the library
   DLLLOCAL DateTime(qore_date_private *n_priv);
      
public:
   //! constructor for an empty object
   /**
      @param r sets the "relative" flag for the object
   */
   DLLEXPORT DateTime(bool r = false);

   //! constructor for setting all parameters
   /** note that for absolute date/time values, the local time zone will be assumed
       @param n_year the year value
       @param n_month the months value
       @param n_day the days value
       @param n_hour the hours value
       @param n_minute the minutes value
       @param n_second the seconds value
       @param n_ms the milliseconds value
       @param n_relative the relative flag
   */
   DLLEXPORT DateTime(int n_year, int n_month, int n_day, int n_hour = 0, int n_minute = 0, int n_second = 0, short n_ms = 0, bool n_relative = false);

   //! constructor for setting an absolute date based on the number of seconds from January 1, 1970
   /** note that the local time zone will be assumed
       @param seconds the number of seconds from January 1, 1970
   */
   DLLEXPORT DateTime(int64 seconds);

   //! constructor for setting an absolute date based on the number of seconds from January 1, 1970 (plus milliseconds)
   /** note that the local time zone will be assumed
       @param seconds the number of seconds from January 1, 1970
       @param ms the milliseconds portion of the time	 
   */
   DLLEXPORT DateTime(int64 seconds, int ms);

   //! constructor for setting the date from a string in the format YYYYMMDDHHmmSS
   /** additionally a milliseconds value can be appended with a period and 3 integers in the format [.xxx]
       @param date the string to use to set the date in the format YYYYMMDDHHmmSS[.xxx]
   */
   DLLEXPORT DateTime(const char *date);

   //! constructor for setting an absolute date based on a "struct tm"
   /**
      @param tms a structure giving the absolute date to set 
   */
   DLLEXPORT DateTime(const struct tm *tms);

   //! copy constructor
   DLLEXPORT DateTime(const DateTime &dt);

   //! destroys the object and frees all memory
   DLLEXPORT ~DateTime();

   //! sets a "struct tm" from the current date/time value for the time zone for the object; use DateTime::getInfo() instead
   DLLEXPORT void getTM(struct tm *tms) const;

   //! sets the absolute date value based on the number of seconds from January 1, 1970
   /** note that the local time zone will be assumed
       @param seconds the number of seconds from January 1, 1970
   */
   DLLEXPORT void setDate(int64 seconds);

   //! sets the absolute date value based on the number of seconds from January 1, 1970 UTC (plus milliseconds)
   /** note that the local time zone will be assumed
       @param seconds the number of seconds from January 1, 1970 UTC
       @param ms the milliseconds portion of the time	 
   */
   DLLEXPORT void setDate(int64 seconds, int ms);

   //! sets the absolute date value based on the number of seconds from January 1, 1970 UTC (plus microseconds)
   /** @param zone the time zone for the time
       @param seconds the number of seconds from January 1, 1970 UTC
       @param us the microseconds portion of the time	 
   */
   DLLEXPORT void setDate(const AbstractQoreZoneInfo *zone, int64 seconds, int us);

   //! sets the date to an absolute date/time as given
   DLLEXPORT void setDate(const AbstractQoreZoneInfo *n_zone, int n_year, int n_month, int n_day, int n_hour = 0, int n_minute = 0, int n_second = 0, int n_us = 0);

   //! sets an absolute date value from a string in the format YYYYMMDDHHmmSS
   /** additionally a milliseconds value can be appended with a period and 3 integers in the format [.xxx]
       note that the local time zone will be assumed
       @param str the string to use to set the date in the format YYYYMMDDHHmmSS[.xxx]
   */
   DLLEXPORT void setDate(const char *str);

   //! sets a relative date from a string in the format YYYYMMDDHHmmSS
   DLLEXPORT void setRelativeDate(const char *str);

   //! sets the absolute date from a "struct tm" pointer and millisecond value
   DLLEXPORT void setDate(const struct tm *tms, short ms = 0);

   //! sets the date from a DateTime reference
   DLLEXPORT void setDate(const DateTime &date);

   //! sets the time from hours, minutes, seconds, and milliseconds
   /**
      @param h the hours value
      @param m the minutes value
      @param s the seconds value
      @param ms the milliseconds value
   */
   DLLEXPORT void setTime(int h, int m, int s, short ms = 0);

   DLLEXPORT bool checkValidity() const;
   DLLEXPORT bool isEqual(const DateTime *dt) const;
   DLLEXPORT DateTime *add(const DateTime *dt) const;
   DLLEXPORT DateTime *subtractBy(const DateTime *dt) const;

   //! gets the number of seconds since January 1, 1970 for the current date offset in local time
   /**
      @return the number of seconds since January 1, 1970 offset in local time
   */
   DLLEXPORT int64 getEpochSeconds() const;

   //! gets the number of seconds since January 1, 1970Z for the current date
   /**
      @return the number of seconds since January 1, 1970Z
   */
   DLLEXPORT int64 getEpochSecondsUTC() const;

   //! gets the number of microseconds since January 1, 1970Z for the current date
   /**
      @return the number of microseconds since January 1, 1970Z
   */
   DLLEXPORT int64 getEpochMicrosecondsUTC() const;

   //! gets the number of milliseconds since January 1, 1970Z for the current date
   /**
      @return the number of milliseconds since January 1, 1970Z
   */
   DLLEXPORT int64 getEpochMillisecondsUTC() const;

   //! returns the ordinal number of the day in the year for absolute dates, sometimes (mistakenly) referred to as the Julian date
   /** does not return sensible values for relative dates
       @return the number of the day in the year
   */
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

   //! formats the date/time value to a QoreString
   /** the formatted date/time value will be appended to the QoreString argument according to the format string
       Format codes are as follows:
       - YY: last two-digits of year
       - YYYY: four-digit year
       - M: month number (1-12)
       - MM: month number (1-12), zero padded
       - Month: long month string (ex: January)
       - MONTH: long month string capitalised (ex: JANUARY)
       - Mon: abbreviated month (ex: Jan)
       - MON: abbreviated month capitalised (ex: JAN)
       - D: day number (1 - 31)
       - DD: day number (1 - 31), zero padded
       - Day: long day of week string (ex: Monday)
       - DAY: long day of week string capitalised (ex: MONDAY)
       - Dy: abbreviated day of week string (ex: Mon)
       - DY: abbreviated day of week string capitalised (ex: MON)
       - H: hour number (0 - 23)
       - HH: hour number (00 - 23), zero padded
       - h: hour number (1 - 12)
       - hh: hour number (01 - 12), zero padded
       - m: minute number (0 - 59)
       - mm: minute number (00 - 59), zero padded
       - S: second number (0 - 59)
       - SS: second number (00 - 59), zero padded
       - u: milliseconds (0 - 999)
       - uu or ms: milliseconds (000 - 999), zero padded
       - x: microseconds (0 - 999999)
       - xx: microseconds (000000 - 999999), zero padded
       - z: local time zone name (i.e. 'EST') if available, otherwise the UTC offset (see 'Z')
       - Z: UTC offset like +HH:mm[:SS], seconds are included if non-zero
       - P: AM or PM
       - p: am or pm
       .
       @note currently locale settings are ignored
       @param str the QoreString where the formatted date data will be written (appended)
       @param fmt the format string as per the above description
   */
   DLLEXPORT void format(QoreString &str, const char *fmt) const;

   //! returns true if the value is a relative date-time value
   /**
      @return true if the value is a relative date-time value
   */
   DLLEXPORT bool isRelative() const;

   //! returns true if the value is an absolute date-time value
   /**
      @return true if the value is an absolute date-time value
   */
   DLLEXPORT bool isAbsolute() const;

   //! returns the year portion of the date-time value
   /** @note if more than one of DateTime::getYear(), DateTime::getMonth(), etc is called, then DateTime::getInfo() should be used instead to avoid the broken-down components of the date being calculated for each call that retrieves a discrete value from the date.  This restriction does not apply to relative date/time values.
       @return the year portion of the date-time value
   */
   DLLEXPORT short getYear() const;

   //! returns the month portion of the date-time value
   /** @note if more than one of DateTime::getYear(), DateTime::getMonth(), etc is called for an absolute date/time value, then DateTime::getInfo() should be used instead to avoid the broken-down components of the date being calculated for each call that retrieves a discrete value from the date.  This restriction does not apply to relative date/time values.
       @return the month portion of the date-time value
   */
   DLLEXPORT int getMonth() const;

   //! returns the day portion of the date-time value
   /** @note if more than one of DateTime::getYear(), DateTime::getMonth(), DateTime::getDay(), etc is called for an absolute date/time value, then DateTime::getInfo() should be used instead to avoid the broken-down components of the date being calculated for each call that retrieves a discrete value from the date.  This restriction does not apply to relative date/time values.
       @return the day portion of the date-time value
   */
   DLLEXPORT int getDay() const;

   //! returns the hour portion of the date-time value
   /** @note if more than one of DateTime::getYear(), DateTime::getMonth(), DateTime::getHour(), etc is called for an absolute date/time value, then DateTime::getInfo() should be used instead to avoid the broken-down components of the date being calculated for each call that retrieves a discrete value from the date
       @return the hour portion of the date-time value
   */
   DLLEXPORT int getHour() const;

   //! returns the minute portion of the date-time value
   /** @note if more than one of DateTime::getYear(), DateTime::getMonth(), DateTime::getMinute(), etc is called for an absolute date/time value, then DateTime::getInfo() should be used instead to avoid the broken-down components of the date being calculated for each call that retrieves a discrete value from the date
       @return the minute portion of the date-time value
   */
   DLLEXPORT int getMinute() const;

   //! returns the second portion of the date-time value
   /** @note if more than one of DateTime::getYear(), DateTime::getMonth(), DateTime::getSecond(), etc is called for an absolute date/time value, then DateTime::getInfo() should be used instead to avoid the broken-down components of the date being calculated for each call that retrieves a discrete value from the date
       @return the second portion of the date-time value
   */
   DLLEXPORT int getSecond() const;

   //! returns the microsecond portion of the date-time value divided by 1000
   /** @note if more than one of DateTime::getYear(), DateTime::getMonth(), DateTime::getMillisecond() etc is called for an absolute date/time value, then DateTime::getInfo() should be used instead to avoid the broken-down components of the date being calculated for each call that retrieves a discrete value from the date.  This restriction does not apply to relative date/time values.
       @return the microsecond portion of the date-time value divided by 1000
       @see DateTime::getMicrosecond(), DateTime::getInfo()
   */
   DLLEXPORT int getMillisecond() const;

   //! returns the microsecond portion of the date-time value
   /** @note if more than one of DateTime::getYear(), DateTime::getMonth(), DateTime::getMicrosecond() etc is called for an absolute date/time value, then DateTime::getInfo() should be used instead to avoid the broken-down components of the date being calculated for each call that retrieves a discrete value from the date.  This restriction does not apply to relative date/time values.
       @return the microsecond portion of the date-time value
   */
   DLLEXPORT int getMicrosecond() const;

   //! returns the difference as the number of seconds between the date/time value and the local time at the moment of the call for absolute date/time values; for relative date/time values, the duration is converted to seconds and returned as an integer
   /**
       @return the difference as the number of seconds between the date/time value and the local time at the moment of the call for absolute date/time values; for relative date/time values, the duration is converted to seconds and returned as an integer
   */
   DLLEXPORT int64 getRelativeSeconds() const;

   //! returns the difference as the number of milliseconds between the date/time value and the local time at the moment of the call, for absolute date/time values; for relative date/time values, the duration is converted to milliseconds and returned as an integer
   /**
       @return the difference as the number of milliseconds between the date/time value and the local time at the moment of the call, for absolute date/time values; for relative date/time values, the duration is converted to milliseconds and returned as an integer
   */
   DLLEXPORT int64 getRelativeMilliseconds() const;

   //! returns the difference as the number of microseconds between the date/time value and the local time at the moment of the call, for absolute date/time values; for relative date/time values, the duration is converted to microseconds and returned as an integer
   /** 
       @return the difference as the number of microseconds between the date/time value and the local time at the moment of the call, for absolute date/time values; for relative date/time values, the duration is converted to microseconds and returned as an integer
   */
   DLLEXPORT int64 getRelativeMicroseconds() const;

   //! returns true if the object has a value, false if not (zero value = 1970-01-01Z for absolute times, or all relative components = 0)
   DLLEXPORT bool hasValue() const;

   //! returns the negative time from the current time
   DLLEXPORT DateTime *unaryMinus() const;

   //! converts the current value to the negative of itself
   DLLEXPORT void unaryMinusInPlace();

   //! returns the broken-down time in the given time zone (n_zone = 0 means UTC)
   DLLEXPORT void getInfo(const AbstractQoreZoneInfo *n_zone, qore_tm &info) const;

   //! returns the broken-down time in the current time zone
   DLLEXPORT void getInfo(qore_tm &info) const;

   //! changes the time zone for the time without updating the epoch offset
   DLLEXPORT void setZone(const AbstractQoreZoneInfo *n_zone);

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
   DLLEXPORT static DateTime *getDateFromISOWeek(int year, int week, int day, ExceptionSink *xsink);

   //! returns -1, 0, or 1 if the left date is less than, equal, or greater than the right date
   DLLEXPORT static int compareDates(const DateTime *left, const DateTime *right);

   //! static "constructor" to create an absolute time, including microseconds
   DLLEXPORT static DateTime *makeAbsolute(const AbstractQoreZoneInfo *n_zone, int n_year, int n_month, int n_day, int n_hour = 0, int n_minute = 0, int n_second = 0, int n_us = 0);

   //! static "constructor" to create an absolute time as an offset from the epoch, including microseconds
   /**
      @param zone time zone for the date/time value, 0 = UTC, @see currentTZ()
      @param seconds the number of seconds from January 1, 1970Z
      @param us the microseconds portion of the time	 
   */
   DLLEXPORT static DateTime *makeAbsolute(const AbstractQoreZoneInfo *zone, int64 seconds, int us = 0);

   //! static "constructor" to create an absolute time as an offset from the given time zone's epoch, including microseconds
   /**
      @param zone time zone for the date/time value, 0 = UTC, @see currentTZ()
      @param seconds the number of seconds from January 1, 1970 in the time zone passed as the first argument
      @param us the microseconds portion of the time
   */
   DLLEXPORT static DateTime *makeAbsoluteLocal(const AbstractQoreZoneInfo *zone, int64 seconds, int us = 0);

   //! static "constructor" to create a relative time, including microseconds
   DLLEXPORT static DateTime *makeRelative(int n_year, int n_month, int n_day, int n_hour = 0, int n_minute = 0, int n_second = 0, int n_us = 0);
};

#endif
