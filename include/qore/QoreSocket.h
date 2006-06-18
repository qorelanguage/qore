/*
  QoreSocket.h

  IPv4 Socket Class
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  will unlink (delete) UNIX domain socket files when closed

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

#ifndef _QORE_QORESOCKET_H

#define _QORE_QORESOCKET_H

#ifndef DEFAULT_SOCKET_BUFSIZE
#define DEFAULT_SOCKET_BUFSIZE 4096
#endif

#include <qore/config.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/thread.h>
#include <qore/Object.h>
#include <qore/QoreNet.h>
#include <qore/BinaryObject.h>
#include <qore/charset.h>
#include <qore/Exception.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>

class SSLSocketHelper
{
   private:
      SSL_METHOD *meth;
      SSL_CTX *ctx;
      SSL *ssl;

   public:
      inline SSLSocketHelper()
      {
	 meth = NULL;
	 ctx = NULL;
	 ssl = NULL;
      }

      inline ~SSLSocketHelper()
      {
	 if (ssl)
	    SSL_free(ssl);
	 if (ctx)
	    SSL_CTX_free(ctx);
      }

      inline void sslError(class ExceptionSink *xsink)
      {
	 long e;
	 char buf[121];
	 while ((e = ERR_get_error()))
	 {
	    ERR_error_string(e, buf);
	    xsink->raiseException("SOCKET-SSL-ERROR", buf);
	 }
      }

      inline int setClient(int sd, class ExceptionSink *xsink)
      {
	 meth = SSLv23_client_method();
	 ctx  = SSL_CTX_new(meth);
	 if (!ctx)
	 {
	    sslError(xsink);
	    return -1;
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

      inline int setServer(int sd, class ExceptionSink *xsink)
      {
	 meth = SSLv23_server_method();
	 ctx  = SSL_CTX_new(meth);
	 if (!ctx)
	 {
	    sslError(xsink);
	    return -1;
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

      // returns 0 for success
      inline int connect(class ExceptionSink *xsink)
      {
	 if (SSL_connect(ssl) <= 0)
	 {
	    sslError(xsink);
	    return -1;
	 }
	 return 0;
      }

      // returns 0 for success
      inline int accept(class ExceptionSink *xsink)
      {
	 if (SSL_accept(ssl) <= 0)
	 {
	    sslError(xsink);
	    return -1;
	 }
	 return 0;
      }

      // returns 0 for success
      inline int shutdown()
      {
	 if (SSL_shutdown(ssl) < 0)
	    return -1;
	 return 0;
      }

      // returns 0 for success
      inline int shutdown(class ExceptionSink *xsink)
      {
	 if (SSL_shutdown(ssl) < 0)
	 {
	    sslError(xsink);
	    return -1;
	 }
	 return 0;
      }
      
      // returns 0 for success
      inline int read(char *buf, int size)
      {
	 return SSL_read(ssl, buf, size);
      }

      // returns 0 for success
      inline int write(void *buf, int size, class ExceptionSink *xsink)
      {
	 int rc;
	 if ((rc = SSL_write(ssl, buf, size)) <= 0)
	 {
	    sslError(xsink);
	    return rc;
	 }
	 return rc;
      }

      inline int write(void *buf, int size)
      {
	 return SSL_write(ssl, buf, size);
      }

      inline const char *getCipherName()
      {
	 return SSL_get_cipher_name(ssl);
      }

      inline const char *getCipherVersion()
      {
	 return SSL_get_cipher_version(ssl);
      }
};

class QoreSocket 
{
   private:
      int sock, type, port, sendTimeout, recvTimeout;
      class QoreEncoding *charsetid;
      bool del;
      char *socketname;
      class SSLSocketHelper *ssl;

      inline QoreSocket(int s, int t);
      // code common to all constructors
      inline void init();
      // opens an INET socket
      inline int openINET();
      // opens a UNIX socket
      inline int openUNIX();
      // accepts a connection and returns the new socket
      int acceptInternal(QoreString *source);
      // closes a socket but does not reset type
      inline int closeInternal() 
      {
	 //printd(5, "QoreSocket::closeInternal(this=%08x) sock=%d\n", this, sock);
	 if (sock)
	 {
	    // if an SSL connection has been established, shut it down first
	    if (ssl)
	    {
	       ssl->shutdown();
	       delete ssl;
	       ssl = NULL;
	    }

	    if (socketname)
	    {
	       if (del)
		  unlink(socketname);
	       free(socketname);
	       socketname = NULL;
	    }
	    del = false;
	    port = -1;
	    int rc = ::close(sock); 
	    //printd(5, "QoreSocket::closeInternal(this=%08x) close(%d) returned %d\n", this, sock, rc);
	    sock = 0;
	    return rc;
	 }
	 else 
	    return 0; 
      }
      inline void reuse(int opt)
      {
	 //printf("Socket::reuse(%s)\n", opt ? "true" : "false");
	 setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
      }
      inline int recv(char *buf, int bs, int flags, int timeout);

   public:
      inline QoreSocket();
      inline ~QoreSocket();
      inline int connect(char *name, class ExceptionSink *xsink = NULL);
      int connectINET(char *host, int prt, class ExceptionSink *xsink = NULL);
      int connectUNIX(char *p, class ExceptionSink *xsink = NULL);
      inline int connectSSL(char *name, class ExceptionSink *xsink);
      inline int connectINETSSL(char *host, int prt, class ExceptionSink *xsink);
      inline int connectUNIXSSL(char *p, class ExceptionSink *xsink);
      // to bind to a UNIX domain socket or INET interface:port
      int bind(char *name, bool reuseaddr = false);
      // to bind to an INET tcp port on all interfaces
      inline int bind(int prt, bool reuseaddr);
      // to bind an open socket to an INET tcp port on a specific interface
      inline int bind(char *interface, int prt, bool reuseaddr = false);
      // to bind an INET socket to a specific socket address
      inline int bind(struct sockaddr *addr, int size);
      // to find out our port number
      inline int getPort();
      inline class QoreSocket *accept(class QoreString *source, class ExceptionSink *xsink = NULL);
      inline class QoreSocket *acceptSSL(class QoreString *source, class ExceptionSink *xsink);
      inline int acceptAndReplace(class QoreString *source);
      inline int listen();
      // send a buffer of a particular size
      inline int send(char *buf, int size);
      // send a null-terminated string
      inline int send(class QoreString *msg, class ExceptionSink *xsink);
      // send a binary object
      inline int send(class BinaryObject *msg);
      // send from a file descriptor
      int send(int fd, int size = -1);
      // send integer value in network byte order
      int sendi1(char i);
      int sendi2(short i);
      int sendi4(int i);
      int sendi8(int64 i);
      int sendi2LSB(short i);
      int sendi4LSB(int i);
      int sendi8LSB(int64 i);
      // receive integer values and convert from network byte order
      int recvi1(int timeout, char *val);
      int recvi2(int timeout, short *val);
      int recvi4(int timeout, int *val);
      int recvi8(int timeout, int64 *val);
      int recvi2LSB(int timeout, short *val);
      int recvi4LSB(int timeout, int *val);
      int recvi8LSB(int timeout, int64 *val);
      // receive a certain number of bytes
      class QoreString *recv(int bufsize, int timeout, int *prc);
      // receive a certain number of bytes as a binary object
      class BinaryObject *recvBinary(int bufsize, int timeout, int *prc);
      // receive a message
      class QoreString *recv(int timeout, int *prc);
      // receive and write data to a file descriptor
      int recv(int fd, int size, int timeout);
      // send an HTTP message
      int sendHTTPMessage(char *method, char *path, char *http_version, class Hash *headers, void *data, int size);
      // send an HTTP response
      int sendHTTPResponse(int code, char *desc, char *http_version, class Hash *headers, void *data, int size);
      // read and parse HTTP header
      class QoreNode *readHTTPHeader(int timeout, int *prc);
      // set send timeout in milliseconds
      inline int setSendTimeout(int ms);
      // set recv timeout in milliseconds
      inline int setRecvTimeout(int ms);
      // get send timeout in milliseconds
      inline int getSendTimeout();
      // get recv timeout in milliseconds
      inline int getRecvTimeout();
      inline int close() 
      {
	 int rc = closeInternal();
	 type = AF_UNSPEC;
	 sock = 0;

	 return rc;
      }
      inline int shutdown()
      {
	 int rc;
	 if (sock)
	    rc = ::shutdown(sock, SHUT_RDWR); 
	 else 
	    rc = 0; 

	 return rc;
      }
      inline int shutdownSSL(class ExceptionSink *xsink)
      {
	 if (!sock)
	    return 0;
	 if (!ssl)
	    return 0;
	 return ssl->shutdown(xsink);
      }
      inline int getSocket() { return sock; }
      inline class QoreEncoding *getEncoding() { return charsetid; }
      inline void setEncoding(class QoreEncoding *id) { charsetid = id; } 
      bool isDataAvailable(int timeout = 0);

      inline const char *getSSLCipherName()
      {
	 if (!ssl)
	    return NULL;
	 return ssl->getCipherName();
      }

      inline const char *getSSLCipherVersion()
      {
	 if (!ssl)
	    return NULL;
	 return ssl->getCipherVersion();
      }

      inline bool isSecure()
      {
	 return (bool)ssl;
      }
};

inline void QoreSocket::init()
{
   sendTimeout = recvTimeout = port = -1;
   socketname = NULL;
   charsetid = QCS_DEFAULT;
}

inline QoreSocket::QoreSocket()
{
   ssl = NULL;
   del = false;
   type = AF_UNSPEC;
   sock = 0;
   init();
}

inline QoreSocket::QoreSocket(int s, int t)
{
   ssl = NULL;
   del = false;
   type = t;
   sock = s;
   init();
}

inline QoreSocket::~QoreSocket()
{
   closeInternal();
}

inline int QoreSocket::recv(char *buf, int bs, int flags, int timeout)
{
   if (timeout == -1)
   {
      if (ssl)
	 return ssl->read(buf, bs);
      return ::recv(sock, buf, bs, flags);
   }

   if (!isDataAvailable(timeout))
      return -2;

   if (ssl)
      return ssl->read(buf, bs);
   return ::recv(sock, buf, bs, flags);
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// opens and connects to a remote socket
// for AF_INET sockets:
// * QoreSocket::connect("hostname:<port_number>");
// for AF_UNIX sockets:
// * QoreSocket::connect("filename");
inline int QoreSocket::connect(char *name, class ExceptionSink *xsink)
{
   char *p;
   if ((p = index(name, ':')))
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
inline int QoreSocket::connectSSL(char *name, class ExceptionSink *xsink)
{
   char *p;
   if ((p = index(name, ':')))
   {
      char *host = (char *)malloc(sizeof(char) * (p - name + 1));
      strncpy(host, name, p - name);
      host[p - name] = '\0';
      int prt = strtol(p + 1, NULL, 10);
      int rc = connectINETSSL(host, prt, xsink);
      free(host);
      return rc;
   }
   // else assume it's a file name for a UNIX domain socket
   return connectUNIXSSL(name, xsink);
}

inline int QoreSocket::connectINETSSL(char *host, int prt, class ExceptionSink *xsink)
{
   int rc = connectINET(host, prt, xsink);
   if (rc)
      return rc;
   ssl = new SSLSocketHelper();
   if ((rc = ssl->setClient(sock, xsink)) || ssl->connect(xsink))
   {
      delete ssl;
      ssl = NULL;
      return rc;
   }
   return 0;
}

inline int QoreSocket::connectUNIXSSL(char *p, class ExceptionSink *xsink)
{
   int rc = connectUNIX(p, xsink);
   if (rc)
      return rc;
   ssl = new SSLSocketHelper();
   if ((rc = ssl->setClient(sock, xsink)) || ssl->connect(xsink))
   {
      delete ssl;
      ssl = NULL;
      return rc;
   }
   return 0;
}

// returns 0 = success, -1 = error
inline int QoreSocket::openUNIX()
{
   if (sock)
      close();

   if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
   {
      sock = 0;
      return -1;
   }
   type = AF_UNIX;
   port = -1;
   return 0;
}

// returns 0 = success, -1 = error
inline int QoreSocket::openINET()
{
   if (sock)
      close();

   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      sock = 0;
      return -1;
   }
   type = AF_INET;
   port = -1;
   return 0;
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// opens INET socket and binds to a tcp port on all interfaces
// closes socket if already open, because the socket will be
// bound to all interfaces
// * bind(port);
inline int QoreSocket::bind(int prt, bool reuseaddr)
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

   if ((::bind(sock, (const sockaddr *)&addr_p, sizeof(struct sockaddr_in))) == -1)
   {
      close();
      return -1;
   }
   // set port number if known
   port = prt ? prt : -1;
   return 0;
}

// to bind to an INET tcp port on a specific interface
inline int QoreSocket::bind(char *interface, int prt, bool reuseaddr)
{
   printd(5, "QoreSocket::bind(%s, %d)\n", interface, prt);

   // close if it's already been opened as an INET socket
   if (sock && type != AF_INET)
      close();

   // try to open socket if necessary
   if (!sock && openINET())
      return -1;

   reuse(reuseaddr);

   struct sockaddr_in addr_p;

   bzero((char *) &addr_p, sizeof(struct sockaddr_in));
   addr_p.sin_family = AF_INET;
   addr_p.sin_port = htons(prt);

   if (q_gethostbyname(interface, &addr_p.sin_addr))
   {
      printd(5, "QoreSocket::bind(%s, %d) gethostbyname failed for %s\n",
	     interface, port, interface);
      return -1;
   }

   if ((::bind(sock, (const sockaddr *)&addr_p, sizeof(struct sockaddr_in))) == -1)
      return -1;
   // set port number if known
   port = prt ? prt : -1;
   //printd(5, "QoreSocket::bind(interface, port) returning 0 (success)\n");
   return 0;   
}

// to bind an INET socket to a particular address
inline int QoreSocket::bind(struct sockaddr *addr, int size)
{
   // close if it's already been opened as an INET socket
   if (sock && type != AF_INET)
      close();

   // try to open socket if necessary
   if (!sock && openINET())
      return -1;

   if ((::bind(sock, addr, size)) == -1)
      return -1;
   // set port number to unknown
   port = -1;
   //printd(5, "QoreSocket::bind(interface, port) returning 0 (success)\n");
   return 0;   
}

// find out what port we're connected to
inline int QoreSocket::getPort()
{
   // if we don't need to find out what port we are, then return current value
   if (!sock || type != AF_INET || port != -1)
      return port;

   // otherwise find out what port we're connected to
   struct sockaddr_in add;
   socklen_t socksize = sizeof(add);

   if (getsockname(sock, (struct sockaddr *) &add, &socksize) < 0)
      return -1;

   port = ntohs(add.sin_port);
   return port;
}

// QoreSocket::accept()
// returns a new socket
inline QoreSocket *QoreSocket::accept(QoreString *source, class ExceptionSink *xsink)
{
   int rc = acceptInternal(source);
   if (rc == -1)
   {
      if (xsink)
	 xsink->raiseException("SOCKET-ACCEPT-ERROR", strerror(errno));
      return NULL;
   }

   return new QoreSocket(rc, type);
}

// QoreSocket::acceptSSL()
// accepts a new connection, negotiates an SSL connection, and returns the new socket
inline QoreSocket *QoreSocket::acceptSSL(QoreString *source, class ExceptionSink *xsink)
{
   class QoreSocket *s = accept(source, xsink);
   if (!s)
      return NULL;

   s->ssl = new SSLSocketHelper();
   if (s->ssl->setServer(s->sock, xsink))
   {
      delete s;
      return NULL;
   }
   if (s->ssl->accept(xsink))
   {
      delete s;
      return NULL;
   }
   return s;
}

// accept a connection and replace the socket with the new connection
int QoreSocket::acceptAndReplace(class QoreString *source)
{
   tracein("QoreSocket::acceptAndReplace()");
   int rc = acceptInternal(source);
   if (rc == -1)
   {
      traceout("QoreSocket::acceptAndReplace()");
      return -1;
   }
   closeInternal();
   sock = rc;
   traceout("QoreSocket::acceptAndReplace()");
   return 0;
}

inline int QoreSocket::listen()
{
   if (!sock)
      return -1;
   return ::listen(sock, 5);
}

/*
static inline void add_to_buffer(char **buf, int *len, void *data, int size)
{
   (*buf) = (char *)realloc(*buf, (*len) + size);
   memcpy((void *)((*buf) + (*len)), data, size);
   (*len) += size;
}
*/

inline int QoreSocket::send(char *buf, int size)
{
   if (!sock)
      return -1;

   int bs = 0;
   while (1)
   {
      int rc;
      if (!ssl)
	 rc = ::send(sock, buf + bs, size - bs, 0);
      else
	 rc = ssl->write(buf + bs, size - bs);
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
inline int QoreSocket::send(class QoreString *msg, class ExceptionSink *xsink)
{
   class QoreString *tstr;
   if (msg->getEncoding() && charsetid && msg->getEncoding() != charsetid)
   {
      tstr = msg->convertEncoding(charsetid, xsink);
      if (xsink->isEvent())
	 return -1;
   }
   else
      tstr = msg;
   int rc = send((char *)tstr->getBuffer(), tstr->strlen());
   if (tstr != msg)
      delete tstr;
   return rc;
}

inline int QoreSocket::send(class BinaryObject *b)
{
   return send((char *)b->getPtr(), b->size());
}

inline int QoreSocket::setSendTimeout(int ms)
{
   struct timeval tv;
   tv.tv_sec  = ms / 1000;
   tv.tv_usec = (ms % 1000) * 1000;

   return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, sizeof(struct timeval));
}

inline int QoreSocket::setRecvTimeout(int ms)
{
   struct timeval tv;
   tv.tv_sec  = ms / 1000;
   tv.tv_usec = (ms % 1000) * 1000;

   return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void *)&tv, sizeof(struct timeval));
}

inline int QoreSocket::getSendTimeout()
{
   struct timeval tv;
   int len = sizeof(struct timeval);
   if (getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, (socklen_t *)&len))
      return -1;

   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

inline int QoreSocket::getRecvTimeout()
{
   struct timeval tv;
   int len = sizeof(struct timeval);
   if (getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void *)&tv, (socklen_t *)&len))
      return -1;

   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#endif // _QORE_QORESOCKET_H
