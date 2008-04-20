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

#include <string>
#include <map>
#include <set>

#include <ctype.h>

// ssl-enabled protocols are stored as negative numbers, non-ssl as positive
#define make_protocol(a, b) ((a) * ((b) ? -1 : 1))
#define get_port(a) ((a) * (((a) < 0) ? -1 : 1))
#define get_ssl(a) ((a) * (((a) < 0) ? true : false))

#ifdef DEBUG_TESTS
#  include "tests/QoreHTTPClient_tests.cc"
#endif

// protocol map class to recognize user-defined protocols (mostly useful for derived classes)
typedef std::map<std::string, int> prot_map_t;
typedef std::set<const char *, ltcstrcase> ccharcase_set_t;
typedef std::set<std::string, ltstrcase> strcase_set_t;
typedef std::map<std::string, std::string> header_map_t;

static ccharcase_set_t method_set;
static strcase_set_t header_ignore;

//! used for having a QoreHashNode on the stack
/** this is not safe because the object could be misused (i.e. refSelf() called and reference used elsewhere)
    therefore it's a private object just implemented in this file
 */
class StackHash : public QoreHashNode
{
   private:
      class ExceptionSink *xsink;
   
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t); 

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL StackHash(const StackHash&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL StackHash& operator=(const StackHash&);
   
   public:
      //! creates the hash on the stack, "xs" will be used when the object is deleted
      DLLLOCAL StackHash(class ExceptionSink *xs)
      {
	 xsink = xs;
      }

      //! dereferences the members of the hash and destroys the object
      DLLLOCAL ~StackHash()
      {
	 derefImpl(xsink);
      }
};

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

void QoreHTTPClient::setEncoding(const QoreEncoding *qe)
{
   priv->m_socket.setEncoding(qe);
}

const QoreEncoding *QoreHTTPClient::getEncoding() const
{
   return priv->m_socket.getEncoding();
}

int QoreHTTPClient::setOptions(const QoreHashNode* opts, ExceptionSink* xsink)
{
   // process new protocols
   const AbstractQoreNode *n = opts->getKeyValue("protocols");  
   {
      if (n && n->getType() == NT_HASH) {
	 const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(n);
	 ConstHashIterator hi(h);
	 while (hi.next())
	 {
	    const AbstractQoreNode *v = hi.getValue();
	    qore_type_t vtype = v ? v->getType() : 0;
	    if (!v || (vtype != NT_HASH && vtype != NT_INT))
	    {
	       xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "value of protocol hash key '%s' is not a hash or an int", hi.getKey());
	       return -1;
	    }
	    bool need_ssl = false;
	    int need_port;
	    if (vtype == NT_INT)
	       need_port = (reinterpret_cast<const QoreBigIntNode *>(v))->val;
	    else
	    {
	       const QoreHashNode *vh = reinterpret_cast<const QoreHashNode *>(v);
	       const AbstractQoreNode *p = vh->getKeyValue("port");
	       need_port = p ? p->getAsInt() : 0;
	       if (!need_port)
	       {
		  xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "'port' key in protocol hash key '%s' is missing or zero", hi.getKey());
		  return -1;
	       }
	       p = vh->getKeyValue("ssl");
	       need_ssl = p ? p->getAsBool() : false;
	    }
	    priv->prot_map[hi.getKey()] = make_protocol(need_port, need_ssl);
	 }
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
   if (n && n->getType() == NT_STRING)
      if (set_proxy_url_unlocked((reinterpret_cast<const QoreStringNode *>(n))->getBuffer(), xsink))
	 return -1;

   // parse url option if present
   n = opts->getKeyValue("url");  
   if (n && n->getType() == NT_STRING)
      if (set_url_unlocked((reinterpret_cast<const QoreStringNode *>(n))->getBuffer(), xsink))
	 return -1;

   n = opts->getKeyValue("default_path");  
   if (n && n->getType() == NT_STRING)
      priv->default_path = (reinterpret_cast<const QoreStringNode *>(n))->getBuffer();

   // set default timeout if given in option hash - accept relative date/time values as well as integers
   n = opts->getKeyValue("timeout");  
   if (n)
      priv->timeout = getMsZeroInt(n);

   n = opts->getKeyValue("http_version");  
   if (n)
   {
      if (n->getType() == NT_STRING)
      {
	 if (setHTTPVersion((reinterpret_cast<const QoreStringNode *>(n))->getBuffer(), xsink))
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
   
   const QoreString *tmp = url.getPath();
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

QoreStringNode *QoreHTTPClient::getURL()
{
   SafeLocker sl(this);
   QoreStringNode *pstr = new QoreStringNode("http");
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

int QoreHTTPClient::set_proxy_url_unlocked(const char *pstr, ExceptionSink *xsink) 
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
   
   const QoreString *tmp = url.getPath();
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

int QoreHTTPClient::setProxyURL(const char *proxy, ExceptionSink *xsink) 
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

QoreStringNode *QoreHTTPClient::getProxyURL() 
{
   SafeLocker sl(this);

   if (!priv->proxy_port)
      return 0;

   QoreStringNode *pstr = new QoreStringNode("http");
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

int QoreHTTPClient::connect_unlocked(ExceptionSink *xsink)
{
   bool connect_ssl = priv->proxy_port ? priv->proxy_ssl : priv->ssl;

   int rc;
   if (connect_ssl)
      rc = priv->m_socket.connectSSL(priv->socketpath.c_str(), 0, 0, xsink);
   else
      rc = priv->m_socket.connect(priv->socketpath.c_str(), xsink);
   if (!rc)
      priv->connected = true;
   return rc;
}

int QoreHTTPClient::connect(ExceptionSink *xsink)
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

QoreHashNode *QoreHTTPClient::getResponseHeader(const char *meth, const char *mpath, class QoreHashNode &nh, const void *data, unsigned size, int &code, ExceptionSink *xsink)
{
   QoreString pathstr(priv->m_socket.getEncoding());
   const char *msgpath = getMsgPath(mpath, pathstr);

   if (connect_unlocked(xsink))
      return 0;

   // send the message
   int rc = priv->m_socket.sendHTTPMessage(meth, msgpath, priv->http11 ? "1.1" : "1.0", &nh, data, size);

   if (rc)
   {
      if (rc == -2)
	 xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "socket was closed at the remote end before the message could be sent");
      else 
	 xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));
      return 0;
   }

   QoreHashNode *ah;
   while (true)
   {
      ReferenceHolder<AbstractQoreNode> ans(priv->m_socket.readHTTPHeader(priv->timeout, &rc), xsink);
      if (!(*ans)) {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket %s closed on remote end without a response", priv->socketpath.c_str());
	 return 0;
      }
      if ((*ans)->getType() != NT_HASH) {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "malformed HTTP header received from socket %s, could not parse header", priv->socketpath.c_str());
	 return 0;
      }
      ah = reinterpret_cast<QoreHashNode *>(*ans);

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

	 return 0;
      }

      // check HTTP status code
      AbstractQoreNode *v = ah->getKeyValue("status_code");
      if (!v)
      {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "no HTTP status code received in response");
	 return 0;
      }
   
      code = v->getAsInt();
      // continue processing if "100 Continue" response received (ignore this response)
      if (code == 100)
	 continue;
      ans.release();
      break;
   }

   return ah;
}

// always generate a Host header pointing to the host hosting the resource, not the proxy
// (RFC 2616 is not totally clear on this, but other clients do it this way)
AbstractQoreNode *QoreHTTPClient::getHostHeaderValue()
{
   if (priv->port == 80)
      return new QoreStringNode(priv->host.c_str());

   QoreStringNode *str = new QoreStringNode();
   str->sprintf("%s:%d", priv->host.c_str(), priv->port);
   return str;
}

QoreHashNode *QoreHTTPClient::send_internal(const char *meth, const char *mpath, const QoreHashNode *headers, const void *data, unsigned size, bool getbody, ExceptionSink *xsink)
{
   // check if method is valid
   ccharcase_set_t::const_iterator i = method_set.find(meth);
   if (i == method_set.end())
   {
      xsink->raiseException("HTTP-CLIENT-METHOD-ERROR", "HTTP method (%n) not recognized.", meth);
      return 0;
   }
   // make sure the capitalized version is used
   meth = *i;

   SafeLocker sl(this);
   class StackHash nh(xsink);
   bool keep_alive = true;

   if (headers)
   {
      ConstHashIterator hi(headers);
      while (hi.next())
      {
	 if (!strcasecmp(hi.getKey(), "connection") || (priv->proxy_port && !strcasecmp(hi.getKey(), "proxy-connection")))
	 {
	    const AbstractQoreNode *v = hi.getValue();
	    if (v && v->getType() == NT_STRING && !strcasecmp((reinterpret_cast<const QoreStringNode *>(v))->getBuffer(), "close"))
	       keep_alive = false;
	 }

	 // if one of the mandatory headers is found, then ignore it
	 strcase_set_t::iterator i = header_ignore.find(hi.getKey());
	 if (i != header_ignore.end())
	    continue;

	 // otherwise set the value in the hash
	 const AbstractQoreNode *n = hi.getValue();
	 if (!is_nothing(n))
	    nh.setKeyValue(hi.getKey(), n->refSelf(), xsink);
      }
   }

   // add default headers if they weren't overridden
   for (header_map_t::const_iterator i = priv->default_headers.begin(), e = priv->default_headers.end(); i != e; ++i)
   {
      // look in original headers to see if the key was already given
      if (headers)
      {
	 bool skip = false;
	 ConstHashIterator hi(headers);
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
      nh.setKeyValue(i->first.c_str(), new QoreStringNode(i->second.c_str()), xsink);
   }
   if (!priv->username.empty())
   {
      // check for "Authorization" header
      bool auth_found = false;
      if (headers)
      {
	 ConstHashIterator hi(headers);
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
	 QoreStringNode *auth_str = new QoreStringNode("Basic ");
	 auth_str->concatBase64(&tmp);
	 nh.setKeyValue("Authorization", auth_str, xsink);
      }
   }
   if (priv->proxy_port && !priv->proxy_username.empty())
   {
      // check for "Proxy-Authorization" header
      bool auth_found = false;
      if (headers)
      {
	 ConstHashIterator hi(headers);
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
	 QoreStringNode *auth_str = new QoreStringNode("Basic ");
	 auth_str->concatBase64(&tmp);
	 nh.setKeyValue("Proxy-Authorization", auth_str, xsink);
      }
   }

   bool host_override = headers ? (bool)headers->getKeyValue("Host") : false;

   int code;
   ReferenceHolder<QoreHashNode> ans(xsink);
   AbstractQoreNode *v;
   int redirect_count = 0;
   class StackHash redirect_hash(xsink);
   while (true)
   {
      // set host field automatically if not overridden
      if (!host_override)
	 nh.setKeyValue("Host", getHostHeaderValue(), xsink);

      ans = getResponseHeader(meth, mpath, nh, data, size, code, xsink);
      if (!ans)
	 return 0;

      if (code >= 300 && code < 400)
      {
	 if (++redirect_count > priv->max_redirects)
	    break;
	 disconnect_unlocked();

	 host_override = false;
	 v = ans->getKeyValue("location");
	 const char *location = v ? (reinterpret_cast<QoreStringNode *>(v))->getBuffer() : 0;

	 if (!location)
	 {
	    sl.unlock();
	    v = ans->getKeyValue("status_message");
	    const char *mess = v ? (reinterpret_cast<QoreStringNode *>(v))->getBuffer() : "<no message>";
	    xsink->raiseException("HTTP-CLIENT-REDIRECT-ERROR", "no redirect location given for status code %d: message: '%s'", code, mess);
	    return 0;
	 }

	 QoreString tmp;
	 tmp.sprintf("qore-redirect-stack-%d", redirect_count);
	 redirect_hash.setKeyValue(tmp.getBuffer(), v->refSelf(), xsink);

	 if (set_url_unlocked(location, xsink))
	 {
	    sl.unlock();
	    v = ans->getKeyValue("status_message");
	    const char *mess = v ? (reinterpret_cast<QoreStringNode *>(v))->getBuffer() : "<no message>";
	    xsink->raiseException("HTTP-CLIENT-REDIRECT-ERROR", "exception occurred while setting URL for new location '%s' (code %d: message: '%s')", location, code, mess);
	    return 0;
	 }
	 // set mpath to NULL so that the new path will be taken
	 mpath = 0;
	 continue;
      }
      break;
   }

   if (code >= 300 && code < 400)
   {
      sl.unlock();
      v = ans->getKeyValue("status_message");
      const char *mess = v ? (reinterpret_cast<QoreStringNode *>(v))->getBuffer() : "<no message>";
      v = ans->getKeyValue("location");
      const char *location = v ? (reinterpret_cast<QoreStringNode *>(v))->getBuffer() : "<no location>";
      xsink->raiseException("HTTP-CLIENT-MAXIMUM-REDIRECTS-EXCEEDED", "maximum redirections (%d) exceeded; redirect code %d to '%s' ignored (message: '%s')", priv->max_redirects, code, location, mess);
      return 0;
   }

   if (code < 200 || code >= 300)
   {
      sl.unlock();
      v = ans->getKeyValue("status_message");
      const char *mess = v ? (reinterpret_cast<QoreStringNode *>(v))->getBuffer() : "<no message>";
      xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "HTTP status code %d received: message: %s", code, mess);
      return 0;
   }

   // process content-type
   v = ans->getKeyValue("content-type");
   // see if there is a character set specification in the content-type header
   if (v)
   {
      const char *str = (reinterpret_cast<QoreStringNode *>(v))->getBuffer();
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
	 QoreStringNode *nc = new QoreStringNode();
	 // skip any spaces before the charset=
	 while (p != str && (*(p - 1) == ' ' || *(p - 1) == ';'))
	    p--;
	 if (p != str)
	    nc->concat(str, p - str);
	 if (*c)
	    nc->concat(*c);
	 ans->setKeyValue("content-type", nc, xsink);
	 str = nc->getBuffer();
      }
      // split into a list if ";" characters are present
      p = strchr(str, ';');
      if (p)
      {
	 QoreListNode *l = new QoreListNode();
	 do {
	    l->push(new QoreStringNode(str, p - str, priv->m_socket.getEncoding()));
	    str = p + 1;
	 } while ((p = strchr(str, ';')));
	 // add last field
	 if (*str)
	    l->push(new QoreStringNode(str, priv->m_socket.getEncoding()));
	 ans->setKeyValue("content-type", l, xsink);
      }
   }

   // see if we should do a binary or string read
   const char *content_encoding = 0;
   v = ans->getKeyValue("content-encoding");
   if (v)
   {
      content_encoding = (reinterpret_cast<QoreStringNode *>(v))->getBuffer();
      // check for misuse of this field by including a character encoding value
      if (!strncasecmp(content_encoding, "iso", 3) || !strncasecmp(content_encoding, "utf-", 4))
      {
	 priv->m_socket.setEncoding(QEM.findCreate(content_encoding));
	 content_encoding = 0;
      }
   }

   AbstractQoreNode *te = ans->getKeyValue("transfer-encoding");
   
   // get response body, if any
   v = ans->getKeyValue("content-length");
   int len = v ? v->getAsInt() : 0;

   AbstractQoreNode *body = 0;
   if (te && !strcasecmp((reinterpret_cast<QoreStringNode *>(te))->getBuffer(), "chunked")) // check for chunked response body
   {
      ReferenceHolder<QoreHashNode> nah(xsink);
      if (content_encoding)
	 nah = priv->m_socket.readHTTPChunkedBodyBinary(priv->timeout, xsink);
      else
	 nah = priv->m_socket.readHTTPChunkedBody(priv->timeout, xsink);
      if (!nah)
	 return 0;
      
      body = nah->takeKeyValue("body");
      ans->merge(*nah, xsink);
   }
   else if (getbody || (len && strcmp(meth, "HEAD")))
   {
      int rc;

      if (content_encoding)
      {
	 SimpleRefHolder<BinaryNode> bobj(priv->m_socket.recvBinary(len, priv->timeout, &rc));
	 if (rc > 0 && bobj)
	    body = bobj.release();
      }
      else
      {
	 QoreStringNodeHolder bstr(priv->m_socket.recv(len, priv->timeout, &rc));
	 if (rc > 0 && bstr)
	    body = bstr.release();
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

	 return 0;
      }
   }

   // check for connection: close header
   if (!keep_alive || ((v = ans->getKeyValue("connection")) && !strcasecmp((reinterpret_cast<QoreStringNode *>(v))->getBuffer(), "close")))
      disconnect_unlocked();

   sl.unlock();

   // for content-encoding processing we can run unlocked

   // add body to result hash and process content encoding if necessary
   if (body && content_encoding)
   {
      BinaryNode *bobj = reinterpret_cast<BinaryNode *>(body);
      QoreStringNode *str = 0;
      if (!strcasecmp(content_encoding, "deflate") || !strcasecmp(content_encoding, "x-deflate"))
	 str = qore_inflate_to_string(bobj, priv->m_socket.getEncoding(), xsink);
      else if (!strcasecmp(content_encoding, "gzip") || !strcasecmp(content_encoding, "x-gzip"))
	 str = qore_gunzip_to_string(bobj, priv->m_socket.getEncoding(), xsink);
      else
      {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "don't know how to handle content-encoding '%s'", content_encoding);
	 ans = 0;
      }
      bobj->deref();
      body = str;
   }
   
   // merge redirect hash keys
   if (redirect_hash.size())
   {
      HashIterator hi(&redirect_hash);
      while (hi.next())
      {
	 ans->setKeyValue(hi.getKey(), hi.getValue(), xsink);
	 hi.takeValueAndDelete();
      }
   }
   
   if (body)
      ans->setKeyValue("body", body, xsink);

   return ans.release();
}

QoreHashNode *QoreHTTPClient::send(const char *meth, const char *new_path, const QoreHashNode *headers, const void *data, unsigned size, bool getbody, ExceptionSink *xsink)
{
   return send_internal(meth, new_path, headers, data, size, getbody, xsink);
}

AbstractQoreNode *QoreHTTPClient::get(const char *new_path, const QoreHashNode *headers, ExceptionSink *xsink)
{
   ReferenceHolder<QoreHashNode> ans(send_internal("GET", new_path, headers, 0, 0, true, xsink), xsink);
   if (!ans)
      return 0;

   return ans->takeKeyValue("body");
}

QoreHashNode *QoreHTTPClient::head(const char *new_path, const QoreHashNode *headers, ExceptionSink *xsink)
{
   return send_internal("HEAD", new_path, headers, 0, 0, false, xsink);
}

AbstractQoreNode *QoreHTTPClient::post(const char *new_path, const QoreHashNode *headers, const void *data, unsigned size, ExceptionSink *xsink)
{
   ReferenceHolder<QoreHashNode> ans(send_internal("POST", new_path, headers, data, size, true, xsink), xsink);
   if (!ans)
      return 0;

   return ans->takeKeyValue("body");
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
