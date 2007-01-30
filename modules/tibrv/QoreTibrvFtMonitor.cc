/*
  modules/TIBCO/QoreTibrvFtMonitor.cc

  TIBCO Rendezvous integration to QORE

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

#include <qore/Qore.h>

#include "QoreTibrvFtMonitor.h"

QoreTibrvFtMonitor::QoreTibrvFtMonitor(char *groupname, int64 li, char *desc, char *service, char *network, char *daemon, class ExceptionSink *xsink) : QoreTibrvTransport(desc, service, network, daemon, xsink)
{
   callback = NULL;
   if (xsink->isException())
      return;

   TibrvStatus status = queue.create();
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVFTMONITOR-CONSTRUCTOR-ERROR", "cannot create queue: %s", status.getText());
      return;
   }

   callback = new QoreTibrvFtMonitorCallback();

   // convert integer millisecond values to floating-point seconds
   tibrv_f64 lostInterval = (tibrv_f64)li / 1000;

   //printd(5, "QoreTibrvFtMonitor::QoreTibrvFtMonitor() group=%s lostInterval=%g\n", groupname, lostInterval);

   status = ftMonitor.create(&queue, callback, &transport, groupname, lostInterval, NULL);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVFTMONITOR-CONSTRUCTOR-ERROR", "cannot create fault tolerant monitor object: %s", status.getText());
      return;
   }
}
