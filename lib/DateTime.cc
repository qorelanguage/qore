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

static struct months_s {
      char *long_name;
      char *abbr;
} months[] = {
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

static struct days_s {
      char *long_name;
      char *abbr;
} days[] = {
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

void DateTime::setDate(int64 date)
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
      nd->day = getLastDayOfFebrary(nd->year);
   // otherwise set day to last day of month if necessary
   else if (nd->day > month_lengths[nd->month])
      nd->day = month_lengths[nd->month];

   // first add days
   if (dt->day)
   {
      struct tm nt;

      // set the time to 12 noon to avoid problems with dst
      nd->hour = 12;

      nd->getTM(&nt);
      time_t t = mktime(&nt) + (3600 * 24 * dt->day);

      struct tm tms;
      nd->setDate(q_localtime(&t, &tms));
   }

   // now add time, first write over the time possibly set above
   nd->hour = hour;
   nd->minute = minute;
   nd->second = second;
   // calculate milliseconds and additional seconds to add
   nd->millisecond += dt->millisecond;
   if (nd->millisecond >= 1000)
   {
      dt->second += (nd->millisecond / 1000);
      nd->millisecond %= 1000;
   }
   if (dt->hour || dt->minute || dt->second)
   {
      struct tm nt;

      nd->getTM(&nt);
      time_t t = mktime(&nt) + (3600 * dt->hour) + (60 * dt->minute) + dt->second;

      struct tm tms;
      nd->setDate(q_localtime(&t, &tms));
   }
   return nd;   
}

class DateTime *DateTime::subtractAbsoluteByRelative(class DateTime *dt)
{
   class DateTime *nd = new DateTime();
   nd->relative = false;

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
      nd->day = getLastDayOfFebrary(nd->year);
   // otherwise set day to last day of month if necessary
   else if (nd->day > month_lengths[nd->month])
      nd->day = month_lengths[nd->month];

   // subtract days and time
   if (dt->day)
   {
      struct tm nt;

      // set the time to 12 noon to avoid problems with dst
      nd->hour = 12;

      nd->getTM(&nt);
      time_t t = mktime(&nt) - (3600 * 24 * dt->day);

      struct tm tms;
      nd->setDate(q_localtime(&t, &tms));
   }

   // now subtract time, first write over the time possibly set above
   nd->hour = hour;
   nd->minute = minute;
   nd->second = second;
   // calculate milliseconds and additional seconds to subtract
   if (dt->millisecond > 1000)
   {
      dt->second += (dt->millisecond / 1000);
      dt->millisecond = dt->millisecond % 1000;
   }
   nd->millisecond = millisecond - dt->millisecond;
   if (nd->millisecond < 0)
   {
      nd->millisecond += 1000;
      dt->second++;
   }
   if (dt->hour || dt->minute || dt->second)
   {
      struct tm nt;

      nd->getTM(&nt);
      time_t t = mktime(&nt) - (3600 * dt->hour) - (60 * dt->minute) - dt->second;

      struct tm tms;
      nd->setDate(q_localtime(&t, &tms));
   }
   return nd;   
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

