/*
  QoreHTTPClient.h

  Qore Programming Language

  Copyright (C) 2006, 2007 QoreTechnologies
  
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
#include <qore/AbstractPrivateData.h>
#include <qore/LockedObject.h>
#include <qore/QoreSocket.h>

#include <string>
#include <map>
#include <set>

// ssl-enabled protocols are stored as negative numbers, non-ssl as positive
#define make_protocol(a, b) ((a) * ((b) ? -1 : 1))
#define get_port(a) ((a) * (((a) < 0) ? -1 : 1))
#define get_ssl(a) ((a) * (((a) < 0) ? true : false))

#define HTTPCLIENT_DEFAULT_PORT 80
#define HTTPCLIENT_DEFAULT_HOST "localhost"

// set default timeout to 5 minutes (300,000 ms)
#define HTTPCLIENT_DEFAULT_TIMEOUT 300000

// case-insensitive map
class ltstrcase
{
  public:
   bool operator()(std::string s1, std::string s2) const
   {
      return strcasecmp(s1.c_str(), s2.c_str()) < 0;
   }
};

// protocol map class to recognize user-defined protocols (mostly useful for derived classes)
typedef std::map<std::string, int> prot_map_t;
typedef std::set<std::string> str_set_t;
typedef std::set<std::string, ltstrcase> strcase_set_t;

typedef std::map<std::string, std::string> header_map_t;

class SafeHash : public Hash
{
      // none of these operators/methods are implemented - here to make sure they are not used
      DLLLOCAL void *operator new(size_t); 
      DLLLOCAL SafeHash(bool i);
      DLLLOCAL void deleteAndDeref(class ExceptionSink *xsink);

   public:
      DLLLOCAL SafeHash()
      {
      }
      DLLLOCAL ~SafeHash()
      {
	 dereference(NULL);
      }
};

class QoreHTTPClient : public AbstractPrivateData, public LockedObject
{
   private:
      static str_set_t method_set;
      static strcase_set_t header_ignore;
      static header_map_t default_headers;
      static class SafeHash mandatory_headers;
   
      // are we using http 1.1 or 1.0?
      bool http11;
      prot_map_t prot_map;

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
      DLLLOCAL void disconnect_unlocked();
      DLLLOCAL class QoreNode *send_internal(char *meth, char *path, class Hash *headers, void *data, unsigned size, bool getbody, class ExceptionSink *xsink);
      DLLLOCAL void setSocketPath();

   protected:
      DLLLOCAL virtual ~QoreHTTPClient();
      
   public:
      DLLLOCAL static void static_init();

      DLLEXPORT QoreHTTPClient();
      // set options with a hash, returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int setOptions(Hash* opts, ExceptionSink* xsink);
      // useful for c++ derived classes
      DLLEXPORT void setDefaultPort(int prt);
      // useful for c++ derived classes
      DLLEXPORT void setDefaultPath(char *pth);

      DLLEXPORT void setTimeout(int to);
      DLLEXPORT int getTimeout() const;

      DLLEXPORT void setEncoding(class QoreEncoding *qe);
      DLLEXPORT class QoreEncoding *getEncoding() const;

      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int setHTTPVersion(char* version, ExceptionSink* xsink);
      DLLEXPORT const char* getHTTPVersion() const;
      DLLEXPORT void setHTTP11(bool h11);
      DLLEXPORT bool isHTTP11() const;

      DLLEXPORT void setSecure(bool is_secure);
      DLLEXPORT bool isSecure() const;

      DLLEXPORT long verifyPeerCertificate();
      DLLEXPORT const char* getSSLCipherName();
      DLLEXPORT const char* getSSLCipherVersion();
      
      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int connect(class ExceptionSink *xsink);
      DLLEXPORT void disconnect();

      DLLEXPORT class QoreNode *send(char *meth, char *path, class Hash *headers, void *data, unsigned size, bool getbody, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *get(char *path, class Hash *headers, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *head(char *path, class Hash *headers, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *post(char *path, class Hash *headers, void *data, unsigned size, class ExceptionSink *xsink);
};

#endif 

// EOF

