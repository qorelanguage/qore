/*
  DateTimeNode.h

  DateTimeNode Class Definition

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

#ifndef _QORE_DATETIMENODE_H

#define _QORE_DATETIMENODE_H

#include <qore/AbstractQoreNode.h>
#include <qore/DateTime.h>

//! Qore's parse tree/value type for date-time values, reference-counted, dynamically-allocated only
class DateTimeNode : public SimpleValueQoreNode, public DateTime
{
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL DateTimeNode(const DateTime *);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL DateTimeNode& operator=(const DateTimeNode &);

      //! returns a boolean value based on the number of seconds after 1970-01-01 00:00:00 (start of the UNIX epoch)
      /** so 1970-01-01 00:00:00.000 is false (ZeroDate), every other date is true
	  basically the date is converted to the epoch seconds offset then 0 is false, non-0 is true
	  @return false if the date is 1970-01-01 00:00:00.000 (ZeroDate), true for every other date
       */
      DLLEXPORT virtual bool getAsBoolImpl() const;

      //! returns the seconds offset from 1970-01-01 00:00:00 (start of the UNIX epoch) 
      /**
	 @return the seconds offset from 1970-01-01 00:00:00 (start of the UNIX epoch) 
       */
      DLLEXPORT virtual int getAsIntImpl() const;

      //! returns the seconds offset from 1970-01-01 00:00:00 (start of the UNIX epoch) 
      /**
	 @return the seconds offset from 1970-01-01 00:00:00 (start of the UNIX epoch) 
       */
      DLLEXPORT virtual int64 getAsBigIntImpl() const;

      //! returns the seconds offset from 1970-01-01 00:00:00 (start of the UNIX epoch) 
      /**
	 @return the seconds offset from 1970-01-01 00:00:00 (start of the UNIX epoch) 
       */
      DLLEXPORT virtual double getAsFloatImpl() const;

   protected:
      //! protected destructor only called when references = 0, use deref() instead
      DLLEXPORT virtual ~DateTimeNode();

   public:
      //! constructor for an empty object
      /**
	 @param r sets the "relative" flag for the object
       */
      DLLEXPORT DateTimeNode(bool r = false);

      //! constructor for setting all parameters
      /**
	 @param n_year the year value
	 @param n_month the months value
	 @param n_day the days value
	 @param n_hour the hours value
	 @param n_minute the minutes value
	 @param n_second the seconds value
	 @param n_ms the milliseconds value
	 @param n_relative the relative flag
       */
      DLLEXPORT DateTimeNode(int n_year, int n_month, int n_day, int n_hour = 0, int n_minute = 0, int n_second = 0, short n_ms = 0, bool n_relative = false);

      //! constructor for setting an absolute date based on the number of seconds from January 1, 1970
      /**
	 @param seconds the number of seconds from January 1, 1970
       */
      DLLEXPORT DateTimeNode(int64 seconds);

      //! constructor for setting an absolute date based on the number of seconds from January 1, 1970 (plus milliseconds)
      /**
	 @param seconds the number of seconds from January 1, 1970
	 @param ms the milliseconds portion of the time	 
       */
      DLLEXPORT DateTimeNode(int64 seconds, int ms);

      //! constructor for setting the date from a string in the format YYYYMMDDHHmmSS
      /** additionally a milliseconds value can be appended with a period and 3 integers in the format [.xxx]
	  @param date the string to use to set the date in the format YYYYMMDDHHmmSS[.xxx]
       */
      DLLEXPORT DateTimeNode(const char *date);

      //! constructor for setting an absolute date based on a "struct tm"
      /**
	 @param tms a structure giving the absolute date to set 
      */
      DLLEXPORT DateTimeNode(struct tm *tms);

      //! copy constructor
      DLLEXPORT DateTimeNode(const DateTimeNode &dt);

      //! constructor to set the date from a DateTime value
      DLLEXPORT DateTimeNode(const DateTime &dt);

      //! returns a string in the format YYYYMMDDHHmmSS, del is set to true
      /** NOTE: do not use this function directly, use QoreStringValueHelper instead
	  @param del output parameter: if del is true, then the resulting QoreString pointer belongs to the caller (and must be deleted manually), if false it must not be
	  @return a QoreString pointer, use the del output parameter to determine ownership of the pointer
	  @see QoreStringValueHelper
       */
      DLLEXPORT virtual QoreString *getStringRepresentation(bool &del) const;

      //! concatentates the date/time value in the format YYYYMMDDHHmmDD to an existing QoreString reference
      /**
	 @param str a reference to a QoreString where date/time value will be concatenated in the format YYYYMMDDHHmmDD
       */
      DLLEXPORT virtual void getStringRepresentation(QoreString &str) const;

      //! returns "this" as a DateTime, del is set to false
      /** NOTE: Use the DateTimeValueHelper class instead of using this function directly
	  @param del output parameter: if del is true, then the returned DateTime pointer belongs to the caller (and must be deleted manually), if false, then it must not be
	  @see DateTimeValueHelper
       */
      DLLEXPORT virtual class DateTime *getDateTimeRepresentation(bool &del) const;

      //! assigns this date/time representation to the passed DateTime reference
      /**
	 @param dt the reference where the current date/time value will be copied
       */
      DLLEXPORT virtual void getDateTimeRepresentation(DateTime &dt) const;

      //! returns the date/time value as a formatted string for %n and %N printf formatting, del is set to true
      /** the format for absolute date/time value is: YYYY-MM-DD HH:mm:SS 
	  the format for relative date/time values is: <time: x years, x months, ...>
	  NOTE: do not use this function directly, use QoreStringValueHelper instead
	  @param del output parameter: always set to true by this function, meaning that the caller owns the QoreString pointer returned (and must delete it manually)
	  @param foff ignored for this implementation of the file
	  @param xsink ignored for this implementation of the file
	  @return a QoreString pointer, use the del output parameter to determine ownership of the pointer
	  @see QoreStringValueHelper
       */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! concatenates a string representation of the date/time value (designed for %n and %N printf formatting) to a QoreString reference
      /** the format for absolute date/time value is: YYYY-MM-DD HH:mm:SS
	  the format for relative date/time values is: <time: x years, x months, ...>
	  @param str the QoreString reference to concatenate the date/time value to
	  @param foff ignored for this implementation of the file
	  @param xsink ignored for this implementation of the file
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      //! tests for equality with possible type conversion (soft compare)
      /** this function does not throw any Qore-language exceptions
	  @param v the value to compare
	  @param xsink is not used in this implementation of the function
       */
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "date";
      }

      //! returns a copy of the DateTimeNode, the caller owns the pointer's reference count
      /**
	 @return a copy of the DateTimeNode, the caller owns the pointer's reference count
       */
      DLLEXPORT DateTimeNode *copy() const;

      //! adds a DateTime value to the current value and returns the new value, the caller owns the pointer's reference count
      /** 
	  @return a new DateTimeNode value, the caller owns the pointer's reference count
       */
      DLLEXPORT DateTimeNode *add(const class DateTime *dt) const;

      //! subtracts a DateTime value from the current value and returns the new value, the caller owns the pointer's reference count
      /** 
	  @return a new DateTimeNode value, the caller owns the pointer's reference count
       */
      DLLEXPORT DateTimeNode *subtractBy(const class DateTime *dt) const;

      //! returns a DateTimeNode value as generated from the ISO-8601 week information
      /** NOTE: ISO-8601 week days go from 1 - 7 = Mon - Sun, a 0 return value means an exception was raised
	  in the case the ISO-8601 week information is invalid
	  @param year the ISO-8601 year (may differ from the actual calendar year)
	  @param week the ISO-8601 week number in the year
	  @param day the ISO-8601 day number (1=Mon, 7=Sun)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT static DateTimeNode *getDateFromISOWeek(int year, int week, int day, class ExceptionSink *xsink);
};

DLLEXPORT extern DateTimeNode *ZeroDate;

//! manages calls to AbstractQoreNode::getDateTimeRepresentation() when a simple DateTime value is required
/** calls to this function include a flag that indicates if the value should be deleted or not afterwards;
    this class manages the return value in an easy and exception-safe way
    if a DateTimeNode value is required instead, use DateTimeNodeValueHelper
    @see DateTimeNodeValueHelper
 */
class DateTimeValueHelper {
   private:
      const DateTime *dt;
      bool del;

      DLLLOCAL DateTimeValueHelper(const DateTimeValueHelper&); // not implemented
      DLLLOCAL DateTimeValueHelper& operator=(const DateTimeValueHelper&); // not implemented
      DLLLOCAL void *operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      //! gets the DateTime value and set the delete flag
      DLLLOCAL DateTimeValueHelper(const AbstractQoreNode *n)
      {
	 // optmization without virtual function call for most common case
	 if (n) {
	    if (n->getType() == NT_DATE) {
	       dt = reinterpret_cast<const DateTimeNode *>(n);
	       del = false;
	    }
	    else
	       dt = n->getDateTimeRepresentation(del);
	 }
	 else {
	    dt = ZeroDate;
	    del = false;
	 }
      }

      //! deletes the DateTime value being managed if necessary
      DLLLOCAL ~DateTimeValueHelper()
      {
	 if (del)
	    delete const_cast<DateTime *>(dt);
      }
      DLLLOCAL const DateTime *operator->() { return dt; }
      DLLLOCAL const DateTime *operator*() { return dt; }
};

//! manages calls to AbstractQoreNode::getDateTimeRepresentation() when a DateTimeNode value is required
/** if a simple DateTime value is required instead, use DateTimeValueHelper
    @see DateTimeNodeHelper
 */
class DateTimeNodeValueHelper {
   private:
      DateTimeNode *dt;
      bool temp;

      DLLLOCAL DateTimeNodeValueHelper(const DateTimeNodeValueHelper&); // not implemented
      DLLLOCAL DateTimeNodeValueHelper& operator=(const DateTimeNodeValueHelper&); // not implemented
      DLLLOCAL void *operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      //! gets the DateTimeNode value and sets the temporary flag
      DLLLOCAL DateTimeNodeValueHelper(const AbstractQoreNode *n)
      {
	 if (!n) {
	    dt = ZeroDate;
	    temp = false;
	    return;
	 }

	 // optmization without virtual function call for most common case
	 if (n->getType() == NT_DATE) {
	    dt = const_cast<DateTimeNode *>(reinterpret_cast<const DateTimeNode *>(n));
	    temp = false;
	    return;
	 }

	 dt = new DateTimeNode();
	 n->getDateTimeRepresentation(*dt);
	 temp = true;
      }

      //! dereferences the DateTimeNode value if necessary
      DLLLOCAL ~DateTimeNodeValueHelper()
      {
	 if (dt && temp)
	    dt->deref();
      }

      DLLLOCAL const DateTimeNode *operator->() { return dt; }
      DLLLOCAL const DateTimeNode *operator*() { return dt; }

      //! returns a referenced value - the caller will own the reference
      /**
	  The value is referenced if necessary (if it was a temporary value)
	  @return the DateTimeNode value, where the caller will own the reference count
      */
      DLLLOCAL DateTimeNode *getReferencedValue()
      {
	 if (temp)
	    temp = false;
	 else if (dt)
	    dt->ref();
	 return dt;
      }
};

#endif
