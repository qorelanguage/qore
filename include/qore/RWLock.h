/* 
  RWLock.h

  Read-Write Lock object (default: prefer readers)

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

#ifndef _QORE_CLASS_RWLOCK

#define _QORE_CLASS_RWLOCK

class RWLock
{
   private:
      int readers, writers, readRequests, writeRequests;
      pthread_mutex_t m;
      pthread_cond_t read;
      pthread_cond_t write;
      bool prefer_writers;

   protected:

   public:
      DLLLOCAL RWLock(bool p = false);
      DLLLOCAL ~RWLock();
      DLLLOCAL void readLock();
      DLLLOCAL void readUnlock();
      DLLLOCAL int tryReadLock();
      DLLLOCAL void writeLock();
      DLLLOCAL void writeUnlock();
      DLLLOCAL void writeToRead();
      DLLLOCAL int tryWriteLock();
      DLLLOCAL int numReaders() const;
};

#endif // _QORE_CLASS_RWLOCK
