/*
  modules/TIBCO/QoreTibrvFtMember.cc

  TIBCO Rendezvous integration to QORE

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>

#include "QoreTibrvFtMember.h"

QoreTibrvFtMember::QoreTibrvFtMember(char *groupname, int weight, int activeGoal, int64 hb, int64 prep, int64 act,
				     char *service, char *network, char *daemon, char *desc,
				     class ExceptionSink *xsink) : QoreTibrvTransport(service, network, daemon, desc, xsink)
{
   callback = NULL;
   if (xsink->isException())
      return;

   TibrvStatus status = queue.create();
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "cannot create queue: %s", status.getText());
      return;
   }

   callback = new QoreTibrvFtMemberCallback();

   // convert integer millisecond values to floating-point seconds
   tibrv_f64 heartbeat   = (tibrv_f64)hb / 1000;
   tibrv_f64 preparation = (tibrv_f64)prep / 1000;
   tibrv_f64 activation  = (tibrv_f64)act / 1000;

   printd(0, "QoreTibrvFtMember::QoreTibrvFtMember() group=%s weight=%d activeGoal=%d heartbeat=%g preparation=%g activation=%g\n", groupname, weight, activeGoal, heartbeat, preparation, activation);

   status = ftMember.create(&queue, callback, &transport, groupname, (tibrv_u16)weight, (tibrv_u16)activeGoal, heartbeat, preparation, activation, NULL);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "cannot create fault tolerant member object: %s", status.getText());
      return;
   }
}
