/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_date_private.h

  DateTime private implementation

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

#ifndef QORE_QORE_DATE_PRIVATE_H
#define QORE_QORE_DATE_PRIVATE_H 

/*
struct abstract_qore_date_private {
public:
   DLLLOCAL isRelative() const = 0;
};
*/

class qore_date_private {
protected:   
   // static constants
   static const int month_lengths[];
   // for calculating the days passed in a year
   static const int positive_months[];
   static const int negative_months[];

   DLLLOCAL void setDatePositive(int64 ty, int64 seconds) {
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
      year += ty + 1970;
      
      day = seconds / 86400;
      seconds -= day * 86400;

      //printd(5, "seconds=%lld year=%d day=%d\n", seconds, year, day);
      bool ly = isLeapYear(year);

      // FIXME: this is inefficient - needs to be optimized
      for (int i = 1; ;i++) {
         //printd(5, "day=%d, positive_months[%d]=%d, adjusted=%d\n", day, i, positive_months[i], (positive_months[i] + (ly && i > 1 ? 1 : 0)));
         if ((positive_months[i] + (ly && i > 1 ? 1 : 0)) > day) {
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
   }

   DLLLOCAL void setDateNegative(int64 ty, int64 seconds) {
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
      year += ty + 1969;

      //printf("year:%d\n", year);
      if (!seconds) {
         // calculate actual year
         year++;
         month = 1;
         day = 1;
         hour = 00;
         minute = 00;
         second = 00;
         return;
      }

      //printd(5, "1: year=%d seconds=%lld\n", year, seconds);
      day = seconds / 86400;
      seconds -= day * 86400;
      // move back a day if there is further time to subtract
      if (seconds)
         day--;
      bool ly = isLeapYear(year);
      //printd(5, "1.1: seconds=%lld day=%d ly=%s\n", seconds, day, ly ? "true" : "false");
      // FIXME: this is inefficient - needs to be optimized
      for (int i = 1; ; i++) {
#if 0
         printd(5, "nm[%d]=%d nm[%d]=%d adj=%d len=%d adj=%d day=%d (mon=%d day=%d)\n", i, negative_months[i], i - 1, 
                negative_months[i - 1], (negative_months[i - 1] - (ly && i == 12 ? 1 : 0)),
                month_lengths[13 - i], month_lengths[13 - i] + (ly && (13 - i) == 2 ? 1 : 0),
                day, 13 - i,
                month_lengths[13 - i] + (ly && (13 - i) == 2) + day - negative_months[i - 1] + (ly && i == 12 ? 0 : 1));
#endif
         // check to find out what month we're in - add extra days for jan & feb if it's a leap year
         if ((negative_months[i] - (ly && i > 10 ? 1 : 0)) <= day) {
            month = 13 - i;
            day = month_lengths[month] + (ly && month == 2 ? 1 : 0) + day - negative_months[i - 1] + (ly && i == 12 ? 2 : 1);
            break;
         }
      }
      //printd(5, "2: seconds=%lld\n", seconds);
      hour = seconds / 3600;
      seconds -= hour * 3600;
      minute = seconds / 60;
      seconds -= minute * 60;
      if ((second = seconds)) {
         second += 60;
         minute--;
      }
      if (minute) {
         minute += 60;
         hour--;
      }
      if (hour) hour += 24;
   }

   //! adds relative date "dt" to absolute date "this"
   /** @param dt the relative date to add
   */
   DLLLOCAL void addRelativeToAbsolute(const qore_date_private &dt) {
      assert(!relative);
      assert(dt.relative);

      // add years
      year += dt.year;

      // add months
      if (dt.month >= 12) {
         year += (dt.month / 12);
         month += (dt.month % 12);
      }
      else
         month += dt.month;

      if (month < 1) {
         year--;
         month += 12;
      }
      else if (month > 12) {
         year++;
         month -= 12;
      }

      // fix days if necessary
      // check for leap years
      if (month == 2 && day > 28)
         day = isLeapYear(year) ? 29 : 28;
      // otherwise set day to last day of month if necessary
      else if (day > month_lengths[month])
         day = month_lengths[month];

      int my_ms = millisecond;

#ifdef QORE_DST_AWARE
      // first add days
      if (dt.day) {
         // set the time to 12 noon to avoid problems with dst
         hour = 12;
         setDate(getEpochSeconds() + 86400 * dt.day);
         hour = hour;
      }
#endif
   
      int ms = my_ms + dt.millisecond;
      int sec = dt.second;
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
   
      if (dt.hour || dt.minute || sec
#ifndef QORE_DST_AWARE
          || dt.day
#endif
         )
         setDate(getEpochSeconds() 
#ifndef QORE_DST_AWARE
                        + (86400 * dt.day)
#endif
                        + (3600 * dt.hour) + (60 * dt.minute) + sec);
      millisecond = ms;
   }

   // add relative date "dt" to relative date "this"
   DLLLOCAL void addRelativeToRelative(const qore_date_private &dt) {
      assert(relative);
      assert(dt.relative);

      year += dt.year;
      month += dt.month;
      day += dt.day;
      hour += dt.hour;
      minute += dt.minute;
      second += dt.second;
      millisecond += dt.millisecond;
   }

   // subtract relative date "dt" from absolute date "this"
   DLLLOCAL void subtractRelativeFromAbsolute(const qore_date_private &dt) {
      assert(!relative);
      assert(dt.relative);

      // subtract years
      year -= dt.year;

      // subtract months
      if (dt.month >= 12) {
         year -= (dt.month / 12);
         month -= (dt.month % 12);
      }
      else
         month -= dt.month;

      if (month < 1) {
         year--;
         month += 12;
      }
      else if (month > 12) {
         year++;
         month -= 12;
      }

      // fix days if necessary
      // check for leap years
      if (month == 2 && day > 28)
         day = isLeapYear(year) ? 29 : 28;
      // otherwise set day to last day of month if necessary
      else if (day > month_lengths[month])
         day = month_lengths[month];

      int my_ms = millisecond;
#ifdef QORE_DST_AWARE
      // subtract days aresult time
      if (dt.day) {
         // set the time to 12 noon to avoid problems with dst
         hour = 12;
         // there are 86400 seconds in an average day
         setDate(getEpochSeconds() - (86400 * dt.day));
         hour = hour;
      }
#endif
   
      // now subtract time
      int ms = my_ms - dt.millisecond;
      // calculate milliseconds a result additional seconds to subtract
      int sec = dt.second;
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

      if (dt.hour || dt.minute || sec
#ifndef QORE_DST_AWARE
          || dt.day
#endif
         )
         setDate(getEpochSeconds() 
#ifndef QORE_DST_AWARE
                        - (86400 * dt.day)
#endif
                        - (3600 * dt.hour) - (60 * dt.minute) - sec);

      millisecond = ms;
   }

   DLLLOCAL void subtractRelativeFromRelative(const qore_date_private &dt) {
      assert(relative);
      assert(dt.relative);

      year -= dt.year;
      month -= dt.month;
      day -= dt.day;
      hour -= dt.hour;
      minute -= dt.minute;
      second -= dt.second;
      millisecond -= dt.millisecond;
   }

   DLLLOCAL void normalize_relative() {
      assert(relative);

      //printd(5, "DT:cD() sec=%lld ms=%d\n", sec, ms);

      // normalize seconds from milliseconds   
      normalize_units<int, int>(second, millisecond, 1000);

      // no longer normalize days, as with DST not all days are 24 hours

      // normalize hours from seconds
      normalize_units<int, int>(hour, second, 3600);

      // normalize minutes from seconds
      normalize_units<int, int>(minute, second, 60);
   }

   template <typename T1, typename T2>
   DLLLOCAL static void normalize_units(T1 &bigger, T2 &smaller, int ratio) {
      if (smaller <= -ratio || smaller >= ratio) {
         int64 units = smaller / ratio;
         bigger += units;
         smaller -= units * ratio;
      }

      // perform further sign normalization
      if (bigger > 0) {
         if (smaller < 0) {
            smaller += ratio;
            --bigger;
         }
      }
      else if (bigger < 0 && smaller > 0) {
         smaller -= ratio;
         ++bigger;
      }
      
   }

   // takes the different between this - dt and sets this to the relative date/time difference
   DLLLOCAL void setDifference(const qore_date_private &dt) {
      assert(!relative);
      assert(!dt.relative);

      int64 sec = getEpochSeconds() - dt.getEpochSeconds();
      millisecond -= dt.millisecond;      

      year = month = day = hour = minute = 0;
      relative = true;

      // normalize seconds from milliseconds   
      normalize_units<int64, int>(sec, millisecond, 1000);

      // no longer normalize days, as with DST not all days are 24 hours

      // normalize hours from seconds
      normalize_units<int, int64>(hour, sec, 3600);

      // normalize minutes from seconds
      normalize_units<int, int64>(minute, sec, 60);

      second = sec;
      normalize_relative();
   }

public:
   int year;
   int month;
   int day;
   int hour;
   int minute;
   int second;
   int millisecond;
   bool relative;

   DLLLOCAL qore_date_private(bool r = false) : hour(0), minute(0), second(0), millisecond(0), relative(r) {
      if (r) {
         year = 0;
         month = 0;
         day = 0;
      }
      else {
         year = 1970;
         month = 1;
         day = 1;
      }
   }

   DLLLOCAL qore_date_private(int y, int mo, int d, int h, int mi, int s, short ms, bool r) : hour(h), minute(mi), second(s), millisecond(ms), relative(r) {
      if (!r && !y && !mo && !d) {
         year = 1970;
         month = 1;
         day = 1;
      }
      else {
         year = y;
         month = mo;
         day = d;
      }
   }

   DLLLOCAL static int compareDates(const qore_date_private *left, const qore_date_private *right) {
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

   DLLLOCAL void setDate(const qore_date_private *p) {
      year = p->year;
      month = p->month;
      day = p->day;
      hour = p->hour;
      minute = p->minute;
      second = p->second;
      millisecond = p->millisecond;
      relative = p->relative;
   }

   // set the date from the number of seconds since January 1, 1970 (UNIX epoch)
   DLLLOCAL void setDate(int64 seconds) {
      // we are setting an absolute date
      relative = false;
      millisecond = year = 0;

      // there are 97 leap days every 400 years (12622780800 seconds)
      int64 ty = seconds/12622780800ll;
      if (ty) {
         year += ty * 400;
         seconds -= ty * 12622780800ll;
      }
      // there are 24 leap days every 100 years (3155673600 seconds)
      ty = seconds/3155673600ll;
      if (ty) {
         year += ty * 100;
         seconds -= ty * 3155673600ll;
      }
      // then there is 1 leap day every 4 years (126230400 seconds)
      ty = seconds/126230400;
      if (ty) {
         year += ty * 4;
         seconds -= ty * 126230400;
      }

      //printd(0, "seconds: %lld year: %d\n", seconds, year);
      if (seconds >= 0)
         setDatePositive(ty, seconds);
      else
         setDateNegative(ty, seconds);
   }

   DLLLOCAL void setDate(const struct tm &tms, short ms) {
      year = 1900 + tms.tm_year;
      month = tms.tm_mon + 1;
      day = tms.tm_mday;
      hour = tms.tm_hour;
      minute = tms.tm_min;
      second = tms.tm_sec;
      millisecond = ms;
      relative = false;
   }

   DLLLOCAL void setDate(const char *str) {
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
      millisecond = atoi(p + 1);
   }

   DLLLOCAL void setRelativeDate(const char *str) {
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
      millisecond = atoi(p + 1);
   }

   DLLLOCAL bool isRelative() const {
      return relative;
   }

   DLLLOCAL short getYear() const {
      return year;
   }

   DLLLOCAL int getMonth() const {
      return month;
   }

   DLLLOCAL int getDay() const {
      return day;
   }

   DLLLOCAL int getHour() const {
      return hour;
   }

   DLLLOCAL int getMinute() const {
      return minute;
   }

   DLLLOCAL int getSecond() const {
      return second;
   }

   DLLLOCAL int getMillisecond() const {
      return millisecond;
   }

   DLLLOCAL int64 getEpochSeconds() const {
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

   DLLLOCAL int getDayNumber() const {
      if (relative)
         return 0;
      return positive_months[(month < 13 ? month : 12) - 1] + day + (month > 2 && isLeapYear(year) ? 1 : 0);
   }

   // it's not legal to call with this=relative and dt=absolute
   DLLLOCAL void add(const qore_date_private &dt) {
      if (!relative) {
         if (dt.relative)
            addRelativeToAbsolute(dt);
         else
            setDate(getEpochSeconds() + dt.getEpochSeconds());
         return;
      }

      assert(dt.relative);
      addRelativeToRelative(dt);
   }

   // it's not legal to call with this=relative and dt=absolute
   DLLLOCAL void subtractBy(const qore_date_private &dt) {
      if (!relative) {
         if (dt.relative)
            subtractRelativeFromAbsolute(dt);
         else {
            if (!dt.relative)
               setDifference(dt);
            else
               setDate(getEpochSeconds() - dt.getEpochSeconds());
         }
         return;
      }

      assert(dt.relative);
      subtractRelativeFromRelative(dt);      
   }

   DLLLOCAL void setTime(int h, int m, int s, short ms) {
      hour = h;
      minute = m;
      second = s;
      millisecond = ms;
   }

   DLLLOCAL void setDate(int64 seconds, int ms) {
      normalize_units<int64, int>(seconds, ms, 1000);
      setDate(seconds);
      millisecond = ms;
   }

   DLLLOCAL void setDateLiteral(int64 date) {
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

      if (second > 59) {
         minute += (second / 60);
         second %= 60;
      }
      if (minute > 59) {
         hour += (minute / 60);
         minute %= 60;
      }
      if (hour > 23) {
         day += (hour / 24);
         hour %= 24;
      }
      // adjust month and year
      if (month > 12) {
         year += ((month - 1)/ 12);
         month = ((month - 1) % 12) + 1;
      }
      // now check day
      if (day) {
         int i;
         while (day > (i = getLastDayOfMonth(month, year))) {
            day -= i;
            month++;
            if (month == 13) {
               month = 1;
               year++;
            }
         }
      }
      relative = false;
   }

   DLLLOCAL void setRelativeDateLiteral(int64 date) {
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

   DLLLOCAL int64 getRelativeSeconds() const {
      return getRelativeMilliseconds() / 1000;
   }

   DLLLOCAL int64 getRelativeMilliseconds() const {
      if (relative)
         return millisecond + second * 1000ll + minute * 60000ll + hour * 3600000ll + day * 86400000ll 
            + (month ? month * 2592000000ll : 0)
            + (year ? year * 31536000000ll : 0);
   
      // find the difference between localtime and now
      time_t ct = time(0);
      struct tm tms;
      qore_date_private od(q_localtime(&ct, &tms));
      int64 diff = (od.getEpochSeconds() - getEpochSeconds()) * 1000 + od.millisecond - millisecond;
      if (diff < 0)
         return 0;
      return diff;
   }

   DLLLOCAL bool isEqual(const qore_date_private &dt) const {
      if (year != dt.year)
         return false;
      if (month != dt.month)
         return false;
      if (day != dt.day)
         return false;
      if (hour != dt.hour)
         return false;
      if (minute != dt.minute)
         return false;
      if (second != dt.second)
         return false;
      if (millisecond != dt.millisecond)
         return false;
      return true;
   }

   DLLLOCAL void getTM(struct tm &tms) const {
      tms.tm_year = year - 1900;
      tms.tm_mon = month - 1;
      tms.tm_mday = day;
      tms.tm_hour = hour;
      tms.tm_min = minute;
      tms.tm_sec = second;
      tms.tm_isdst = 0;
      tms.tm_wday = 0;
      tms.tm_yday = 0;
      tms.tm_isdst = -1;
   }

   DLLLOCAL void format(QoreString &str, const char *fmt) const;

   DLLLOCAL void getAsString(QoreString &str) const {
      if (!relative) {
         format(str, "YYYY-MM-DD HH:mm:SS");
         if (millisecond)
            str.sprintf(".%03d", millisecond);
         return;
      }

      int f = 0;
      str.concat("<time:");

#define PL(n) (n == 1 ? "" : "s")

      if (year)
         str.sprintf(" %d year%s", year, PL(year)), f++;
      if (month)
         str.sprintf(" %d month%s", month, PL(month)), f++;
      if (day)
         str.sprintf(" %d day%s", day, PL(day)), f++;
      if (hour)
         str.sprintf(" %d hour%s", hour, PL(hour)), f++;
      if (minute)
         str.sprintf(" %d minute%s", minute, PL(minute)), f++;
      if (second || (!f && !millisecond))
         str.sprintf(" %d second%s", second, PL(second));
      if (millisecond)
         str.sprintf(" %d millisecond%s", millisecond, PL(millisecond));

#undef PL

      str.concat('>');
      return;
   }

   DLLLOCAL static bool isLeapYear(int year) {
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

   // get the number of seconds before or after January 1, 1970 (UNIX epoch) for a particular date
   DLLLOCAL static int64 getEpochSeconds(int year, int month, int day) {
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

   // note that ISO-8601 week days go from 1 - 7 = Mon - Sun
   // return value: 0 = an exception was raised, not 0 = OK
   DLLLOCAL static qore_date_private *getDateFromISOWeek(DateTime &result, int year, int week, int day, ExceptionSink *xsink) {
      if (week <= 0) {
         xsink->raiseException("ISO-8601-INVALID-WEEK", "week numbers must be positive (value passed: %d)", week);
         return 0;
      }

      // get day of week of jan 1 of this year
      int jan1 = getDayOfWeek(year, 1, 1);

      if (week > 52) {
         // get maximum week number in this year
         int mw = 52 + ((jan1 == 4 && !isLeapYear(year)) || (jan1 == 3 && isLeapYear(year)));
         if (week > mw) {
            xsink->raiseException("ISO-8601-INVALID-WEEK", "there are only %d calendar weeks in year %d (week value passed: %d)", mw, year, week);
            return 0;
         }
      }
   
      if (day < 1 || day > 7) {
         xsink->raiseException("ISO-8601-INVALID-DAY", "calendar week days must be between 1 and 7 for Mon - Sun (day value passed: %f)", day);
         return 0;
      }

      // get year, month, day for start of iso-8601 calendar year
      int y, m, d;
      // if jan1 is mon, then the iso-8601 year starts with the normal year
      if (jan1 == 1) {
         y = year;
         m = 1;
         d = 1;
      }
      // if jan1 is tue - thurs, iso-8601 year starts in dec of previous real year
      else if (jan1 > 1 && jan1 < 5) {
         y = year - 1;
         m = 12;
         d = 33 - jan1;
      }
      else {
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

   DLLLOCAL static int negative_leap_years(int year) {
      year = 1970 - year - 1;
   
      if (year <= 0)
         return 0;
   
      year += 2;
   
      return -year/4 + year/100 - year/400;
   }

   // static method
   DLLLOCAL static int positive_leap_years(int year, int month) {
      if (month < 3 && isLeapYear(year))
         year--;

      year -= 1970;

      if (year <= 0)
         return 0;

      year += 2;
      
      return year/4 - year/100 + year/400;
   }

   DLLLOCAL static int getLastDayOfMonth(int month, int year) {
      if (month != 2)
         return month_lengths[month];
      return isLeapYear(year) ? 29 : 28;
   }

   // static method
   DLLLOCAL static int getDayOfWeek(int year, int month, int day) {
      int a = (14 - month) / 12;
      int y = year - a;
      int m = month + 12 * a - 2;
      return (day + y + y / 4 - y / 100 + y / 400 + (31 * m / 12)) % 7;
   }
};

#endif
