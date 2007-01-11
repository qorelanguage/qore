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
#include <map>

// ssl-enabled protocols are stored as negative numbers, non-ssl as positive
#define make_protocol(a, b) ((a) * ((b) ? -1 : 1))
#define get_port(a) ((a) * (((a) < 0) ? -1 : 1))
#define get_ssl(a) ((a) * (((a) < 0) ? true : false))

#define HTTPCLIENT_DEFAULT_PORT 80
#define HTTPCLIENT_DEFAULT_HOST "localhost"

// set default timeout to 5 minutes (300,000 ms)
#define HTTPCLIENT_DEFAULT_TIMEOUT 300000

// protocol map class to recognize user-defined protocols (mostly useful for derived classes)
typedef std::map<std::string, int> prot_map_t;

class QoreHTTPClient
{
   private:
      // are we using http 1.1 or 1.0?
      bool http11;
      prot_map_t prot_map;

      LockedObject lock;
      bool ssl;
      int port, default_port;
      std::string host;
      std::string path;
      std::string username;
      std::string password;
      std::string default_path;
      int timeout;
      std::string socketpath;
      bool connected;
      QoreSocket m_socket;
      
      // returns -1 if an exception was thrown, 0 for OK
      DLLLOCAL int process_url(class QoreString *str, ExceptionSink* xsink);
      // returns -1 if an exception was thrown, 0 for OK
      DLLLOCAL int connect_unlocked(class ExceptionSink *xsink);
      
   public:
      DLLEXPORT QoreHTTPClient();
      DLLEXPORT ~QoreHTTPClient();
      // set options with a hash
      DLLEXPORT setOptions(Hash* opts, ExceptionSink* xsink);
      // useful for c++ derived classes
      DLLEXPORT setDefaultPort(int prt);
      // useful for c++ derived classes
      DLLEXPORT setDefaultPath(char *pth);

      

      DLLEXPORT void setTimeout(int to);
      DLLEXOPRT int getTimeout();

      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int setHTTPVersion(char* version, ExceptionSink* xsink);
      DLLEXPORT const char* getHTTPVersion() const;
      
      DLLEXPORT void setSecure(bool is_secure);
      DLLEXPORT bool isSecure() const;

      DLLEXPORT long verifyPeerCertificate();
      DLLEXPORT const char* getSSLCipherName();
      DLLEXPORT const char* getSSLCipherVersion();
};

#endif 

// EOF

