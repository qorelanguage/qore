/*
  QoreHTTPClient.cpp

  Qore Programming Language

  Copyright (C) 2006 - 2013 Qore Technologies
  
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
#  include "tests/QoreHTTPClient_tests.cpp"
#endif

// protocol map class to recognize user-defined protocols (mostly useful for derived classes)
typedef std::map<std::string, int> prot_map_t;
typedef std::set<const char *, ltcstrcase> ccharcase_set_t;
typedef std::set<std::string, ltstrcase> strcase_set_t;
typedef std::map<std::string, std::string> header_map_t;

static ccharcase_set_t method_set;
static strcase_set_t header_ignore;

struct con_info {
   bool ssl;
   int port;
   std::string host, path, username, password;

   DLLLOCAL con_info(int n_port = 0) : ssl(false), port(n_port) {
   }

   DLLLOCAL int set_url(QoreURL &url, bool &port_set, ExceptionSink *xsink) {
      if (url.getPort()) {
	 port = url.getPort();
	 port_set = true;
      }
   
      host = url.getHost() ? url.getHost()->getBuffer() : "";

      // check if hostname is really a local port number (for a URL string like: "8080")
      if (!url.getPort() && !host.empty()) {
	 char *aux;
	 int val = strtol(host.c_str(), &aux, 10);
	 if (aux == (host.c_str() + host.size())) {
	    host = HTTPCLIENT_DEFAULT_HOST;
	    port = val;
	    port_set = true;
	 }
      }
   
      const QoreString *tmp = url.getPath();
      path = tmp ? tmp->getBuffer() : "";
      tmp = url.getUserName();
      username = tmp ? tmp->getBuffer() : "";
      tmp = url.getPassword();
      password = tmp ? tmp->getBuffer() : "";

      if (username.empty() && !password.empty()) {
	 xsink->raiseException("HTTP-CLIENT-URL-ERROR", "invalid authorization credentials: password set without username");
	 return -1;
      }

      if (!username.empty() && password.empty()) {
	 xsink->raiseException("HTTP-CLIENT-URL-ERROR", "invalid authorization credentials: username set without password");
	 return -1;
      }

      return 0;
   }

   DLLLOCAL QoreStringNode *get_url() const {
      QoreStringNode *pstr = new QoreStringNode("http");
      if (ssl)
	 pstr->concat("s://");
      else
	 pstr->concat("://");
      if (!username.empty())
	 pstr->sprintf("%s:%s@", username.c_str(), password.c_str());

      pstr->concat(host.c_str());
      if (port != 80)
	 pstr->sprintf(":%d", port);
      pstr->concat(path.c_str());
      return pstr;
   }

   DLLLOCAL void setUserPassword(const char *user, const char *pass) {
      assert(user && pass);
      username = user;
      password = pass;
   }

   DLLLOCAL void clearUserPassword() {
      username.clear();
      password.clear();
   }

   DLLLOCAL void clear() {
      port = 0;
      username.clear();
      password.clear();
      host.clear();
      path.clear();
      ssl = false;
   }
};

struct qore_qtc_private {
   mutable QoreThreadLock m;
   bool http11;       // are we using http 1.1 or 1.0?
   prot_map_t prot_map;

   con_info connection, proxy_connection;

   bool connected, 
      nodelay, 
      proxy_connected; // means that a CONNECT message has been processed and the connection is now made as if it were directly with the client
   int default_port, max_redirects;
   std::string default_path;
   int timeout;
   std::string socketpath;
   QoreSocket m_socket;
   header_map_t default_headers;
   int connect_timeout_ms;   
  
   DLLLOCAL qore_qtc_private() : http11(true), connection(HTTPCLIENT_DEFAULT_PORT),
				 connected(false), nodelay(false), proxy_connected(false),
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
      default_headers["User-Agent"] = "Qore-HTTP-Client/" PACKAGE_VERSION;
      default_headers["Accept-Encoding"] = "deflate,gzip,bzip2";
   }

   DLLLOCAL ~qore_qtc_private() {
   }

   DLLLOCAL void setSocketPathIntern(const con_info& con) {
      if (con.path.empty() || !con.host.empty()) {
	 socketpath = con.host;
	 socketpath += ":";
	 char buff[20];
	 sprintf(buff, "%d", con.port);
	 socketpath += buff;
	 return;
      }

      socketpath = con.path;
   }

   DLLLOCAL void setSocketPath() {
      setSocketPathIntern(proxy_connection.port ? proxy_connection : connection);
      //printd(5, "setSocketPath() '%s'\n", socketpath.c_str());
   }

   DLLLOCAL void lock() { m.lock(); }
   DLLLOCAL void unlock() { m.unlock(); }

   // returns -1 if an exception was thrown, 0 for OK
   DLLLOCAL int connect_unlocked(ExceptionSink *xsink) {
      bool connect_ssl = proxy_connection.port ? proxy_connection.ssl : connection.ssl;
      
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
	 proxy_connected = false;
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

   DLLLOCAL int set_url_unlocked(const char *str, ExceptionSink* xsink) {
      QoreURL url(str);

      if (!url.isValid()) {
	 xsink->raiseException("HTTP-CLIENT-URL-ERROR", "URL '%s' cannot be parsed", str);
	 return -1;
      }
      
      bool port_set = false;
      if (connection.set_url(url, port_set, xsink))
	 return -1;

      const QoreString *tmp = url.getProtocol();
      if (tmp) {
	 prot_map_t::const_iterator i = prot_map.find(tmp->getBuffer());
	 if (i == prot_map.end()) {
	    xsink->raiseException("HTTP-CLIENT-UNKNOWN-PROTOCOL", "protocol '%s' is not supported.", tmp->getBuffer());
	    return -1;
	 }

	 // set port only if it wasn't overridden in the URL
	 if (!port_set)
	    connection.port = get_port(i->second);
	 
	 // set SSL setting from protocol default
	 connection.ssl = get_ssl(i->second);
      }
      else {
	 connection.ssl = false;
	 if (!port_set)
	    connection.port = default_port;
      }

      if (!proxy_connection.port)
	 setSocketPath();

      return 0;
   }

   DLLLOCAL int set_proxy_url_unlocked(const char *pstr, ExceptionSink *xsink) { 
      QoreURL url(pstr);

      if (!url.isValid()) {
	 xsink->raiseException("HTTP-CLIENT-URL-ERROR", "proxy URL '%s' cannot be parsed", pstr);
	 return -1;
      }

      bool port_set = false;
      if (proxy_connection.set_url(url, port_set, xsink))
	 return -1;

      const QoreString *tmp = url.getProtocol();
      if (tmp) {
	 if (strcasecmp(tmp->getBuffer(), "http") && strcasecmp(tmp->getBuffer(), "https")) {
	    xsink->raiseException("HTTP-CLIENT-PROXY-PROTOCOL-ERROR", "protocol '%s' is not supported for proxies, only 'http' and 'https'", tmp->getBuffer());
	    return -1;
	 }

	 prot_map_t::const_iterator i = prot_map.find(tmp->getBuffer());
	 assert(i != prot_map.end());

	 // set port only if it wasn't overridden in the URL
	 if (!port_set)
	    proxy_connection.port = get_port(i->second);

	 // set SSL setting from protocol default
	 proxy_connection.ssl = get_ssl(i->second);
      }
      else {
	 proxy_connection.ssl = false;
	 if (!port_set)
	    proxy_connection.port = default_port;
      }

      setSocketPath();
      return 0;
   }

   DLLLOCAL QoreHashNode* getPeerInfo(ExceptionSink* xsink) const {
      AutoLocker al(m);

      return m_socket.getPeerInfo(xsink);
   }

   DLLLOCAL QoreHashNode* getSocketInfo(ExceptionSink* xsink) const {
      AutoLocker al(m);

      return m_socket.getSocketInfo(xsink);
   }

   DLLLOCAL void setUserPassword(const char *user, const char *pass) {
      assert(user && pass);
      AutoLocker al(m);

      connection.setUserPassword(user, pass);
   }

   DLLLOCAL void clearUserPassword() {
      AutoLocker al(m);
      connection.clearUserPassword();
   }

   DLLLOCAL void setProxyUserPassword(const char *user, const char *pass) {
      assert(user && pass);
      AutoLocker al(m);

      proxy_connection.setUserPassword(user, pass);
   }

   DLLLOCAL void clearProxyUserPassword() {
      AutoLocker al(m);
      proxy_connection.clearUserPassword();
   }

   // always generate a Host header pointing to the host hosting the resource, not the proxy
   // (RFC 2616 is not totally clear on this, but other clients do it this way)
   DLLLOCAL AbstractQoreNode *getHostHeaderValue() {
      if (connection.port == 80)
	 return new QoreStringNode(connection.host.c_str());

      QoreStringNode *str = new QoreStringNode;
      str->sprintf("%s:%d", connection.host.c_str(), connection.port);
      return str;
   }
   
   DLLLOCAL QoreHashNode *getResponseHeader(const char *meth, const char *mpath, const QoreHashNode &nh, const void *data, unsigned size, int &code,  bool suppress_content_length, QoreHashNode *info, bool with_connect, ExceptionSink *xsink);

   DLLLOCAL const char *getMsgPath(const char *mpath, QoreString &pstr) {
      pstr.clear();

      // use default path if no path is set
      if (!mpath || !mpath[0])
	 mpath = connection.path.empty() 
	    ? (default_path.empty() ? "/" : (const char *)default_path.c_str()) 
	    : (const char *)connection.path.c_str();

      if (proxy_connection.port) {
         // create URL string for path for proxy
	 pstr.concat("http");
	 if (connection.ssl)
	    pstr.concat('s');
	 pstr.concat("://");
	 pstr.concat(connection.host.c_str());
	 if (connection.port != 80)
	    pstr.sprintf(":%d", connection.port);
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

   DLLLOCAL QoreHashNode *send_internal(const char *meth, const char *mpath, const QoreHashNode *headers, const void *data, unsigned size, bool getbody, QoreHashNode *info, ExceptionSink *xsink, bool suppress_content_length = false);

   DLLLOCAL void addProxyAuthorization(const QoreHashNode *headers, QoreHashNode &h, ExceptionSink *xsink) {
      if (proxy_connection.username.empty())
	 return;

      AbstractQoreNode *pauth = 0;
      // check for "Proxy-Authorization" header
      if (headers) {
	 ConstHashIterator hi(headers);
	 while (hi.next()) {
	    if (!strcasecmp(hi.getKey(), "Proxy-Authorization")) {
	       pauth = hi.getReferencedValue();
	       h.setKeyValue("Proxy-Authorization", pauth, xsink);
	       assert(!*xsink);
	       break;
	    }
	 }
      }

      if (!pauth) {
	 QoreString tmp;
	 tmp.sprintf("%s:%s", proxy_connection.username.c_str(), proxy_connection.password.c_str());
	 QoreStringNode *auth_str = new QoreStringNode("Basic ");
	 auth_str->concatBase64(&tmp);
	 h.setKeyValue("Proxy-Authorization", auth_str, xsink);
	 assert(!*xsink);
      }
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
   priv->setSocketPath();
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

void QoreHTTPClient::setDefaultPort(int def_port) {
   priv->default_port = def_port;
}

const char* QoreHTTPClient::getDefaultPath() const {
   return priv->default_path.empty() ? 0 : priv->default_path.c_str();
}

const char* QoreHTTPClient::getConnectionPath() const {
   return priv->connection.path.empty() ? getDefaultPath() : priv->connection.path.c_str();
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
	    xsink->raiseException("HTTP-CLIENT-OPTION-ERROR", "value of protocol hash key '%s' is not a hash or an int", hi.getKey());
	    return -1;
	 }
	 bool need_ssl = false;
	 int need_port;
	 if (vtype == NT_INT)
	    need_port = (int)((reinterpret_cast<const QoreBigIntNode *>(v))->val);
	 else {
	    const QoreHashNode *vh = reinterpret_cast<const QoreHashNode *>(v);
	    const AbstractQoreNode *p = vh->getKeyValue("port");
	    need_port = p ? p->getAsInt() : 0;
	    if (!need_port) {
	       xsink->raiseException("HTTP-CLIENT-OPTION-ERROR", "'port' key in protocol hash key '%s' is missing or zero", hi.getKey());
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
      if (priv->set_proxy_url_unlocked((reinterpret_cast<const QoreStringNode *>(n))->getBuffer(), xsink))
	 return -1;

   // parse url option if present
   n = opts->getKeyValue("url");  
   if (n && n->getType() == NT_STRING)
      if (priv->set_url_unlocked((reinterpret_cast<const QoreStringNode *>(n))->getBuffer(), xsink))
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
	 xsink->raiseException("HTTP-CLIENT-OPTION-ERROR", "expecting string version ('1.0', '1.1' as value for http_version key in options hash");
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

   if (priv->connection.path.empty())
      priv->connection.path = priv->default_path.empty() ? "/" : priv->default_path;

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
   if (!priv->proxy_connection.port)
      priv->disconnect_unlocked();
   return priv->set_url_unlocked(str, xsink);
}

QoreStringNode *QoreHTTPClient::getURL() {
   SafeLocker sl(priv->m);

   if (!priv->connection.port)
      return 0;

   return priv->connection.get_url();
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

int QoreHTTPClient::setProxyURL(const char *proxy, ExceptionSink *xsink)  {
   SafeLocker sl(priv->m);
   priv->disconnect_unlocked();
   if (!proxy || !proxy[0]) {
      priv->proxy_connection.clear();
      return 0;
   }
   return priv->set_proxy_url_unlocked(proxy, xsink);
}

QoreStringNode *QoreHTTPClient::getProxyURL()  {
   SafeLocker sl(priv->m);

   if (!priv->proxy_connection.port)
      return 0;

   return priv->proxy_connection.get_url();
}

void QoreHTTPClient::clearProxyURL() {
   SafeLocker sl(priv->m);
   priv->proxy_connection.clear();
   priv->setSocketPath();
}

void QoreHTTPClient::setSecure(bool is_secure) { 
   lock();
   priv->connection.ssl = is_secure; 
   unlock();
}

bool QoreHTTPClient::isSecure() const { 
   return priv->connection.ssl; 
}

void QoreHTTPClient::setProxySecure(bool is_secure) { 
   lock();
   priv->proxy_connection.ssl = is_secure; 
   unlock();
}

bool QoreHTTPClient::isProxySecure() const { 
   return priv->proxy_connection.ssl; 
}

long QoreHTTPClient::verifyPeerCertificate() { 
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

QoreHashNode *qore_qtc_private::getResponseHeader(const char *meth, const char *mpath, const QoreHashNode &nh, const void *data, unsigned size, int &code, bool suppress_content_length, QoreHashNode *info, bool with_connect, ExceptionSink *xsink) {
   QoreString pathstr(m_socket.getEncoding());
   const char *msgpath = with_connect ? mpath : getMsgPath(mpath, pathstr);

   if (!connected && connect_unlocked(xsink))
      return 0;

   // send the message
   int rc = m_socket.sendHTTPMessage(xsink, info, meth, msgpath, http11 ? "1.1" : "1.0", &nh, data, size, QORE_SOURCE_HTTPCLIENT, timeout);

   if (rc) {
      assert(*xsink);
      if (rc == QSE_NOT_OPEN)
	 disconnect_unlocked();
      return 0;
   }

   QoreHashNode *ah = 0;
   while (true) {
      ReferenceHolder<QoreHashNode> ans(m_socket.readHTTPHeader(xsink, info, timeout, QORE_SOURCE_HTTPCLIENT), xsink);
      if (!(*ans)) {
	 disconnect_unlocked();
	 assert(*xsink);
	 return 0;
      }

      // check HTTP status code
      AbstractQoreNode *v = ans->getKeyValue("status_code");
      if (!v) {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "no HTTP status code received in response");
	 return 0;
      }
   
      code = v->getAsInt();
      // continue processing if "100 Continue" response received (ignore this response)
      if (code == 100)
	 continue;

      ah = ans.release();
      break;
   }

   return ah;
}

static void do_content_length_event(Queue *cb_queue, int64 id, int len) {
   if (cb_queue) {
      ExceptionSink xsink;
      QoreHashNode* h = new QoreHashNode;
      h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_HTTP_CONTENT_LENGTH), 0);
      h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_HTTPCLIENT), 0);
      h->setKeyValue("id", new QoreBigIntNode(id), 0);
      h->setKeyValue("len", new QoreBigIntNode(len), 0);
      cb_queue->pushAndTakeRef(h);
   }
}

static void do_redirect_event(Queue *cb_queue, int64 id, const QoreStringNode *loc, const QoreStringNode *msg) {
   if (cb_queue) {
      ExceptionSink xsink;
      QoreHashNode* h = new QoreHashNode;
      h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_HTTP_REDIRECT), 0);
      h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_HTTPCLIENT), 0);
      h->setKeyValue("id", new QoreBigIntNode(id), 0);
      h->setKeyValue("location", loc->refSelf(), 0);
      if (msg)
	 h->setKeyValue("status_message", msg->refSelf(), 0);
      cb_queue->pushAndTakeRef(h);
   }
}

static void do_event(Queue *cb_queue, int64 id, int event) {
   if (cb_queue) {
      ExceptionSink xsink;
      QoreHashNode* h = new QoreHashNode;
      h->setKeyValue("event", new QoreBigIntNode(event), 0);
      h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_HTTPCLIENT), 0);
      h->setKeyValue("id", new QoreBigIntNode(id), 0);
      cb_queue->pushAndTakeRef(h);
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

QoreHashNode *qore_qtc_private::send_internal(const char *meth, const char *mpath, const QoreHashNode *headers, const void *data, unsigned size, bool getbody, QoreHashNode *info, ExceptionSink *xsink, bool suppress_content_length) {
   //printd(5, "QoreHTTPClient::send_internal(meth: %s mpath: %s data: %p size: %u info: %p)\n", meth, mpath, data, size, info);

   // check if method is valid
   ccharcase_set_t::const_iterator i = method_set.find(meth);
   if (i == method_set.end()) {
      xsink->raiseException("HTTP-CLIENT-METHOD-ERROR", "HTTP method (%s) not recognized.", meth);
      return 0;
   }
   // make sure the capitalized version is used
   meth = *i;

   SafeLocker sl(m);
   Queue *cb_queue = m_socket.getQueue();

   ReferenceHolder<QoreHashNode> nh(new QoreHashNode, xsink);
   bool keep_alive = true;

   if (headers) {
      ConstHashIterator hi(headers);
      while (hi.next()) {
	 if (!strcasecmp(hi.getKey(), "connection") || (proxy_connection.port && !strcasecmp(hi.getKey(), "proxy-connection"))) {
	    const AbstractQoreNode *v = hi.getValue();
	    if (v && v->getType() == NT_STRING && !strcasecmp((reinterpret_cast<const QoreStringNode *>(v))->getBuffer(), "close"))
	       keep_alive = false;
	 }

	 // if one of the mandatory headers is found, then ignore it
	 strcase_set_t::iterator si = header_ignore.find(hi.getKey());
	 if (si != header_ignore.end())
	    continue;

	 // otherwise set the value in the hash
	 const AbstractQoreNode *n = hi.getValue();
	 if (!is_nothing(n))
	    nh->setKeyValue(hi.getKey(), n->refSelf(), xsink);
      }
   }

   // add default headers if they weren't overridden
   for (header_map_t::const_iterator hdri = default_headers.begin(), e = default_headers.end(); hdri != e; ++hdri) {
      // look in original headers to see if the key was already given
      if (headers) {
	 bool skip = false;
	 ConstHashIterator hi(headers);
	 while (hi.next()) {
	    if (!strcasecmp(hi.getKey(), hdri->first.c_str())) {
	       skip = true;
	       break;
	    }
	 }
	 if (skip)
	    continue;
      }
      // if there is no message body then do not send the "content-type" header
      if (!data && !strcmp(hdri->first.c_str(), "Content-Type"))
	 continue;
      nh->setKeyValue(hdri->first.c_str(), new QoreStringNode(hdri->second.c_str()), xsink);
   }

   if (!connection.username.empty()) {
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
	 tmp.sprintf("%s:%s", connection.username.c_str(), connection.password.c_str());
	 QoreStringNode *auth_str = new QoreStringNode("Basic ");
	 auth_str->concatBase64(&tmp);
	 nh->setKeyValue("Authorization", auth_str, xsink);
      }
   }

   // save original HTTP method in case we have to issue a CONNECT request to a proxy for an HTTPS connection
   const char *meth_orig = meth;

   bool use_proxy_connect = false;
   const char *proxy_path = 0;
   ReferenceHolder<QoreHashNode> proxy_headers(xsink);
   QoreString hostport;
   if (!proxy_connected && proxy_connection.port) {
      // use CONNECT if we need to make an HTTPS connection from the proxy
      if (!proxy_connection.ssl && connection.ssl) {
	 meth = "CONNECT";
	 use_proxy_connect = true;
	 hostport.sprintf("%s:%d", connection.host.c_str(), connection.port);
	 proxy_path = hostport.getBuffer();
	 proxy_headers = new QoreHashNode;
	 proxy_headers->setKeyValue("Host", new QoreStringNode(hostport), xsink);

	 addProxyAuthorization(headers, **proxy_headers, xsink);
      }
      else
	 addProxyAuthorization(headers, **nh, xsink);
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

      //printd(5, "qore_qtc_private::send_internal() meth=%s proxy_path=%s mpath=%s upc=%d\n", meth, proxy_path ? proxy_path : "n/a", mpath, use_proxy_connect);

      // send HTTP message and get response header
      if (use_proxy_connect)
	 ans = getResponseHeader(meth, proxy_path, *(*proxy_headers), 0, 0, code, suppress_content_length, info, true, xsink);
      else
	 ans = getResponseHeader(meth, mpath, *(*nh), data, size, code, suppress_content_length, info, false, xsink);
      if (!ans)
	 return 0;

      if (info) {
	 info->setKeyValue("response-headers", ans->refSelf(), xsink);
	 if (*xsink)
	    return 0;
      }

      if (code >= 300 && code < 400) {
	 if (++redirect_count > max_redirects)
	    break;
	 disconnect_unlocked();

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
	    do_redirect_event(cb_queue, m_socket.getObjectIDForEvents(), loc, mess);

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
      else if (use_proxy_connect) {
	 meth = meth_orig;
	 use_proxy_connect = false;
	 proxy_path = 0;
	 if (m_socket.upgradeClientToSSL(0, 0, xsink))	       
	    return 0;
	 proxy_connected = true;
	 
	 // remove "Proxy-Authorization" header
	 nh->removeKey("Proxy-Authorization", xsink);
	 if (*xsink)
	    return 0;

	 // try again as if we are talking directly to the client
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
      xsink->raiseException("HTTP-CLIENT-MAXIMUM-REDIRECTS-EXCEEDED", "maximum redirections (%d) exceeded; redirect code %d to '%s' ignored (message: '%s')", max_redirects, code, location, mess);
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
	 m_socket.setEncoding(QEM.findCreate(&enc));
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
	       check_headers(str, len, multipart, *(*ans), m_socket.getEncoding(), xsink);
	       l->push(new QoreStringNode(str, len, m_socket.getEncoding()));
	    }
	    str = p + 1;
	 } while ((p = strchr(str, ';')));
	 // skip whitespace
	 while (*str == ' ') str++;
	 // add last field
	 if (*str) {
	    check_headers(str, strlen(str), multipart, *(*ans), m_socket.getEncoding(), xsink);
	    l->push(new QoreStringNode(str, m_socket.getEncoding()));
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
	 m_socket.setEncoding(QEM.findCreate(content_encoding));
	 content_encoding = 0;
      }
   }

   const AbstractQoreNode *te = ans->getKeyValue("transfer-encoding");
   
   // get response body, if any
   v = ans->getKeyValue("content-length");
   int len = v ? v->getAsInt() : 0;

   if (v && cb_queue)
      do_content_length_event(cb_queue, m_socket.getObjectIDForEvents(), len);

   AbstractQoreNode *body = 0;
   if (te && !strcasecmp((reinterpret_cast<const QoreStringNode *>(te))->getBuffer(), "chunked")) { // check for chunked response body
      if (cb_queue)
	 do_event(cb_queue, m_socket.getObjectIDForEvents(), QORE_EVENT_HTTP_CHUNKED_START);
      ReferenceHolder<QoreHashNode> nah(xsink);
      if (content_encoding)
	 nah = m_socket.readHTTPChunkedBodyBinary(timeout, xsink, QORE_SOURCE_HTTPCLIENT);
      else
	 nah = m_socket.readHTTPChunkedBody(timeout, xsink, QORE_SOURCE_HTTPCLIENT);
      if (cb_queue)
	 do_event(cb_queue, m_socket.getObjectIDForEvents(), QORE_EVENT_HTTP_CHUNKED_END);

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
      if (content_encoding) {
	 SimpleRefHolder<BinaryNode> bobj(m_socket.recvBinary(len, timeout, xsink));
	 if (!(*xsink) && bobj)
	    body = bobj.release();
      }
      else {
	 QoreStringNodeHolder bstr(m_socket.recv(len, timeout, xsink));
	 if (!(*xsink) && bstr)
	    body = bstr.release();
      }

      //printf("body=%p\n", body);
   }

   // check for connection: close header
   if (!keep_alive || ((v = ans->getKeyValue("connection")) && !strcasecmp((reinterpret_cast<const QoreStringNode *>(v))->getBuffer(), "close")))
      disconnect_unlocked();

   sl.unlock();

   // for content-encoding processing we can run unlocked

   // add body to result hash and process content encoding if necessary
   if (body && content_encoding) {
      BinaryNode *bobj = reinterpret_cast<BinaryNode *>(body);
      QoreStringNode *str = 0;
      if (!strcasecmp(content_encoding, "deflate") || !strcasecmp(content_encoding, "x-deflate"))
	 str = qore_inflate_to_string(bobj, m_socket.getEncoding(), xsink);
      else if (!strcasecmp(content_encoding, "gzip") || !strcasecmp(content_encoding, "x-gzip"))
	 str = qore_gunzip_to_string(bobj, m_socket.getEncoding(), xsink);
      else if (!strcasecmp(content_encoding, "bzip2") || !strcasecmp(content_encoding, "x-bzip2"))
	 str = qore_bunzip2_to_string(bobj, m_socket.getEncoding(), xsink);
      else {
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "don't know how to handle content-encoding '%s'", content_encoding);
	 ans = 0;
      }
      bobj->deref();
      body = str;
   }

   if (body)
      ans->setKeyValue("body", body, xsink);

   if (code < 200 || code >= 300) {
      v = ans->getKeyValue("status_message");
      const char *mess = v ? (reinterpret_cast<const QoreStringNode *>(v))->getBuffer() : "<no message>";
      xsink->raiseExceptionArg("HTTP-CLIENT-RECEIVE-ERROR", ans.release(), "HTTP status code %d received: message: %s", code, mess);

      return 0;
   }

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

   return priv->send_internal(meth, new_path, headers, data, size, getbody, info, xsink, true);
}
*/

QoreHashNode *QoreHTTPClient::send(const char *meth, const char *new_path, const QoreHashNode *headers, const void *data, unsigned size, bool getbody, QoreHashNode *info, ExceptionSink *xsink) {
   return priv->send_internal(meth, new_path, headers, data, size, getbody, info, xsink);
}

// returns *string
AbstractQoreNode *QoreHTTPClient::get(const char *new_path, const QoreHashNode *headers, QoreHashNode *info, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> ans(priv->send_internal("GET", new_path, headers, 0, 0, true, info, xsink), xsink);
   if (!ans)
      return 0;

   return ans->takeKeyValue("body");
}

QoreHashNode *QoreHTTPClient::head(const char *new_path, const QoreHashNode *headers, QoreHashNode *info, ExceptionSink *xsink) {
   return priv->send_internal("HEAD", new_path, headers, 0, 0, false, info, xsink);
}

// returns *string
AbstractQoreNode *QoreHTTPClient::post(const char *new_path, const QoreHashNode *headers, const void *data, unsigned size, QoreHashNode *info, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> ans(priv->send_internal("POST", new_path, headers, data, size, true, info, xsink), xsink);
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

void QoreHTTPClient::setUserPassword(const char *user, const char *pass) {
   priv->setUserPassword(user, pass);
}

void QoreHTTPClient::clearUserPassword() {
   priv->clearUserPassword();
}

void QoreHTTPClient::setProxyUserPassword(const char *user, const char *pass) {
   priv->setProxyUserPassword(user, pass);
}

void QoreHTTPClient::clearProxyUserPassword() {
   priv->clearProxyUserPassword();
}

QoreHashNode* QoreHTTPClient::getPeerInfo(ExceptionSink* xsink) const {
   return priv->getPeerInfo(xsink);
}

QoreHashNode* QoreHTTPClient::getSocketInfo(ExceptionSink* xsink) const {
   return priv->getSocketInfo(xsink);
}

