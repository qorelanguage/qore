/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreHTTPClient.h

  Qore Programming Language

  Copyright (C) 2006 - 2014 QoreTechnologies
  
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

/* this file is deprecated; use QoreHttpClientObject instead
   this class is only kept for ABI compatibility
*/

#ifndef QORE_HTTP_CLIENT_H_
#define QORE_HTTP_CLIENT_H_

#include <qore/common.h>
#include <qore/AbstractPrivateData.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreSocket.h>
#include <qore/QoreHttpClientObject.h>

class Queue;

//! provides a way to communicate with HTTP servers using Qore data structures
/** thread-safe, uses QoreSocket for socket communication
*/
class QoreHTTPClient : public QoreHttpClientObject {
public:
   //! creates the QoreHTTPClient object
   DLLEXPORT QoreHTTPClient();

   //! destroys the object and frees all associated memory
   DLLEXPORT virtual ~QoreHTTPClient();

   DLLEXPORT void setDefaultPath(const char* pth);
   DLLEXPORT void setDefaultHeaderValue(const char* header, const char* val);
   DLLEXPORT void addProtocol(const char* prot, int port, bool ssl = false);

   DLLEXPORT int setOptions(const QoreHashNode* opts, ExceptionSink* xsink);
   DLLEXPORT const QoreEncoding* getEncoding() const;

   DLLEXPORT int connect(ExceptionSink* xsink);
   DLLEXPORT QoreHashNode* send(const char* meth, const char* path, const QoreHashNode* headers, const void* data, unsigned size, bool getbody, QoreHashNode* info, ExceptionSink* xsink);

   DLLEXPORT void setEventQueue(Queue* cbq, ExceptionSink* xsink);
};

#endif
