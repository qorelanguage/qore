/*
  QoreHTTPClient.h

  Qore Programming Language

  Copyright (C) 2006 QoreTechnologies
  
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

#ifndef QORE_HTTP_FILE_H_
#define QORE_HTTP_FILE_H_

#include <qore/config.h>
#include <qore/ReferenceObject.h>
#include <qore/LockedObject.h>
#include <qore/QoreSocket.h>

#include <string>

class QoreHTTPClient : public ReferenceObject
{
private:
  Hash* protocols;
  LockedObject lock;
  bool ssl;
  int port;
  std::string host;
  std::string path;
  std::string username;
  std::string password;
  std::string default_path;
  int timeout;
  std::string http_version;
  std::string socketpath;
  bool connected;
  QoreSocket m_socket;

  DLLLOCAL void process_url(Hash* opts, ExceptionSink* xsink);

public:
  DLLLOCAL static Hash* get_DEFAULT_PROTOCOLS();
  DLLLOCAL static List* get_ALLOWED_VERSIONS();
  static const int defaultTimeout = 300000;
  static const char* defaultHTTPVersion;

public:
  DLLLOCAL QoreHTTPClient(Hash* opts, ExceptionSink* xsink);
  DLLLOCAL ~QoreHTTPClient();

  DLLLOCAL void deref() {
    if (ROdereference()) {
      delete this;
    }
  }

  DLLLOCAL void setHTTPVersion(char* version, ExceptionSink* xsink);
  DLLLOCAL const char* getHTTPVersion() const { return http_version.c_str(); }

  DLLLOCAL void setSecure(bool is_secure) { ssl = is_secure; }
  DLLLOCAL bool isSecure() const { return ssl; }

  DLLLOCAL long verifyPeerCertificate() { return m_socket.verifyPeerCertificate(); }
  DLLLOCAL const char* getSSLCipherName() { return m_socket.getSSLCipherName(); }
  DLLLOCAL const char* getSSLCipherVersion() { return m_socket.getSSLCipherVersion(); }
};

#endif 

// EOF

