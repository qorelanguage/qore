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
#include <qore/qore_thread.h>
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

      DLLLOCAL int setIntern(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink);

   public:
      DLLLOCAL SSLSocketHelper();
      DLLLOCAL ~SSLSocketHelper();
      DLLLOCAL void sslError(class ExceptionSink *xsink);
      DLLLOCAL int setClient(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink);
      DLLLOCAL int setServer(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int connect(class ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int accept(class ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int shutdown();
      // returns 0 for success
      DLLLOCAL int shutdown(class ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int read(char *buf, int size);
      // returns 0 for success
      DLLLOCAL int write(void *buf, int size, class ExceptionSink *xsink);
      DLLLOCAL int write(void *buf, int size);
      DLLLOCAL const char *getCipherName() const;
      DLLLOCAL const char *getCipherVersion() const;
      DLLLOCAL X509 *getPeerCertificate() const;
      DLLLOCAL long verifyPeerCertificate() const;
};

// another helper class
class SocketSource {
   private:
      class QoreString *address;
      class QoreString *hostname;      

   public:
      DLLEXPORT SocketSource();
      DLLEXPORT ~SocketSource();
      DLLEXPORT void setAddress(class QoreString *addr);
      DLLEXPORT void setAddress(char *addr);
      DLLEXPORT void setHostName(char *host);
      DLLEXPORT void setHostName(class QoreString *host);
      DLLEXPORT class QoreString *takeAddress();
      DLLEXPORT class QoreString *takeHostName();
      DLLEXPORT void setAll(class Object *o, class ExceptionSink *xsink);
};

class QoreSocket 
{
   private:
      int sock, type, port, sendTimeout, recvTimeout;
      class QoreEncoding *charsetid;
      bool del;
      char *socketname;
      class SSLSocketHelper *ssl;

      DLLLOCAL QoreSocket(int s, int t, class QoreEncoding *csid);
      // code common to all constructors
      DLLLOCAL void init();
      // opens an INET socket
      DLLLOCAL int openINET();
      // opens a UNIX socket
      DLLLOCAL int openUNIX();
      // accepts a connection and returns the new socket
      DLLLOCAL int acceptInternal(class SocketSource *source);
      // closes a socket but does not reset type
      DLLLOCAL int closeInternal();
      DLLLOCAL void reuse(int opt);
      DLLLOCAL int recv(char *buf, int bs, int flags, int timeout);
      DLLLOCAL int upgradeClientToSSLIntern(X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink);
      DLLLOCAL int upgradeServerToSSLIntern(X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink);
	   
   public:
      DLLEXPORT QoreSocket();
      DLLEXPORT ~QoreSocket();
      DLLEXPORT int connect(char *name, class ExceptionSink *xsink = NULL);
      DLLEXPORT int connectINET(char *host, int prt, class ExceptionSink *xsink = NULL);
      DLLEXPORT int connectUNIX(char *p, class ExceptionSink *xsink = NULL);
      DLLEXPORT int connectSSL(char *name, X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink);
      DLLEXPORT int connectINETSSL(char *host, int prt, X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink);
      DLLEXPORT int connectUNIXSSL(char *p, X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink);
      // to bind to a UNIX domain socket or INET interface:port
      DLLEXPORT int bind(char *name, bool reuseaddr = false);
      // to bind to an INET tcp port on all interfaces
      DLLEXPORT int bind(int prt, bool reuseaddr);
      // to bind an open socket to an INET tcp port on a specific interface
      DLLEXPORT int bind(char *interface, int prt, bool reuseaddr = false);
      // to bind an INET socket to a specific socket address
      DLLEXPORT int bind(struct sockaddr *addr, int size);
      // to find out our port number, also assigns the interal port number if it must be discovered
      DLLEXPORT int getPort();
      DLLEXPORT class QoreSocket *accept(class SocketSource *source, class ExceptionSink *xsink);
      DLLEXPORT class QoreSocket *acceptSSL(class SocketSource *source, X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink);
      DLLEXPORT int acceptAndReplace(class SocketSource *source);
      DLLEXPORT int listen();
      // send a buffer of a particular size
      DLLEXPORT int send(char *buf, int size);
      // send a null-terminated string
      DLLEXPORT int send(class QoreString *msg, class ExceptionSink *xsink);
      // send a binary object
      DLLEXPORT int send(class BinaryObject *msg);
      // send from a file descriptor
      DLLEXPORT int send(int fd, int size = -1);
      // send integer value in network byte order
      DLLEXPORT int sendi1(char i);
      DLLEXPORT int sendi2(short i);
      DLLEXPORT int sendi4(int i);
      DLLEXPORT int sendi8(int64 i);
      DLLEXPORT int sendi2LSB(short i);
      DLLEXPORT int sendi4LSB(int i);
      DLLEXPORT int sendi8LSB(int64 i);
      // receive integer values and convert from network byte order
      DLLEXPORT int recvi1(int timeout, char *val);
      DLLEXPORT int recvi2(int timeout, short *val);
      DLLEXPORT int recvi4(int timeout, int *val);
      DLLEXPORT int recvi8(int timeout, int64 *val);
      DLLEXPORT int recvi2LSB(int timeout, short *val);
      DLLEXPORT int recvi4LSB(int timeout, int *val);
      DLLEXPORT int recvi8LSB(int timeout, int64 *val);
      // receive a certain number of bytes
      DLLEXPORT class QoreString *recv(int bufsize, int timeout, int *prc);
      // receive a certain number of bytes as a binary object
      DLLEXPORT class BinaryObject *recvBinary(int bufsize, int timeout, int *prc);
      // receive a message
      DLLEXPORT class QoreString *recv(int timeout, int *prc);
      // receive and write data to a file descriptor
      DLLEXPORT int recv(int fd, int size, int timeout);
      // send an HTTP message
      DLLEXPORT int sendHTTPMessage(char *method, char *path, char *http_version, class Hash *headers, void *data, int size);
      // send an HTTP response
      DLLEXPORT int sendHTTPResponse(int code, char *desc, char *http_version, class Hash *headers, void *data, int size);
      // read and parse HTTP header
      DLLEXPORT class QoreNode *readHTTPHeader(int timeout, int *prc);
      // set send timeout in milliseconds
      DLLEXPORT int setSendTimeout(int ms);
      // set recv timeout in milliseconds
      DLLEXPORT int setRecvTimeout(int ms);
      // get send timeout in milliseconds
      DLLEXPORT int getSendTimeout() const;
      // get recv timeout in milliseconds
      DLLEXPORT int getRecvTimeout() const;
      DLLEXPORT bool isDataAvailable(int timeout = 0);
      DLLEXPORT int close();
      DLLEXPORT int shutdown();
      DLLEXPORT int shutdownSSL(class ExceptionSink *xsink);
      DLLEXPORT int getSocket() const;
      DLLEXPORT class QoreEncoding *getEncoding() const;
      DLLEXPORT void setEncoding(class QoreEncoding *id);
      DLLEXPORT bool isOpen() const;
      DLLEXPORT const char *getSSLCipherName() const;
      DLLEXPORT const char *getSSLCipherVersion() const;
      DLLEXPORT bool isSecure() const;
      DLLEXPORT long verifyPeerCertificate() const;
      DLLEXPORT int upgradeClientToSSL(X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink);
      DLLEXPORT int upgradeServerToSSL(X509 *cert, EVP_PKEY *pkey, class ExceptionSink *xsink);
};

#endif // _QORE_QORESOCKET_H
