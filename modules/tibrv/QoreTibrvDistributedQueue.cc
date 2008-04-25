/*
  modules/TIBCO/QoreTibrvDistributedQueue.cc

  TIBCO Rendezvous integration to QORE

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

#include "QoreTibrvDistributedQueue.h"

QoreTibrvDistributedQueue::QoreTibrvDistributedQueue(const char *cmName, unsigned workerWeight, unsigned workerTasks, 
						     unsigned short schedulerWeight, int64 schedulerHeartbeat, int64 schedulerActivation,
						     const char *desc, const char *service, const char *network, const char *daemon, 
						     class ExceptionSink *xsink) : QoreTibrvTransport(desc, service, network, daemon, xsink)
{
   if (xsink->isException())
      return;

   // convert integer millisecond values to floating-point seconds
   tibrv_f64 schedHeartbeat  = (tibrv_f64)schedulerHeartbeat / 1000;
   tibrv_f64 schedActivation = (tibrv_f64)schedulerActivation / 1000;

   TibrvStatus status = cmQueueTransport.create(&transport, cmName, workerWeight, workerTasks, schedulerWeight, schedHeartbeat, schedActivation);
   if (status != TIBRV_OK)
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "cannot create distributed queue transport object: %s", status.getText());
}
