/*
  QoreCounter.h

  Qore Programming Language

  Copyright (C) David Nichols 2005 - 2009

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

#ifndef _QORE_QORECOUNTER_H

#define _QORE_QORECOUNTER_H

#include <qore/Qore.h>
#include <qore/QoreCondition.h>

//! a simple thread-safe counter object; objects can block on it until the counter reaches zero
class QoreCounter
{
   private:
      //! private implementation of the counter
      struct qore_counter_private *priv;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreCounter(const QoreCounter&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreCounter& operator=(const QoreCounter&);

   public:
      //! creates the counter and initializes the count
      DLLEXPORT QoreCounter(int nc = 0);

      //! destroys the object and frees all memory
      DLLEXPORT ~QoreCounter();

      //! throws an exception if there are any waiting threads and wakes them all up
      DLLEXPORT void destructor(class ExceptionSink *xsink);

      //! increments the counter
      DLLEXPORT void inc();

      //! decrements the counter and wakes up any threads if the counter reaches 0
      /** a Qore-language exception will be raised here if QoreCounter::destructor() has
	  already been run before calling this function.
	  @param xsink any Qore-language exception thrown will be added here
       */
      DLLEXPORT void dec(class ExceptionSink *xsink);

      //! blocks the calling thread until the counter reaches 0
      /** a Qore-language exception will be raised here if QoreCounter::destructor() is run 
	  while threads are still blocked
	  @param xsink any Qore-language exception thrown will be added here
	  @param timeout_ms indicates a timeout in milliseconds to wait, 0 means no timeout
	  @return non-zero means an exception was thrown
       */
      DLLEXPORT int waitForZero(class ExceptionSink *xsink, int timeout_ms = 0);

      //! returns the current count
      DLLEXPORT int getCount() const;

      //! returns the number of threads blocked on this object
      DLLEXPORT int getWaiting() const;

      // internal use only - for internal counters
      DLLLOCAL void waitForZero();
      DLLLOCAL void dec();
};

#endif
