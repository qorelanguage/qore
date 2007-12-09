/*
  QoreHTTPClient.cc

  Qore Programming Language

  Copyright (C) 2006, 2007 Qore Technologies
  
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

/*
  RFC 2616 HTTP 1.1
  RFC 2617 HTTP authentication
  RFC 3986 HTTP URI specification
*/

#include <qore/Qore.h>
#include <qore/QoreURL.h>
#include <qore/QoreHTTPClient.h>
#include <qore/intern/ql_misc.h>
#include <qore/minitest.hpp>

#include <ctype.h>

#ifdef DEBUG_TESTS
#  include "tests/QoreHTTPClient_tests.cc"
#endif

DLLLOCAL ccharcase_set_t QoreHTTPClient::method_set;
DLLLOCAL strcase_set_t QoreHTTPClient::header_ignore;

struct qore_qtc_private {
      // are we using http 1.1 or 1.0?
      bool http11;
      prot_map_t prot_map;

      bool ssl, proxy_ssl;
      int port, proxy_port, default_port, max_redirects;
      std::string host, path, username, password;
      std::string proxy_host, proxy_path, proxy_username, proxy_password;
      std::string default_path;
      int timeout;
      std::string socketpath;
      bool connected;
      QoreSocket m_socket;
      header_map_t default_headers;

      DLLLOCAL qore_qtc_private() : http11(true), ssl(false), proxy_ssl(false),
				    port(HTTPCLIENT_DEFAULT_PORT), proxy_port(0),
				    default_port(HTTPCLIENT_DEFAULT_PORT), 
				    max_redirects(HTTPCLIENT_DEFAULT_MAX_REDIRECTS),
				    timeout(HTTPCLIENT_DEFAULT_TIMEOUT),
				    connected(false)
      {
	 // setup protocol map
	 prot_map["http"] = make_protocol(80, false);
	 prot_map["https"] = make_protocol(443, true);

	 // setup default headers
	 default_headers["Accept"] = "text/html";
	 default_headers["Content-Type"] = "text/html";
	 default_headers["Connection"] = "Keep-Alive";
	 default_headers["User-Agent"] = "Qore HTTP Client v" PACKAGE_VERSION;
	 default_headers["Accept-Encoding"] = "deflate,gzip";
      }
      DLLLOCAL ~qore_qtc_private()
      {
      }

      DLLLOCAL void setSocketPath()
      {
	 if (proxy_port)
	 {
	    socketpath = proxy_host;
	    socketpath += ":";
	    char buff[20];
	    sprintf(buff, "%d", proxy_port);
	    socketpath += buff;
	 }
	 else
	 {
	    socketpath = host;
	    socketpath += ":";
	    char buff[20];
	    sprintf(buff, "%d", port);
	    socketpath += buff;
	 }
      }

};

// static initialization
void QoreHTTPClient::static_init()
{
   // setup static members of QoreHTTPClient class
   method_set.insert("OPTIONS");
   method_set.insert("GET");
   method_set.insert("HEAD");
   method_set.insert("POST");
   method_set.insert("PUT");
   method_set.insert("DELETE");
   method_set.insert("TRACE");
   method_set.insert("CONNECT");
   
   header_ignore.insert("Content-Length");
}

QoreHTTPClient::QoreHTTPClient() : priv(new qore_qtc_private)
{
   setSocketPath();
}

QoreHTTPClient::~QoreHTTPClient()
{
   delete priv;
}

void QoreHTTPClient::setSocketPath()
{
   priv->setSocketPath();
}

void QoreHTTPClient::setDefaultPort(int def_port)
{
   priv->default_port = def_port;
}

void QoreHTTPClient::setDefaultPath(const char *def_path)
{
   priv->default_path = def_path;
}

void QoreHTTPClient::setTimeout(int to)
{
   priv->timeout = to;
}

int QoreHTTPClient::getTimeout() const
{
   return priv->timeout;
}

void QoreHTTPClient::setEncoding(class QoreEncoding *qe)
{
   priv->m_socket.setEncoding(qe);
}

class QoreEncoding *QoreHTTPClient::getEncoding() const
{
   return priv->m_socket.getEncoding();
}

int QoreHTTPClient::setOptions(QoreHash* opts, ExceptionSink* xsink)
{
   // process new protocols
   class QoreNode *n = opts->getKeyValue("protocols");  
   if (n && n->type == NT_HASH)
   {
      HashIterator hi(n->val.hash);
      while (hi.next())
      {
	 class QoreNode *v = hi.getValue();
	 if (!v || (v->type != NT_HASH && n->type != NT_INT))
	 {
	    xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "value of protocol hash key '%s' is not a hash or an int", hi.getKey());
	    return -1;
	 }
	 bool need_ssl = false;
	 int need_port;
	 if (v->type == NT_INT)
	    need_port = v->val.intval;
	 else
	 {
	    class QoreNode *p = v->val.hash->getKeyValue("port");
	    need_port = p ? p->getAsInt() : 0;
	    if (!need_port)
	    {
	       xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "'port' key in protocol hash key '%s' is missing or zero", hi.getKey());
	       return -1;
	    }
	    p = v->val.hash->getKeyValue("ssl");
	    need_ssl = p ? p->getAsBool() : false;
	 }
	 priv->prot_map[hi.getKey()] = make_protocol(need_port, need_ssl);
      }
   }

   n = opts->getKeyValue("max_redirects");
   if (n)
      priv->max_redirects = n->getAsInt();

   n = opts->getKeyValue("default_port");  
   if (n)
      priv->default_port = n->getAsInt();
   else
      priv->default_port = HTTPCLIENT_DEFAULT_PORT;

   // check if proxy is true
   n = opts->getKeyValue("proxy");  
   if (n && n->type == NT_STRING)
      if (set_proxy_url_unlocked(n->val.String->getBuffer(), xsink))
	 return -1;

   // parse url option if present
   n = opts->getKeyValue("url");  
   if (n && n->type == NT_STRING)
      if (set_url_unlocked(n->val.String->getBuffer(), xsink))
	 return -1;

   n = opts->getKeyValue("default_path");  
   if (n && n->type == NT_STRING)
      priv->default_path = n->val.String->getBuffer();

   // set default timeout if given in option hash - accept relative date/time values as well as integers
   n = opts->getKeyValue("timeout");  
   if (n)
      priv->timeout = getMsZeroInt(n);

   n = opts->getKeyValue("http_version");  
   if (n)
   {
      if (n->type == NT_STRING)
      {
	 if (setHTTPVersion(n->val.String->getBuffer(), xsink))
	    return -1;
      }
      else
      {
	 xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "expecting string version ('1.0', '1.1' as value for http_version key in options hash");
	 return -1;
      }
   }

   if (priv->path.empty())
      priv->path = priv->default_path.empty() ? "/" : priv->default_path;

   return 0;
}

int QoreHTTPClient::set_url_unlocked(const char *str, ExceptionSink* xsink)
{
   QoreURL url(str);
   if (!url.isValid()) {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "url parameter cannot be parsed");
      return -1;
   }

   bool port_set = false;
   if (url.getPort())
   {
      priv->port = url.getPort();
      port_set = true;
   }

   // host is always set if URL is valid
   priv->host = url.getHost()->getBuffer();

   // check if hostname is really a local port number (for a URL string like: "8080")
   if (!url.getPort()) {
      char* aux;
      int val = strtol(priv->host.c_str(), &aux, 10);
      if (aux != priv->host.c_str()) {
	 priv->host = HTTPCLIENT_DEFAULT_HOST;
	 priv->port = val;
	 port_set = true;
      }
   }
   
   class QoreString *tmp = url.getPath();
   priv->path = tmp ? tmp->getBuffer() : "";
   tmp = url.getUserName();
   priv->username = tmp ? tmp->getBuffer() : "";
   tmp = url.getPassword();
   priv->password = tmp ? tmp->getBuffer() : "";

   if (priv->username.empty() && !priv->password.empty())
   {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "invalid authorization credentials: password set without username");
      return -1;
   }

   if (!priv->username.empty() && priv->password.empty())
   {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "invalid authorization credentials: username set without password");
      return -1;
   }

   tmp = url.getProtocol();
   if (tmp) {
      prot_map_t::const_iterator i = priv->prot_map.find(tmp->getBuffer());
      if (i == priv->prot_map.end())
      {
	 xsink->raiseException("HTTP-CLIENT-UNKNOWN-PROTOCOL", "protocol '%s' is not supported.", tmp->getBuffer());
	 return -1;
      }

      // set port only if it wasn't overridden in the URL
      if (!port_set)
	 priv->port = get_port(i->second);

      // set SSL setting from protocol default
      priv->ssl = get_ssl(i->second);
   }
   else
   {
      priv->ssl = false;
      if (!port_set)
	 priv->port = priv->default_port;
   }

   if (!priv->proxy_port)
      setSocketPath();
   return 0;
}

int QoreHTTPClient::setURL(const char *str, ExceptionSink* xsink)
{
   SafeLocker sl(this);
   // disconnect immediately if not using a proxy
   if (!priv->proxy_port)
      disconnect_unlocked();
   return set_url_unlocked(str, xsink);
}

class QoreString *QoreHTTPClient::getURL()
{
   SafeLocker sl(this);
   class QoreString *pstr = new QoreString("http");
   if (priv->ssl)
      pstr->concat("s://");
   else
      pstr->concat("://");
   if (!priv->username.empty())
      pstr->sprintf("%s:%s@", priv->username.c_str(), priv->password.c_str());

   pstr->concat(priv->proxy_host.c_str());
   if (priv->port != 80)
      pstr->sprintf(":%d", priv->port);
   pstr->concat(priv->proxy_path.c_str());
   return pstr;
}

int QoreHTTPClient::setHTTPVersion(const char* version, ExceptionSink* xsink)
{
   int rc = 0;
   SafeLocker sl(this);
   if (!strcmp(version, "1.0"))
      priv->http11 = false;
   else if (!strcmp(version, "1.1"))
      priv->http11 = true;
   else
   {
      xsink->raiseException("HTTP-VERSION-ERROR", "only '1.0' and '1.1' are valid (value passed: '%s')", version);
      rc = -1;
   }
   return rc;
}

const char *QoreHTTPClient::getHTTPVersion() const
{
   return priv->http11 ? "1.1" : "1.0";
}

void QoreHTTPClient::setHTTP11(bool val)
{
   priv->http11 = val;
}

bool QoreHTTPClient::isHTTP11() const 
{ 
   return priv->http11;
}

int QoreHTTPClient::set_proxy_url_unlocked(const char *pstr, class ExceptionSink *xsink) 
{ 
   QoreURL url(pstr);
   if (!url.isValid()) {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "proxy URL '%s' cannot be parsed", pstr);
      return -1;
   }

   bool port_set = false;
   if (url.getPort())
   {
      priv->proxy_port = url.getPort();
      port_set = true;
   }

   // host is always set if valid
   priv->proxy_host = url.getHost()->getBuffer();

   // check if hostname is really a local port number (for a URL string like: "8080")
   if (!url.getPort()) {
      char* aux;
      int val = strtol(priv->host.c_str(), &aux, 10);
      if (aux != priv->host.c_str()) {
	 priv->proxy_host = HTTPCLIENT_DEFAULT_HOST;
	 priv->proxy_port = val;
	 port_set = true;
      }
   }
   
   class QoreString *tmp = url.getPath();
   priv->proxy_path = tmp ? tmp->getBuffer() : "";
   tmp = url.getUserName();
   priv->proxy_username = tmp ? tmp->getBuffer() : "";
   tmp = url.getPassword();
   priv->proxy_password = tmp ? tmp->getBuffer() : "";

   if (priv->proxy_username.empty() && !priv->proxy_password.empty())
   {
      xsink->raiseException("HTTP-CLIENT-SET-PROXY-ERROR", "invalid authorization credentials: password set without username");
      return -1;
   }

   if (!priv->proxy_username.empty() && priv->proxy_password.empty())
   {
      xsink->raiseException("HTTP-CLIENT-SET-PROXY-ERROR", "invalid authorization credentials: username set without password");
      return -1;
   }

   tmp = url.getProtocol();
   if (tmp) {
      if (strcasecmp(tmp->getBuffer(), "http") && strcasecmp(tmp->getBuffer(), "https"))
      {
	 xsink->raiseException("HTTP-CLIENT-PROXY-PROTOCOL-ERROR", "protocol '%s' is not supported for proxies, only 'http' and 'https'", tmp->getBuffer());
	 return -1;
      }

      prot_map_t::const_iterator i = priv->prot_map.find(tmp->getBuffer());
      assert(i != priv->prot_map.end());

      // set port only if it wasn't overridden in the URL
      if (!port_set)
	 priv->proxy_port = get_port(i->second);

      // set SSL setting from protocol default
      priv->proxy_ssl = get_ssl(i->second);
   }
   else
   {
      priv->proxy_ssl = false;
      if (!port_set)
	 priv->proxy_port = priv->default_port;
   }

   setSocketPath();
   return 0;
}

int QoreHTTPClient::setProxyURL(const char *proxy, class ExceptionSink *xsink) 
{
   SafeLocker sl(this);
   disconnect_unlocked();
   if (!proxy || !proxy[0])
   {
      clearProxyURL();
      return 0;
   }
   return set_proxy_url_unlocked(proxy, xsink);
}

class QoreString *QoreHTTPClient::getProxyURL() 
{
   SafeLocker sl(this);

   if (!priv->proxy_port)
      return NULL;

   class QoreString *pstr = new QoreString("http");
   if (priv->proxy_ssl)
      pstr->concat("s://");
   else
      pstr->concat("://");
   if (!priv->proxy_username.empty())
      pstr->sprintf("%s:%s@", priv->proxy_username.c_str(), priv->proxy_password.c_str());

   pstr->concat(priv->proxy_host.c_str());
   if (priv->proxy_port != 80)
      pstr->sprintf(":%d", priv->proxy_port);
   pstr->concat(priv->proxy_path.c_str());
   return pstr;
}

void QoreHTTPClient::clearProxyURL()
{
   SafeLocker sl(this);
   priv->proxy_port = 0;
   priv->proxy_username.clear();
   priv->proxy_password.clear();
   priv->proxy_host.clear();
   priv->proxy_path.clear();
   priv->proxy_ssl = false;
   setSocketPath();
}

void QoreHTTPClient::setSecure(bool is_secure) 
{ 
   lock();
   priv->ssl = is_secure; 
   unlock();
}

bool QoreHTTPClient::isSecure() const 
{ 
   return priv->ssl; 
}

void QoreHTTPClient::setProxySecure(bool is_secure) 
{ 
   lock();
   priv->proxy_ssl = is_secure; 
   unlock();
}

bool QoreHTTPClient::isProxySecure() const 
{ 
   return priv->proxy_ssl; 
}

long QoreHTTPClient::verifyPeerCertificate()
{ 
   SafeLocker sl(this);
   return priv->m_socket.verifyPeerCertificate(); 
}

const char* QoreHTTPClient::getSSLCipherName()
{ 
   SafeLocker sl(this);
   return priv->m_socket.getSSLCipherName(); 
}

const char* QoreHTTPClient::getSSLCipherVersion() 
{ 
   SafeLocker sl(this);
   return priv->m_socket.getSSLCipherVersion(); 
}

int QoreHTTPClient::connect_unlocked(class ExceptionSink *xsink)
{
   bool connect_ssl = priv->proxy_port ? priv->proxy_ssl : priv->ssl;

   int rc;
   if (connect_ssl)
      rc = priv->m_socket.connectSSL(priv->socketpath.c_str(), NULL, NULL, xsink);
   else
      rc = priv->m_socket.connect(priv->socketpath.c_str(), xsink);
   if (!rc)
      priv->connected = true;
   return rc;
}

int QoreHTTPClient::connect(class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return connect_unlocked(xsink);
}

void QoreHTTPClient::disconnect_unlocked()
{
   if (priv->connected)
   {
      priv->m_socket.close();
      priv->connected = false;
   }
}

void QoreHTTPClient::disconnect()
{
   SafeLocker sl(this);
   disconnect_unlocked();
}

const char *QoreHTTPClient::getMsgPath(const char *mpath, class QoreString &pstr)
{
   pstr.clear();

   // use default path if no path is set
   if (!mpath || !mpath[0])
      mpath = priv->path.empty() ? (priv->default_path.empty() ? "/" : (const char *)priv->default_path.c_str()) : (const char *)priv->path.c_str();

   if (priv->proxy_port)
   {
      // create URL string for path for proxy
      pstr.concat("http");
      if (priv->ssl)
	 pstr.concat("s://");
      else
	 pstr.concat("://");
      pstr.concat(priv->host.c_str());
      if (priv->port != 80)
	 pstr.sprintf(":%d", priv->port);
      if (mpath[0] != '/')
	 pstr.concat('/');
   }
   // concat mpath to pstr, performing URL encoding
   const char *p = mpath;
   while (*p) {
      // encode spaces only
      if (*p == ' ')
	 pstr.concat("%20");
      // according to RFC 3896 it'S not necessary to encode non-ascii characters
      else
	 pstr.concat(*p);
      ++p;
   }
   return (const char *)pstr.getBuffer();
}

class QoreNode *QoreHTTPClient::getResponseHeader(const char *meth, const char *mpath, class QoreHash &nh, const void *data, unsigned size, int &code, class ExceptionSink *xsink)
{
   class QoreString pathstr(priv->m_socket.getEncoding());
   const char *msgpath = getMsgPath(mpath, pathstr);

   if (connect_unlocked(xsink))
      return NULL;

   // send the message
   int rc = priv->m_socket.sendHTTPMessage(meth, msgpath, priv->http11 ? "1.1" : "1.0", &nh, data, size);

   if (rc)
   {
      if (rc == -2)
	 xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "socket was closed at the remote end before the message could be sent");
      else 
	 xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));
      return NULL;
   }

   class QoreNode *ans;
   while (true)
   {
      ans = priv->m_socket.readHTTPHeader(priv->timeout, &rc);

      if (!ans || ans->type != NT_HASH)
      {
	 if (ans)
	 {
	    ans->deref(xsink);
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "malformed HTTP header received from socket %s, could not parse header", priv->socketpath.c_str());
	 }
	 else
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket %s closed on remote end without a response", priv->socketpath.c_str());
	 return NULL;
      }

      if (rc <= 0)
      {
	 if (!rc)             // remote end has closed the connection
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "remote end has closed the connection");
	 else if (rc == -1)   // recv() error
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", strerror(errno));
	 else if (rc == -2)
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket was closed at the remote end");
	 else if (rc == -3)   // timeout
	    xsink->raiseException("HTTP-CLIENT-TIMEOUT", "timed out waiting %dms for response on socket %s", priv->timeout, priv->socketpath.c_str());
	 else
	    assert(false);

	 ans->deref(xsink);
	 return NULL;
      }

      // check HTTP status code
      class QoreNode *v = ans->val.hash->getKeyValue("status_code");
      if (!v)
      {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "no HTTP status code received in response");
	 ans->deref(xsink);
	 return NULL;
      }
   
      code = v->getAsInt();
      // continue processing if "100 Continue" response received (ignore this response)
      if (code == 100)
	 continue;
      break;
   }

   return ans;
}

// always generate a Host header pointing to the host hosting the resource, not the proxy
// (RFC 2616 is not totally clear on this, but other clients do it this way)
class QoreNode *QoreHTTPClient::getHostHeaderValue()
{
   QoreNode *hv;

   if (priv->port == 80)
      hv = new QoreNode(priv->host.c_str());
   else
   {
      QoreString *str = new QoreString();
      str->sprintf("%s:%d", priv->host.c_str(), priv->port);
      hv = new QoreNode(str);
   }
   return hv;
}

class QoreNode *QoreHTTPClient::send_internal(const char *meth, const char *mpath, class QoreHash *headers, const void *data, unsigned size, bool getbody, class ExceptionSink *xsink)
{
   // check if method is valid
   ccharcase_set_t::const_iterator i = method_set.find(meth);
   if (i == method_set.end())
   {
      xsink->raiseException("HTTP-CLIENT-METHOD-ERROR", "HTTP method (%n) not recognized.", meth);
      return NULL;
   }
   // make sure the capitalized version is used
   meth = *i;

   SafeLocker sl(this);
   class StackHash nh(xsink);
   bool keep_alive = true;

   if (headers)
   {
      HashIterator hi(headers);
      while (hi.next())
      {
	 if (!strcasecmp(hi.getKey(), "connection") || (priv->proxy_port && !strcasecmp(hi.getKey(), "proxy-connection")))
	 {
	    class QoreNode *v = hi.getValue();
	    if (v && v->type == NT_STRING && !strcasecmp(v->val.String->getBuffer(), "close"))
	       keep_alive = false;
	 }

	 // if one of the mandatory headers is found, then ignore it
	 strcase_set_t::iterator i = header_ignore.find(hi.getKey());
	 if (i != header_ignore.end())
	    continue;

	 // otherwise set the value in the hash
	 class QoreNode *n = hi.getValue();
	 if (!is_nothing(n))
	    nh.setKeyValue(hi.getKey(), n->RefSelf(), xsink);
      }
   }

   // add default headers if they weren't overridden
   for (header_map_t::const_iterator i = priv->default_headers.begin(), e = priv->default_headers.end(); i != e; ++i)
   {
      // look in original headers to see if the key was already given
      if (headers)
      {
	 bool skip = false;
	 HashIterator hi(headers);
	 while (hi.next())
	 {
	    if (!strcasecmp(hi.getKey(), i->first.c_str()))
	    {
	       skip = true;
	       break;
	    }
	 }
	 if (skip)
	    continue;
      }
      // if there is no message body then do not send the "content-type" header
      if (!data && !strcmp(i->first.c_str(), "Content-Type"))
	 continue;
      nh.setKeyValue(i->first.c_str(), new QoreNode(i->second.c_str()), xsink);
   }
   if (!priv->username.empty())
   {
      // check for "Authorization" header
      bool auth_found = false;
      if (headers)
      {
	 HashIterator hi(headers);
	 while (hi.next())
	 {
	    if (!strcasecmp(hi.getKey(), "Authorization"))
	    {
	       auth_found = true;
	       break;
	    }
	 }
      }
      if (!auth_found)
      {
	 class QoreString tmp;
	 tmp.sprintf("%s:%s", priv->username.c_str(), priv->password.c_str());
	 class QoreString *auth_str = new QoreString("Basic ");
	 auth_str->concatBase64(&tmp);
	 nh.setKeyValue("Authorization", new QoreNode(auth_str), xsink);
      }
   }
   if (priv->proxy_port && !priv->proxy_username.empty())
   {
      // check for "Proxy-Authorization" header
      bool auth_found = false;
      if (headers)
      {
	 HashIterator hi(headers);
	 while (hi.next())
	 {
	    if (!strcasecmp(hi.getKey(), "Proxy-Authorization"))
	    {
	       auth_found = true;
	       break;
	    }
	 }
      }
      if (!auth_found)
      {
	 class QoreString tmp;
	 tmp.sprintf("%s:%s", priv->proxy_username.c_str(), priv->proxy_password.c_str());
	 class QoreString *auth_str = new QoreString("Basic ");
	 auth_str->concatBase64(&tmp);
	 nh.setKeyValue("Proxy-Authorization", new QoreNode(auth_str), xsink);
      }
   }

   bool host_override = headers ? (bool)headers->getKeyValue("Host") : false;

   int code;
   class QoreNode *ans;
   class QoreHash *ah;
   class QoreNode *v;
   int redirect_count = 0;
   class StackHash redirect_hash(xsink);
   while (true)
   {
      // set host field automatically if not overridden
      if (!host_override)
	 nh.setKeyValue("Host", getHostHeaderValue(), xsink);

      ans = getResponseHeader(meth, mpath, nh, data, size, code, xsink);
      if (!ans)
	 return NULL;
      ah = ans->val.hash;

      if (code >= 300 && code < 400)
      {
	 if (++redirect_count > priv->max_redirects)
	    break;
	 disconnect_unlocked();

	 host_override = false;
	 v = ah->getKeyValue("location");
	 const char *location = v ? v->val.String->getBuffer() : NULL;

	 if (!location)
	 {
	    sl.unlock();
	    v = ah->getKeyValue("status_message");
	    const char *mess = v ? v->val.String->getBuffer() : "<no message>";
	    xsink->raiseException("HTTP-CLIENT-REDIRECT-ERROR", "no redirect location given for status code %d: message: '%s'", code, mess);
	    ans->deref(xsink);
	    return NULL;
	 }

	 QoreString tmp;
	 tmp.sprintf("qore-redirect-stack-%d", redirect_count);
	 redirect_hash.setKeyValue(tmp.getBuffer(), v->RefSelf(), xsink);

	 if (set_url_unlocked(location, xsink))
	 {
	    sl.unlock();
	    v = ah->getKeyValue("status_message");
	    const char *mess = v ? v->val.String->getBuffer() : "<no message>";
	    xsink->raiseException("HTTP-CLIENT-REDIRECT-ERROR", "exception occurred while setting URL for new location '%s' (code %d: message: '%s')", location, code, mess);
	    ans->deref(xsink);
	    return NULL;
	 }
	 // set mpath to NULL so that the new path will be taken
	 mpath = NULL;
	 ans->deref(xsink);
	 continue;
      }
      break;
   }

   if (code >= 300 && code < 400)
   {
      sl.unlock();
      v = ah->getKeyValue("status_message");
      const char *mess = v ? v->val.String->getBuffer() : "<no message>";
      v = ah->getKeyValue("location");
      const char *location = v ? v->val.String->getBuffer() : "<no location>";
      xsink->raiseException("HTTP-CLIENT-MAXIMUM-REDIRECTS-EXCEEDED", "maximum redirections (%d) exceeded; redirect code %d to '%s' ignored (message: '%s')", priv->max_redirects, code, location, mess);
      ans->deref(xsink);
      return NULL;
   }

   if (code < 200 || code >= 300)
   {
      sl.unlock();
      v = ah->getKeyValue("status_message");
      const char *mess = v ? v->val.String->getBuffer() :"<no message>";
      xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "HTTP status code %d received: message: %s", code, mess);
      ans->deref(xsink);
      return NULL;
   }

   // process content-type
   v = ah->getKeyValue("content-type");
   // see if there is a character set specification in the content-type header
   if (v)
   {
      const char *str = v->val.String->getBuffer();
      const char *p = strstr(str, "charset=");
      if (p && (p == str || *(p - 1) == ';' || *(p - 1) == ' '))
      {
	 // move p to start of encoding
	 const char *c = p + 8;
	 QoreString enc;
	 while (*c && *c != ';' && *c != ' ')
	    enc.concat(*(c++));
	 // set new encoding
	 priv->m_socket.setEncoding(QEM.findCreate(&enc));
	 // strip from content-type
	 class QoreString *nc = new QoreString();
	 // skip any spaces before the charset=
	 while (p != str && (*(p - 1) == ' ' || *(p - 1) == ';'))
	    p--;
	 if (p != str)
	    nc->concat(str, p - str);
	 if (*c)
	    nc->concat(*c);
	 ah->setKeyValue("content-type", new QoreNode(nc), xsink);
	 str = nc->getBuffer();
      }
      // split into a list if ";" characters are present
      p = strchr(str, ';');
      if (p)
      {
	 class QoreList *l = new QoreList();
	 do {
	    l->push(new QoreNode(new QoreString(str, p - str, priv->m_socket.getEncoding())));
	    str = p + 1;
	 } while ((p = strchr(str, ';')));
	 // add last field
	 if (*str)
	    l->push(new QoreNode(new QoreString(str, priv->m_socket.getEncoding())));
	 ah->setKeyValue("content-type", new QoreNode(l), xsink);
      }
   }

   // see if we should do a binary or string read
   const char *content_encoding = NULL;
   v = ah->getKeyValue("content-encoding");
   if (v)
   {
      content_encoding = v->val.String->getBuffer();
      // check for misuse of this field by including a character encoding value
      if (!strncasecmp(content_encoding, "iso", 3) || !strncasecmp(content_encoding, "utf-", 4))
      {
	 priv->m_socket.setEncoding(QEM.findCreate(content_encoding));
	 content_encoding = NULL;
      }
   }

   class QoreNode *te = ah->getKeyValue("transfer-encoding");
   
   // get response body, if any
   v = ah->getKeyValue("content-length");
   int len = v ? v->getAsInt() : 0;

   class QoreNode *body = NULL;   
   if (te && !strcasecmp(te->val.String->getBuffer(), "chunked")) // check for chunked response body
   {
      class QoreHash *nah;
      if (content_encoding)
	 nah = priv->m_socket.readHTTPChunkedBodyBinary(priv->timeout, xsink);
      else
	 nah = priv->m_socket.readHTTPChunkedBody(priv->timeout, xsink);
      if (!nah)
      {
	 ans->deref(xsink);
	 return NULL;
      }
      
      body = nah->takeKeyValue("body");
      ah->assimilate(nah, xsink);
   }
   else if (getbody || (len && strcmp(meth, "HEAD")))
   {
      int rc;

      if (content_encoding)
      {
	 class BinaryObject *bobj = priv->m_socket.recvBinary(len, priv->timeout, &rc);
	 if (rc > 0 && bobj)
	    body = new QoreNode(bobj);
      }
      else
      {
	 class QoreString *bstr = priv->m_socket.recv(len, priv->timeout, &rc);
	 if (rc > 0 && bstr)
	    body = new QoreNode(bstr);
      }

      //printf("body=%08p\n", body);
      if (rc <= 0)
      {
	 sl.unlock();
	 if (!rc)             // remote end has closed the connection
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "remote end closed the connection while receiving response message body");
	 else if (rc == -1)   // recv() error
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", strerror(errno));
	 else if (rc == -2)
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket was closed at the remote end while receiving response message body");
	 else if (rc == -3)   // timeout
	    xsink->raiseException("HTTP-CLIENT-TIMEOUT", "timed out waiting %dms for response message body of length %d on socket %s", priv->timeout, len, priv->socketpath.c_str());
	 else
	    assert(false);

	 ans->deref(xsink);
	 return NULL;
      }
   }

   // check for connection: close header
   if (!keep_alive || ((v = ah->getKeyValue("connection")) && !strcasecmp(v->val.String->getBuffer(), "close")))
      disconnect_unlocked();

   sl.unlock();

   // for content-encoding processing we can run unlocked

   // add body to result hash and process content encoding if necessary
   if (body && content_encoding)
   {
      class BinaryObject *bobj = body->val.bin;
      class QoreString *str = NULL;
      if (!strcasecmp(content_encoding, "deflate") || !strcasecmp(content_encoding, "x-deflate"))
	 str = qore_inflate_to_string(bobj, priv->m_socket.getEncoding(), xsink);
      else if (!strcasecmp(content_encoding, "gzip") || !strcasecmp(content_encoding, "x-gzip"))
	 str = qore_gunzip_to_string(bobj, priv->m_socket.getEncoding(), xsink);
      else
      {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "don't know how to handle content-encoding '%s'", content_encoding);
	 ans->deref(xsink);
	 ans = NULL;
      }
      body->deref(xsink);
      if (str)
	 body = new QoreNode(str);
      else
	 body = NULL;	    
   }
   
   // merge redirect hash keys
   if (redirect_hash.size())
   {
      HashIterator hi(&redirect_hash);
      while (hi.next())
      {
	 ah->setKeyValue(hi.getKey(), hi.getValue(), xsink);
	 hi.takeValueAndDelete();
      }
   }
   
   if (body)
      ah->setKeyValue("body", body, xsink);

   return ans;
}

class QoreNode *QoreHTTPClient::send(const char *meth, const char *new_path, class QoreHash *headers, const void *data, unsigned size, bool getbody, class ExceptionSink *xsink)
{
   return send_internal(meth, new_path, headers, data, size, getbody, xsink);
}

class QoreNode *QoreHTTPClient::get(const char *new_path, class QoreHash *headers, class ExceptionSink *xsink)
{
   class QoreNode *ans = send_internal("GET", new_path, headers, NULL, 0, true, xsink);
   if (!ans)
      return NULL;
   class QoreNode *rv = ans->val.hash->takeKeyValue("body");
   ans->deref(xsink);
   return rv;
}

class QoreNode *QoreHTTPClient::head(const char *new_path, class QoreHash *headers, class ExceptionSink *xsink)
{
   return send_internal("HEAD", new_path, headers, NULL, 0, false, xsink);
}

class QoreNode *QoreHTTPClient::post(const char *new_path, class QoreHash *headers, const void *data, unsigned size, class ExceptionSink *xsink)
{
   class QoreNode *ans = send_internal("POST", new_path, headers, data, size, true, xsink);
   if (!ans)
      return NULL;
   // check for 2** return code, if not throw an exception
   
   class QoreNode *rv = ans->val.hash->takeKeyValue("body");
   ans->deref(xsink);
   return rv;
}

void QoreHTTPClient::addProtocol(const char *prot, int new_port, bool new_ssl)
{
   priv->prot_map[prot] = make_protocol(new_port, new_ssl);
}

void QoreHTTPClient::setMaxRedirects(int max)
{
   priv->max_redirects = max;
}

int QoreHTTPClient::getMaxRedirects() const
{
   return priv->max_redirects;
}

void QoreHTTPClient::setDefaultHeaderValue(const char *header, const char *val)
{
   priv->default_headers[header] = val;
}

// EOF
