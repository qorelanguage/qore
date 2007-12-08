/*
  QoreCounter.h

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

#ifndef _QORE_QORECOUNTER_H

#define _QORE_QORECOUNTER_H 1

#include <qore/Qore.h>
#include <qore/QoreCondition.h>

class QoreCounter
{
   private:
      struct qore_counter_private *priv;

   public:
      DLLEXPORT QoreCounter(int nc = 0);
      DLLEXPORT ~QoreCounter();
      DLLEXPORT void destructor(class ExceptionSink *xsink);
      DLLEXPORT void inc();
      DLLEXPORT void dec(class ExceptionSink *xsink);
      DLLEXPORT int waitForZero(class ExceptionSink *xsink, int timeout_ms = 0);
      DLLEXPORT int getCount() const;
      DLLEXPORT int getWaiting() const;

      // internal use only - for internal counters
      DLLLOCAL void waitForZero();
      DLLLOCAL void dec();
};

#endif
