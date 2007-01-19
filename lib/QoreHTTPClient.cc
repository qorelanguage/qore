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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreHTTPClient.h>
#include <qore/minitest.hpp>

#ifdef DEBUG
#  include "tests/QoreHTTPClient_tests.cc"
#endif

str_set_t QoreHTTPClient::method_set;
strcase_set_t QoreHTTPClient::header_ignore;
header_map_t QoreHTTPClient::default_headers;
class SafeHash QoreHTTPClient::mandatory_headers;

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

   default_headers["Accept"] = "text/html";
   default_headers["Content-Type"] = "text/html";
   default_headers["Connection"] = "Keep-Alive";
   default_headers["User-Agent"] = "Qore HTTP Client v" PACKAGE_VERSION;

   char buf[HOSTNAMEBUFSIZE + 1];
   if (gethostname(buf, HOSTNAMEBUFSIZE))
      mandatory_headers.setKeyValue("Host", new QoreNode("localhost"), NULL);
   else
      mandatory_headers.setKeyValue("Host", new QoreNode(buf), NULL);
   
   header_ignore.insert("Host");
   header_ignore.insert("Content-Length");
}

//-----------------------------------------------------------------------------
QoreHTTPClient::QoreHTTPClient()
   : http11(true),
     ssl(false),
     port(HTTPCLIENT_DEFAULT_PORT),
     default_port(HTTPCLIENT_DEFAULT_PORT),
     host(HTTPCLIENT_DEFAULT_HOST),
     timeout(HTTPCLIENT_DEFAULT_TIMEOUT),
     connected(false)
{
   // setup protocol map
   prot_map["http"] = make_protocol(80, false);
   prot_map["https"] = make_protocol(443, true);

   setSocketPath();
}

void QoreHTTPClient::setSocketPath()
{
   // setup socketpath
   socketpath = host + ":";
   char buff[20];
   sprintf(buff, "%d", port);
   socketpath += buff;
}

void QoreHTTPClient::setDefaultPort(int def_port)
{
   default_port = def_port;
}

void QoreHTTPClient::setDefaultPath(char *def_path)
{
   default_path = def_path;
}

void QoreHTTPClient::setTimeout(int to)
{
   timeout = to;
}

int QoreHTTPClient::getTimeout() const
{
   return timeout;
}

void QoreHTTPClient::setEncoding(class QoreEncoding *qe)
{
   m_socket.setEncoding(qe);
}

class QoreEncoding *QoreHTTPClient::getEncoding() const
{
   return m_socket.getEncoding();
}

int QoreHTTPClient::setOptions(Hash* opts, ExceptionSink* xsink)
{
   class QoreNode *n = opts->getKeyValue("default_port");  
   if (n)
      default_port = n->getAsInt();

   // process new protocols
   n = opts->getKeyValue("protocols");  
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
	 bool ssl = false;
	 int port;
	 if (v->type == NT_INT)
	    port = v->val.intval;
	 else
	 {
	    class QoreNode *p = v->val.hash->getKeyValue("port");
	    port = p ? p->getAsInt() : 0;
	    if (!port)
	    {
	       xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "'port' key in protocol hash key '%s' is missing or zero", hi.getKey());
	       return -1;
	    }
	    p = v->val.hash->getKeyValue("ssl");
	    ssl = p ? p->getAsBool() : false;
	 }
	 prot_map[hi.getKey()] = make_protocol(port, ssl);
      }
   }

   n = opts->getKeyValue("default_path");  
   if (n && n->type == NT_STRING)
      default_path = n->val.String->getBuffer();

   // set default timeout if given in option hash - accept relative date/time values as well as integers
   n = opts->getKeyValue("timeout");  
   if (n)
      timeout = getMsZeroInt(n);

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

   // parse url option if present
   n = opts->getKeyValue("url");  
   if (n && n->type == NT_STRING)
      if (process_url(n->val.String, xsink))
	 return -1;

   if (!path.empty()) {
      default_path = "/" + path;
   } else {
      n = opts->getKeyValue("default_path");
      if (n && n->type == NT_STRING) {
	 default_path = "/";
	 default_path += n->val.String->getBuffer();
      }
   }

   setSocketPath();
   return 0;
}

//-----------------------------------------------------------------------------
QoreHTTPClient::~QoreHTTPClient()
{
}

//-----------------------------------------------------------------------------
int QoreHTTPClient::process_url(class QoreString *str, ExceptionSink* xsink)
{
   QoreURL url(str);
   if (!url.isValid()) {
      xsink->raiseException("HTTP-CLIENT-CONSTRUCTOR-ERROR", "url parameter cannot be parsed");
      return -1;
   }

   if (url.getPort())
      port = url.getPort();

   // host is always set if valid
   host = url.getHost()->getBuffer();

   // check if hostname is really a local port number (for a URL string like: "8080")
   if (!url.getPort()) {
      char* aux;
      int val = strtol(host.c_str(), &aux, 10);
      if (aux != host.c_str()) {
	 host = HTTPCLIENT_DEFAULT_HOST;
	 port = val;
      }
   }

   class QoreString *tmp = url.getPath();
   path = tmp ? tmp->getBuffer() : "";
   tmp = url.getUserName();
   username = tmp ? tmp->getBuffer() : "";
   tmp = url.getPassword();
   password = tmp ? tmp->getBuffer() : "";

   tmp = url.getProtocol();
   if (tmp) {
      prot_map_t::const_iterator i = prot_map.find(tmp->getBuffer());
      if (i == prot_map.end())
      {
	 xsink->raiseException("HTTP-CLIENT-UNKNOWN-PROTOCOL", "protocol '%s' is not supported.", tmp->getBuffer());
	 return -1;
      }

      // set port only if it wasn't overridden in the URL
      if (!port)
	 port = get_port(i->second);

      // set SSL setting from protocol default
      ssl = get_ssl(i->second);
   }
   return 0;
}

//-----------------------------------------------------------------------------
int QoreHTTPClient::setHTTPVersion(char* version, ExceptionSink* xsink)
{
   if (!strcmp(version, "1.0"))
   {
      http11 = false;
      return 0;
   }
   if (!strcmp(version, "1.1"))
   {
      http11 = true;
      return 0;
   }
   xsink->raiseException("HTTP-VERSION-ERROR", "only '1.0' and '1.1' are valid (value passed: '%s')", version);
   return -1;
}

const char *QoreHTTPClient::getHTTPVersion() const
{
   return http11 ? "1.1" : "1.0";
}

void QoreHTTPClient::setHTTP11(bool val)
{
   http11 = val;
}

bool QoreHTTPClient::isHTTP11() const 
{ 
   return http11;
}
      
void QoreHTTPClient::setSecure(bool is_secure) 
{ 
   ssl = is_secure; 
}

bool QoreHTTPClient::isSecure() const 
{ 
   return ssl; 
}

long QoreHTTPClient::verifyPeerCertificate()
{ 
   return m_socket.verifyPeerCertificate(); 
}

const char* QoreHTTPClient::getSSLCipherName()
{ 
   return m_socket.getSSLCipherName(); 
}

const char* QoreHTTPClient::getSSLCipherVersion() 
{ 
   return m_socket.getSSLCipherVersion(); 
}

int QoreHTTPClient::connect_unlocked(class ExceptionSink *xsink)
{
   int rc;
   if (ssl)
      rc = m_socket.connectSSL(socketpath.c_str(), NULL, NULL, xsink);
   else
      rc = m_socket.connect(socketpath.c_str(), xsink);
   if (!rc)
      connected = true;
   return rc;
}

int QoreHTTPClient::connect(class ExceptionSink *xsink)
{
   int rc;
   lock();
   rc = connect_unlocked(xsink);
   unlock();
   return rc;
}

void QoreHTTPClient::disconnect_unlocked()
{
   if (connected)
   {
      m_socket.close();
      connected = false;
   }
}

void QoreHTTPClient::disconnect()
{
   lock();
   disconnect_unlocked();
   unlock();
}

class QoreNode *QoreHTTPClient::send_internal(char *meth, char *path, class Hash *headers, void *data, unsigned size, bool getbody, class ExceptionSink *xsink)
{
      // check if method is valid
   str_set_t::const_iterator i = method_set.find(meth);
   if (i == method_set.end())
   {
      xsink->raiseException("HTTP-CLIENT-METHOD-ERROR", "HTTP method (%n) not recognized.", meth);
      return NULL;
   }
   
   lock();
   if (connect_unlocked(xsink))
   {
      unlock();
      return NULL;
   }

   class Hash *nh = mandatory_headers.copy();
   bool keep_alive = true;

   if (headers)
   {
      HashIterator hi(headers);
      while (hi.next())
      {
	 if (!strcasecmp(hi.getKey(), "connection"))
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
	    nh->setKeyValue(hi.getKey(), n->RefSelf(), xsink);
      }
   }

   // add default headers if they weren't overridden
   for (header_map_t::const_iterator i = default_headers.begin(), e = default_headers.end(); i != e; ++i)
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
      nh->setKeyValue((char *)i->first.c_str(), new QoreNode(i->second.c_str()), xsink);
   }

   // send the message
   int rc = m_socket.sendHTTPMessage(meth, path, http11 ? "1.1" : "1.0", nh, data, size);
   nh->derefAndDelete(xsink);
   
   if (rc)
   {
      unlock();
      if (rc == -2)
	 xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "socket was closed at the remote end before the message could be sent");
      else 
	 xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));
      return NULL;
   }

   class QoreNode *ans = m_socket.readHTTPHeader(timeout, &rc);

   if (!ans || ans->type != NT_HASH)
   {
      unlock();
      if (ans)
	 ans->deref(xsink);
      xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "malformed HTTP header received from socket %s, could not parse header", socketpath.c_str());
      return NULL;
   }

   if (rc <= 0)
   {
      unlock();
      if (!rc)             // remote end has closed the connection
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "remote end has closed the connection");
      else if (rc == -1)   // recv() error
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", strerror(errno));
      else if (rc == -2)
	 xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket was closed at the remote end");
      else if (rc == -3)   // timeout
	 xsink->raiseException("HTTP-CLIENT-TIMEOUT", "timed out waiting %dms for response on socket %s", timeout, socketpath.c_str());

      ans->deref(xsink);
      return NULL;
   }

   // check HTTP status code
   class Hash *ah = ans->val.hash;
   class QoreNode *v = ah->getKeyValue("status_code");
   if (!v)
   {
      unlock();
      xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "no HTTP status code received in response");
      ans->deref(xsink);
      return NULL;
   }
   
   int code = v->getAsInt();
   if (code >= 300 && code < 400)
   {
      unlock();
      v = ah->getKeyValue("status_message");
      char *mess = v ? v->val.String->getBuffer() : (char *)"<no message>";
      v = ah->getKeyValue("location");
      char *location = v ? v->val.String->getBuffer() : (char *)"<no location>";
      xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "HTTP redirect (%d) to %s ignored: message: %s", code, location, mess);
      ans->deref(xsink);
      return NULL;
   }

   if (code < 200 && code >= 300)
   {
      unlock();
      v = ah->getKeyValue("status_message");
      char *mess = v ? v->val.String->getBuffer() : (char *)"<no message>";
      xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "HTTP status code %d received: message: %s", code, mess);
      ans->deref(xsink);
      return NULL;
   }

   // process content-type
   v = ah->getKeyValue("content-type");
   // see if there is a character set specification in the content-type header
   if (v)
   {
      char *str = v->val.String->getBuffer();
      char *p = strstr(str, "charset=");
      if (p && (p == str || *(p - 1) == ';'))
      {
	 // move p to start of encoding
	 char *c = p + 8;
	 QoreString enc;
	 while (*c && *c != ';')
	    enc.concat(*c);
	 // set new encoding
	 m_socket.setEncoding(QEM.findCreate(&enc));
	 // strip from content-type
	 class QoreString *nc;
	 if (p != str)
	    nc->concat(str, str - p - 1);
	 if (*c)
	    nc->concat(*c);
	 ah->setKeyValue("content-type", new QoreNode(nc), xsink);
	 str = nc->getBuffer();
      }
      // split into a list if ";" characters are present
      p = strchr(str, ';');
      if (p)
      {
	 class List *l = new List();
	 do {
	    l->push(new QoreNode(new QoreString(str, p - str, m_socket.getEncoding())));
	    str = p + 1;
	 } while ((p = strchr(str, ';')));
	 // add last field
	 if (*str)
	    l->push(new QoreNode(new QoreString(str, m_socket.getEncoding())));
	 ah->setKeyValue("content-type", new QoreNode(l), xsink);
      }
   }

   // get response body, if any
   v = ah->getKeyValue("content-length");
   int len = v ? v->getAsInt() : 0;
   if (len || getbody)
   {
      int rc;
      class QoreString *body = m_socket.recv(len, timeout, &rc);

      if (rc <= 0)
      {
	 unlock();
	 if (!rc)             // remote end has closed the connection
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "remote end closed the connection while receiving response message body");
	 else if (rc == -1)   // recv() error
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", strerror(errno));
	 else if (rc == -2)
	    xsink->raiseException("HTTP-CLIENT-RECEIVE-ERROR", "socket was closed at the remote end while receiving response message body");
	 else if (rc == -3)   // timeout
	    xsink->raiseException("HTTP-CLIENT-TIMEOUT", "timed out waiting %dms for response message body of length %d on socket %s", timeout, len, socketpath.c_str());
	 ans->deref(xsink);
	 return NULL;
      }

      ah->setKeyValue("body", new QoreNode(body), NULL);
   }

   // check for connection: close header
   if (!keep_alive || ((v = ah->getKeyValue("connection")) && !strcasecmp(v->val.String->getBuffer(), "close")))
      disconnect_unlocked();

   unlock();
   return ans;
}

class QoreNode *QoreHTTPClient::send(char *meth, char *path, class Hash *headers, void *data, unsigned size, bool getbody, class ExceptionSink *xsink)
{
   class QoreNode *ans = send_internal(meth, path, headers, data, size, getbody, xsink);
   if (!ans)
      return NULL;
   class QoreNode *rv = ans->val.hash->takeKeyValue("body");
   ans->deref(xsink);
   return rv;
}

class QoreNode *QoreHTTPClient::get(char *path, class Hash *headers, class ExceptionSink *xsink)
{
   class QoreNode *ans = send_internal("GET", path, headers, NULL, 0, true, xsink);
   if (!ans)
      return NULL;
   class QoreNode *rv = ans->val.hash->takeKeyValue("body");
   ans->deref(xsink);
   return rv;
}

class QoreNode *QoreHTTPClient::head(char *path, class Hash *headers, class ExceptionSink *xsink)
{
   return send_internal("HEAD", path, headers, NULL, 0, false, xsink);
}

class QoreNode *QoreHTTPClient::post(char *path, class Hash *headers, void *data, unsigned size, class ExceptionSink *xsink)
{
   class QoreNode *ans = send_internal("POST", path, headers, data, size, true, xsink);
   if (!ans)
      return NULL;
   class QoreNode *rv = ans->val.hash->takeKeyValue("body");
   ans->deref(xsink);
   return rv;
}

// EOF
