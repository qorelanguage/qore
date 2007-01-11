/*
  QoreHTTPClient.cc

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreHTTPClient.h>
#include <qore/minitest.hpp>

#ifdef DEBUG
#  include "tests/QoreHTTPClient_tests.cc"
#endif

//-----------------------------------------------------------------------------
QoreHTTPClient::QoreHTTPClient()
: ssl(false),
  port(HTTPCLIENT_DEFAULT_PORT),
  default_port(HTTPCLIENT_DEFAULT_PORT),
  timeout(HTTPCLIENT_DEFAULT_TIMEOUT),
  host(HTTPCLIENT_DEFAULT_HOST),
  connected(false),
{
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

int QoreHTTPClient::getTimeout()
{
   return timeout;
}

QoreHTTPClient::setOptions(Hash* opts, ExceptionSink* xsink)
{
   // setup protocol map
   prot_map["http"] = make_protocol(80, false);
   prot_map["https"] = make_protocol(443, true);

   n = opts->getKeyValue("default_port");  
   if (n)
      default_port = n->getAsInt();

   n = opts->getKeyValue("default_path");  
   if (n && n->type == NT_STRING)
      default_path = n->val.String->getBuffer();

   // set default timeout if given in option hash - accept relative date/time values as well as integers
   n = opts->getKeyValue("timeout");  
   if (n)
      timeout = getMsZeroInt(n);

   n = opts->getKeyValue("http_version");  
   if (n && n->type == NT_STRING && setHTTPVersion(n->val.String, xsink))
      return;

   // parse url option if present
   QoreNode* n = opts->getKeyValue("url");  
   if (n && n->type == NT_STRING)
      if (process_url(n->val.String, xsink))
	 return;

   if (!path.empty()) {
      default_path = "/" + path;
   } else {
      n = opts->getKeyValue("default_path");
      if (n && n->type == NT_STRING) {
	 default_path = "/" + n->val.String->getBuffer();
      }
   }

   // setup socketpath
   socketpath = host + ":";
   char buff[20];
   sprintf(buff, "%d", port);
   socketpath += buff;
}

//-----------------------------------------------------------------------------
QoreHTTPClient::~QoreHTTPClient()
{
}

//-----------------------------------------------------------------------------
int QoreHTTPClient::process_url(Hash* opts, ExceptionSink* xsink)
{
   QoreURL url(n->val.String);
   if (!url.isValid()) {
      xsink->raiseException("HTTPCLIENT-CONSTRUCTOR-ERROR", "url parameter cannot be parsed");
      return -1;
   }

   if (url.getPort())
      port = url.getPort();

   // host is always set if valid
   host = url.host->getBuffer();

   // check if hostname is really a local port number (for a URL string like: "8080")
   if (!url.getPort()) {
      char* aux;
      int val = strtol(host.c_str(), &aux, 10);
      if (aux != host.c_str()) {
	 host = HTTPCLIENT_DEFAULT_HOST;
	 port = val;
      }
   }

   path = url.take_path();
   username = url.take_username();
   password = url.take_password();

   class QoreString *prot = url.getProtocol();
   if (prot) {
      prot_map_t::const_iterator i = prot_map.find(prot->getBuffer());
      if (i == prot_map.end())
      {
	 xsink->raiseException("HTTPCLIENT-UNKNOWN-PROTOCOL", "protocol '%s' is not supported.", url.protocol->getBuffer());
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
   îf (!strcmp(version, "1.0"))
   {
      http11 = false;
      return 0;
   }
   else if (!strcmp(version, "1.1"))
   {
      http11 = true;
      return 0;
   }
   xsink->raiseException("HTTPCLIENT-SETHTTPVERSION-ERROR", "only '1.0' and '1.1' are valid (value passed: '%s')", version);
   return -1;
}

void QoreHTTPClient::setHTTP11()
{
   http11 = true;
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
}

// EOF

