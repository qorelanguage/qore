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
   
   //printd(5, "Object::evalMember() find_key(%s)=%08p myclass=%s\n", mem, find_key(mem), myclass ? myclass->getName() : "NONE");
   // if accessed outside the class and the member is a private member 
   class Object *obj = getStackObject();
   printd(5, "Object::evalMember(%s) obj=%08p class=%s ID=%d stack obj=%08p class=%s ID=%d isPrivateMember=%s\n", mem, this, myclass->getName(), myclass->getID(), obj, obj ? obj->myclass->getName() : "(null)", obj ? obj->myclass->getID() : -1, myclass->isPrivateMember(mem) ? "true" : "false");
	  
   // if accessed outside the class and the member is a private member 
   if ((!obj || (obj && obj->myclass->getID() != myclass->getID())) && myclass->isPrivateMember(mem))
   {
      if (myclass->hasMemberGate()) // execute the member gate if it exists for private members
	 rv = myclass->evalMemberGate(this, member, xsink);
      else
      {
	 xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, myclass->getName());
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

// gate is already held
inline void Object::doDeleteIntern(class ExceptionSink *xsink)
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

// does a deep dereference and execs destructor if necessary
void Object::dereference(ExceptionSink *xsink)
{
   printd(5, "Object::dereference(this=%08p) class=%s %d->%d\n", this, myclass->getName(), references, references - 1);
   if (ROdereference())
   {
      g.enter();
      printd(5, "Object::dereference() class=%s deleting this=%08p\n", myclass->getName(), this);
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
   printd(5, "Object::obliterate(this=%08p) class=%s %d->%d\n", this, myclass->getName(), references, references - 1);
   if (ROdereference())
   {
      g.enter();
      printd(5, "Object::obliterate() class=%s deleting this=%08p\n", myclass->getName(), this);
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

void Object::doDelete(class ExceptionSink *xsink)
{
   g.enter();
   doDeleteIntern(xsink);
}

// NOTE: caller must unlock
class QoreNode **Object::getMemberValuePtr(char *key, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   vl->add(&g);
   return data->getKeyValuePtr(key);
}

// NOTE: caller must unlock
class QoreNode **Object::getMemberValuePtr(QoreString *key, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   vl->add(&g);
   return data->getKeyValuePtr(key, xsink);
}

// NOTE: caller must unlock
class QoreNode *Object::getMemberValueNoMethod(QoreString *key, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), myclass->getName());
      return NULL;
   }

   QoreNode *rv = data->getKeyValue(key, xsink);
   if (rv)
      vl->add(&g);
   else
      g.exit();
   return rv;
}

// NOTE: caller must unlock
class QoreNode *Object::getMemberValueNoMethod(char *key, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, myclass->getName());
      return NULL;
   }

   QoreNode *rv = data->getKeyValue(key);
   if (rv)
      vl->add(&g);
   else
      g.exit();
   return rv;
}

void Object::deleteMemberValue(QoreString *key, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), myclass->getName());
      return;
   }

   data->deleteKey(key, xsink);
   g.exit();
}

void Object::deleteMemberValue(char *key, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, myclass->getName());
      return;
   }

   data->deleteKey(key, xsink);
   g.exit();
}

class List *Object::getMemberList(class ExceptionSink *xsink)
{
   class List *lst;
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, myclass->getName());
      return NULL;
   }
   lst = data->getKeys();
   g.exit();
   return lst;
}

void Object::setValue(char *key, class QoreNode *value, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, myclass->getName());
      return;
   }
   data->setKeyValue(key, value, xsink);
   g.exit();
}

/*
// FIXME: convert to default character set
inline void Object::setValue(class QoreString *key, class QoreNode *value, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), myclass->getName());
      return;
   }
   data->setKeyValue(key->getBuffer(), value, xsink);
   g.exit();
}
*/

int Object::size()
{
   int rc;

   g.enter();
   if (status == OS_DELETED)
      rc = 0;
   else
      rc = data->size();
   g.exit();
   return rc;
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
void Object::merge(class Hash *h, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, myclass->getName());
      return;
   }
   data->merge(h, xsink);
   g.exit();
}

// adds all elements (already referenced) from the hash passed, deletes the
// hash passed
void Object::assimilate(class Hash *h, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, myclass->getName());
      return;
   }
   data->assimilate(h, xsink);
   g.exit();
}

/*
// to be called only in builtin constructors - no locking necessary
inline void Object::doDeleteNoDestructor(class ExceptionSink *xsink)
{
   status = OS_DELETED;
   data->dereference(xsink);
}
*/

class QoreNode *Object::evalFirstKeyValue(class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, myclass->getName());
      return NULL;
   }
   class QoreNode *rv = data->evalFirstKeyValue(xsink);
   g.exit();
   return rv;
}

class QoreNode *Object::evalMemberNoMethod(char *mem, class ExceptionSink *xsink)
{
   printd(5, "Object::evalMemberNoMethod(this=%08p, mem=%08p (%s), xsink=%08p, data->size()=%d)\n",
	  this, mem, mem, xsink, data ? data->size() : -1);
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, myclass->getName());
      return NULL;
   }
   class QoreNode *rv = data->evalKey(mem, xsink);
   g.exit();
   return rv;
}

// it's OK to return NULL here to duplicate the behaviour of NOTHING
class QoreNode *Object::evalMemberExistence(char *mem, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, myclass->getName());
      return NULL;
   }
   class QoreNode *rv = data->evalKeyExistence(mem, xsink);
   g.exit();
   return rv;
}

class Hash *Object::evalData(class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      // need to return an empty hash here
      return new Hash();
   }
   class Hash *rv = data->eval(xsink);
   g.exit();
   return rv;
}

// NOTE: caller must unlock lock
// we check if the object is already locked
class QoreNode **Object::getExistingValuePtr(class QoreString *mem, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem->getBuffer(), myclass->getName());
      return NULL;
   }
   class QoreNode **rv = data->getExistingValuePtr(mem, xsink);
   if (!rv)
      g.exit();
   else
      vl->add(&g);
   return rv;
}

// NOTE: caller must unlock lock
// we check if the object is already locked
class QoreNode **Object::getExistingValuePtr(char *mem, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, myclass->getName());
      return NULL;
   }
   class QoreNode **rv = data->getExistingValuePtr(mem);
   if (!rv)
      g.exit();
   else
      vl->add(&g);
   return rv;
}

class KeyNode *Object::getReferencedPrivateDataNode(int key) 
{ 
   class KeyNode *rv = NULL;

   g.enter();
   if (status != OS_DELETED && privateData)
      rv = privateData->getReferencedPrivateDataNode(key);
   g.exit();
   return rv;
}

void *Object::getReferencedPrivateData(int key)
{ 
   void *rv = NULL;

   g.enter();
   if (status != OS_DELETED && privateData)
      rv = privateData->getReferencedPrivateData(key);
   g.exit();

   return rv;
}

void *Object::getAndClearPrivateData(int key)
{ 
   void *rv = NULL;
   g.enter();
   if (privateData)
   {
      class KeyNode *kn = privateData->find(key);
      if (kn)
	 rv = kn->getAndClearPtr();
   }
   g.exit(); 
   return rv;
}

void Object::setPrivate(int key, void *pd, q_private_t pdref, q_private_t pdderef)
{ 
   g.enter();
   if (!privateData)
      privateData = new KeyList();
   privateData->insert(key, pd, pdref, pdderef);
   g.exit();
}

void Object::addPrivateDataToString(class QoreString *str)
{
   str->concat('(');
   g.enter();
   if (privateData)
   {
      privateData->addToString(str);
      str->terminate(str->strlen() - 2);
   }
   else
      str->concat("<NO PRIVATE DATA>");
   g.exit();
   str->concat(')');
}

void Object::cleanup(class ExceptionSink *xsink, class Hash *td)
{
   if (privateData)
   {
      delete privateData;
#ifdef DEBUG
      privateData = NULL;
#endif
   }
   
   if (pgm)
   {
      printd(5, "Object::cleaup() this=%08p (%s) calling QoreProgram::depDeref() (%08p)\n", this, myclass->getName(), pgm);
      pgm->depDeref(xsink);
#ifdef DEBUG
      pgm = NULL;
#endif
   }
   
   td->derefAndDelete(xsink);
}

