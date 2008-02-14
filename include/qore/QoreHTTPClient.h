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

#include <qore/common.h>
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

// set default maximum number of redirections
#define HTTPCLIENT_DEFAULT_MAX_REDIRECTS 5

// protocol map class to recognize user-defined protocols (mostly useful for derived classes)
typedef std::map<std::string, int> prot_map_t;
typedef std::set<const char *, ltcstrcase> ccharcase_set_t;
typedef std::set<std::string, ltstrcase> strcase_set_t;
typedef std::map<std::string, std::string> header_map_t;

class QoreHTTPClient : public AbstractPrivateData, public LockedObject
{
   private:
      DLLLOCAL static ccharcase_set_t method_set;
      DLLLOCAL static strcase_set_t header_ignore;

      struct qore_qtc_private *priv;
      
      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int set_url_unlocked(const char *url, class ExceptionSink *xsink);
      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int set_proxy_url_unlocked(const char *url, class ExceptionSink *xsink);
      // returns -1 if an exception was thrown, 0 for OK
      DLLLOCAL int connect_unlocked(class ExceptionSink *xsink);
      DLLLOCAL void disconnect_unlocked();
      DLLLOCAL class QoreHashNode *send_internal(const char *meth, const char *mpath, const class QoreHashNode *headers, const void *data, unsigned size, bool getbody, class ExceptionSink *xsink);
      DLLLOCAL void setSocketPath();
      DLLLOCAL const char *getMsgPath(const char *mpath, class QoreString &pstr);
      DLLLOCAL class QoreHashNode *getResponseHeader(const char *meth, const char *mpath, class QoreHashNode &nh, const void *data, unsigned size, int &code, class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *getHostHeaderValue();

      // not implemented
      DLLLOCAL QoreHTTPClient(const QoreHTTPClient&);
      DLLLOCAL QoreHTTPClient& operator=(const QoreHTTPClient&);

   public:
      DLLLOCAL static void static_init();

      DLLEXPORT QoreHTTPClient();
      DLLEXPORT virtual ~QoreHTTPClient();

      // set options with a hash, returns -1 if an exception was thrown, 0 for OK
      // NOTE: this function is unlocked and designed only to be called with the constructor
      DLLEXPORT int setOptions(const class QoreHashNode *opts, ExceptionSink* xsink);
      // useful for c++ derived classes
      DLLEXPORT void setDefaultPort(int prt);
      // useful for c++ derived classes
      DLLEXPORT void setDefaultPath(const char *pth);
      // useful for c++ derived classes
      DLLEXPORT void addProtocol(const char *prot, int port, bool ssl = false);

      DLLEXPORT void setTimeout(int to);
      DLLEXPORT int getTimeout() const;

      DLLEXPORT void setEncoding(const class QoreEncoding *qe);
      DLLEXPORT const class QoreEncoding *getEncoding() const;

      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int setHTTPVersion(const char* version, ExceptionSink* xsink);
      DLLEXPORT const char* getHTTPVersion() const;
      DLLEXPORT void setHTTP11(bool h11);
      DLLEXPORT bool isHTTP11() const;

      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int setURL(const char *url, class ExceptionSink *xsink);
      // caller owns the QoreString returned
      DLLEXPORT class QoreStringNode *getURL();

      DLLEXPORT int setProxyURL(const char *proxy, class ExceptionSink *xsink);
      // caller owns the QoreString returned
      DLLEXPORT class QoreStringNode *getProxyURL();
      DLLEXPORT void clearProxyURL();

      DLLEXPORT void setSecure(bool is_secure);
      DLLEXPORT bool isSecure() const;

      DLLEXPORT void setProxySecure(bool is_secure);
      DLLEXPORT bool isProxySecure() const;

      DLLEXPORT void setMaxRedirects(int max);
      DLLEXPORT int getMaxRedirects() const;

      DLLEXPORT long verifyPeerCertificate();
      DLLEXPORT const char *getSSLCipherName();
      DLLEXPORT const char *getSSLCipherVersion();
      
      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int connect(class ExceptionSink *xsink);
      DLLEXPORT void disconnect();

      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT class QoreHashNode *send(const char *meth, const char *path, const class QoreHashNode *headers, const void *data, unsigned size, bool getbody, class ExceptionSink *xsink);
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT class AbstractQoreNode *get(const char *path, const class QoreHashNode *headers, class ExceptionSink *xsink);
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT class QoreHashNode *head(const char *path, const class QoreHashNode *headers, class ExceptionSink *xsink);
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT class AbstractQoreNode *post(const char *path, const class QoreHashNode *headers, const void *data, unsigned size, class ExceptionSink *xsink);
      DLLEXPORT void setDefaultHeaderValue(const char *header, const char *val);
};

#endif 

// EOF

