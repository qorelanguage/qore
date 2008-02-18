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

class QoreCondition
{
   private:
      pthread_cond_t c;

      // not implemented
      DLLLOCAL QoreCondition(const QoreCondition&);
      DLLLOCAL QoreCondition& operator=(const QoreCondition&);

   public:
      DLLEXPORT QoreCondition();
      DLLEXPORT ~QoreCondition();
      DLLEXPORT int signal();
      DLLEXPORT int broadcast();
      DLLEXPORT int wait(pthread_mutex_t *m);
      DLLEXPORT int wait(pthread_mutex_t *m, int timeout_ms); // timeout in milli seconds
      DLLEXPORT int wait(QoreThreadLock *l) { return wait(&l->ptm_lock); }
      DLLEXPORT int wait(QoreThreadLock *l, int timeout) { return wait(&l->ptm_lock, timeout); } // timeout in milli seconds
};

#endif // QORE_CONDITION
