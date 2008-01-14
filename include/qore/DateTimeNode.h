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

#include <qore/QoreNode.h>
#include <qore/DateTime.h>

class DateTimeNode : public QoreNode, public DateTime
{
   protected:
      // destructor only called when references = 0, use deref() instead
      DLLEXPORT virtual ~DateTimeNode();

   public:
      DLLEXPORT DateTimeNode();

};

extern QoreNode *ZeroDate;

class DateTimeValueHelper {
   private:
      const DateTime *dt;
      bool del;

      DLLLOCAL DateTimeValueHelper(const DateTimeValueHelper&); // not implemented
      DLLLOCAL DateTimeValueHelper& operator=(const DateTimeValueHelper&); // not implemented
      DLLLOCAL void *operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLLOCAL DateTimeValueHelper(const QoreNode *n)
      {
	 if (n)
	    dt = n->getDateTimeRepresentation(del);
	 else {
	    dt = ZeroDate->val.date_time;
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
      //DateTimeNode *
      QoreNode *dt;
      bool temp;

      DLLLOCAL DateTimeNodeValueHelper(const DateTimeNodeValueHelper&); // not implemented
      DLLLOCAL DateTimeNodeValueHelper& operator=(const DateTimeNodeValueHelper&); // not implemented
      DLLLOCAL void *operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLLOCAL DateTimeNodeValueHelper(QoreNode *n)
      {
	 if (!n) {
	    dt = ZeroDate;
	    temp = false;
	    return;
	 }

	 else if (n->type == NT_DATE) {
	    dt = n;
	    temp = false;
	 }

	 else {
	    dt = new QoreNode(n->getDateTimeRepresentation(temp));
	    temp = true;
	 }
      }

      DLLLOCAL ~DateTimeNodeValueHelper()
      {
	 if (dt && temp)
	    dt->deref(0);
      }

      //DateTimeNode *
      DLLLOCAL const QoreNode *operator->() { return dt; }
      //DateTimeNode *
      DLLLOCAL const QoreNode *operator*() { return dt; }

      // takes the referenced value and leaves this object empty, value is referenced if necessary
      //DateTimeNode *
      DLLLOCAL QoreNode *takeReferencedValue()
      {
         QoreNode *rv = dt && !temp ? dt->RefSelf() : dt;
         dt = 0;
         temp = false;
         return rv;
      }
};

#endif
