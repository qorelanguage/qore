/*
  Object.cc

  thread-safe object definition

  references: how many variables are pointing at this object?

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/Object.h>

// NOTE: caller must unlock
class QoreNode *Object::getMemberValue(class QoreNode *member, class VLock *vl, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   class QoreNode *rv = data->getKeyValueExistence(member->val.String, xsink);

   // if the member does not exist, then try the memberGate method
   if (rv == (QoreNode *)-1)
   {
      if (myclass->hasMemberGate())
      {
	 // run memberGate method to get member value, in case the method is very slow, we run with locks
	 // disabled: reference, unlock gate, run method, dereference, check return value, require lock, check object status
	 ref();
	 g.exit();
	 //vl->del();
	 rv = myclass->evalMemberGate(this, member, xsink);
	 dereference(xsink);
	 if (!rv)
	    return NULL;
	 if (xsink->isEvent())
	 {
	    rv->deref(xsink);
	    return NULL;
	 }

	 // now grab lock again
	 g.enter();
	 if (status == OS_DELETED)
	 {
	    g.exit();
	    rv->deref(xsink);
	    return NULL;
	 }
      }
      else
	 rv = NULL;
   }
   if (!rv)
   {
      g.exit();
      return NULL;
   }

   vl->add(&g);
   return rv;
}

class QoreNode *Object::evalMember(class QoreNode *member, class ExceptionSink *xsink)
{
   // make sure to convert string encoding if necessary to default character set
   class QoreString *tstr = member->val.String;
   if (tstr->getEncoding() != QCS_DEFAULT)
   {
      tstr = tstr->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
   }

   class QoreNode *rv;
   char *mem = tstr->getBuffer();
   
   //printd(5, "Object::evalMember() find_key(%s)=%08p myclass=%s\n", mem, find_key(mem), myclass ? myclass->name : "NONE");
   // if accessed outside the class and the member is a private member 
   class Object *obj = getStackObject();
   printd(5, "Object::evalMember(%s) obj=%08p class=%s ID=%d stack obj=%08p class=%s ID=%d isPrivateMember=%s\n", mem, this, myclass->name, myclass->getID(), obj, obj ? obj->myclass->name : "(null)", obj ? obj->myclass->getID() : -1, myclass->isPrivateMember(mem) ? "true" : "false");
	  
   // if accessed outside the class and the member is a private member 
   if ((!obj || (obj && obj->myclass->getID() != myclass->getID())) && myclass->isPrivateMember(mem))
   {
      if (myclass->hasMemberGate()) // execute the member gate if it exists for private members
	 rv = myclass->evalMemberGate(this, member, xsink);
      else
      {
	 xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, myclass->name);
	 rv = NULL;
      }
   }
   else
   {
      g.enter();
      if (status == OS_DELETED)
      {
	 g.exit();
	 rv = NULL;
      }
      else
      {
	 rv = data->evalKeyExistence(mem, xsink);
	 g.exit();
	 
	 // execute memberGate method for objects where no member exists
	 if (rv == (QoreNode *)-1)
	    rv = myclass->evalMemberGate(this, member, xsink);
      }
   }
   if (tstr != member->val.String)
      delete tstr;

   return rv;
}

// 0 = equal, 1 = not equal
bool Object::compareSoft(class Object *obj, class ExceptionSink *xsink)
{
   if (obj->myclass != myclass)
      return 1;

   // to avoid deadlocks when an object is compared with itself
   if (this == obj)
      return 0;

   // objects are not equal if they have private data and they
   // do not point to the same objects
   if (privateData != obj->privateData)
      return 1;

   int rc;

   g.enter();
   obj->g.enter();
   if (status == OS_DELETED)
   {
      if (obj->status == OS_DELETED)
	 rc = 0;
      else
	 rc = 1;
   }
   else if (obj->status == OS_DELETED)
      rc = 1;
   else
      rc = data->compareSoft(obj->data, xsink);

   obj->g.exit();
   g.exit();
   return rc;
}

// 0 = equal, 1 = not equal
bool Object::compareHard(class Object *obj)
{
   if (obj->myclass != myclass)
      return 1;

   // to avoid deadlocks when an object is compared with itself
   if (this == obj)
      return 0;

   // objects are not equal if they have private data and they
   // do not point to the same objects
   if (privateData != obj->privateData)
      return 1;

   int rc;

   g.enter();
   obj->g.enter();

   if (status == OS_DELETED)
   {
      if (obj->status == OS_DELETED)
	 rc = 0;
      else
	 rc = 1;
   }
   else if (obj->status == OS_DELETED)
      rc = 1;
   else
      rc = data->compareHard(obj->data);

   obj->g.exit();
   g.exit();
   return rc;
}

// does a deep dereference and execs destructor if necessary
void Object::dereference(ExceptionSink *xsink)
{
   printd(5, "Object::dereference(this=%08p) class=%s %d->%d\n", this, myclass->name, references, references - 1);
   if (ROdereference())
   {
      g.enter();
      printd(5, "Object::dereference() class=%s deleting this=%08p\n", myclass->name, this);
      if (status == OS_OK)
      {
	 // reference for destructor
	 ROreference();
	 doDeleteIntern(xsink);
	 ROdereference();
      }
      else
      {
	 g.exit();
	 printd(5, "Object::dereference() %08p data=%08p status=%d\n", this, data, status);
      }
      tDeref();
   }
}

// this method is called when there is an exception in a constructor and the object should be deleted
void Object::obliterate(ExceptionSink *xsink)
{
   printd(5, "Object::obliterate(this=%08p) class=%s %d->%d\n", this, myclass->name, references, references - 1);
   if (ROdereference())
   {
      g.enter();
      printd(5, "Object::obliterate() class=%s deleting this=%08p\n", myclass->name, this);
      if (status == OS_OK)
      {
	 status = OS_DELETED;
	 Hash *td = data;
	 data = NULL;
	 g.exit();

	 if (privateData)
	    privateData->derefAll();

	 cleanup(xsink, td);
      }
      else
      {
	 g.exit();
	 printd(5, "Object::obliterate() %08p data=%08p status=%d\n", this, data, status);
      }
      tDeref();
   }
}

// gate is already held
void Object::doDeleteIntern(class ExceptionSink *xsink)
{
   if (status == OS_DELETED)
   {
      g.exit();
      return;
   }
   if (status == OS_BEING_DELETED)
   {
      g.exit();
      xsink->raiseException("DOUBLE-DELETE-EXCEPTION", "destructor called from within destructor");
      return;
   }
   status = OS_BEING_DELETED;
   g.exit();
   
   printd(5, "Object::doDelete(this=%08p) calling destructor()\n", this);   
   myclass->execDestructor(this, xsink);
   
   g.enter();
   status = OS_DELETED;
   Hash *td = data;
   data = NULL;
   g.exit();

   cleanup(xsink, td);
}
