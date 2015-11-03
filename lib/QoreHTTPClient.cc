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
#include <qore/intern/QC_Queue.h>

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

struct qore_qtc_private {
   QoreThreadLock m;
   bool http11;       // are we using http 1.1 or 1.0?
   prot_map_t prot_map;

   bool ssl, proxy_ssl, connected, nodelay;
   int port, proxy_port, default_port, max_redirects;
   std::string host, path, username, password;
   std::string proxy_host, proxy_path, proxy_username, proxy_password;
   std::string default_path;
   int timeout;
   std::string socketpath;
   QoreSocket m_socket;
   header_map_t default_headers;
   int connect_timeout_ms;
  
   DLLLOCAL qore_qtc_private() : http11(true), ssl(false), proxy_ssl(false),
				 connected(false), nodelay(false),
				 port(HTTPCLIENT_DEFAULT_PORT), proxy_port(0),
				 default_port(HTTPCLIENT_DEFAULT_PORT), 
				 max_redirects(HTTPCLIENT_DEFAULT_MAX_REDIRECTS),
				 timeout(HTTPCLIENT_DEFAULT_TIMEOUT),
				 connect_timeout_ms(-1) {
      // setup protocol map
      prot_map["http"] = make_protocol(80, false);
      prot_map["https"] = make_protocol(443, true);

      // setup default headers
      default_headers["Accept"] = "text/html";
      default_headers["Content-Type"] = "text/html";
      default_headers["Connection"] = "Keep-Alive";
      default_headers["User-Agent"] = "Qore HTTP Client v" PACKAGE_VERSION;
      default_headers["Accept-Encoding"] = "deflate,gzip,bzip2";
   }

   DLLLOCAL ~qore_qtc_private() {
   }

   DLLLOCAL void setSocketPath() {
      if (proxy_port) {
	 socketpath = proxy_host;
	 socketpath += ":";
	 char buff[20];
	 sprintf(buff, "%d", proxy_port);
	 socketpath += buff;
      }
      else {
	 socketpath = host;
	 socketpath += ":";
	 char buff[20];
	 sprintf(buff, "%d", port);
	 socketpath += buff;	    
      }
      //printd(5, "setSocketPath() '%s'\n", socketpath.c_str());
   }

   DLLLOCAL void lock() { m.lock(); }
   DLLLOCAL void unlock() { m.unlock(); }

   // returns -1 if an exception was thrown, 0 for OK
   DLLLOCAL int connect_unlocked(ExceptionSink *xsink) {
      bool connect_ssl = proxy_port ? proxy_ssl : ssl;
      
      int rc;
      if (connect_ssl)
	 rc = m_socket.connectSSL(socketpath.c_str(), connect_timeout_ms, 0, 0, xsink);
      else
	 rc = m_socket.connect(socketpath.c_str(), connect_timeout_ms, xsink);

      if (!rc) {
	 connected = true;
	 if (nodelay) {
	    if (m_socket.setNoDelay(1))
	       nodelay = false;
	 }
      }
      return rc;
   }

   DLLLOCAL void disconnect_unlocked() {
      if (connected) {
	 m_socket.close();
	 connected = false;
      }
   }

   DLLLOCAL int setNoDelay(bool nd) {
      AutoLocker al(m);
      
      if (!connected) {
	 nodelay = true;
	 return 0;
      }

      if (nodelay)
	 return 0;

      if (m_socket.setNoDelay(1))
	 return -1;

      nodelay = true;
      return 0;
   }

   DLLLOCAL bool getNoDelay() const {
      return nodelay;
   }
};

// static initialization
void QoreHTTPClient::static_init() {
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

QoreHTTPClient::QoreHTTPClient() : priv(new qore_qtc_private) {
   setSocketPath();
}

QoreHTTPClient::~QoreHTTPClient() {
   delete priv;
}

void QoreHTTPClient::deref(ExceptionSink *xsink) {
    if (ROdereference()) {
	cleanup(xsink);
	delete this;
    }
}

void QoreHTTPClient::setSocketPath() {
   priv->setSocketPath();
}

void QoreHTTPClient::setDefaultPort(int def_port) {
   priv->default_port = def_port;
}

void QoreHTTPClient::setDefaultPath(const char *def_path) {
   priv->default_path = def_path;
}

void QoreHTTPClient::setTimeout(int to) {
   priv->timeout = to;
}

int QoreHTTPClient::getTimeout() const {
   return priv->timeout;
}

void QoreHTTPClient::setEncoding(const QoreEncoding *qe) {
   priv->m_socket.setEncoding(qe);
}

const QoreEncoding *QoreHTTPClient::getEncoding() const {
   return priv->m_socket.getEncoding();
}

int QoreHTTPClient::setOptions(const QoreHashNode* opts, ExceptionSink* xsink) {
   // process new protocols
   const AbstractQoreNode *n = opts->getKeyValue("protocols");  

   if (n && n->getType() == NT_HASH) {
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(n);
      ConstHashIterator hi(h);
      while (hi.next()) {
	 const AbstractQoreNode *v = hi.getValue();
	 qore_type_t vtype = v ? v->getType() : 0;
	 if (!v || (vtype != NT_HASH && vtype != NT_INT)) {
	    xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "value of protocol hash key '%s' is not a hash or an int", hi.getKey());
	    return -1;
	 }
	 bool need_ssl = false;
	 int need_port;
	 if (vtype == NT_INT)
	    need_port = (reinterpret_cast<const QoreBigIntNode *>(v))->val;
	 else {
	    const QoreHashNode *vh = reinterpret_cast<const QoreHashNode *>(v);
	    const AbstractQoreNode *p = vh->getKeyValue("port");
	    need_port = p ? p->getAsInt() : 0;
	    if (!need_port) {
	       xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "'port' key in protocol hash key '%s' is missing or zero", hi.getKey());
	       return -1;
	    }
	    p = vh->getKeyValue("ssl");
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
   if (n) {
      if (n->getType() == NT_STRING) {
	 if (setHTTPVersion((reinterpret_cast<const QoreStringNode *>(n))->getBuffer(), xsink))
	    return -1;
      }
      else {
	 xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "expecting string version ('1.0', '1.1' as value for http_version key in options hash");
	 return -1;
      }
   }

   n = opts->getKeyValue("event_queue");
   if (n) {
       const QoreObject *o = n->getType() == NT_OBJECT ? reinterpret_cast<const QoreObject *>(n) : 0;
       Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
       if (*xsink)
	   return -1;

       if (q) { // pass reference from QoreObject::getReferencedPrivateData() to function
	   priv->m_socket.setEventQueue(q, xsink);
       }
   }

   priv->connect_timeout_ms = getMsMinusOneInt(opts->getKeyValue("connect_timeout"));

   if (priv->path.empty())
      priv->path = priv->default_path.empty() ? "/" : priv->default_path;

   return 0;
}

int QoreHTTPClient::set_url_unlocked(const char *str, ExceptionSink* xsink) {
   QoreURL url(str);
   if (!url.getHost() && priv->host.empty()) {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "missing host specification in URL '%s'", str);
      return -1;
   }

   if (!url.isValid()) {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "url parameter '%s' cannot be parsed", str);
      return -1;
   }

   bool port_set = false;
   if (url.getPort()) {
      priv->port = url.getPort();
      port_set = true;
   }

   if (url.getHost())
      priv->host = url.getHost()->getBuffer();

   // check if hostname is really a local port number (for a URL string like: "8080")
   if (!url.getPort()) {
      char *aux;
      int val = strtol(priv->host.c_str(), &aux, 10);
      if (aux == (priv->host.c_str() + priv->host.size())) {
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

   if (priv->username.empty() && !priv->password.empty()) {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "invalid authorization credentials: password set without username");
      return -1;
   }

   if (!priv->username.empty() && priv->password.empty()) {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "invalid authorization credentials: username set without password");
      return -1;
   }

   tmp = url.getProtocol();
   if (tmp) {
      prot_map_t::const_iterator i = priv->prot_map.find(tmp->getBuffer());
      if (i == priv->prot_map.end()) {
	 xsink->raiseException("HTTP-CLIENT-UNKNOWN-PROTOCOL", "protocol '%s' is not supported.", tmp->getBuffer());
	 return -1;
      }

      // set port only if it wasn't overridden in the URL
      if (!port_set)
	 priv->port = get_port(i->second);

      // set SSL setting from protocol default
      priv->ssl = get_ssl(i->second);
   }
   else {
      priv->ssl = false;
      if (!port_set)
	 priv->port = priv->default_port;
   }

   if (!priv->proxy_port)
      setSocketPath();
   return 0;
}

void QoreHTTPClient::setConnectTimeout(int ms) {
}

int QoreHTTPClient::getConnectTimeout() const {
   return priv->connect_timeout_ms;
}

int QoreHTTPClient::setURL(const char *str, ExceptionSink* xsink) {
   SafeLocker sl(priv->m);
   // disconnect immediately if not using a proxy
   if (!priv->proxy_port)
      priv->disconnect_unlocked();
   return set_url_unlocked(str, xsink);
}

QoreStringNode *QoreHTTPClient::getURL() {
   SafeLocker sl(priv->m);
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

int QoreHTTPClient::setHTTPVersion(const char* version, ExceptionSink* xsink) {
   int rc = 0;
   SafeLocker sl(priv->m);
   if (!strcmp(version, "1.0"))
      priv->http11 = false;
   else if (!strcmp(version, "1.1"))
      priv->http11 = true;
   else {
      xsink->raiseException("HTTP-VERSION-ERROR", "only '1.0' and '1.1' are valid (value passed: '%s')", version);
      rc = -1;
   }
   return rc;
}

const char *QoreHTTPClient::getHTTPVersion() const {
   return priv->http11 ? "1.1" : "1.0";
}

void QoreHTTPClient::setHTTP11(bool val) {
   priv->http11 = val;
}

bool QoreHTTPClient::isHTTP11() const { 
   return priv->http11;
}

int QoreHTTPClient::set_proxy_url_unlocked(const char *pstr, ExceptionSink *xsink) { 
   QoreURL url(pstr);
   if (!url.getHost()) {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "missing host specification in proxy URL");
      return -1;
   }

   if (!url.isValid()) {
      xsink->raiseException("HTTP-CLIENT-URL-ERROR", "proxy URL '%s' cannot be parsed", pstr);
      return -1;
   }

   bool port_set = false;
   if (url.getPort()) {
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

   if (priv->proxy_username.empty() && !priv->proxy_password.empty()) {
      xsink->raiseException("HTTP-CLIENT-SET-PROXY-ERROR", "invalid authorization credentials: password set without username");
      return -1;
   }

   if (!priv->proxy_username.empty() && priv->proxy_password.empty()) {
      xsink->raiseException("HTTP-CLIENT-SET-PROXY-ERROR", "invalid authorization credentials: username set without password");
      return -1;
   }

   tmp = url.getProtocol();
   if (tmp) {
      if (strcasecmp(tmp->getBuffer(), "http") && strcasecmp(tmp->getBuffer(), "https")) {
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
   else {
      priv->proxy_ssl = false;
      if (!port_set)
	 priv->proxy_port = priv->default_port;
   }

   setSocketPath();
   return 0;
}

int QoreHTTPClient::setProxyURL(const char *proxy, ExceptionSink *xsink) 
{
   SafeLocker sl(priv->m);
   priv->disconnect_unlocked();
   if (!proxy || !proxy[0]) {
      clearProxyURL();
      return 0;
   }
   return set_proxy_url_unlocked(proxy, xsink);
}

QoreStringNode *QoreHTTPClient::getProxyURL() 
{
   SafeLocker sl(priv->m);

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
   SafeLocker sl(priv->m);
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
   SafeLocker sl(priv->m);
   return priv->m_socket.verifyPeerCertificate(); 
}

const char* QoreHTTPClient::getSSLCipherName() { 
   SafeLocker sl(priv->m);
   return priv->m_socket.getSSLCipherName(); 
}

const char* QoreHTTPClient::getSSLCipherVersion() { 
   SafeLocker sl(priv->m);
   return priv->m_socket.getSSLCipherVersion(); 
}

int QoreHTTPClient::connect(ExceptionSink *xsink) {
   SafeLocker sl(priv->m);
   return priv->connect_unlocked(xsink);
}

void QoreHTTPClient::disconnect() {
   SafeLocker sl(priv->m);
   priv->disconnect_unlocked();
}

const char *QoreHTTPClient::getMsgPath(const char *mpath, class QoreString &pstr) {
   pstr.clear();

   // use default path if no path is set
   if (!mpath || !mpath[0])
      mpath = priv->path.empty() ? (priv->default_path.empty() ? "/" : (const char *)priv->default_path.c_str()) : (const char *)priv->path.c_str();

   if (priv->proxy_port) {
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
      // according to RFC 3896 it's not necessary to encode non-ascii characters
      else
	 pstr.concat(*p);
      ++p;
   }
   return (const char *)pstr.getBuffer();
}

QoreHashNode *QoreHTTPClient::getResponseHeader(const char *meth, const char *mpath, const QoreHashNode &nh, const void *data, unsigned size, int &code, bool suppress_content_length, ExceptionSink *xsink) {
   QoreString pathstr(priv->m_socket.getEncoding());
   const char *msgpath = getMsgPath(mpath, pathstr);

   if (!priv->connected && priv->connect_unlocked(xsink))
      return 0;

   // send the message
   int rc = priv->m_socket.sendHTTPMessage(meth, msgpath, priv->http11 ? "1.1" : "1.0", &nh, data, size, QORE_SOURCE_HTTPCLIENT);

   if (rc) {
      if (rc == -2) {
	 priv->disconnect_unlocked();
	 xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "socket was closed at the remote end before the message could be sent");
      }
      else
	 xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));
      return 0;
   }

   QoreHashNode *ah = 0;
   while (true) {
      ReferenceHolder<AbstractQoreNode> ans(priv->m_socket.readHTTPHeader(priv->timeout, &rc, QORE_SOURCE_HTTPCLIENT), xsink);
      if (!(*ans)) {
	 priv->disconnect_unlocked();
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket %s closed on remote end without a response", priv->socketpath.c_str());
	 return 0;
      }
      if ((*ans)->getType() != NT_HASH) {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "malformed HTTP header received from socket %s, could not parse header", priv->socketpath.c_str());
	 return 0;
      }
      ah = reinterpret_cast<QoreHashNode *>(*ans);

      if (rc <= 0) {
	 if (!rc) {           // remote end has closed the connection
	    priv->disconnect_unlocked();
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "remote end has closed the connection");
	 }
	 else if (rc == -1)   // recv() error
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", strerror(errno));
	 else if (rc == -2) {
	    priv->disconnect_unlocked();
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket was closed at the remote end");
	 }
	 else if (rc == -3)   // timeout
	    xsink->raiseException("HTTP-CLIENT-TIMEOUT", "timed out waiting %dms for response on socket %s", priv->timeout, priv->socketpath.c_str());
	 else
	    assert(false);

	 return 0;
      }

      // check HTTP status code
      AbstractQoreNode *v = ah->getKeyValue("status_code");
      if (!v) {
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
AbstractQoreNode *QoreHTTPClient::getHostHeaderValue() {
   if (priv->port == 80)
      return new QoreStringNode(priv->host.c_str());

   QoreStringNode *str = new QoreStringNode();
   str->sprintf("%s:%d", priv->host.c_str(), priv->port);
   return str;
}

static void do_content_length_event(Queue *cb_queue, int64 id, int len) {
   if (cb_queue) {
      ExceptionSink xsink;
      ReferenceHolder<QoreHashNode> h(new QoreHashNode, &xsink);
      h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_HTTP_CONTENT_LENGTH), 0);
      h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_HTTPCLIENT), 0);
      h->setKeyValue("id", new QoreBigIntNode(id), 0);
      h->setKeyValue("len", new QoreBigIntNode(len), 0);
      // FIXME: should implement a QoreQueue::push_temporary() method to take reference
      cb_queue->push(*h);      
   }
}

static void do_redirect_event(Queue *cb_queue, int64 id, const QoreStringNode *loc, const QoreStringNode *msg) {
   if (cb_queue) {
      ExceptionSink xsink;
      ReferenceHolder<QoreHashNode> h(new QoreHashNode, &xsink);
      h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_HTTP_REDIRECT), 0);
      h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_HTTPCLIENT), 0);
      h->setKeyValue("id", new QoreBigIntNode(id), 0);
      h->setKeyValue("location", loc->refSelf(), 0);
      if (msg)
	 h->setKeyValue("status_message", msg->refSelf(), 0);
      // FIXME: should implement a QoreQueue::push_temporary() method to take reference
      cb_queue->push(*h);      
   }
}

static void do_event(Queue *cb_queue, int64 id, int event) {
   if (cb_queue) {
      ExceptionSink xsink;
      ReferenceHolder<QoreHashNode> h(new QoreHashNode, &xsink);
      h->setKeyValue("event", new QoreBigIntNode(event), 0);
      h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_HTTPCLIENT), 0);
      h->setKeyValue("id", new QoreBigIntNode(id), 0);
      // FIXME: should implement a QoreQueue::push_temporary() method to take reference
      cb_queue->push(*h);      
   }
}

static void check_headers(const char *str, int len, bool &multipart, QoreHashNode &ans, const QoreEncoding *enc, ExceptionSink *xsink) {
   // see if the string starts with "multipart/"
   if (!multipart) {
      if (len > 10 && !strncasecmp(str, "multipart/", 10)) {
	 ans.setKeyValue("_qore_multipart", new QoreStringNode(str + 10, len - 10, enc), xsink);
	 multipart = true;
      }
   }
   else {
      if (len > 9 && !strncasecmp(str, "boundary=", 9))
	 ans.setKeyValue("_qore_multipart_boundary", new QoreStringNode(str + 9, len - 9, enc), xsink);
      else if (len > 6 && !strncasecmp(str, "start=", 6))
	 ans.setKeyValue("_qore_multipart_start", new QoreStringNode(str + 6, len - 6, enc), xsink);
   }
}

QoreHashNode *QoreHTTPClient::send_internal(const char *meth, const char *mpath, const QoreHashNode *headers, const void *data, unsigned size, bool getbody, QoreHashNode *info, ExceptionSink *xsink, bool suppress_content_length) {
   //printd(5, "QoreHTTPClient::send_internal(meth=%s, mpath=%s, info=%08p)\n", meth, mpath, info);

   // check if method is valid
   ccharcase_set_t::const_iterator i = method_set.find(meth);
   if (i == method_set.end()) {
      xsink->raiseException("HTTP-CLIENT-METHOD-ERROR", "HTTP method (%n) not recognized.", meth);
      return 0;
   }
   // make sure the capitalized version is used
   meth = *i;

   SafeLocker sl(priv->m);
   Queue *cb_queue = priv->m_socket.getQueue();

   ReferenceHolder<QoreHashNode> nh(new QoreHashNode, xsink);
   bool keep_alive = true;

   if (headers) {
      ConstHashIterator hi(headers);
      while (hi.next()) {
	 if (!strcasecmp(hi.getKey(), "connection") || (priv->proxy_port && !strcasecmp(hi.getKey(), "proxy-connection"))) {
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
	    nh->setKeyValue(hi.getKey(), n->refSelf(), xsink);
      }
   }

   // add default headers if they weren't overridden
   for (header_map_t::const_iterator i = priv->default_headers.begin(), e = priv->default_headers.end(); i != e; ++i) {
      // look in original headers to see if the key was already given
      if (headers) {
	 bool skip = false;
	 ConstHashIterator hi(headers);
	 while (hi.next()) {
	    if (!strcasecmp(hi.getKey(), i->first.c_str())) {
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
      nh->setKeyValue(i->first.c_str(), new QoreStringNode(i->second.c_str()), xsink);
   }

   if (!priv->username.empty()) {
      // check for "Authorization" header
      bool auth_found = false;
      if (headers) {
	 ConstHashIterator hi(headers);
	 while (hi.next()) {
	    if (!strcasecmp(hi.getKey(), "Authorization")) {
	       auth_found = true;
	       break;
	    }
	 }
      }

      if (!auth_found) {
	 QoreString tmp;
	 tmp.sprintf("%s:%s", priv->username.c_str(), priv->password.c_str());
	 QoreStringNode *auth_str = new QoreStringNode("Basic ");
	 auth_str->concatBase64(&tmp);
	 nh->setKeyValue("Authorization", auth_str, xsink);
      }
   }

   if (priv->proxy_port && !priv->proxy_username.empty()) {
      // check for "Proxy-Authorization" header
      bool auth_found = false;
      if (headers) {
	 ConstHashIterator hi(headers);
	 while (hi.next()) {
	    if (!strcasecmp(hi.getKey(), "Proxy-Authorization")) {
	       auth_found = true;
	       break;
	    }
	 }
      }
      if (!auth_found) {
	 QoreString tmp;
	 tmp.sprintf("%s:%s", priv->proxy_username.c_str(), priv->proxy_password.c_str());
	 QoreStringNode *auth_str = new QoreStringNode("Basic ");
	 auth_str->concatBase64(&tmp);
	 nh->setKeyValue("Proxy-Authorization", auth_str, xsink);
      }
   }

   bool host_override = headers ? (bool)headers->getKeyValue("Host") : false;

   int code;
   ReferenceHolder<QoreHashNode> ans(xsink);
   int redirect_count = 0;
   while (true) {
      // set host field automatically if not overridden
      if (!host_override)
	 nh->setKeyValue("Host", getHostHeaderValue(), xsink);

      if (info) {
	 info->setKeyValue("headers", nh->copy(), xsink);
	 if (*xsink)
	    return 0;
      }

      // send HTTP message and get response header
      ans = getResponseHeader(meth, mpath, *(*nh), data, size, code, suppress_content_length, xsink);
      if (!ans)
	 return 0;

      if (info) {
	 info->setKeyValue("response-headers", ans->refSelf(), xsink);
	 if (*xsink)
	    return 0;
      }

      if (code >= 300 && code < 400) {
	 if (++redirect_count > priv->max_redirects)
	    break;
	 priv->disconnect_unlocked();

	 host_override = false;
	 const QoreStringNode *loc = reinterpret_cast<QoreStringNode *>(ans->getKeyValue("location"));
	 const char *location = loc ? loc->getBuffer() : 0;
	 const QoreStringNode *mess = reinterpret_cast<QoreStringNode *>(ans->getKeyValue("status_message"));

	 if (!location) {
	    sl.unlock();
	    const char *msg = mess ? mess->getBuffer() : "<no message>";
	    xsink->raiseException("HTTP-CLIENT-REDIRECT-ERROR", "no redirect location given for status code %d: message: '%s'", code, msg);
	    return 0;
	 }

	 if (cb_queue)
	    do_redirect_event(cb_queue, priv->m_socket.getObjectIDForEvents(), loc, mess);

	 if (set_url_unlocked(location, xsink)) {
	    sl.unlock();
	    const char *msg = mess ? mess->getBuffer() : "<no message>";
	    xsink->raiseException("HTTP-CLIENT-REDIRECT-ERROR", "exception occurred while setting URL for new location '%s' (code %d: message: '%s')", location, code, msg);
	    return 0;
	 }

	 // set redirect info in info hash if present
	 if (info) {
	    QoreString tmp;
	    tmp.sprintf("redirect-%d", redirect_count);
	    info->setKeyValue(tmp.getBuffer(), loc->refSelf(), xsink);
	    if (*xsink)
	       return 0;
	    
	    tmp.clear();
	    tmp.sprintf("redirect-message-%d", redirect_count);
	    info->setKeyValue(tmp.getBuffer(), mess ? mess->refSelf() : 0, xsink);
	 }

	 // set mpath to NULL so that the new path will be taken
	 mpath = 0;
	 continue;
      }

      break;
   }

   if (code >= 300 && code < 400) {
      sl.unlock();
      const AbstractQoreNode *v = ans->getKeyValue("status_message");
      const char *mess = v ? (reinterpret_cast<const QoreStringNode *>(v))->getBuffer() : "<no message>";
      v = ans->getKeyValue("location");
      const char *location = v ? (reinterpret_cast<const QoreStringNode *>(v))->getBuffer() : "<no location>";
      xsink->raiseException("HTTP-CLIENT-MAXIMUM-REDIRECTS-EXCEEDED", "maximum redirections (%d) exceeded; redirect code %d to '%s' ignored (message: '%s')", priv->max_redirects, code, location, mess);
      return 0;
   }

   // process content-type
   const AbstractQoreNode *v = ans->getKeyValue("content-type");
   // see if there is a character set specification in the content-type header
   if (v) {
      // save original content-type header before processing
      ans->setKeyValue("_qore_orig_content_type", v->refSelf(), xsink);

      const char *str = (reinterpret_cast<const QoreStringNode *>(v))->getBuffer();
      const char *p = strstr(str, "charset=");
      if (p && (p == str || *(p - 1) == ';' || *(p - 1) == ' ')) {
	 // move p to start of encoding
	 const char *c = p + 8;
	 char quote = '\0';
	 if (*c == '\'' || *c == '"') {
	    quote = *c;
	    ++c;
	 }
	 QoreString enc;
	 while (*c && *c != ';' && *c != ' ' && *c != quote)
	    enc.concat(*(c++));
	 
	 if (quote && *c == quote)
	    ++c;

	 printd(5, "QoreHTTPClient::send_intern() setting encoding to '%s' from content-type header: '%s' (cs=%p c=%p %d)\n", enc.getBuffer(), str, p + 8, c);

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
	    nc->concat(c);
	 ans->setKeyValue("content-type", nc, xsink);
	 str = nc->getBuffer();
      }
      // split into a list if ";" characters are present
      p = strchr(str, ';');
      if (p) {
	 bool multipart = false;
	 QoreListNode *l = new QoreListNode();
	 do {
	    // skip whitespace
	    while (*str == ' ') str++;
	    if (str != p) {
	       int len = p - str;
	       check_headers(str, len, multipart, *(*ans), priv->m_socket.getEncoding(), xsink);
	       l->push(new QoreStringNode(str, len, priv->m_socket.getEncoding()));
	    }
	    str = p + 1;
	 } while ((p = strchr(str, ';')));
	 // skip whitespace
	 while (*str == ' ') str++;
	 // add last field
	 if (*str) {
	    check_headers(str, strlen(str), multipart, *(*ans), priv->m_socket.getEncoding(), xsink);
	    l->push(new QoreStringNode(str, priv->m_socket.getEncoding()));
	 }
	 ans->setKeyValue("content-type", l, xsink);
      }
   }

   // see if we should do a binary or string read
   const char *content_encoding = 0;
   v = ans->getKeyValue("content-encoding");
   if (v) {
      content_encoding = (reinterpret_cast<const QoreStringNode *>(v))->getBuffer();
      // check for misuse (? not sure: check RFCs again) of this field by including a character encoding value
      if (!strncasecmp(content_encoding, "iso", 3) || !strncasecmp(content_encoding, "utf-", 4)) {
	 priv->m_socket.setEncoding(QEM.findCreate(content_encoding));
	 content_encoding = 0;
      }
   }

   const AbstractQoreNode *te = ans->getKeyValue("transfer-encoding");
   
   // get response body, if any
   v = ans->getKeyValue("content-length");
   int len = v ? v->getAsInt() : 0;

   if (v && cb_queue)
      do_content_length_event(cb_queue, priv->m_socket.getObjectIDForEvents(), len);

   AbstractQoreNode *body = 0;
   if (te && !strcasecmp((reinterpret_cast<const QoreStringNode *>(te))->getBuffer(), "chunked")) { // check for chunked response body
      if (cb_queue)
	 do_event(cb_queue, priv->m_socket.getObjectIDForEvents(), QORE_EVENT_HTTP_CHUNKED_START);
      ReferenceHolder<QoreHashNode> nah(xsink);
      if (content_encoding)
	 nah = priv->m_socket.readHTTPChunkedBodyBinary(priv->timeout, xsink, QORE_SOURCE_HTTPCLIENT);
      else
	 nah = priv->m_socket.readHTTPChunkedBody(priv->timeout, xsink, QORE_SOURCE_HTTPCLIENT);
      if (cb_queue)
	 do_event(cb_queue, priv->m_socket.getObjectIDForEvents(), QORE_EVENT_HTTP_CHUNKED_END);

      if (!nah)
	 return 0;
      
      if (info) {
	 info->setKeyValue("chunked", &True, xsink);
	 if (*xsink)
	    return 0;
      }

      body = nah->takeKeyValue("body");
      ans->merge(*nah, xsink);
   }
   else if (getbody || (len && strcmp(meth, "HEAD"))) {
      int rc;

      if (content_encoding) {
	 SimpleRefHolder<BinaryNode> bobj(priv->m_socket.recvBinary(len, priv->timeout, &rc));
	 if (rc > 0 && bobj)
	    body = bobj.release();
      }
      else {
	 QoreStringNodeHolder bstr(priv->m_socket.recv(len, priv->timeout, &rc));
	 if (rc > 0 && bstr)
	    body = bstr.release();
      }

      //printf("body=%08p\n", body);
      if (rc <= 0) {
	 if (!rc) {             // remote end has closed the connection
	    priv->disconnect_unlocked();
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "remote end closed the connection while receiving response message body");
	 }
	 else if (rc == -1)   // recv() error
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", strerror(errno));
	 else if (rc == -2) {
	    priv->disconnect_unlocked();
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket was closed at the remote end while receiving response message body");
	 }
	 else if (rc == -3)   // timeout
	    xsink->raiseException("HTTP-CLIENT-TIMEOUT", "timed out waiting %dms for response message body of length %d on socket %s", priv->timeout, len, priv->socketpath.c_str());
	 else
	    assert(false);

	 return 0;
      }
   }

   // check for connection: close header
   if (!keep_alive || ((v = ans->getKeyValue("connection")) && !strcasecmp((reinterpret_cast<const QoreStringNode *>(v))->getBuffer(), "close")))
      priv->disconnect_unlocked();

   sl.unlock();

   // for content-encoding processing we can run unlocked

   // add body to result hash and process content encoding if necessary
   if (body && content_encoding) {
      BinaryNode *bobj = reinterpret_cast<BinaryNode *>(body);
      QoreStringNode *str = 0;
      if (!strcasecmp(content_encoding, "deflate") || !strcasecmp(content_encoding, "x-deflate"))
	 str = qore_inflate_to_string(bobj, priv->m_socket.getEncoding(), xsink);
      else if (!strcasecmp(content_encoding, "gzip") || !strcasecmp(content_encoding, "x-gzip"))
	 str = qore_gunzip_to_string(bobj, priv->m_socket.getEncoding(), xsink);
      else if (!strcasecmp(content_encoding, "bzip2") || !strcasecmp(content_encoding, "x-bzip2"))
	 str = qore_bunzip2_to_string(bobj, priv->m_socket.getEncoding(), xsink);
      else {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "don't know how to handle content-encoding '%s'", content_encoding);
	 ans = 0;
      }
      bobj->deref();
      body = str;
   }

   if (code < 200 || code >= 300) {
      v = ans->getKeyValue("status_message");
      const char *mess = v ? (reinterpret_cast<const QoreStringNode *>(v))->getBuffer() : "<no message>";
      xsink->raiseExceptionArg("HTTP-CLIENT-RECEIVE-ERROR", body, "HTTP status code %d received: message: %s", code, mess);
      return 0;
   }
      
   if (body)
      ans->setKeyValue("body", body, xsink);

   return ans.release();
}

/*
int QoreHTTPClient::buildMultipart(QoreString &msg, const char *mp_content_type, QoreHashNode &headers, const QoreListNode *mp_msg_list, ExceptionSink *xsink) {   
   // find content-type header
}

QoreHashNode *QoreHTTPClient::sendMultipart(const char *mp_content_type, const char *meth, const char *new_path, const QoreHashNode *headers, const QoreListNode &mp_msg_list, bool getbody, QoreHashNode *info, ExceptionSink *xsink) {
   if (mp_msg_list.size() < 2) {
      xsink->raiseException("HTTP-SEND-MULTIPART-ERROR", "multipart message list must have at least 2 elements; list passed has %d", mp_msg_list.size());
      return 0;
   }

   QoreString msg;
   ReferenceHolder<QoreHashNode> hdr_holder(headers ? headers->copy() : new QoreHashNode, xsink);

   if (buildMultipart(msg, mp_content_type, *(*hdr_holder), mp_msg_list, xsink))
      return 0;

   return send_internal(meth, new_path, headers, data, size, getbody, info, xsink, true);
}
*/

QoreHashNode *QoreHTTPClient::send(const char *meth, const char *new_path, const QoreHashNode *headers, const void *data, unsigned size, bool getbody, QoreHashNode *info, ExceptionSink *xsink) {
   return send_internal(meth, new_path, headers, data, size, getbody, info, xsink);
}

AbstractQoreNode *QoreHTTPClient::get(const char *new_path, const QoreHashNode *headers, QoreHashNode *info, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> ans(send_internal("GET", new_path, headers, 0, 0, true, info, xsink), xsink);
   if (!ans)
      return 0;

   return ans->takeKeyValue("body");
}

QoreHashNode *QoreHTTPClient::head(const char *new_path, const QoreHashNode *headers, QoreHashNode *info, ExceptionSink *xsink) {
   return send_internal("HEAD", new_path, headers, 0, 0, false, info, xsink);
}

AbstractQoreNode *QoreHTTPClient::post(const char *new_path, const QoreHashNode *headers, const void *data, unsigned size, QoreHashNode *info, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> ans(send_internal("POST", new_path, headers, data, size, true, info, xsink), xsink);
   if (!ans)
      return 0;

   return ans->takeKeyValue("body");
}

void QoreHTTPClient::addProtocol(const char *prot, int new_port, bool new_ssl) {
   priv->prot_map[prot] = make_protocol(new_port, new_ssl);
}

void QoreHTTPClient::setMaxRedirects(int max) {
   priv->max_redirects = max;
}

int QoreHTTPClient::getMaxRedirects() const {
   return priv->max_redirects;
}

void QoreHTTPClient::setDefaultHeaderValue(const char *header, const char *val) {
   priv->default_headers[header] = val;
}

void QoreHTTPClient::setEventQueue(Queue *cbq, ExceptionSink *xsink) {
   AutoLocker al(priv->m);
   priv->m_socket.setEventQueue(cbq, xsink);
}

void QoreHTTPClient::cleanup(ExceptionSink *xsink) {
   AutoLocker al(priv->m);
   priv->m_socket.cleanup(xsink);
}

void QoreHTTPClient::lock() {
   priv->m.lock();
}

void QoreHTTPClient::unlock() {
   priv->m.unlock();
}

int QoreHTTPClient::setNoDelay(bool nd) {
   return priv->setNoDelay(nd);
}

bool QoreHTTPClient::getNoDelay() const {
   return priv->getNoDelay();
}

bool QoreHTTPClient::isConnected() const {
   return priv->connected;
}
