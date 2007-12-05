/*
  modules/TIBCO/QoreTibrvSender.h

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

#ifndef _QORE_TIBCO_QORETIBRVSENDER_H

#define _QORE_TIBCO_QORETIBRVSENDER_H

#include <qore/Qore.h>

#include "QoreTibrvTransport.h"

class QoreTibrvSender : public AbstractPrivateData, public QoreTibrvTransport
{
   private:

   protected:
      virtual ~QoreTibrvSender() {}

   public:
      inline QoreTibrvSender(const char *desc, const char *service, const char *network, const char *daemon, class ExceptionSink *xsink) : QoreTibrvTransport(desc, service, network, daemon, xsink)
      { }

      void sendSubject(const char *subject, class QoreHash *data, const char *replySubject, class ExceptionSink *xsink);
      
      // timout in ms
      class QoreHash *sendSubjectWithSyncReply(const char *subject, class QoreHash *data, int64 timeout, class ExceptionSink *xsink);
};

#endif
