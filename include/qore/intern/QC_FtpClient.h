/*
  QC_FtpClient.h

  QoreFtpClient class

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_CLASS_FTPCLIENT_H

#define _QORE_CLASS_FTPCLIENT_H

#include <qore/AbstractPrivateData.h>
#include <qore/QoreFtpClient.h>

DLLEXPORT extern qore_classid_t CID_FTPCLIENT;

DLLLOCAL class QoreClass *initFtpClientClass();

class QoreFtpClientClass : public AbstractPrivateData, public QoreFtpClient
{
   protected:
      DLLLOCAL virtual ~QoreFtpClientClass() {}

   public:
      DLLLOCAL inline QoreFtpClientClass(const QoreString *url, ExceptionSink *xsink) : QoreFtpClient(url, xsink) {}

      DLLLOCAL virtual void deref(ExceptionSink *xsink) {
	  if (ROdereference()) {
	      cleanup(xsink);
	      delete this;
	  }
      }
};

#endif // _QORE_CLASS_FTPCLIENT_H
