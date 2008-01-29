/*
  QoreSocket.cc

  IPv4 Socket Class
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include <qore/Qore.h>
#include <qore/QoreSocket.h>
#include <qore/intern/SSLSocketHelper.h>

#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifndef DEFAULT_SOCKET_BUFSIZE
#define DEFAULT_SOCKET_BUFSIZE 4096
#endif

int SSLSocketHelper::setIntern(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink)
{
   ctx  = SSL_CTX_new(meth);
   if (!ctx)
   {
      sslError(xsink);
      return -1;
   }
   if (cert)
   {
      if (!SSL_CTX_use_certificate(ctx, cert))
      {
	 sslError(xsink);
	 return -1;
      }
   }
   if (pk)
   {
      if (!SSL_CTX_use_PrivateKey(ctx, pk))
      {
	 sslError(xsink);
	 return -1;
      }
   }
   
   ssl = SSL_new(ctx);
   if (!ssl)
   {
      sslError(xsink);
      return -1;
   }
   SSL_set_fd(ssl, sd);
   return 0;
}

SSLSocketHelper::SSLSocketHelper()
{
   meth = NULL;
   ctx = NULL;
   ssl = NULL;
}

SSLSocketHelper::~SSLSocketHelper()
{
   if (ssl)
      SSL_free(ssl);
   if (ctx)
      SSL_CTX_free(ctx);
}

void SSLSocketHelper::sslError(class ExceptionSink *xsink)
{
   long e;
   char buf[121];
   while ((e = ERR_get_error()))
   {
      ERR_error_string(e, buf);
      xsink->raiseException("SOCKET-SSL-ERROR", buf);
   }
}

int SSLSocketHelper::setClient(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink)
{
   meth = SSLv23_client_method();
   return setIntern(sd, cert, pk, xsink);
}

int SSLSocketHelper::setServer(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink)
{
   meth = SSLv23_server_method();
   return setIntern(sd, cert, pk, xsink);
}

// returns 0 for success
int SSLSocketHelper::connect(class ExceptionSink *xsink)
{
   if (SSL_connect(ssl) <= 0)
   {
      sslError(xsink);
      return -1;
   }
   return 0;
}

// returns 0 for success
int SSLSocketHelper::accept(class ExceptionSink *xsink)
{
   if (SSL_accept(ssl) <= 0)
   {
      sslError(xsink);
      return -1;
   }
   return 0;
}

// returns 0 for success
int SSLSocketHelper::shutdown()
{
   if (SSL_shutdown(ssl) < 0)
      return -1;
   return 0;
}

// returns 0 for success
int SSLSocketHelper::shutdown(class ExceptionSink *xsink)
{
   if (SSL_shutdown(ssl) < 0)
   {
      sslError(xsink);
      return -1;
   }
   return 0;
}

// returns 0 for success
int SSLSocketHelper::read(char *buf, int size)
{
   return SSL_read(ssl, buf, size);
}

// returns 0 for success
int SSLSocketHelper::write(const void *buf, int size, class ExceptionSink *xsink)
{
   int rc;
   if ((rc = SSL_write(ssl, buf, size)) <= 0)
   {
      sslError(xsink);
      return rc;
   }
   return rc;
}

int SSLSocketHelper::write(const void *buf, int size)
{
   return SSL_write(ssl, buf, size);
}

const char *SSLSocketHelper::getCipherName() const
{
   return SSL_get_cipher_name(ssl);
}

const char *SSLSocketHelper::getCipherVersion() const
{
   return SSL_get_cipher_version(ssl);
}

X509 *SSLSocketHelper::getPeerCertificate() const
{
   return SSL_get_peer_certificate(ssl);
}

long SSLSocketHelper::verifyPeerCertificate() const
{	 
   X509 *cert = SSL_get_peer_certificate(ssl);
   
   if (!cert)
      return -1;
   
   long rc = SSL_get_verify_result(ssl);
   X509_free(cert);
   return rc;
}

struct qore_socketsource_private {
      class QoreStringNode *address;
      class QoreStringNode *hostname;

      DLLLOCAL qore_socketsource_private()
      {
	 address = hostname = 0;
      }
      DLLLOCAL ~qore_socketsource_private()
      {
	 if (address)  address->deref();
	 if (hostname) hostname->deref();
      }
};

SocketSource::SocketSource() : priv(new qore_socketsource_private)
{
}

SocketSource::~SocketSource()
{
   delete priv;
}

void SocketSource::setAddress(class QoreStringNode *addr)
{
   assert(!priv->address);
   priv->address = addr;
}

void SocketSource::setAddress(const char *addr)
{
   assert(!priv->address);
   priv->address = new QoreStringNode(addr);
}

void SocketSource::setHostName(const char *host)
{
   assert(!priv->hostname);
   priv->hostname = new QoreStringNode(host);
}

void SocketSource::setHostName(class QoreStringNode *host)
{
   assert(!priv->hostname);
   priv->hostname = host;
}

class QoreStringNode *SocketSource::takeAddress()
{
   class QoreStringNode *addr = priv->address;
   priv->address = NULL;
   return addr;
}

class QoreStringNode *SocketSource::takeHostName()
{
   class QoreStringNode *host = priv->hostname;
   priv->hostname = NULL;
   return host;
}

const char *SocketSource::getAddress() const
{
   return priv->address ? priv->address->getBuffer() : 0;
}

const char *SocketSource::getHostName() const
{
   return priv->hostname ? priv->hostname->getBuffer() : 0;
}

void SocketSource::setAll(class QoreObject *o, class ExceptionSink *xsink)
{
   if (priv->address)
   {
      o->setValue("source", priv->address, xsink);
      priv->address = NULL;
   }
   if (priv->hostname)
   {
      o->setValue("source_host", priv->hostname, xsink);
      priv->hostname = NULL;
   }
}

struct qore_socket_private {
      int sock, type, port; //, sendTimeout, recvTimeout;
      const QoreEncoding *charsetid;
      bool del;
      std::string socketname;
      class SSLSocketHelper *ssl;

      DLLLOCAL qore_socket_private()
      {
	 del = false;
	 //sendTimeout = recvTimeout = -1
	 port = -1;
	 ssl = NULL;
      }

      DLLLOCAL ~qore_socket_private()
      {
      }
};

QoreSocket::QoreSocket() : priv(new qore_socket_private)
{
   priv->type = AF_UNSPEC;
   priv->sock = 0;
   priv->charsetid = QCS_DEFAULT;
}

QoreSocket::QoreSocket(int s, int t, const QoreEncoding *csid) : priv(new qore_socket_private)
{
   priv->type = t;
   priv->sock = s;
   priv->charsetid = csid;
}

QoreSocket::~QoreSocket()
{
   closeInternal();
   delete priv;
}

int QoreSocket::closeInternal() 
{
   //printd(5, "QoreSocket::closeInternal(this=%08p) sock=%d\n", this, priv->sock);
   if (priv->sock)
   {
      // if an SSL connection has been established, shut it down first
      if (priv->ssl)
      {
	 priv->ssl->shutdown();
	 delete priv->ssl;
	 priv->ssl = NULL;
      }
      
      if (!priv->socketname.empty())
      {
	 if (priv->del)
	    unlink(priv->socketname.c_str());
	 priv->socketname.clear();
      }
      priv->del = false;
      priv->port = -1;
      int rc = ::close(priv->sock); 
      //printd(5, "QoreSocket::closeInternal(this=%08p) close(%d) returned %d\n", this, priv->sock, rc);
      priv->sock = 0;
      return rc;
   }
   else 
      return 0; 
}

void QoreSocket::reuse(int opt)
{
   //printf("Socket::reuse(%s)\n", opt ? "true" : "false");
   setsockopt(priv->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
}

int QoreSocket::close() 
{
   int rc = closeInternal();
   priv->type = AF_UNSPEC;
   
   return rc;
}

int QoreSocket::shutdown()
{
   int rc;
   if (priv->sock)
      rc = ::shutdown(priv->sock, SHUT_RDWR); 
   else 
      rc = 0; 
   
   return rc;
}

int QoreSocket::shutdownSSL(class ExceptionSink *xsink)
{
   if (!priv->sock)
      return 0;
   if (!priv->ssl)
      return 0;
   return priv->ssl->shutdown(xsink);
}

int QoreSocket::getSocket() const
{
   return priv->sock; 
}

const QoreEncoding *QoreSocket::getEncoding() const
{
   return priv->charsetid; 
}

void QoreSocket::setEncoding(const QoreEncoding *id) 
{ 
   priv->charsetid = id; 
} 

bool QoreSocket::isOpen() const
{ 
   return (bool)priv->sock; 
}

const char *QoreSocket::getSSLCipherName() const
{
   if (!priv->ssl)
      return NULL;
   return priv->ssl->getCipherName();
}

const char *QoreSocket::getSSLCipherVersion() const
{
   if (!priv->ssl)
      return NULL;
   return priv->ssl->getCipherVersion();
}

bool QoreSocket::isSecure() const
{
   return (bool)priv->ssl;
}

long QoreSocket::verifyPeerCertificate() const
{
   if (!priv->ssl)
      return -1;
   return priv->ssl->verifyPeerCertificate();
}

int QoreSocket::upgradeClientToSSLIntern(X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink)
{
   priv->ssl = new SSLSocketHelper();
   int rc;
   if ((rc = priv->ssl->setClient(priv->sock, cert, pkey, xsink)) || priv->ssl->connect(xsink))
   {
      delete priv->ssl;
      priv->ssl = NULL;
      return rc;
   }
   return 0;
}

int QoreSocket::upgradeServerToSSLIntern(X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink)
{
   priv->ssl = new SSLSocketHelper();
   if (priv->ssl->setServer(priv->sock, cert, pkey, xsink) || priv->ssl->accept(xsink))
   {
      delete priv->ssl;
      priv->ssl = NULL;
      return -1;
   }
   return 0;
}

// QoreSocket::acceptInternal()
// returns a new socket
int QoreSocket::acceptInternal(class SocketSource *source)
{
   if (!priv->sock)
      return -2;

   int rc;
   if (priv->type == AF_UNIX)
   {
      struct sockaddr_un addr_un;

#ifdef HPUX_ACC_SOCKLEN_HACK
      int size = sizeof(struct sockaddr_un);
#else
      socklen_t size = sizeof(struct sockaddr_un);
#endif
      rc = ::accept(priv->sock, (struct sockaddr *)&addr_un, &size);
      //printd(1, "QoreSocket::accept() %d bytes returned\n", size);
      
      if (rc > 0 && source)
      {
	 class QoreStringNode *addr = new QoreStringNode(priv->charsetid);
	 addr->sprintf("UNIX socket: %s", priv->socketname.c_str());
	 source->setAddress(addr);
	 source->setHostName("localhost");
      }
   }
   else if (priv->type == AF_INET)
   {
      struct sockaddr_in addr_in;
#ifdef HPUX_ACC_SOCKLEN_HACK
      int size = sizeof(struct sockaddr_in);
#else
      socklen_t size = sizeof(struct sockaddr_in);
#endif

      rc = ::accept(priv->sock, (struct sockaddr *)&addr_in, &size);
      //printd(1, "QoreSocket::accept() %d bytes returned\n", size);

      if (rc > 0 && source)
      {
	 char *host;
	 if ((host = q_gethostbyaddr((const char *)&addr_in.sin_addr.s_addr, sizeof(addr_in.sin_addr.s_addr), AF_INET)))
	 {
	    int len = strlen(host);
	    class QoreStringNode *hostname = new QoreStringNode(host, len, len + 1, priv->charsetid);
	    source->setHostName(hostname);
	 }

	 // get IP address
	 char ifname[80];
	 if (inet_ntop(AF_INET, &addr_in.sin_addr, ifname, sizeof(ifname)))
	    source->setAddress(ifname);
      }
   }
   else
      rc = -1;
   return rc;
}

// hardcoded to SOCK_STREAM (tcp only)
int QoreSocket::connectINET(const char *host, int prt, class ExceptionSink *xsink)
{
   tracein("QoreSocket::connectINET()");

   // close socket if already open
   close();

   printd(5, "QoreSocket::connectINET(%s:%d)\n", host, prt);

   struct sockaddr_in addr_p;

   addr_p.sin_family = AF_INET;
   addr_p.sin_port = htons(prt);

   if (q_gethostbyname(host, &addr_p.sin_addr))
   {
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", "cannot resolve hostname '%s'", host);
      traceout("QoreSocket::connectINET()");
      return -1;
   }

   if ((priv->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      priv->sock = 0;
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", strerror(errno));
      traceout("QoreSocket::connectINET()");
      return -1;
   }

   if ((::connect(priv->sock, (const sockaddr *)&addr_p, sizeof(struct sockaddr_in))) == -1)
   {
      ::close(priv->sock);
      priv->sock = 0;
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", strerror(errno));
      traceout("QoreSocket::connectINET()");
      return -1;
   }
   priv->type = AF_INET;
   priv->port = prt;
   printd(5, "QoreSocket::connectINET(this=%08p, host='%s', port=%d) success, sock=%d\n", this, host, priv->port, priv->sock);
   traceout("QoreSocket::connectINET()");
   return 0;
}

int QoreSocket::connectUNIX(const char *p, class ExceptionSink *xsink)
{
   tracein("connectUNIX()");

   // close socket if already open
   close();

   printd(5, "QoreSocket::connectUNIX(%s)\n", p);

   struct sockaddr_un addr;

   addr.sun_family = AF_UNIX;
   // copy path and terminate if necessary
   strncpy(addr.sun_path, p, UNIX_PATH_MAX - 1);
   addr.sun_path[UNIX_PATH_MAX - 1] = '\0';
   if ((priv->sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
   {
      priv->sock = 0;
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", strerror(errno));
      traceout("connectUNIX()");
      return -1;
   }
   if ((::connect(priv->sock, (const sockaddr *)&addr, sizeof(struct sockaddr_un))) == -1)
   {
      ::close(priv->sock);
      priv->sock = 0;
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", strerror(errno));
      traceout("connectUNIX()");
      return -1;
   }
   // save file name for deleting when socket is closed
   priv->socketname = addr.sun_path;
   priv->type = AF_UNIX;
   traceout("connectUNIX()");
   return 0;
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// if there is no port specifier, opens UNIX domain socket (if necessary)
// and binds to a local UNIX socket file
// for UNIX domain sockets (AF_UNIX)
// * bind("filename");
// for INET sockets (AF_INET)
// * bind("interface:port");
int QoreSocket::bind(const char *name, bool reuseaddr)
{
   //printd(5, "QoreSocket::bind(%s)\n", name);
   // see if there is a port specifier
   const char *p = strchr(name, ':');
   if (p)
   {
      int prt = atoi(p + 1);
      //printd(5, "QoreSocket::bind() port=%d\n", prt);
      if (prt < 0)
	 return -1;
      class QoreString host(name);
      host.terminate(p - name);

      return bind(host.getBuffer(), prt, reuseaddr);
   }
   else // bind to UNIX domain socket file
   {
      // close if it's already been opened as an INET socket
      if (priv->sock && priv->type != AF_UNIX)
	 close();

      // try to open socket if necessary
      if (!priv->sock && openUNIX())
	 return -1;

      // set SO_REUSEADDR option if necessary
      reuse(reuseaddr);

      struct sockaddr_un addr;
      addr.sun_family = AF_UNIX;
      // copy path and terminate if necessary
      strncpy(addr.sun_path, name, UNIX_PATH_MAX - 1);
      addr.sun_path[UNIX_PATH_MAX - 1] = '\0';
      if ((::bind(priv->sock, (const sockaddr *)&addr, sizeof(struct sockaddr_un))) == -1)
      {
	 close();
	 return -1;
      }
      // save socket file name for deleting on close
      priv->socketname = addr.sun_path;
      // delete UNIX domain socket on close
      priv->del = true;
   }
   return 0;
}

int QoreSocket::sendi1(char i)
{
   if (!priv->sock)
      return -1;

   int rc = send(&i, 1);

   if (!rc || rc < 0)
      return -1;

   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);

   return 0;
}

int QoreSocket::sendi2(short i)
{
   if (!priv->sock)
      return -1;

   // convert to network byte order
   i = htons(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 2 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 2)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);

   return 0;
}

int QoreSocket::sendi4(int i)
{
   if (!priv->sock)
      return -1;

   // convert to network byte order
   i = htonl(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 4 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 4)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

int QoreSocket::sendi8(int64 i)
{
   if (!priv->sock)
      return -1;

   // convert to network byte order
   i = i8MSB(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 8 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 8)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

int QoreSocket::sendi2LSB(short i)
{
   if (!priv->sock)
      return -1;

   // convert to network byte order
   i = i2LSB(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 2 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 2)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);

   return 0;
}

int QoreSocket::sendi4LSB(int i)
{
   if (!priv->sock)
      return -1;

   // convert to network byte order
   i = i4LSB(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 4 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 4)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

int QoreSocket::sendi8LSB(int64 i)
{
   if (!priv->sock)
      return -1;

   // convert to network byte order
   i = i8LSB(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 8 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 8)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

// receive integer values and convert from network byte order
int QoreSocket::recvi1(int timeout, char *val)
{
   if (!priv->sock)
      return -1;

   return recv(val, 1, 0, timeout);
}

int QoreSocket::recvi2(int timeout, short *val)
{
   if (!priv->sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 2 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 2)
	 break;
   }

   *val = ntohs(*val);
   return 2;
}

int QoreSocket::recvi4(int timeout, int *val)
{
   if (!priv->sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 4 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 4)
	 break;
   }

   *val = ntohl(*val);
   return 4;
}

int QoreSocket::recvi8(int timeout, int64 *val)
{
   if (!priv->sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 8 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 8)
	 break;
   }

   *val = MSBi8(*val);
   return 8;
}

int QoreSocket::recvi2LSB(int timeout, short *val)
{
   if (!priv->sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 2 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 2)
	 break;
   }

   *val = LSBi2(*val);
   return 2;
}

int QoreSocket::recvi4LSB(int timeout, int *val)
{
   if (!priv->sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 4 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 4)
	 break;
   }

   *val = LSBi4(*val);
   return 4;
}

int QoreSocket::recvi8LSB(int timeout, int64 *val)
{
   if (!priv->sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 8 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 8)
	 break;
   }

   *val = LSBi8(*val);
   return 4;
}

int QoreSocket::recvu1(int timeout, unsigned char *val)
{
   if (!priv->sock)
      return -1;
   
   return recv((char *)val, 1, 0, timeout);
}

int QoreSocket::recvu2(int timeout, unsigned short *val)
{
   if (!priv->sock)
      return -1;
   
   char *buf = (char *)val;
   
   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 2 - br, 0, timeout);
      if (rc <= 0)
	 return rc;
      
      br += rc;
      
      if (br >= 2)
	 break;
   }
   
   *val = ntohs(*val);
   return 2;
}

int QoreSocket::recvu4(int timeout, unsigned int *val)
{
   if (!priv->sock)
      return -1;
   
   char *buf = (char *)val;
   
   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 4 - br, 0, timeout);
      if (rc <= 0)
	 return rc;
      
      br += rc;
      
      if (br >= 4)
	 break;
   }
   
   *val = ntohl(*val);
   return 4;
}

int QoreSocket::recvu2LSB(int timeout, unsigned short *val)
{
   if (!priv->sock)
      return -1;
   
   char *buf = (char *)val;
   
   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 2 - br, 0, timeout);
      if (rc <= 0)
	 return rc;
      
      br += rc;
      
      if (br >= 2)
	 break;
   }
   
   *val = LSBi2(*val);
   return 2;
}

int QoreSocket::recvu4LSB(int timeout, unsigned int *val)
{
   if (!priv->sock)
      return -1;
   
   char *buf = (char *)val;
   
   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 4 - br, 0, timeout);
      if (rc <= 0)
	 return rc;
      
      br += rc;
      
      if (br >= 4)
	 break;
   }
   
   *val = LSBi4(*val);
   return 4;
}

int QoreSocket::send(int fd, int size)
{
   if (!priv->sock || !size)
   {
      printd(5, "QoreSocket::send() ERROR: sock=%d size=%d\n", priv->sock, size);
      return -1;
   }

   char *buf = (char *)malloc(sizeof(char) * DEFAULT_SOCKET_BUFSIZE);

   int rc = 0;
   int bs = 0;
   while (true)
   {
      // calculate bytes needed
      int bn;
      if (size < 0)
	 bn = DEFAULT_SOCKET_BUFSIZE;
      else
      {
	 bn = size - bs;
	 if (bn > DEFAULT_SOCKET_BUFSIZE)
	    bn = DEFAULT_SOCKET_BUFSIZE;
      }
      rc = read(fd, buf, bn);
      if (!rc)
	 break;
      if (rc < 0)
      {
	 printd(5, "QoreSocket::send() read error: %s\n", strerror(errno));
	 break;
      }

      // send buffer
      rc = send(buf, rc);
      if (rc < 0)
      {
	 printd(5, "QoreSocket::send() send error: %s\n", strerror(errno));
	 break;
      }
      bs += rc;
      if (bs >= size)
      {
	 rc = 0;
	 break;
      }
   }
   free(buf);
   return rc;
}

class BinaryNode *QoreSocket::recvBinary(int bufsize, int timeout, int *rc)
{
   if (!priv->sock)
      return NULL;

   int bs = bufsize > 0 && bufsize < DEFAULT_SOCKET_BUFSIZE ? bufsize : DEFAULT_SOCKET_BUFSIZE;

   SimpleRefHolder<BinaryNode> b(new BinaryNode());

   char *buf = (char *)malloc(sizeof(char) * bs);
   int br = 0; // bytes received
   while (true)
   {
      *rc = recv(buf, bs, 0, timeout);
      if ((*rc) <= 0)
      {
	 if (*rc || !br || (!*rc && bufsize > 0))
	    b = 0; // free binary object

	 break;
      }
      b->append(buf, *rc);
      br += *rc;

      if (bufsize > 0)
      {
	 if (bufsize - br < bs)
	    bs = bufsize - br;
	 if (br >= bufsize)
	    break;
      }
   }
   free(buf);
   // "fix" return code value if no buffer size was set
   if (bufsize <= 0 && !(*rc))
      *rc = 1;
   printd(5, "QoreSocket::recvBinary() received %d byte(s), bufsize=%d, strlen=%d\n", br, bufsize, b->size());
   return b.release();
}

class QoreStringNode *QoreSocket::recv(int bufsize, int timeout, int *rc)
{
   if (!priv->sock)
   {
      *rc = -3;
      return NULL;
   }

   int bs = bufsize > 0 && bufsize < DEFAULT_SOCKET_BUFSIZE ? bufsize : DEFAULT_SOCKET_BUFSIZE;

   QoreStringNode *str = new QoreStringNode(priv->charsetid);

   char *buf = (char *)malloc(sizeof(char) * bs);
   ON_BLOCK_EXIT(free, buf);

   int br = 0; // bytes received
   while (true)
   {
      *rc = recv(buf, bs, 0, timeout);
      if ((*rc) <= 0)
      {
	 printd(5, "QoreSocket::recv(%d, %d) bs=%d, br=%d, rc=%d, errno=%d (%s)\n", bufsize, timeout, bs, br, *rc, errno, strerror(errno));

	 if (*rc || !br || (!*rc && bufsize > 0))
	 {
	    str->deref();
	    str = NULL;
	 }
	 break;
      }
      str->concat(buf, *rc);
      br += *rc;

      if (bufsize > 0)
      {
	 if (br >= bufsize)
	    break;
	 if (bufsize - br < bs)
	    bs = bufsize - br;
      }
   }
   printd(5, "QoreSocket::recv() received %d byte(s), bufsize=%d, strlen=%d str='%s'\n", br, bufsize, str ? str->strlen() : 0, str ? str->getBuffer() : "n/a");
   // "fix" return code value if no buffer size was set
   if (bufsize <= 0 && !(*rc))
      *rc = 1;
   return str;
}

class QoreStringNode *QoreSocket::recv(int timeout, int *rc)
{
   if (!priv->sock)
   {
      *rc = -3;
      return NULL;
   }

   char *buf = (char *)malloc(sizeof(char) * (DEFAULT_SOCKET_BUFSIZE + 1));
   *rc = recv(buf, DEFAULT_SOCKET_BUFSIZE, 0, timeout);
   if ((*rc) <= 0)
   {
      free(buf);
      return NULL;
   }

   buf[*rc] = '\0';
   QoreStringNode *msg = new QoreStringNode(priv->charsetid);
   msg->take(buf);
   return msg;
}

// receive data and write to file descriptor
int QoreSocket::recv(int fd, int size, int timeout)
{
   if (!priv->sock || !size)
      return -1;

   char *buf = (char *)malloc(sizeof(char) * DEFAULT_SOCKET_BUFSIZE);
   int br = 0;
   int rc;
   while (1)
   {
      // calculate bytes needed
      int bn;
      if (size == -1)
	 bn = DEFAULT_SOCKET_BUFSIZE;
      else
      {
	 bn = size - br;
	 if (bn > DEFAULT_SOCKET_BUFSIZE)
	    bn = DEFAULT_SOCKET_BUFSIZE;
      }

      rc = recv(buf, bn, 0, timeout);
      if (rc <= 0)
	 break;
      br += rc;

      // write buffer to file descriptor
      rc = write(fd, buf, rc);
      if (rc <= 0)
	 break;

      if (size > 0 && br >= size)
      {
	 rc = 0;
	 break;
      }
   }
   free(buf);
   return rc;
}

static void do_headers(QoreString &hdr, const QoreHash *headers, int size)
{
   if (headers)
   {
      class ConstHashIterator hi(headers);

      while (hi.next())
      {
	 class QoreNode *v = hi.getValue();
	 if (v)
	 {
	    const QoreType *vtype = v->getType();

	    if (vtype == NT_STRING) {
	       const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(v);
	       hdr.sprintf("%s: %s\r\n", hi.getKey(), str->getBuffer());
	       continue;
	    }
	    if (vtype == NT_INT)
	       hdr.sprintf("%s: %lld\r\n", hi.getKey(), (reinterpret_cast<const QoreBigIntNode *>(v))->val);
	    else if (vtype == NT_FLOAT)
	       hdr.sprintf("%s: %f\r\n", hi.getKey(), reinterpret_cast<const QoreFloatNode *>(v)->f);
	    else if (vtype == NT_BOOLEAN)
	       hdr.sprintf("%s: %d\r\n", hi.getKey(), reinterpret_cast<const QoreBoolNode *>(v)->b);
	 }
      }
   }
   // add data and content-length header if necessary
   if (size)
      hdr.sprintf("Content-Length: %d\r\n", size);

   hdr.concat("\r\n");
}

// returns 0 for success
int QoreSocket::sendHTTPMessage(const char *method, const char *path, const char *http_version, const class QoreHash *headers, const void *data, int size)
{
   // prepare header string
   QoreString hdr(priv->charsetid);

   hdr.sprintf("%s %s HTTP/%s\r\n", method, path && path[0] ? path : "/", http_version);
   // insert headers
   do_headers(hdr, headers, size && data ? size : 0);

   //printf("hdr=%s\n", hdr.getBuffer());
   int rc;
   if ((rc = send(hdr.getBuffer(), hdr.strlen())))
      return rc;

   if (size && data)
      return send((char *)data, size);

   return 0;
}

// returns 0 for success
int QoreSocket::sendHTTPResponse(int code, const char *desc, const char *http_version, const class QoreHash *headers, const void *data, int size)
{
   // prepare header string
   QoreString hdr(priv->charsetid);

   hdr.sprintf("HTTP/%s %03d %s\r\n", http_version, code, desc);
   do_headers(hdr, headers, size && data ? size : 0);

   int rc;
   if ((rc = send(hdr.getBuffer(), hdr.strlen())))
      return rc;

   if (size && data)
      return send((char *)data, size);

   return 0;
}

// state:
//   0 = '\r' received
//   1 = '\r\n' received
//   2 = '\r\n\r' received
//   3 = '\n' received
class QoreStringNode *QoreSocket::readHTTPData(int timeout, int *rc, int state)
{
   // read in HHTP header until \r\n\r\n or \n\n from socket
   TempQoreStringNode hdr(new QoreStringNode(priv->charsetid));

   while (true)
   {
      char c;
      *rc = recv(&c, 1, 0, timeout); // = read(priv->sock, &c, 1);
				     //printd(0, "read char: %c (%03d) (old state: %d)\n", c > 30 ? c : '?', c, state);
      if ((*rc) <= 0)
      {
	 //printd(0, "QoreSocket::readHTTPHeader(timeout=%d) hdr->strlen()=%d, rc=%d, errno=%d (%s)\n", timeout, hdr->strlen(), *rc, errno, strerror(errno));
	 return 0;
      }
      // check if we can progress to the next state
      if (state == -1 && c == '\n')
      {
	 state = 3;
	 continue;
      }
      else if (state == -1 && c == '\r')
      {
	 state = 0;
	 continue;
      }
      else if (state > 0 && c == '\n')
	 break;
      if (!state && c == '\n')
      {
	 state = 1;
	 continue;
      }
      else if (state == 1 && c == '\r')
      {
	 state = 2;
	 continue;
      }
      else
      {
	 if (!state)
	    hdr->concat('\r');
	 else if (state == 1)
	    hdr->concat("\r\n");
	 else if (state == 2)
	    hdr->concat("\r\n\r");
	 else if (state == 3)
	    hdr->concat('\n');
	 state = -1;
	 hdr->concat(c);
      }
   }
   hdr->concat('\n');
   
   return hdr.release();
}

// static method
void QoreSocket::convertHeaderToHash(class QoreHash *h, char *p)
{
   while (*p)
   {
      char *buf = p;
      
      if ((p = strstr(buf, "\r\n")))
      {
	 *p = '\0';
	 p += 2;
      }
      else if ((p = strchr(buf, '\n')))
      {
	 *p = '\0';
	 p++;
      }
      else
	 break;
      char *t = strchr(buf, ':');
      if (!t)
	 break;
      *t = '\0';
      t++;
      while (t && isblank(*t))
	 t++;
      strtolower(buf);
      h->setKeyValue(buf, new QoreStringNode(t), NULL);
   }
}

// FIXME: implement a maximum header size - otherwise a malicious message could fill up all memory
// rc is:
//    0 for remote end shutdown
//   -1 for socket error
//   -2 for socket not open
//   -3 for timeout
class QoreNode *QoreSocket::readHTTPHeader(int timeout, int *rc)
{
   if (!priv->sock)
   {
      *rc = -2;
      return NULL;
   }

   TempQoreStringNode hdr(readHTTPData(timeout, rc));
   if (!hdr)
      return NULL;

   const char *buf = hdr->getBuffer();
   //printd(0, "HTTP header=%s", buf);

   char *p;
   if ((p = (char *)strstr(buf, "\r\n")))
   {
     *p = '\0';
      p += 2;
   }
   else if ((p = (char *)strchr(buf, '\n')))
   {
     *p = '\0';
      p++;
   }
   else
   {
      //printd(5, "can't find first EOL marker\n");
      return hdr.release();
   }
   char *t1;
   if (!(t1 = (char *)strstr(buf, "HTTP/1.")))
      return hdr.release();

   QoreHashNode *h = new QoreHashNode();

#if 0
   h->setKeyValue("dbg_hdr", new QoreStringNode(buf), NULL);
#endif

   // get version
   h->setKeyValue("http_version", new QoreStringNode(t1 + 5, 3, priv->charsetid), NULL);

   // if we are getting a response
   if (t1 == buf)
   {
      char *t2 = (char *)strchr(buf + 8, ' ');
      if (t2)
      {
	 t2++;
	 if (isdigit(*(t2)))
	 {
	    h->setKeyValue("status_code", new QoreBigIntNode(atoi(t2)), NULL);
	    if (strlen(t2) > 4)
	       h->setKeyValue("status_message", new QoreStringNode(t2 + 4), NULL);
	 }
      }
   }
   else // get method and path
   {
      char *t2 = (char *)strchr(buf, ' ');
      if (t2)
      {
	 *t2 = '\0';
	 h->setKeyValue("method", new QoreStringNode(buf), NULL);
	 t2++;
	 t1 = strchr(t2, ' ');
	 if (t1)
	 {
	    *t1 = '\0';
	    // the path is returned as-is with no decodings - use decode_url() to decode
	    h->setKeyValue("path", new QoreStringNode(t2, priv->charsetid), 0);
	 }
      }
   }
   
   convertHeaderToHash(h, p);
   return h;
}

void QoreSocket::doException(int rc, const char *meth, class ExceptionSink *xsink)
{
   if (!rc)
      xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
   else if (rc == -1)   // recv() error
      xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
   else if (rc == -2)   
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::%s() call", meth);   
   // rc == -3: TIMEOUT returns NOTHING
}

// receive a binary message in HTTP chunked format
class QoreHashNode *QoreSocket::readHTTPChunkedBodyBinary(int timeout, class ExceptionSink *xsink)
{
   SimpleRefHolder<BinaryNode> b(new BinaryNode());
   class QoreString str; // for reading the size of each chunk
   
   int rc;
   // read the size then read the data and append to buffer
   while (true)
   {
      // state = 0, nothing
      // state = 1, \r received
      int state = 0;
      while (true)
      {
	 char c;
	 rc = recv(&c, 1, 0, timeout);
	 if (rc <= 0)
	 {
	    doException(rc, "readHTTPChunkedBodyBinary", xsink);
	    return 0;
	 }
	 
	 if (!state && c == '\r')
	    state = 1;
	 else if (state && c == '\n')
	    break;
	 else
	 {
	    if (state)
	    {
	       state = 0;
	       str.concat('\r');
	    }
	    str.concat(c);
	 }
      }
      // DEBUG
      //printd(0, "got chunk size (%d bytes) string: %s\n", str.strlen(), str.getBuffer());
      
      // terminate string at ';' char if present
      char *p = (char *)strchr(str.getBuffer(), ';');
      if (p)
	 *p = '\0';
      long size = strtol(str.getBuffer(), NULL, 16);
      if (size == 0)
	 break;
      if (size < 0)
      {
	 xsink->raiseException("READ-HTTP-CHUNK-ERROR", "negative value given for chunk size (%d)", size);
	 return NULL;
      }
      
      // prepare string for chunk
      str.ensureBufferSize(size + 1);
      
      // read chunk directly into string buffer    
      int bs = size < DEFAULT_SOCKET_BUFSIZE ? size : DEFAULT_SOCKET_BUFSIZE;
      int br = 0; // bytes received
      while (true)
      {
	 rc = recv((char *)str.getBuffer() + br, bs, 0, timeout);
	 if (rc <= 0)
	 {
	    doException(rc, "readHTTPChunkedBodyBinary", xsink);
	    return NULL;
	 }
	 br += rc;
	 
	 if (br >= size)
	    break;
	 if (size - br < bs)
	    bs = size - br;
      }
      // copy string buffer to binary object
      b->append(str.getBuffer(), size);
      // DEBUG
      //printd(0, "got chunk (%d bytes): %s\n", br, str.getBuffer() -  size);
      
      // read crlf after chunk
      char crlf[2];
      br = 0;
      while (br < 2)
      {
	 rc = recv(crlf, 2 - br, 0, timeout);
	 if (rc <= 0)
	 {
	    doException(rc, "readHTTPChunkedBodyBinary", xsink);
	    return NULL;
	 }
	 br += rc;
      }      

      // ensure string is blanked for next read
      str.clear();
   }

   // read footers or nothing
   TempQoreStringNode hdr(readHTTPData(timeout, &rc, 1));
   if (!hdr)
   {
      doException(rc, "readHTTPChunkedBodyBinary", xsink);
      return NULL;
   }
   class QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("body", b.release(), xsink);
   
   if (hdr->strlen() >= 2 && hdr->strlen() <= 4)
      return h;

   convertHeaderToHash(h, (char *)hdr->getBuffer());
   return h; 
}

// receive a message in HTTP chunked format
class QoreHashNode *QoreSocket::readHTTPChunkedBody(int timeout, class ExceptionSink *xsink)
{
   TempQoreStringNode buf(new QoreStringNode(priv->charsetid));
   class QoreString str; // for reading the size of each chunk
   
   int rc;
   // read the size then read the data and append to buf
   while (true)
   {
      // state = 0, nothing
      // state = 1, \r received
      int state = 0;
      while (true)
      {
	 char c;
	 rc = recv(&c, 1, 0, timeout);
	 if (rc <= 0)
	 {
	    doException(rc, "readHTTPChunkedBody", xsink);
	    return NULL;
	 }
      
	 if (!state && c == '\r')
	    state = 1;
	 else if (state && c == '\n')
	    break;
	 else
	 {
	    if (state)
	    {
	       state = 0;
	       str.concat('\r');
	    }
	    str.concat(c);
	 }
      }
      // DEBUG
      //printd(0, "got chunk size (%d bytes) string: %s\n", str.strlen(), str.getBuffer());

      // terminate string at ';' char if present
      char *p = (char *)strchr(str.getBuffer(), ';');
      if (p)
	 *p = '\0';
      long size = strtol(str.getBuffer(), NULL, 16);
      if (size == 0)
	 break;
      if (size < 0)
      {
	 xsink->raiseException("READ-HTTP-CHUNK-ERROR", "negative value given for chunk size (%d)", size);
	 return NULL;
      }
      // ensure string is blanked for next read
      str.clear();
      
      // prepare string for chunk
      buf->ensureBufferSize((unsigned)(buf->strlen() + size + 1));
      
      // read chunk directly into string buffer    
      int bs = size < DEFAULT_SOCKET_BUFSIZE ? size : DEFAULT_SOCKET_BUFSIZE;
      int br = 0; // bytes received
      while (true)
      {
	 rc = recv((char *)buf->getBuffer() + buf->strlen() + br, bs, 0, timeout);
	 if (rc <= 0)
	 {
	    doException(rc, "readHTTPChunkedBody", xsink);
	    return NULL;
	 }
	 br += rc;
	 
	 if (br >= size)
	    break;
	 if (size - br < bs)
	    bs = size - br;
      }
      // ensure new data read is included in string size
      buf->allocate(buf->strlen() + size);
      // DEBUG
      //printd(0, "got chunk (%d bytes): %s\n", br, buf->getBuffer() + buf->strlen() -  size);

      // read crlf after chunk
      char crlf[2];
      br = 0;
      while (br < 2)
      {
	 rc = recv(crlf, 2 - br, 0, timeout);
	 if (rc <= 0)
	 {
	    doException(rc, "readHTTPChunkedBody", xsink);
	    return NULL;
	 }
	 br += rc;
      }      
   }

   // read footers or nothing
   TempQoreStringNode hdr(readHTTPData(timeout, &rc, 1));
   if (!hdr)
   {
      doException(rc, "readHTTPChunkedBody", xsink);
      return NULL;
   }
   class QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("body", buf.release(), xsink);
   
   if (hdr->strlen() >= 2 && hdr->strlen() <= 4)
      return h;

   convertHeaderToHash(h, (char *)hdr->getBuffer());
   return h;
}

bool QoreSocket::isDataAvailable(int timeout) const
{
   if (!priv->sock)
      return false;

   fd_set sfs;

   struct timeval tv;
   tv.tv_sec  = timeout / 1000;
   tv.tv_usec = (timeout % 1000) * 1000;

   FD_ZERO(&sfs);
   FD_SET(priv->sock, &sfs);
   return select(priv->sock + 1, &sfs, NULL, NULL, &tv);

#if 0
   struct pollfd pfd;
   pfd.fd = priv->sock;
   pfd.events = POLLIN|POLLPRI;
   pfd.revents = 0;

   return poll(&pfd, 1, timeout);
#endif
}

int QoreSocket::recv(char *buf, int bs, int flags, int timeout)
{
   if (timeout == -1)
   {
      if (priv->ssl)
	 return priv->ssl->read(buf, bs);
      return ::recv(priv->sock, buf, bs, flags);
   }

   if (!isDataAvailable(timeout))
      return -3;

   if (priv->ssl)
      return priv->ssl->read(buf, bs);
   return ::recv(priv->sock, buf, bs, flags);
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// opens and connects to a remote socket
// for AF_INET sockets:
// * QoreSocket::connect("hostname:<port_number>");
// for AF_UNIX sockets:
// * QoreSocket::connect("filename");
int QoreSocket::connect(const char *name, class ExceptionSink *xsink)
{
   const char *p;
   if ((p = strchr(name, ':')))
   {
      char *host = (char *)malloc(sizeof(char) * (p - name + 1));
      strncpy(host, name, p - name);
      host[p - name] = '\0';
      int prt = strtol(p + 1, NULL, 10);
      int rc = connectINET(host, prt, xsink);
      free(host);
      return rc;
   }
   // else assume it's a file name for a UNIX domain socket
   return connectUNIX(name, xsink);
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// opens and connects to a remote socket and negotiates an SSL connection
// for AF_INET sockets:
// * QoreSocket::connectSSL("hostname:<port_number>");
// for AF_UNIX sockets:
// * QoreSocket::connectSSL("filename");
int QoreSocket::connectSSL(const char *name, X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink)
{
   const char *p;
   if ((p = strchr(name, ':')))
   {
      char *host = (char *)malloc(sizeof(char) * (p - name + 1));
      strncpy(host, name, p - name);
      host[p - name] = '\0';
      int prt = strtol(p + 1, NULL, 10);
      int rc = connectINETSSL(host, prt, cert, pkey, xsink);
      free(host);
      return rc;
   }
   // else assume it's a file name for a UNIX domain socket
   return connectUNIXSSL(name, cert, pkey, xsink);
}

int QoreSocket::connectINETSSL(const char *host, int prt, X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink)
{
   int rc = connectINET(host, prt, xsink);
   if (rc)
      return rc;
   return upgradeClientToSSLIntern(cert, pkey, xsink);
}

int QoreSocket::connectUNIXSSL(const char *p, X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink)
{
   int rc = connectUNIX(p, xsink);
   if (rc)
      return rc;
   return upgradeClientToSSLIntern(cert, pkey, xsink);
}

int QoreSocket::upgradeClientToSSL(X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink)
{
   if (!priv->sock)
      return -1;
   if (priv->ssl)
      return 0;
   return upgradeClientToSSLIntern(cert, pkey, xsink);
}

int QoreSocket::upgradeServerToSSL(X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink)
{
   if (!priv->sock)
      return -1;
   if (priv->ssl)
      return 0;
   return upgradeServerToSSLIntern(cert, pkey, xsink);
}

// returns 0 = success, -1 = error
int QoreSocket::openUNIX()
{
   if (priv->sock)
      close();

   if ((priv->sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
   {
      priv->sock = 0;
      return -1;
   }
   priv->type = AF_UNIX;
   priv->port = -1;
   return 0;
}

// returns 0 = success, -1 = error
int QoreSocket::openINET()
{
   if (priv->sock)
      close();

   if ((priv->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      priv->sock = 0;
      return -1;
   }
   priv->type = AF_INET;
   priv->port = -1;
   return 0;
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// opens INET socket and binds to a tcp port on all interfaces
// closes socket if already open, because the socket will be
// bound to all interfaces
// * bind(port);
int QoreSocket::bind(int prt, bool reuseaddr)
{
   // close socket if already open (will be bound to all interfaces)
   close();

   if (openINET())
      return -1;

   reuse(reuseaddr);

   struct sockaddr_in addr_p;

   bzero((char *) &addr_p, sizeof(struct sockaddr_in));
   addr_p.sin_family = AF_INET;
   addr_p.sin_port = htons(prt);
   addr_p.sin_addr.s_addr = INADDR_ANY;

   if ((::bind(priv->sock, (const sockaddr *)&addr_p, sizeof(struct sockaddr_in))) == -1)
   {
      close();
      return -1;
   }
   // set port number if known
   priv->port = prt ? prt : -1;
   return 0;
}

// to bind to an INET tcp port on a specific interface
int QoreSocket::bind(const char *interface, int prt, bool reuseaddr)
{
   printd(5, "QoreSocket::bind(%s, %d)\n", interface, prt);

   // close if it's already been opened as an INET socket
   if (priv->sock && priv->type != AF_INET)
      close();

   // try to open socket if necessary
   if (!priv->sock && openINET())
      return -1;

   reuse(reuseaddr);

   struct sockaddr_in addr_p;

   bzero((char *) &addr_p, sizeof(struct sockaddr_in));
   addr_p.sin_family = AF_INET;
   addr_p.sin_port = htons(prt);

   if (q_gethostbyname(interface, &addr_p.sin_addr))
   {
      printd(5, "QoreSocket::bind(%s, %d) gethostbyname failed for %s\n",
	     interface, priv->port, interface);
      return -1;
   }

   if ((::bind(priv->sock, (const sockaddr *)&addr_p, sizeof(struct sockaddr_in))) == -1)
      return -1;
   // set port number if known
   priv->port = prt ? prt : -1;
   //printd(5, "QoreSocket::bind(interface, port) returning 0 (success)\n");
   return 0;   
}

// to bind an INET socket to a particular address
int QoreSocket::bind(const struct sockaddr *addr, int size)
{
   // close if it's already been opened as an INET socket
   if (priv->sock && priv->type != AF_INET)
      close();

   // try to open socket if necessary
   if (!priv->sock && openINET())
      return -1;

   if ((::bind(priv->sock, addr, size)) == -1)
      return -1;
   // set port number to unknown
   priv->port = -1;
   //printd(5, "QoreSocket::bind(interface, port) returning 0 (success)\n");
   return 0;   
}

// find out what port we're connected to
int QoreSocket::getPort()
{
   // if we don't need to find out what port we are, then return current value
   if (!priv->sock || priv->type != AF_INET || priv->port != -1)
      return priv->port;

   // otherwise find out what port we're connected to
   struct sockaddr_in add;
#ifdef HPUX_ACC_SOCKLEN_HACK
   int socksize = sizeof(add);
#else
   socklen_t socksize = sizeof(add);
#endif

   if (getsockname(priv->sock, (struct sockaddr *) &add, &socksize) < 0)
      return -1;

   priv->port = ntohs(add.sin_port);
   return priv->port;
}

// QoreSocket::accept()
// returns a new socket
QoreSocket *QoreSocket::accept(class SocketSource *source, class ExceptionSink *xsink)
{
   if (!priv->sock)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened and in listening state before Socket::accept() call");
      return NULL;
   }
   int rc = acceptInternal(source);
   if (rc < 0)
   {
      xsink->raiseException("SOCKET-ACCEPT-ERROR", "error in accept: ", strerror(errno));
      return NULL;
   }

   return new QoreSocket(rc, priv->type, priv->charsetid);
}

// QoreSocket::acceptSSL()
// accepts a new connection, negotiates an SSL connection, and returns the new socket
QoreSocket *QoreSocket::acceptSSL(class SocketSource *source, X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink)
{
   class QoreSocket *s = accept(source, xsink);
   if (!s)
      return NULL;

   if (s->upgradeServerToSSLIntern(cert, pkey, xsink))
   {
      delete s;
      return NULL;
   }
   
   return s;
}

// accept a connection and replace the socket with the new connection
int QoreSocket::acceptAndReplace(class SocketSource *source)
{
   tracein("QoreSocket::acceptAndReplace()");
   int rc = acceptInternal(source);
   if (rc == -1)
   {
      traceout("QoreSocket::acceptAndReplace()");
      return -1;
   }
   closeInternal();
   priv->sock = rc;
   traceout("QoreSocket::acceptAndReplace()");
   return 0;
}

int QoreSocket::listen()
{
   if (!priv->sock)
      return -2;
   return ::listen(priv->sock, 5);
}

/*
static inline void add_to_buffer(char **buf, int *len, void *data, int size)
{
   (*buf) = (char *)realloc(*buf, (*len) + size);
   memcpy((void *)((*buf) + (*len)), data, size);
   (*len) += size;
}
*/

int QoreSocket::send(const char *buf, int size)
{
   if (!priv->sock)
      return -2;

   int bs = 0;
   while (1)
   {
      int rc;
      if (!priv->ssl)
	 rc = ::send(priv->sock, buf + bs, size - bs, 0);
      else
	 rc = priv->ssl->write(buf + bs, size - bs);
      if (rc < 0)
	 return rc;
      bs += rc;
      if (bs >= size)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

// converts to socket encoding if necessary
int QoreSocket::send(const class QoreString *msg, class ExceptionSink *xsink)
{
   const class QoreString *tstr;
   if (msg->getEncoding() != priv->charsetid)
   {
      tstr = msg->convertEncoding(priv->charsetid, xsink);
      if (xsink->isEvent())
	 return -1;
   }
   else
      tstr = msg;
   int rc = send((const char *)tstr->getBuffer(), tstr->strlen());
   if (tstr != msg)
      delete tstr;
   return rc;
}

int QoreSocket::send(const class BinaryNode *b)
{
   return send((char *)b->getPtr(), b->size());
}

int QoreSocket::setSendTimeout(int ms)
{
   struct timeval tv;
   tv.tv_sec  = ms / 1000;
   tv.tv_usec = (ms % 1000) * 1000;

   return setsockopt(priv->sock, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, sizeof(struct timeval));
}

int QoreSocket::setRecvTimeout(int ms)
{
   struct timeval tv;
   tv.tv_sec  = ms / 1000;
   tv.tv_usec = (ms % 1000) * 1000;

   return setsockopt(priv->sock, SOL_SOCKET, SO_RCVTIMEO, (void *)&tv, sizeof(struct timeval));
}

int QoreSocket::getSendTimeout() const
{
   struct timeval tv;
#ifdef HPUX_ACC_SOCKLEN_HACK
   int len = sizeof(struct timeval);
#else
   socklen_t len = sizeof(struct timeval);
#endif
   if (getsockopt(priv->sock, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, &len))
      return -1;

   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int QoreSocket::getRecvTimeout() const
{
   struct timeval tv;
#ifdef HPUX_ACC_SOCKLEN_HACK
   int len = sizeof(struct timeval);
#else
   socklen_t len = sizeof(struct timeval);
#endif
   if (getsockopt(priv->sock, SOL_SOCKET, SO_RCVTIMEO, (void *)&tv, &len))
      return -1;

   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

