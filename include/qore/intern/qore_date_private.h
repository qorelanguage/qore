/*
  qore_date_private.h

  DateTime private implementation

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

#ifndef QORE_QORE_DATE_PRIVATE_H
#define QORE_QORE_DATE_PRIVATE_H 

struct qore_dt_private {
      int year;
      int month;
      int day;
      int hour;
      int minute;
      int second;
      int millisecond;
      bool relative;

      DLLLOCAL static int compareDates(const qore_dt_private *left, const qore_dt_private *right)
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
      DLLLOCAL void setDate(const qore_dt_private *p)
      {
	 year = p->year;
	 month = p->month;
	 day = p->day;
	 hour = p->hour;
	 minute = p->minute;
	 second = p->second;
	 millisecond = p->millisecond;
	 relative = p->relative;
      }
};


#endif
