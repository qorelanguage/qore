/*
 QoreCondition.cc
 
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

#include <qore/Qore.h>
#include <qore/QoreCondition.h>

QoreCondition::QoreCondition()
{
   pthread_cond_init(&c, 0);
}

QoreCondition::~QoreCondition()
{
   pthread_cond_destroy(&c);
}

int QoreCondition::signal()
{
   return pthread_cond_signal(&c);
}

int QoreCondition::broadcast()
{
   return pthread_cond_broadcast(&c);
}

int QoreCondition::wait(pthread_mutex_t *m)
{
   return pthread_cond_wait(&c, m);
}

// timeout is in milliseconds
int QoreCondition::wait(pthread_mutex_t *m, int timeout_ms)
{
   struct timeval now;
   struct timespec tmout;
   
   gettimeofday(&now, 0);
   int secs = timeout_ms / 1000;
   timeout_ms -= secs * 1000;
   tmout.tv_sec = now.tv_sec + secs;
   tmout.tv_nsec = now.tv_usec * 1000 + timeout_ms * 1000000;
   
   return pthread_cond_timedwait(&c, m, &tmout);
}
