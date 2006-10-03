/* 
  QC_RWLock.h

  Read-Write Lock object (default: prefer readers)

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

#ifndef _QORE_CLASS_RWLOCK

#define _QORE_CLASS_RWLOCK

#include <qore/common.h>
#include <qore/qore_thread.h>
#include <qore/support.h>
#include <qore/ReferenceObject.h>

extern int CID_RWLOCK;

class QoreClass *initRWLockClass();

class RWLock : public ReferenceObject 
{
   private:
      int readers, writers, readRequests, writeRequests;
      pthread_mutex_t m;
      pthread_cond_t read;
      pthread_cond_t write;
      int prefer_writers;
   protected:
      inline ~RWLock();
   public:
      inline RWLock(int p = 0);
      inline void readLock();
      inline void readUnlock();
      inline int tryReadLock();
      inline void writeLock();
      inline void writeUnlock();
      inline void writeToRead();
      inline int tryWriteLock();
      inline void deref();
      inline int numReaders() { return readers; }
#ifdef DEBUG
      inline int numWriters() { return writers; }
#endif
};

inline RWLock::RWLock(int p)
{
   prefer_writers = p;
   readers = writers = readRequests = writeRequests = 0;
   pthread_mutex_init(&m, NULL);
   pthread_cond_init(&read, NULL);
   pthread_cond_init(&write, NULL);
}

inline RWLock::~RWLock()
{
   pthread_mutex_destroy(&m);
   pthread_cond_destroy(&read);
   pthread_cond_destroy(&write);
}

inline void RWLock::readLock()
{
   pthread_mutex_lock(&m);
   readRequests++;
   // need a while here, because the same thread that sent the
   // signal could regrab the lock before this thread wakes up
   if (prefer_writers)
      while (writers || writeRequests)
	 pthread_cond_wait(&read, &m);
   else
      while (writers)
	 pthread_cond_wait(&read, &m);
   readRequests--;
   readers++;
   pthread_mutex_unlock(&m);
}

inline void RWLock::readUnlock()
{
   pthread_mutex_lock(&m);
/*
   if (!readers)
   {
      pthread_mutex_unlock(&m);
      xsink->raiseException("RWLOCK-READUNLOCK-EXCEPTION", "call to RWLock::readUnlock() without matching RWLock::readLock() call");
      return;
   }
*/
   readers--;
   if (!readers && writeRequests)
      pthread_cond_signal(&write);
   pthread_mutex_unlock(&m);
}

inline int RWLock::tryReadLock()
{
   pthread_mutex_lock(&m);
   if (writeRequests || writers)
   {
      pthread_mutex_unlock(&m);
      return 1;
   }
   readers++;   
   pthread_mutex_unlock(&m);
   return 0;
}

inline void RWLock::writeLock()
{
   pthread_mutex_lock(&m);
   writeRequests++;
   // need a "while" here, because the same thread that sent the
   // signal could regrab the lock before this thread wakes up
   while (readers || writers)
      pthread_cond_wait(&write, &m);
   writeRequests--;
   writers++;
   pthread_mutex_unlock(&m);
}

inline void RWLock::writeUnlock()
{
   pthread_mutex_lock(&m);
/*
   if (!writers)
   {
      pthread_mutex_unlock(&m);
      xsink->raiseException("RWLOCK-WRITEUNLOCK-EXCEPTION", "call to RWLock::writeUnlock() without matching RWLock::writeLock() call");
      return;
   }
*/
   writers--;
   if (writeRequests)
      pthread_cond_signal(&write);
   else if (readRequests)
      pthread_cond_broadcast(&read);
   pthread_mutex_unlock(&m);
}

// while holding the write lock changes it to a read lock
// and wakes up any blocked readers
// not sure if this function should be allowed if the object
// prefers writers
inline void RWLock::writeToRead()
{
   pthread_mutex_lock(&m);
/*
   if (!writers)
   {
      pthread_mutex_unlock(&m);
      xsink->raiseException("RWLOCK-WRITETOREAD-ERROR", "call to RWLock::writeToRead() without matching RWLock::writeLock() call");
      return;
   }
*/
   writers--;
   readers++;
   if (readRequests)
      pthread_cond_broadcast(&read);
   pthread_mutex_unlock(&m);
}

inline int RWLock::tryWriteLock()
{
   pthread_mutex_lock(&m);
   if (writers || readers)
   {
      pthread_mutex_unlock(&m);
      return 1;
   }
   writers++;   
   pthread_mutex_unlock(&m);
   return 0;
}

inline void RWLock::deref()
{
   if (ROdereference())
      delete this;
}

#endif // _QORE_CLASS_RWLOCK
