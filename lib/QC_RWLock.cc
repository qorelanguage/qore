/*
  QC_RWLock.cc

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
#include <qore/common.h>
#include <qore/thread.h>
#include <qore/QC_RWLock.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_RWLOCK;

static inline void *getRWLock(void *obj)
{
   ((RWLock *)obj)->ROreference();
   return obj;
}

class QoreNode *RWLOCK_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_RWLOCK, new RWLock(), getRWLock);
   return NULL;
}

class QoreNode *RWLOCK_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *m = (RWLock *)self->getAndClearPrivateData(CID_RWLOCK);
   if (m)
      m->deref();
   return NULL;
}

class QoreNode *RWLOCK_readLock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *rwl = (RWLock *)self->getReferencedPrivateData(CID_RWLOCK);
   if (rwl)
   {
      rwl->readLock();
      rwl->deref();
   }
   else
      alreadyDeleted(xsink, "RWLock::readLock");
   return NULL;
}

class QoreNode *RWLOCK_readUnlock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *rwl = (RWLock *)self->getReferencedPrivateData(CID_RWLOCK);
   if (rwl)
   {
      rwl->readUnlock();
      rwl->deref();
   }
   else
      alreadyDeleted(xsink, "RWLock::readUnlock");
   return NULL;
}

class QoreNode *RWLOCK_writeLock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *rwl = (RWLock *)self->getReferencedPrivateData(CID_RWLOCK);
   if (rwl)
   {
      rwl->writeLock();
      rwl->deref();
   }
   else
      alreadyDeleted(xsink, "RWLock::writeLock");
   return NULL;
}

class QoreNode *RWLOCK_writeUnlock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *rwl = (RWLock *)self->getReferencedPrivateData(CID_RWLOCK);
   if (rwl)
   {
      rwl->writeUnlock();
      rwl->deref();
   }
   else
      alreadyDeleted(xsink, "RWLock::writeUnlock");
   return NULL;
}

class QoreNode *RWLOCK_tryReadLock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *rwl = (RWLock *)self->getReferencedPrivateData(CID_RWLOCK);
   QoreNode *rv;
   if (rwl)
   {
      rv = new QoreNode(NT_INT, rwl->tryReadLock());
      rwl->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "RWLock::tryReadLock");
   }
   return rv;
}

class QoreNode *RWLOCK_tryWriteLock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *rwl = (RWLock *)self->getReferencedPrivateData(CID_RWLOCK);
   QoreNode *rv;
   if (rwl)
   {
      rv = new QoreNode(NT_INT, rwl->tryWriteLock());
      rwl->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "RWLock::tryWriteLock");
   }
   return rv;
}

class QoreNode *RWLOCK_numReaders(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *rwl = (RWLock *)self->getReferencedPrivateData(CID_RWLOCK);
   QoreNode *rv;
   if (rwl)
   {
      rv = new QoreNode(NT_INT, rwl->numReaders());
      rwl->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "RWLock::numReaders");
   }
   return rv;
}

#ifdef DEBUG
class QoreNode *RWLOCK_numWriters(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   RWLock *rwl = (RWLock *)self->getReferencedPrivateData(CID_RWLOCK);
   QoreNode *rv;
   if (rwl)
   {
      rv = new QoreNode(NT_INT, rwl->numWriters());
      rwl->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "RWLock::numWriters");
   }
   return rv;
}
#endif

class QoreClass *initRWLockClass()
{
   tracein("initRWLockClass()");

   class QoreClass *QC_RWLOCK = new QoreClass(strdup("RWLock"));
   CID_RWLOCK = QC_RWLOCK->getID();
   QC_RWLOCK->addMethod("constructor",   RWLOCK_constructor);
   QC_RWLOCK->addMethod("destructor",    RWLOCK_destructor);
   QC_RWLOCK->addMethod("copy",          RWLOCK_constructor);
   QC_RWLOCK->addMethod("readLock",      RWLOCK_readLock);
   QC_RWLOCK->addMethod("writeLock",     RWLOCK_writeLock);
   QC_RWLOCK->addMethod("readUnlock",    RWLOCK_readUnlock);
   QC_RWLOCK->addMethod("writeUnlock",   RWLOCK_writeUnlock);
   QC_RWLOCK->addMethod("tryReadLock",   RWLOCK_tryReadLock);
   QC_RWLOCK->addMethod("tryWriteLock",  RWLOCK_tryWriteLock);
   QC_RWLOCK->addMethod("numReaders",    RWLOCK_numReaders);
#ifdef DEBUG
   QC_RWLOCK->addMethod("numWriters",    RWLOCK_numWriters);
#endif

   traceout("initRWLockClass()");
   return QC_RWLOCK;
}
