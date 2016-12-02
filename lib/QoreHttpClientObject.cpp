/*
  QoreHttpClientObject.cpp

  Qore Programming Language

  Copyright (C) 2006 - 2016 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

/*
  RFC 2616 HTTP 1.1
  RFC 2617 HTTP authentication
  RFC 3986 HTTP URI specification
*/

#include <qore/Qore.h>
#include <qore/QoreURL.h>
#include <qore/QoreHttpClientObject.h>
#include "qore/intern/ql_misc.h"
#include "qore/intern/QC_Socket.h"
#include "qore/intern/QC_Queue.h"
#include "qore/intern/QoreHttpClientObjectIntern.h"

#include "qore/intern/qore_socket_private.h"

#include <string>
#include <map>
#include <set>

#include <ctype.h>

method_map_t method_map;
strcase_set_t header_ignore;

struct qore_httpclient_priv {
   my_socket_priv* msock;

   bool http11;       // are we using http 1.1 or 1.0?
   prot_map_t prot_map;

   con_info connection, proxy_connection;

   bool connected,
      nodelay,
      proxy_connected, // means that a CONNECT message has been processed and the connection is now made as if it were directly with the client
      persistent;      // turns off implicit connections for the current connection only
   int default_port, max_redirects;
   std::string default_path;
   int timeout;
   std::string socketpath;
   header_map_t default_headers;
   int connect_timeout_ms;

   method_map_t additional_methods_map;

   DLLLOCAL qore_httpclient_priv(my_socket_priv* ms) :
      msock(ms), http11(true), connection(HTTPCLIENT_DEFAULT_PORT),
      connected(false), nodelay(false), proxy_connected(false),
      persistent(false),
      default_port(HTTPCLIENT_DEFAULT_PORT),
      max_redirects(HTTPCLIENT_DEFAULT_MAX_REDIRECTS),
      timeout(HTTPCLIENT_DEFAULT_TIMEOUT),
      connect_timeout_ms(-1) {
      assert(ms);
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

   DLLLOCAL ~qore_httpclient_priv() {
   }

   DLLLOCAL void setSocketPathIntern(const con_info& con) {
      if (con.path.empty() || !con.host.empty()) {
	 socketpath = con.host;
	 if (!con.is_unix) {
	    socketpath += ":";
	    char buff[20];
	    sprintf(buff, "%d", con.port);
	    socketpath += buff;
	 }
	 return;
      }

      socketpath = con.path;
   }

   DLLLOCAL void setSocketPath() {
      setSocketPathIntern(proxy_connection.has_url() ? proxy_connection : connection);
      //printd(5, "setSocketPath() '%s'\n", socketpath.c_str());
   }

   DLLLOCAL void lock() { msock->m.lock(); }
   DLLLOCAL void unlock() { msock->m.unlock(); }

   // returns -1 if an exception was thrown, 0 for OK
   DLLLOCAL int connect_unlocked(ExceptionSink* xsink) {
      bool connect_ssl = proxy_connection.has_url() ? proxy_connection.ssl : connection.ssl;

      int rc;
      if (connect_ssl)
	 rc = msock->socket->connectSSL(socketpath.c_str(), connect_timeout_ms, msock->cert ? msock->cert->getData() : 0, msock->pk ? msock->pk->getData() : 0, xsink);
      else
	 rc = msock->socket->connect(socketpath.c_str(), connect_timeout_ms, xsink);

      if (!rc) {
	 connected = true;
	 if (nodelay) {
	    if (msock->socket->setNoDelay(1))
	       nodelay = false;
	 }
      }
      return rc;
   }

   DLLLOCAL void disconnect_unlocked() {
      if (connected) {
	 msock->socket->close();
	 connected = false;
	 proxy_connected = false;
	 persistent = false;
      }
   }

   DLLLOCAL int setNoDelay(bool nd) {
      AutoLocker al(msock->m);

      if (!connected) {
	 nodelay = true;
	 return 0;
      }

      if (nodelay)
	 return 0;

      if (msock->socket->setNoDelay(1))
	 return -1;

      nodelay = true;
      return 0;
   }

   DLLLOCAL bool getNoDelay() const {
      return nodelay;
   }

   DLLLOCAL void setPersistent(ExceptionSink* xsink) {
      AutoLocker al(msock->m);

      if (!connected) {
	 xsink->raiseException("PERSISTENCE-ERROR", "HTTPClient::setPersistent() can only be called once an initial connection has been established; currently there is no connection to the server");
	 return;
      }

      if (!persistent)
	 persistent = true;
   }

   DLLLOCAL int set_url_unlocked(const char* str, ExceptionSink* xsink) {
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
	 if (!port_set && !connection.is_unix)
	    connection.port = get_port(i->second);

	 // set SSL setting from protocol default
	 connection.ssl = get_ssl(i->second);
      }
      else {
	 connection.ssl = false;
	 if (!port_set)
	    connection.port = default_port;
      }

      if (!proxy_connection.has_url())
	 setSocketPath();

      return 0;
   }

   DLLLOCAL int set_proxy_url_unlocked(const char* pstr, ExceptionSink* xsink) {
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
	 if (!port_set && !proxy_connection.is_unix)
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

   DLLLOCAL void setUserPassword(const char* user, const char* pass) {
      assert(user && pass);
      AutoLocker al(msock->m);

      connection.setUserPassword(user, pass);
   }

   DLLLOCAL void clearUserPassword() {
      AutoLocker al(msock->m);
      connection.clearUserPassword();
   }

   DLLLOCAL void setProxyUserPassword(const char* user, const char* pass) {
      assert(user && pass);
      AutoLocker al(msock->m);

      proxy_connection.setUserPassword(user, pass);
   }

   DLLLOCAL void clearProxyUserPassword() {
      AutoLocker al(msock->m);
      proxy_connection.clearUserPassword();
   }

   DLLLOCAL void addHttpMethod(const char* method, bool enable) {
       additional_methods_map.insert(method_map_t::value_type(method, enable));
   }


   // always generate a Host header pointing to the host hosting the resource, not the proxy
   // (RFC 2616 is not totally clear on this, but other clients do it this way)
   DLLLOCAL AbstractQoreNode* getHostHeaderValue() {
      if (connection.port == 80)
	 return new QoreStringNode(connection.host.c_str());

      QoreStringNode* str = new QoreStringNode;
      str->concat(connection.host);
      if (connection.port)
	 str->sprintf(":%d", connection.port);
      return str;
   }

   DLLLOCAL QoreHashNode* sendMessageAndGetResponse(const char* mname, const char* meth, const char* mpath, const QoreHashNode& nh, const void* data, unsigned size, const ResolvedCallReferenceNode* send_callback, QoreHashNode* info, bool with_connect, int timeout_ms, int& code, bool& aborted, ExceptionSink* xsink);

   DLLLOCAL const char* getMsgPath(const char* mpath, QoreString &pstr) {
      pstr.clear();

      // use default path if no path is set
      if (!mpath || !mpath[0])
	 mpath = connection.path.empty()
	    ? (default_path.empty() ? "/" : (const char*)default_path.c_str())
	    : (const char*)connection.path.c_str();

      if (proxy_connection.has_url()) {
         // create URL string for path for proxy
	 pstr.concat("http");
	 if (connection.ssl)
	    pstr.concat('s');
	 pstr.concat("://");
	 pstr.concat(connection.host.c_str());
	 if (connection.port && connection.port != 80)
	    pstr.sprintf(":%d", connection.port);
	 if (mpath[0] != '/')
	    pstr.concat('/');
      }

      // concat mpath to pstr, performing URL encoding
      const char* p = mpath;
      while (*p) {
	 // encode spaces only
	 if (*p == ' ')
	    pstr.concat("%20");
	 // according to RFC 3986 it's not necessary to encode non-ascii characters
	 else
	    pstr.concat(*p);
	 ++p;
      }
      return (const char*)pstr.getBuffer();
   }

   DLLLOCAL QoreHashNode* send_internal(ExceptionSink* xsink, const char* mname, const char* meth, const char* mpath, const QoreHashNode* headers, const void* data, unsigned size, const ResolvedCallReferenceNode* send_callback, bool getbody, QoreHashNode* info, int timeout_ms, const ResolvedCallReferenceNode* recv_callback = 0, QoreObject* obj = 0, OutputStream *os = 0);

   DLLLOCAL void addProxyAuthorization(const QoreHashNode* headers, QoreHashNode& h, ExceptionSink* xsink) {
      if (proxy_connection.username.empty())
	 return;

      AbstractQoreNode* pauth = 0;
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
	 QoreStringNode* auth_str = new QoreStringNode("Basic ");
	 auth_str->concatBase64(&tmp);
	 h.setKeyValue("Proxy-Authorization", auth_str, xsink);
	 assert(!*xsink);
      }
   }
};

// static initialization
void QoreHttpClientObject::static_init() {
   // setup static members of QoreHttpClientObject class
   method_map.insert(method_map_t::value_type("OPTIONS", true));
   method_map.insert(method_map_t::value_type("GET", true));
   method_map.insert(method_map_t::value_type("HEAD", false));
   method_map.insert(method_map_t::value_type("POST", true));
   method_map.insert(method_map_t::value_type("PUT", true));
   method_map.insert(method_map_t::value_type("DELETE", true));
   method_map.insert(method_map_t::value_type("TRACE", true));
   method_map.insert(method_map_t::value_type("CONNECT", true));

   header_ignore.insert("Content-Length");
}

QoreHttpClientObject::QoreHttpClientObject() : http_priv(new qore_httpclient_priv(priv)) {
   http_priv->setSocketPath();
}

QoreHttpClientObject::~QoreHttpClientObject() {
   delete http_priv;
}

void QoreHttpClientObject::deref(ExceptionSink* xsink) {
    if (ROdereference()) {
	cleanup(xsink);
	delete this;
    }
}

void QoreHttpClientObject::setDefaultPort(int def_port) {
   http_priv->default_port = def_port;
}

const char* QoreHttpClientObject::getDefaultPath() const {
   return http_priv->default_path.empty() ? 0 : http_priv->default_path.c_str();
}

const char* QoreHttpClientObject::getConnectionPath() const {
   return http_priv->connection.path.empty() ? getDefaultPath() : http_priv->connection.path.c_str();
}

void QoreHttpClientObject::setDefaultPath(const char* def_path) {
   http_priv->default_path = def_path;
}

void QoreHttpClientObject::setTimeout(int to) {
   http_priv->timeout = to;
}

int QoreHttpClientObject::getTimeout() const {
   return http_priv->timeout;
}

void QoreHttpClientObject::setEncoding(const QoreEncoding *qe) {
   priv->socket->setEncoding(qe);
}

const QoreEncoding *QoreHttpClientObject::getEncoding() const {
   return priv->socket->getEncoding();
}

int QoreHttpClientObject::setOptions(const QoreHashNode* opts, ExceptionSink* xsink) {
   // process new protocols
   const AbstractQoreNode* n = opts->getKeyValue("protocols");

   if (n && n->getType() == NT_HASH) {
      const QoreHashNode* h = reinterpret_cast<const QoreHashNode* >(n);
      ConstHashIterator hi(h);
      while (hi.next()) {
	 const AbstractQoreNode* v = hi.getValue();
	 qore_type_t vtype = v ? v->getType() : 0;
	 if (!v || (vtype != NT_HASH && vtype != NT_INT)) {
	    xsink->raiseException("HTTP-CLIENT-OPTION-ERROR", "value of protocol hash key '%s' is not a hash or an int", hi.getKey());
	    return -1;
	 }
	 bool need_ssl = false;
	 int need_port;
	 if (vtype == NT_INT)
	    need_port = (int)((reinterpret_cast<const QoreBigIntNode* >(v))->val);
	 else {
	    const QoreHashNode* vh = reinterpret_cast<const QoreHashNode* >(v);
	    const AbstractQoreNode* p = vh->getKeyValue("port");
	    need_port = p ? p->getAsInt() : 0;
	    if (!need_port) {
	       xsink->raiseException("HTTP-CLIENT-OPTION-ERROR", "'port' key in protocol hash key '%s' is missing or zero", hi.getKey());
	       return -1;
	    }
	    p = vh->getKeyValue("ssl");
	    need_ssl = p ? p->getAsBool() : false;
	 }
	 http_priv->prot_map[hi.getKey()] = make_protocol(need_port, need_ssl);
      }
   }

   n = opts->getKeyValue("max_redirects");
   if (n)
      http_priv->max_redirects = n->getAsInt();

   n = opts->getKeyValue("default_port");
   if (n)
      http_priv->default_port = n->getAsInt();
   else
      http_priv->default_port = HTTPCLIENT_DEFAULT_PORT;

   // check if proxy is true
   n = opts->getKeyValue("proxy");
   if (n && n->getType() == NT_STRING)
      if (http_priv->set_proxy_url_unlocked((reinterpret_cast<const QoreStringNode* >(n))->getBuffer(), xsink))
	 return -1;

   // parse url option if present
   n = opts->getKeyValue("url");
   if (n && n->getType() == NT_STRING)
      if (http_priv->set_url_unlocked((reinterpret_cast<const QoreStringNode* >(n))->getBuffer(), xsink))
	 return -1;

   n = opts->getKeyValue("default_path");
   if (n && n->getType() == NT_STRING)
      http_priv->default_path = (reinterpret_cast<const QoreStringNode* >(n))->getBuffer();

   // set default timeout if given in option hash - accept relative date/time values as well as integers
   n = opts->getKeyValue("timeout");
   if (n)
      http_priv->timeout = getMsZeroInt(n);

   n = opts->getKeyValue("http_version");
   if (n) {
      if (n->getType() == NT_STRING) {
	 if (setHTTPVersion((reinterpret_cast<const QoreStringNode* >(n))->getBuffer(), xsink))
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
	   priv->socket->setEventQueue(q, xsink);
       }
   }

   http_priv->connect_timeout_ms = getMsMinusOneInt(opts->getKeyValue("connect_timeout"));

   if (http_priv->connection.path.empty())
      http_priv->connection.path = http_priv->default_path.empty() ? "/" : http_priv->default_path;

   // additional HTTP methods for customized extensions like WebDAV
   n = opts->getKeyValue("additional_methods");
   if (n) {
       const QoreHashNode *h = n->getType() == NT_HASH ? reinterpret_cast<const QoreHashNode*>(n) : 0;
       if (!h) {
           xsink->raiseException("HTTP-CLIENT-OPTION-ERROR", "Option additional_methods requires a hash as a value; got: %s", n->getTypeName());
           return -1;
       }
       ConstHashIterator hi(h);
       while (hi.next()) {
           http_priv->addHttpMethod(hi.getKey(), hi.getValue()->getAsBool());
       }
   }

   return 0;
}

void QoreHttpClientObject::setConnectTimeout(int ms) {
   SafeLocker sl(priv->m);
   http_priv->connect_timeout_ms = ms < 0 ? -1 : ms;
}

int QoreHttpClientObject::getConnectTimeout() const {
   return http_priv->connect_timeout_ms;
}

int QoreHttpClientObject::setURL(const char* str, ExceptionSink* xsink) {
   SafeLocker sl(priv->m);
   // disconnect immediately if not using a proxy
   if (!http_priv->proxy_connection.has_url())
      http_priv->disconnect_unlocked();
   return http_priv->set_url_unlocked(str, xsink);
}

QoreStringNode* QoreHttpClientObject::getURL() {
   SafeLocker sl(priv->m);

   if (!http_priv->connection.has_url())
      return 0;

   return http_priv->connection.get_url();
}

int QoreHttpClientObject::setHTTPVersion(const char* version, ExceptionSink* xsink) {
   int rc = 0;
   SafeLocker sl(priv->m);
   if (!strcmp(version, "1.0"))
      http_priv->http11 = false;
   else if (!strcmp(version, "1.1"))
      http_priv->http11 = true;
   else {
      xsink->raiseException("HTTP-VERSION-ERROR", "only '1.0' and '1.1' are valid (value passed: '%s')", version);
      rc = -1;
   }
   return rc;
}

const char* QoreHttpClientObject::getHTTPVersion() const {
   return http_priv->http11 ? "1.1" : "1.0";
}

void QoreHttpClientObject::setHTTP11(bool val) {
   http_priv->http11 = val;
}

bool QoreHttpClientObject::isHTTP11() const {
   return http_priv->http11;
}

int QoreHttpClientObject::setProxyURL(const char* proxy, ExceptionSink* xsink)  {
   SafeLocker sl(priv->m);
   http_priv->disconnect_unlocked();
   if (!proxy || !proxy[0]) {
      http_priv->proxy_connection.clear();
      return 0;
   }
   return http_priv->set_proxy_url_unlocked(proxy, xsink);
}

QoreStringNode* QoreHttpClientObject::getProxyURL()  {
   SafeLocker sl(priv->m);

   if (!http_priv->proxy_connection.has_url())
      return 0;

   return http_priv->proxy_connection.get_url();
}

void QoreHttpClientObject::clearProxyURL() {
   SafeLocker sl(priv->m);
   http_priv->proxy_connection.clear();
   http_priv->setSocketPath();
}

void QoreHttpClientObject::setSecure(bool is_secure) {
   lock();
   http_priv->connection.ssl = is_secure;
   unlock();
}

bool QoreHttpClientObject::isSecure() const {
   return http_priv->connection.ssl;
}

void QoreHttpClientObject::setProxySecure(bool is_secure) {
   lock();
   http_priv->proxy_connection.ssl = is_secure;
   unlock();
}

bool QoreHttpClientObject::isProxySecure() const {
   return http_priv->proxy_connection.ssl;
}

int QoreHttpClientObject::connect(ExceptionSink* xsink) {
   SafeLocker sl(priv->m);
   return http_priv->connect_unlocked(xsink);
}

void QoreHttpClientObject::disconnect() {
   SafeLocker sl(priv->m);
   http_priv->disconnect_unlocked();
}

QoreHashNode* qore_httpclient_priv::sendMessageAndGetResponse(const char* mname, const char* meth, const char* mpath, const QoreHashNode& nh, const void* data, unsigned size, const ResolvedCallReferenceNode* send_callback, QoreHashNode* info, bool with_connect, int timeout_ms, int& code, bool& aborted, ExceptionSink* xsink) {
   QoreString pathstr(msock->socket->getEncoding());
   const char* msgpath = with_connect ? mpath : getMsgPath(mpath, pathstr);

   if (!connected) {
      if (persistent) {
	 xsink->raiseException("PERSISTENCE-ERROR", "the current connection has been temporarily marked as persistent, but has been disconnected");
	 return 0;
      }

      if (connect_unlocked(xsink)) {
	 // if we have an info hash then write the request-uri key for reporting/logging purposes
	 if (info)
	    info->setKeyValue("request-uri", new QoreStringNodeMaker("%s %s HTTP/%s", meth, msgpath && msgpath[0] ? msgpath : "/", http11 ? "1.1" : "1.0"), 0);
	 return 0;
      }
   }

   // send the message
   int rc = msock->socket->priv->sendHttpMessage(xsink, info, "HTTPClient", mname, meth, msgpath, http11 ? "1.1" : "1.0", &nh, data, size, send_callback, QORE_SOURCE_HTTPCLIENT, timeout_ms, &msock->m, &aborted);

   //printd(5, "qore_httpclient_priv::sendMessageAndGetResponse() '%s' path: '%s' send_callback: %p aborted: %d rc: %d\n", meth, msgpath, send_callback, aborted, rc);

   // do not exit immediately if the transfer was aborted with a streaming send unless the socket was already closed
   if (rc && (!send_callback || !aborted || !msock->socket->isOpen())) {
      assert(*xsink);
      if (rc == QSE_NOT_OPEN)
	 disconnect_unlocked();
      return 0;
   }

   // if the transfer was aborted with a streaming send, but the socket is still open, then try to read a response
   QoreHashNode* ah = 0;
   while (true) {
      ReferenceHolder<QoreHashNode> ans(msock->socket->readHTTPHeader(xsink, info, timeout, QORE_SOURCE_HTTPCLIENT), xsink);
      if (!(*ans)) {
	 disconnect_unlocked();
	 assert(*xsink);
	 return 0;
      }

      // check HTTP status code
      AbstractQoreNode* v = ans->getKeyValue("status_code");
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

   // only clear exceptions if a streaming (ie chunked) send was aborted and we really got a response from the remote
   if (*xsink) {
      assert(aborted);
      xsink->clear();
   }
   return ah;
}

void do_content_length_event(Queue *cb_queue, int64 id, int len) {
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

void do_redirect_event(Queue *cb_queue, int64 id, const QoreStringNode* loc, const QoreStringNode* msg) {
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

void do_event(Queue *cb_queue, int64 id, int event) {
   if (cb_queue) {
      ExceptionSink xsink;
      QoreHashNode* h = new QoreHashNode;
      h->setKeyValue("event", new QoreBigIntNode(event), 0);
      h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_HTTPCLIENT), 0);
      h->setKeyValue("id", new QoreBigIntNode(id), 0);
      cb_queue->pushAndTakeRef(h);
   }
}

void check_headers(const char* str, int len, bool &multipart, QoreHashNode& ans, const QoreEncoding *enc, ExceptionSink* xsink) {
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

static const QoreStringNode* get_string_header_node(ExceptionSink* xsink, QoreHashNode& h, const char* header, bool allow_multiple = false) {
   AbstractQoreNode* n = h.getKeyValue(header);
   if (!n)
      return 0;

   qore_type_t t = get_node_type(n);
   if (t == NT_STRING)
      return reinterpret_cast<const QoreStringNode*>(n);
   assert(t == NT_LIST);
   if (!allow_multiple) {
      xsink->raiseException("HTTP-HEADER-ERROR", "multiple \"%s\" headers received in HTTP message", header);
      return 0;
   }
   // convert list to a comma-separated string
   QoreListNode* l = reinterpret_cast<QoreListNode*>(n);
   // get first list entry
   n = l->retrieve_entry(0);
   assert(get_node_type(n) == NT_STRING);
   QoreStringNode* rv = reinterpret_cast<QoreStringNode*>(n)->copy();
   for (size_t i = 1; i < l->size(); ++i) {
      n = l->retrieve_entry(i);
      assert(get_node_type(n) == NT_STRING);
      rv->concat(',');
      rv->concat(reinterpret_cast<QoreStringNode*>(n));
   }
   // dereference old list and save reference to return value in header hash
   h.setKeyValue(header, rv, xsink);
   return rv;
}

static const char* get_string_header(ExceptionSink* xsink, QoreHashNode& h, const char* header, bool allow_multiple = false) {
   const QoreStringNode* str = get_string_header_node(xsink, h, header, allow_multiple);
   return str && !str->empty() ? str->getBuffer() : 0;
}

QoreHashNode* qore_httpclient_priv::send_internal(ExceptionSink* xsink, const char* mname, const char* meth, const char* mpath, const QoreHashNode* headers, const void* data, unsigned size, const ResolvedCallReferenceNode* send_callback, bool getbody, QoreHashNode* info, int timeout_ms, const ResolvedCallReferenceNode* recv_callback, QoreObject* obj, OutputStream *os) {
   assert(!(data && send_callback));

   // check if method is valid
   method_map_t::const_iterator i = method_map.find(meth);
   if (i == method_map.end()) {
       i = additional_methods_map.find(meth);
       if (i == additional_methods_map.end()) {
          xsink->raiseException("HTTP-CLIENT-METHOD-ERROR", "HTTP method (%s) not recognized.", meth);
          return 0;
       }
   }

   // make sure the capitalized version is used
   meth = i->first.c_str();
   bool bodyp = i->second;

   // use the default timeout value if a zero value is given in the call
   if (!timeout_ms)
      timeout_ms = timeout;

   SafeLocker sl(msock->m);
   Queue* cb_queue = msock->socket->getQueue();

   ReferenceHolder<QoreHashNode> nh(new QoreHashNode, xsink);
   bool keep_alive = true;

   bool transfer_encoding = false;

   if (headers) {
      ConstHashIterator hi(headers);
      while (hi.next()) {
	 // if one of the mandatory headers is found, then ignore it
	 strcase_set_t::iterator si = header_ignore.find(hi.getKey());
	 if (si != header_ignore.end())
	    continue;

	 // otherwise set the value in the hash
	 const AbstractQoreNode* n = hi.getValue();
	 if (!is_nothing(n)) {
	    if (!strcasecmp(hi.getKey(), "transfer-encoding"))
	       transfer_encoding = true;

	    nh->setKeyValue(hi.getKey(), n->refSelf(), xsink);

	    if (!strcasecmp(hi.getKey(), "connection") || (proxy_connection.has_url() && !strcasecmp(hi.getKey(), "proxy-connection"))) {
	       const char* conn = get_string_header(xsink, **nh, hi.getKey(), true);
	       if (*xsink) {
		  disconnect_unlocked();
		  return 0;
	       }
	       if (conn && !strcasecmp(conn, "close"))
		  keep_alive = false;
	    }
	 }
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
      if (!data && !send_callback && !strcmp(hdri->first.c_str(), "Content-Type"))
	 continue;
      nh->setKeyValue(hdri->first.c_str(), new QoreStringNode(hdri->second.c_str()), xsink);
   }

   // set Transfer-Encoding: chunked if used with a send callback
   if (send_callback && !transfer_encoding)
      nh->setKeyValue("Transfer-Encoding", new QoreStringNode("chunked"), xsink);

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
	 QoreStringNode* auth_str = new QoreStringNode("Basic ");
	 auth_str->concatBase64(&tmp);
	 nh->setKeyValue("Authorization", auth_str, xsink);
      }
   }

   // save original HTTP method in case we have to issue a CONNECT request to a proxy for an HTTPS connection
   const char* meth_orig = meth;

   bool use_proxy_connect = false;
   const char* proxy_path = 0;
   ReferenceHolder<QoreHashNode> proxy_headers(xsink);
   QoreString hostport;
   if (!proxy_connected && proxy_connection.has_url()) {
      // use CONNECT if we need to make an HTTPS connection from the proxy
      if (!proxy_connection.ssl && connection.ssl) {
	 meth = "CONNECT";
	 use_proxy_connect = true;
	 hostport.concat(connection.host);
	 if (connection.port)
	    hostport.sprintf(":%d", connection.port);
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
   const char* location = 0;

   // flag for aborted chunked sends
   bool send_aborted = false;

   while (true) {
      // set host field automatically if not overridden
      if (!host_override)
	 nh->setKeyValue("Host", getHostHeaderValue(), xsink);

      if (info) {
	 info->setKeyValue("headers", nh->copy(), xsink);
	 if (*xsink)
	    return 0;
      }

      //printd(5, "qore_httpclient_priv::send_internal() meth=%s proxy_path=%s mpath=%s upc=%d\n", meth, proxy_path ? proxy_path : "n/a", mpath, use_proxy_connect);
      // send HTTP message and get response header
      if (use_proxy_connect)
	 ans = sendMessageAndGetResponse(mname, meth, proxy_path, *(*proxy_headers), 0, 0, 0, info, true, timeout_ms, code, send_aborted, xsink);
      else
	 ans = sendMessageAndGetResponse(mname, meth, mpath, *(*nh), data, size, send_callback, info, false, timeout_ms, code, send_aborted, xsink);
      if (!ans)
	 return 0;

      if (info) {
	 info->setKeyValue("response-headers", ans->refSelf(), xsink);
	 if (*xsink)
	    return 0;
      }

      if (!ans->is_unique())
         ans = ans->copy();

      if (code >= 300 && code < 400) {
	 disconnect_unlocked();

	 host_override = false;
	 const QoreStringNode* mess = reinterpret_cast<QoreStringNode*>(ans->getKeyValue("status_message"));

	 const QoreStringNode* loc = get_string_header_node(xsink, **ans, "location");
	 if (*xsink)
	    return 0;
	 const char* location = loc && !loc->empty() ? loc->getBuffer() : 0;
	 if (!location) {
	    sl.unlock();
	    const char* msg = mess ? mess->getBuffer() : "<no message>";
	    xsink->raiseException("HTTP-CLIENT-REDIRECT-ERROR", "no redirect location given for status code %d: message: '%s'", code, msg);
	    return 0;
	 }

	 if (cb_queue)
	    do_redirect_event(cb_queue, msock->socket->getObjectIDForEvents(), loc, mess);

	 if (++redirect_count > max_redirects)
	    break;

	 if (set_url_unlocked(location, xsink)) {
	    sl.unlock();
	    const char* msg = mess ? mess->getBuffer() : "<no message>";
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

	 // FIXME: reset send callback and send_aborted here

	 // set mpath to NULL so that the new path will be taken
	 mpath = 0;
	 continue;
      }
      else if (use_proxy_connect) {
	 meth = meth_orig;
	 use_proxy_connect = false;
	 proxy_path = 0;
	 if (msock->socket->upgradeClientToSSL(0, 0, xsink)) {
	    disconnect_unlocked();
	    return 0;
	 }
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
      const char* mess = get_string_header(xsink, **ans, "status_message");
      if (!mess)
	 mess = "<no message>";
      if (!location)
	 location = "<no location>";
      xsink->raiseException("HTTP-CLIENT-MAXIMUM-REDIRECTS-EXCEEDED", "maximum redirections (%d) exceeded; redirect code %d to '%s' ignored (message: '%s')", max_redirects, code, location, mess);
      return 0;
   }

   // process content-type
   const QoreStringNode* v = get_string_header_node(xsink, **ans, "content-type");
   if (*xsink) {
      disconnect_unlocked();
      return 0;
   }

   // see if there is a character set specification in the content-type header
   if (v) {
      // save original content-type header before processing
      ans->setKeyValue("_qore_orig_content_type", v->refSelf(), xsink);

      const char* str = v->getBuffer();
      const char* p = strstr(str, "charset=");
      if (p && (p == str || *(p - 1) == ';' || *(p - 1) == ' ')) {
	 // move p to start of encoding
	 const char* c = p + 8;
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

	 printd(5, "QoreHttpClientObject::send_intern() setting encoding to '%s' from content-type header: '%s' (cs=%p c=%p %d)\n", enc.getBuffer(), str, p + 8, c);

	 // set new encoding
	 msock->socket->setEncoding(QEM.findCreate(&enc));
	 // strip from content-type
	 QoreStringNode* nc = new QoreStringNode;
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
	 QoreListNode* l = new QoreListNode;
	 do {
	    // skip whitespace
	    while (*str == ' ') str++;
	    if (str != p) {
	       int len = p - str;
	       check_headers(str, len, multipart, *(*ans), msock->socket->getEncoding(), xsink);
	       l->push(new QoreStringNode(str, len, msock->socket->getEncoding()));
	    }
	    str = p + 1;
	 } while ((p = strchr(str, ';')));
	 // skip whitespace
	 while (*str == ' ') str++;
	 // add last field
	 if (*str) {
	    check_headers(str, strlen(str), multipart, *(*ans), msock->socket->getEncoding(), xsink);
	    l->push(new QoreStringNode(str, msock->socket->getEncoding()));
	 }
	 ans->setKeyValue("content-type", l, xsink);
      }
   }

   // send headers to recv_callback
   if (recv_callback && msock->socket->priv->runHeaderCallback(xsink, "HTTPClient", mname, *recv_callback, &msock->m, *ans, send_aborted, obj))
      return 0;

   AbstractQoreNode* body = 0;
   const char* content_encoding = 0;

   // do not read any message body for messages that cannot have one
   // rfc 2616 4.4 p1 (http://tools.ietf.org/html/rfc2616#section-4.4)
   /*
     1.Any response message which "MUST NOT" include a message-body (such
     as the 1xx, 204, and 304 responses and any response to a HEAD
     request) is always terminated by the first empty line after the
     header fields, regardless of the entity-header fields present in
     the message.
    */
   //printd(5, "qore_httpclient_priv::send_internal() this: %p bodyp: %d code: %d\n", this, bodyp, code);

   qore_uncompress_to_string_t dec = 0;

   // code >= 300 && < 400 is already handled above
   if (bodyp && (code < 100 || code >= 200) && code != 204) {
      // see if we should do a binary or string read
      content_encoding = get_string_header(xsink, **ans, "content-encoding");
      if (*xsink) {
	 disconnect_unlocked();
	 return 0;
      }

      if (content_encoding && !os) {
	 // check for misuse (? not sure: check RFCs again) of this field by including a character encoding value
	 if (!strncasecmp(content_encoding, "iso", 3) || !strncasecmp(content_encoding, "utf-", 4)) {
	    msock->socket->setEncoding(QEM.findCreate(content_encoding));
	    content_encoding = 0;
	 }
	 else if (!recv_callback) {
	    // only decode message bodies automatically if there is no receive callback
	    if (!strcasecmp(content_encoding, "deflate") || !strcasecmp(content_encoding, "x-deflate"))
	       dec = qore_inflate_to_string;
	    else if (!strcasecmp(content_encoding, "gzip") || !strcasecmp(content_encoding, "x-gzip"))
	       dec = qore_gunzip_to_string;
	    else if (!strcasecmp(content_encoding, "bzip2") || !strcasecmp(content_encoding, "x-bzip2"))
	       dec = qore_bunzip2_to_string;
	 }
      }

      const char* te = get_string_header(xsink, **ans, "transfer-encoding");
      if (*xsink) {
	 disconnect_unlocked();
	 return 0;
      }

      // get response body, if any
      const char* cl = get_string_header(xsink, **ans, "content-length");
      if (*xsink) {
	 disconnect_unlocked();
	 return 0;
      }
      int len = cl ? atoi(cl) : 0;
      // do not try to get a body in any case if Content-Length: 0 is sent
      if (cl && !len)
	 getbody = false;

      if (cl && cb_queue)
	 do_content_length_event(cb_queue, msock->socket->getObjectIDForEvents(), len);

      if (te && !strcasecmp(te, "chunked")) { // check for chunked response body
	 if (cb_queue)
	    do_event(cb_queue, msock->socket->getObjectIDForEvents(), QORE_EVENT_HTTP_CHUNKED_START);
	 ReferenceHolder<QoreHashNode> nah(xsink);
	 if (os) {
            msock->socket->priv->readHttpChunkedBodyBinary(timeout_ms, xsink, "HTTPClient", QORE_SOURCE_HTTPCLIENT, 0, &msock->m, 0, os);
	 } else if (recv_callback) {
	    if (content_encoding)
	       msock->socket->priv->readHttpChunkedBodyBinary(timeout_ms, xsink, "HTTPClient", QORE_SOURCE_HTTPCLIENT, recv_callback, &msock->m, obj);
	    else
	       msock->socket->priv->readHttpChunkedBody(timeout_ms, xsink, "HTTPClient", QORE_SOURCE_HTTPCLIENT, recv_callback, &msock->m, obj);
	 }
	 else {
	    if (content_encoding)
	       nah = msock->socket->priv->readHttpChunkedBodyBinary(timeout_ms, xsink, "HTTPClient", QORE_SOURCE_HTTPCLIENT);
	    else
	       nah = msock->socket->priv->readHttpChunkedBody(timeout_ms, xsink, "HTTPClient", QORE_SOURCE_HTTPCLIENT);
	 }
	 if (cb_queue)
	    do_event(cb_queue, msock->socket->getObjectIDForEvents(), QORE_EVENT_HTTP_CHUNKED_END);

	 if (!nah && !recv_callback) {
	    if (!msock->socket->isOpen())
	       disconnect_unlocked();
	    return 0;
	 }

         if (info)
	    info->setKeyValue("chunked", &True, xsink);

         if (*xsink)
            return 0;

	 if (!recv_callback && !os) {
            // merge all keys except the "body" key into ans
            ConstHashIterator hi(*nah);
            while (hi.next()) {
               if (!strcmp(hi.getKey(), "body")) {
		  assert(!body);
		  body = hi.getReferencedValue();
                  continue;
	       }
               ans->setKeyValue(hi.getKey(), hi.getReferencedValue(), xsink);
               if (*xsink)
                  return 0;
            }
	 }
      }
      else if (getbody || len) {
         if (os) {
            msock->socket->priv->recvToOutputStream(os, len, timeout_ms, xsink, &msock->m);
         } else if (content_encoding) {
	    SimpleRefHolder<BinaryNode> bobj(msock->socket->recvBinary(len, timeout_ms, xsink));
	    if (!(*xsink) && bobj)
	       body = bobj.release();
	 }
	 else {
	    QoreStringNodeHolder bstr(msock->socket->recv(len, timeout_ms, xsink));
	    if (!(*xsink) && bstr)
	       body = bstr.release();
	 }

	 if (*xsink && !msock->socket->isOpen())
	    disconnect_unlocked();
	 //printf("body=%p\n", body);
      }
   }

   // check for connection: close header
   if (!keep_alive)
      disconnect_unlocked();
   else {
      const char* conn = get_string_header(xsink, **ans, "connection", true);
      if (*xsink) {
	 disconnect_unlocked();
	 return 0;
      }
      if (conn && !strcasecmp(conn, "close"))
	 disconnect_unlocked();
   }

   sl.unlock();

   // for content-encoding processing we can run unlocked

   // add body to result hash and process content encoding if necessary
   if (body) {
      if (content_encoding) {
	 if (!dec) {
	    if (!recv_callback) {
	       xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "don't know how to handle content-encoding '%s'", content_encoding);
	       ans = 0;
	    }
	 }
	 else {
	    BinaryNode* bobj = reinterpret_cast<BinaryNode*>(body);
	    QoreStringNode* str = dec(bobj, msock->socket->getEncoding(), xsink);
	    bobj->deref();
	    body = str;
	 }
      }

      if (body) {
	 // send data to recv_callback (already unlocked)
	 if (recv_callback) {
	    ReferenceHolder<> bh(body, xsink);
	    if (msock->socket->priv->runDataCallback(xsink, "HTTPClient", mname, *recv_callback, 0, body, false)
		|| msock->socket->priv->runHeaderCallback(xsink, "HTTPClient", mname, *recv_callback, 0, 0, send_aborted, obj))
	       return 0;
	 }
	 else {
	    ans->setKeyValue("body", body, xsink);
         }
      }
   }

   // do not throw an exception if a receive callback is used
   if (!recv_callback && !*xsink && (code < 100 || code >= 300)) {
      const char* mess = get_string_header(xsink, **ans, "status_message");
      if (!mess)
	 mess = "<no message>";
      assert(!*xsink);

      xsink->raiseExceptionArg("HTTP-CLIENT-RECEIVE-ERROR", ans.release(), "HTTP status code %d received: message: %s", code, mess);
      return 0;
   }

   return *xsink || recv_callback || os ? 0 : ans.release();
}

QoreHashNode* QoreHttpClientObject::send(const char* meth, const char* new_path, const QoreHashNode* headers, const void* data, unsigned size, bool getbody, QoreHashNode* info, ExceptionSink* xsink) {
   return http_priv->send_internal(xsink, "send", meth, new_path, headers, data, size, 0, getbody, info, http_priv->timeout, 0);
}

QoreHashNode* QoreHttpClientObject::sendWithSendCallback(const char* meth, const char* mpath, const QoreHashNode* headers, const ResolvedCallReferenceNode* send_callback, bool getbody, QoreHashNode* info, int timeout_ms, ExceptionSink* xsink) {
   return http_priv->send_internal(xsink, "sendWithSendCallback", meth, mpath, headers, 0, 0, send_callback, getbody, info, timeout_ms, 0);
}

void QoreHttpClientObject::sendWithRecvCallback(const char* meth, const char* mpath, const QoreHashNode* headers, const void* data, unsigned size, bool getbody, QoreHashNode* info, int timeout_ms, const ResolvedCallReferenceNode* recv_callback, QoreObject* obj, ExceptionSink* xsink) {
   http_priv->send_internal(xsink, "sendWithRecvCallback", meth, mpath, headers, data, size, 0, getbody, info, timeout_ms, recv_callback, obj);
}

void QoreHttpClientObject::sendWithOutputStream(const char* meth, const char* mpath, const QoreHashNode* headers, const void* data, unsigned size, bool getbody, QoreHashNode* info, int timeout_ms, const ResolvedCallReferenceNode* recv_callback, QoreObject* obj, OutputStream *os, ExceptionSink* xsink) {
   http_priv->send_internal(xsink, "sendWithOutputStream", meth, mpath, headers, data, size, 0, getbody, info, timeout_ms, recv_callback, obj, os);
}

void QoreHttpClientObject::sendWithCallbacks(const char* meth, const char* mpath, const QoreHashNode* headers, const ResolvedCallReferenceNode* send_callback, bool getbody, QoreHashNode* info, int timeout_ms, const ResolvedCallReferenceNode* recv_callback, QoreObject* obj, ExceptionSink* xsink) {
   http_priv->send_internal(xsink, "sendWithCallbacks", meth, mpath, headers, 0, 0, send_callback, getbody, info, timeout_ms, recv_callback, obj);
}

// returns *string
// @since Qore 0.8.12: do not send getbody = true which only works with completely broken HTTP servers and small messages and causes deadlocks on correct HTTP servers
AbstractQoreNode* QoreHttpClientObject::get(const char* new_path, const QoreHashNode* headers, QoreHashNode* info, ExceptionSink* xsink) {
   ReferenceHolder<QoreHashNode> ans(http_priv->send_internal(xsink, "get", "GET", new_path, headers, 0, 0, 0, false, info, http_priv->timeout), xsink);
   if (!ans)
      return 0;

   return ans->takeKeyValue("body");
}

QoreHashNode* QoreHttpClientObject::head(const char* new_path, const QoreHashNode* headers, QoreHashNode* info, ExceptionSink* xsink) {
   return http_priv->send_internal(xsink, "head", "HEAD", new_path, headers, 0, 0, 0, false, info, http_priv->timeout);
}

// returns *string
// @since Qore 0.8.12: do not send getbody = true which only works with completely broken HTTP servers and small messages and causes deadlocks on correct HTTP servers
AbstractQoreNode* QoreHttpClientObject::post(const char* new_path, const QoreHashNode* headers, const void* data, unsigned size, QoreHashNode* info, ExceptionSink* xsink) {
   ReferenceHolder<QoreHashNode> ans(http_priv->send_internal(xsink, "post", "POST", new_path, headers, data, size, 0, false, info, http_priv->timeout), xsink);
   if (!ans)
      return 0;

   return ans->takeKeyValue("body");
}

void QoreHttpClientObject::addProtocol(const char* prot, int new_port, bool new_ssl) {
   http_priv->prot_map[prot] = make_protocol(new_port, new_ssl);
}

void QoreHttpClientObject::setMaxRedirects(int max) {
   http_priv->max_redirects = max;
}

int QoreHttpClientObject::getMaxRedirects() const {
   return http_priv->max_redirects;
}

void QoreHttpClientObject::setDefaultHeaderValue(const char* header, const char* val) {
   http_priv->default_headers[header] = val;
}

void QoreHttpClientObject::setEventQueue(Queue *cbq, ExceptionSink* xsink) {
   AutoLocker al(priv->m);
   priv->socket->setEventQueue(cbq, xsink);
}

void QoreHttpClientObject::cleanup(ExceptionSink* xsink) {
   AutoLocker al(priv->m);
   priv->socket->cleanup(xsink);
}

void QoreHttpClientObject::lock() {
   priv->m.lock();
}

void QoreHttpClientObject::unlock() {
   priv->m.unlock();
}

int QoreHttpClientObject::setNoDelay(bool nd) {
   return http_priv->setNoDelay(nd);
}

bool QoreHttpClientObject::getNoDelay() const {
   return http_priv->getNoDelay();
}

bool QoreHttpClientObject::isConnected() const {
   return http_priv->connected;
}

void QoreHttpClientObject::setUserPassword(const char* user, const char* pass) {
   http_priv->setUserPassword(user, pass);
}

void QoreHttpClientObject::clearUserPassword() {
   http_priv->clearUserPassword();
}

void QoreHttpClientObject::setProxyUserPassword(const char* user, const char* pass) {
   http_priv->setProxyUserPassword(user, pass);
}

void QoreHttpClientObject::clearProxyUserPassword() {
   http_priv->clearProxyUserPassword();
}

void QoreHttpClientObject::setPersistent(ExceptionSink* xsink) {
   return http_priv->setPersistent(xsink);
}

void QoreHttpClientObject::clearWarningQueue(ExceptionSink* xsink) {
   AutoLocker al(priv->m);
   priv->socket->clearWarningQueue(xsink);
}

void QoreHttpClientObject::setWarningQueue(ExceptionSink* xsink, int64 warning_ms, int64 warning_bs, Queue* wq, AbstractQoreNode* arg, int64 min_ms) {
   AutoLocker al(priv->m);
   priv->socket->setWarningQueue(xsink, warning_ms, warning_bs, wq, arg, min_ms);
}

QoreHashNode* QoreHttpClientObject::getUsageInfo() const {
   AutoLocker al(priv->m);
   return priv->socket->getUsageInfo();
}

void QoreHttpClientObject::clearStats() {
   AutoLocker al(priv->m);
   priv->socket->clearStats();
}
