/*
  DateTimeNode.h

  DateTimeNode Class Definition

  Qore Programming Language

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

#ifndef _QORE_DATETIMENODE_H

#define _QORE_DATETIMENODE_H

#include <qore/AbstractQoreNode.h>
#include <qore/DateTime.h>

class DateTimeNode : public SimpleQoreNode, public DateTime
{
   private:
      // these functions are not actually implemented
      DLLLOCAL DateTimeNode(const DateTime *);
      DLLLOCAL DateTimeNode& operator=(const DateTimeNode &);

   protected:
      // destructor only called when references = 0, use deref() instead
      DLLEXPORT virtual ~DateTimeNode();

   public:
      DLLEXPORT DateTimeNode(bool r = false);
      DLLEXPORT DateTimeNode(int y, int mo, int d, int h = 0, int mi = 0, int s = 0, short ms = 0, bool r = false);
      DLLEXPORT DateTimeNode(int64 seconds);
      DLLEXPORT DateTimeNode(int64 seconds, int ms);
      DLLEXPORT DateTimeNode(const char *date);
      DLLEXPORT DateTimeNode(struct tm *tms);
      DLLEXPORT DateTimeNode(const DateTimeNode &dt);
      DLLEXPORT DateTimeNode(const DateTime &dt);

      // get the value of the type in a string context (default implementation = del = false and returns NullString)
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      // use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
      DLLEXPORT virtual QoreString *getStringRepresentation(bool &del) const;
      // concatenate string representation to a QoreString (no action for complex types = default implementation)
      DLLEXPORT virtual void getStringRepresentation(QoreString &str) const;

      // if del is true, then the returned DateTime * should be deleted, if false, then it should not
      DLLEXPORT virtual class DateTime *getDateTimeRepresentation(bool &del) const;
      // assign date representation to a DateTime (no action for complex types = default implementation)
      DLLEXPORT virtual void getDateTimeRepresentation(DateTime &dt) const;

      DLLEXPORT virtual bool getAsBool() const;
      DLLEXPORT virtual int getAsInt() const;
      DLLEXPORT virtual int64 getAsBigInt() const;
      DLLEXPORT virtual double getAsFloat() const;

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      // performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
      // the "val" passed
      //DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
      // the type passed must always be equal to the current type
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      // returns the data type
      DLLEXPORT virtual const QoreType *getType() const;
      DLLEXPORT virtual const char *getTypeName() const;

      DLLEXPORT DateTimeNode *copy() const;
      
      DLLEXPORT DateTimeNode *add(const class DateTime *dt) const;
      DLLEXPORT DateTimeNode *subtractBy(const class DateTime *dt) const;

      // note that ISO-8601 week days go from 1 - 7 = Mon - Sun
      // a NULL return value means an exception was raised
      DLLEXPORT static DateTimeNode *getDateFromISOWeek(int year, int week, int day, class ExceptionSink *xsink);
};

extern DateTimeNode *ZeroDate;

class DateTimeValueHelper {
   private:
      const DateTime *dt;
      bool del;

      DLLLOCAL DateTimeValueHelper(const DateTimeValueHelper&); // not implemented
      DLLLOCAL DateTimeValueHelper& operator=(const DateTimeValueHelper&); // not implemented
      DLLLOCAL void *operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLLOCAL DateTimeValueHelper(const AbstractQoreNode *n)
      {
	 if (n)
	    dt = n->getDateTimeRepresentation(del);
	 else {
	    dt = ZeroDate;
	    del = false;
	 }
      }
      DLLLOCAL ~DateTimeValueHelper()
      {
	 if (del)
	    delete const_cast<DateTime *>(dt);
      }
      DLLLOCAL const DateTime *operator->() { return dt; }
      DLLLOCAL const DateTime *operator*() { return dt; }
};

class DateTimeNodeValueHelper {
   private:
      DateTimeNode *dt;
      bool temp;

      DLLLOCAL DateTimeNodeValueHelper(const DateTimeNodeValueHelper&); // not implemented
      DLLLOCAL DateTimeNodeValueHelper& operator=(const DateTimeNodeValueHelper&); // not implemented
      DLLLOCAL void *operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLLOCAL DateTimeNodeValueHelper(AbstractQoreNode *n)
      {
	 if (!n) {
	    dt = ZeroDate;
	    temp = false;
	    return;
	 }

	 {
	    DateTimeNode *date = dynamic_cast<DateTimeNode *>(n);
	    if (date) {
	       dt = date;
	       temp = false;
	       return;
	    }
	 }

	 dt = new DateTimeNode();
	 n->getDateTimeRepresentation(*dt);
	 temp = true;
      }

      DLLLOCAL ~DateTimeNodeValueHelper()
      {
	 if (dt && temp)
	    dt->deref();
      }

      DLLLOCAL const DateTimeNode *operator->() { return dt; }
      DLLLOCAL const DateTimeNode *operator*() { return dt; }

      // takes the referenced value and leaves this object empty, value is referenced if necessary
      DLLLOCAL DateTimeNode *takeReferencedValue()
      {
         DateTimeNode *rv = dt;
	 if (dt && !temp)
	    rv->ref();
         dt = 0;
         temp = false;
         return rv;
      }
};

#endif
