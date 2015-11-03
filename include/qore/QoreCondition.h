/*
  QoreCondition.h

  Qore Programming Language

  Copyright (C) David Nichols 2005

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

#ifndef _QORE_QORECONDITION_H

#define _QORE_QORECONDITION_H

#include <pthread.h>
#include <sys/time.h>

#include <qore/QoreThreadLock.h>

//! a thread condition class implementing a wrapper for pthread_cond_t
/** all threads that block on a given QoreCondition object should use the same mutex or QoreThreadLock object for blocking
    @see QoreThreadLock
 */
class QoreCondition
{
   private:
      //! the condition thread primitive
      pthread_cond_t c;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreCondition(const QoreCondition&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreCondition& operator=(const QoreCondition&);

   public:
      //! creates the condition object
      DLLEXPORT QoreCondition();

      //! destroys the condition object
      DLLEXPORT ~QoreCondition();

      //! signals a single waiting thread to wake up
      DLLEXPORT int signal();

      //! singles all threads blocked on this condition to wake up
      DLLEXPORT int broadcast();

      //! blocks a thread on a mutex until the condition is signaled
      /**
	 @param m the mutex to wait on
	 @return 0 for success, non-zero for error
       */
      DLLEXPORT int wait(pthread_mutex_t *m);

      //! blocks a thread on a mutex for a certain number of milliseconds until the condition is signaled
      /**
	 @param m the mutext to wait on
	 @param timeout_ms the timeout value is milliseconds
	 @return a non-zero return value indicates a timeout occured
       */
      DLLEXPORT int wait(pthread_mutex_t *m, int timeout_ms);

      //! blocks a thread on a lock until the condition is signaled
      /**
	 @param l the QoreThreadLock to wait on
	 @return 0 for success, non-zero for error
       */
      DLLEXPORT int wait(QoreThreadLock *l) { return wait(&l->ptm_lock); }

      //! blocks a thread on a lock for a certain number of milliseconds until the condition is signaled
      /**
	 @param l the QoreThreadLock to wait on
	 @param timeout the timeout value is milliseconds
	 @return a non-zero return value indicates a timeout occured
       */
      DLLEXPORT int wait(QoreThreadLock *l, int timeout) { return wait(&l->ptm_lock, timeout); }

      //! blocks a thread on a lock until the condition is signaled
      /**
	 @param l the QoreThreadLock to wait on
	 @return 0 for success, non-zero for error
       */
      DLLEXPORT int wait(QoreThreadLock &l) { return wait(&l.ptm_lock); }

      //! blocks a thread on a lock for a certain number of milliseconds until the condition is signaled
      /**
	 @param l the QoreThreadLock to wait on
	 @param timeout the timeout value is milliseconds
	 @return a non-zero return value indicates a timeout occured
       */
      DLLEXPORT int wait(QoreThreadLock &l, int timeout) { return wait(&l.ptm_lock, timeout); }
};

#endif // QORE_CONDITION
