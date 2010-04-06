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

const int qore_date_private::month_lengths[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int qore_date_private::positive_months[] = { 0,  31,  59,  90,  120,  151,  181,  212,  243,  273,  304,  334,  365 };
const int qore_date_private::negative_months[] = { 0, -31, -61, -92, -122, -153, -184, -214, -245, -275, -306, -334, -365 };

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

void qore_date_private::format(QoreString &str, const char *fmt) const {
   struct tm nt;

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
               str.sprintf("%04d", year);
               s += 2;
            }
            else
               str.sprintf("%02d", year - (year / 100) * 100);
            break;
         case 'M':
            if (s[1] == 'M') {
               str.sprintf("%02d", month);
               s++;
               break;
            }
            if ((s[1] == 'o') && (s[2] == 'n')) {
               s += 2;
               if ((s[1] == 't') && (s[2] == 'h')) {
                  s += 2;
                  if (month && (month <= 12))
                     str.sprintf("%s", months[(int)month - 1].long_name);
                  else
                     str.sprintf("Month%d", month - 1);
                  break;
               }
               if (month && (month <= 12))
                  str.sprintf("%s", months[(int)month - 1].abbr);
               else
                  str.sprintf("M%02d", month);
               break;
            }
            if ((s[1] == 'O') && (s[2] == 'N')) {
               s += 2;
               if ((s[1] == 'T') && (s[2] == 'H')) {
                  s += 2;
                  if (month && (month <= 12)) {
                     char *t = (char *)str.getBuffer() + str.strlen();
                     str.sprintf("%s", months[(int)month - 1].long_name);
                     strtoupper(t);
                  }
                  else
                     str.sprintf("MONTH%d", month);
                  break;
               }
               if (month && (month <= 12)) {
                  char *t = (char *)str.getBuffer() + str.strlen();
                  str.sprintf("%s", months[(int)month - 1].abbr);
                  strtoupper(t);
               }
               else
                  str.sprintf("M%02d", month);
               break;
            }
            str.sprintf("%d", month);
            break;
         case 'D':
            if (s[1] == 'D') {
               str.sprintf("%02d", day);
               s++;
               break;
            }
            if ((s[1] == 'a') && (s[2] == 'y')) {
               s += 2;
               getTM(nt);
               if (mktime(&nt) == -1) // invalid time
                  str.sprintf("Day%d", day);
               else
                  str.sprintf("%s", days[nt.tm_wday].long_name);
               break;
            }
            if ((s[1] == 'A') && (s[2] == 'Y')) {
               s += 2;
               getTM(nt);
               if (mktime(&nt) == -1) // invalid time
                  str.sprintf("DAY%d", day);
               else {
                  char *t = (char *)str.getBuffer() + str.strlen();
                  str.sprintf("%s", days[nt.tm_wday].long_name);
                  strtoupper(t);
               }
               break;
            }
            if ((s[1] == 'y') || (s[1] == 'Y')) {
               s++;;
               getTM(nt);
               if (mktime(&nt) == -1) // invalid time
                  str.sprintf("D%02d", day);
               else
               {
                  char *t = (char *)str.getBuffer() + str.strlen();
                  str.sprintf("%s", days[nt.tm_wday].abbr);
                  if (*s == 'Y')
                     strtoupper(t);
               }
               break;
            }
            str.sprintf("%d", day);
            break;
         case 'H':
            if (s[1] == 'H') {
               str.sprintf("%02d", hour);
               s++;
            }
            else
               str.sprintf("%d", hour);
            break;
         case 'h':
            if (s[1] == 'h') {
               str.sprintf("%02d", ampm(hour));
               s++;
            }
            else
               str.sprintf("%d", ampm(hour));
            break;
         case 'P':
            if (hour > 11)
               str.sprintf("PM");
            else
               str.sprintf("AM");
            break;
         case 'p':
            if (hour > 11)
               str.sprintf("pm");
            else
               str.sprintf("am");
            break;
         case 'm':
            if (s[1] == 'm') {
               str.sprintf("%02d", minute);
               s++;
            }
            else if (s[1] == 's') {
               str.sprintf("%03d", millisecond);
               s++;
            }
            else
               str.sprintf("%d", minute);
            break;
         case 'S':
            if (s[1] == 'S') {
               str.sprintf("%02d", second);
               s++;
            }
            else
               str.sprintf("%d", second);
            break;
         case 'u':
            if (s[1] == 'u') {
               str.sprintf("%03d", millisecond);
               s++;
            }
            else
               str.sprintf("%d", millisecond);
            break;
         default:
	    str.concat(*s);
            break;
      }
      s++;
   }

   printd(5, "qore_date_private::format() returning \"%s\"\n", str.getBuffer());
}
