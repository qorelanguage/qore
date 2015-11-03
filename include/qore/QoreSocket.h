/*
  QoreSocket.h

  IPv4 Socket Class
  
  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#include <qore/Qore.h>

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

#include <openssl/ssl.h>
#include <openssl/err.h>

#define QSE_MISC_ERR  0 //!< error in errno
#define QSE_RECV_ERR -1 //!< error in recv()
#define QSE_NOT_OPEN -2 //!< socket is not open
#define QSE_TIMEOUT  -3 //!< timeout occured

class Queue;

//! a helper class for getting socket origination information
/** objects of this class are used in some QoreSocket functions
    @see QoreSocket::accept()
    @see QoreSocket::acceptSSL()
    @see QoreSocket::acceptAndReplace()
 */
class SocketSource {
   private:
      struct qore_socketsource_private *priv; // private implementation

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL SocketSource(const SocketSource&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL SocketSource& operator=(const SocketSource&);

   public:
      //! creates an empty object
      DLLEXPORT SocketSource();

      //! destroys the object and frees all memory
      DLLEXPORT ~SocketSource();

      //! returns the host address string field and leaves the object's host address field empty; the caller owns the QoreStringNode reference count returned
      /** @return the host address string; the caller owns the QoreStringNode reference count returned
       */
      DLLEXPORT QoreStringNode *takeAddress();

      //! returns the hostname string field and leaves the object's hostname field empty; the caller owns the QoreStringNode reference count returned
      /** @return the hostname string; the caller owns the QoreStringNode reference count returned
       */
      DLLEXPORT QoreStringNode *takeHostName();

      //! returns the host address string as a C string
      /** @return the host address string as a C string
       */
      DLLEXPORT const char *getAddress() const;

      //! returns the hostname string as a C string
      /** @return the hostname string as a C string
       */
      DLLEXPORT const char *getHostName() const;

      DLLLOCAL void setAddress(QoreStringNode *addr);
      DLLLOCAL void setAddress(const char *addr);
      DLLLOCAL void setHostName(const char *host);
      DLLLOCAL void setHostName(QoreStringNode *host);
      DLLLOCAL void setAll(QoreObject *o, ExceptionSink *xsink);
};

//! provides access to sockets using Qore data structures
/** QoreSocket objects also have a QoreEncoding associated with them used for sending
    and receiving string data.  String data received from the socket will be tagged with
    the appropriate encoding; string data send through the socket will be implicitly
    converted to the socket's encoding if necessary.
    This class does no implement any thread locking; thread locking must be performed at
    a higher level (for example, as with QoreHTTPClient and QoreFtpClient).

    @note currently only supports IPv4, TCP sockets
    @see QoreEncoding
 */
class QoreSocket {
   private:
      //! private implementation of the class
      struct qore_socket_private *priv; 

      //! private constructor, not exported in the library's public itnerface
      DLLLOCAL QoreSocket(int s, int t, const QoreEncoding *csid);

      //! opens an INET socket
      DLLLOCAL int openINET();

      //! opens a UNIX socket
      DLLLOCAL int openUNIX();

      DLLLOCAL void reuse(int opt);
      DLLLOCAL int recv(char *buf, qore_size_t bs, int flags, int timeout, bool do_event = true);

      //! read until \\r\\n and return the string
      DLLLOCAL QoreStringNode *readHTTPData(int timeout, int *rc, int state = -1);

      DLLLOCAL static void convertHeaderToHash(QoreHashNode *h, char *p);
      
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreSocket(const QoreSocket&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreSocket& operator=(const QoreSocket&);

   public:
      //! creates an empty, unconnected socket
      DLLEXPORT QoreSocket();

      //! disconnects if necessary, frees all data, and destroys the socket
      DLLEXPORT ~QoreSocket();

      //! connects to a socket and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** If "name" has a ':' in it; it's assumed to be a hostname:port specification and QoreSocket::connectINET() is called.
	  Otherwise "name" is assumed to be a file name for a UNIX domain socket and QoreSocket::connectUNIX() is called.
	  @param name the name of the socket (either hostname:port or file name)
	  @param xsink if not 0, if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @see QoreSocket::connectINET()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectSSL()
	  @see QoreSocket::connectINETSSL()
	  @see QoreSocket::connectUNIXSSL()
       */
      DLLEXPORT int connect(const char *name, ExceptionSink *xsink = 0);

      //! connects to a socket and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** If "name" has a ':' in it; it's assumed to be a hostname:port specification and QoreSocket::connectINET() is called.
	  Otherwise "name" is assumed to be a file name for a UNIX domain socket and QoreSocket::connectUNIX() is called.
	  @param name the name of the socket (either hostname:port or file name)
	  @param timeout_ms the timeout period in milliseconds
	  @param xsink if not 0, if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @see QoreSocket::connectINET()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectSSL()
	  @see QoreSocket::connectINETSSL()
	  @see QoreSocket::connectUNIXSSL()
       */
      DLLEXPORT int connect(const char *name, int timeout_ms, ExceptionSink *xsink = 0);

      //! connects to an INET socket by hostname and port number and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** @param host the name or IP address of the host
	  @param prt the port number of the remote socket
	  @param xsink if not 0, if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @see QoreSocket::connect()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectSSL()
	  @see QoreSocket::connectINETSSL()
	  @see QoreSocket::connectUNIXSSL()
       */
      DLLEXPORT int connectINET(const char *host, int prt, ExceptionSink *xsink = 0);

      //! connects to an INET socket by hostname and port number and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** @param host the name or IP address of the host
	  @param prt the port number of the remote socket
	  @param timeout_ms the timeout period in milliseconds
	  @param xsink if not 0, if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @see QoreSocket::connect()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectSSL()
	  @see QoreSocket::connectINETSSL()
	  @see QoreSocket::connectUNIXSSL()
       */
      DLLEXPORT int connectINET(const char *host, int prt, int timeout_ms, ExceptionSink *xsink = 0);

      //! connects to a UNIX domain socket and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** @param p the file name of the UNIX domain socket
	  @param xsink if not 0, if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @see QoreSocket::connect()
	  @see QoreSocket::connectINET()
	  @see QoreSocket::connectSSL()
	  @see QoreSocket::connectINETSSL()
	  @see QoreSocket::connectUNIXSSL()
       */
      DLLEXPORT int connectUNIX(const char *p, ExceptionSink *xsink = 0);

      //! connects to a socket, negotiates an SSL connection, and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** If "name" has a ':' in it; it's assumed to be a hostname:port specification and QoreSocket::connectINETSSL() is called.
	  Otherwise "name" is assumed to be a file name for a UNIX domain socket and QoreSocket::connectUNIXSSL() is called.
	  @param name the name of the socket (either hostname:port or file name)
	  @param cert the X509 certificate to use for the connection, may be 0 if no certificate should be used
	  @param pkey the private key to use for the connection, may be 0 if no private key should be used
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @note the same as calling QoreSocket::connect() and then QoreSocket::upgradeClientToSSL()
	  @see QoreSocket::connect()
	  @see QoreSocket::connectINET()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectINETSSL()
	  @see QoreSocket::connectUNIXSSL()
	  @see QoreSocket::upgradeClientToSSL()
       */
      DLLEXPORT int connectSSL(const char *name, X509 *cert, EVP_PKEY *pkey, ExceptionSink *xsink);

      //! connects to a socket, negotiates an SSL connection, and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** If "name" has a ':' in it; it's assumed to be a hostname:port specification and QoreSocket::connectINETSSL() is called.
	  Otherwise "name" is assumed to be a file name for a UNIX domain socket and QoreSocket::connectUNIXSSL() is called.
	  @param name the name of the socket (either hostname:port or file name)
	  @param timeout_ms the timeout period in milliseconds
	  @param cert the X509 certificate to use for the connection, may be 0 if no certificate should be used
	  @param pkey the private key to use for the connection, may be 0 if no private key should be used
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @note the same as calling QoreSocket::connect() and then QoreSocket::upgradeClientToSSL()
	  @see QoreSocket::connect()
	  @see QoreSocket::connectINET()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectINETSSL()
	  @see QoreSocket::connectUNIXSSL()
	  @see QoreSocket::upgradeClientToSSL()
       */
      DLLEXPORT int connectSSL(const char *name, int timeout_ms, X509 *cert, EVP_PKEY *pkey, ExceptionSink *xsink);

      //! connects to an INET socket by hostname and port number, negotiates an SSL connection, and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** @param host the name or IP address of the host
	  @param prt the port number of the remote socket
	  @param cert the X509 certificate to use for the connection, may be 0 if no certificate should be used
	  @param pkey the private key to use for the connection, may be 0 if no private key should be used
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @note the same as calling QoreSocket::connectINET() and then QoreSocket::upgradeClientToSSL()
	  @see QoreSocket::connect()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectINET()
	  @see QoreSocket::connectSSL()
	  @see QoreSocket::connectUNIXSSL()
	  @see QoreSocket::upgradeClientToSSL()
       */
      DLLEXPORT int connectINETSSL(const char *host, int prt, X509 *cert, EVP_PKEY *pkey, ExceptionSink *xsink);

      //! connects to an INET socket by hostname and port number, negotiates an SSL connection, and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** @param host the name or IP address of the host
	  @param prt the port number of the remote socket
	  @param timeout_ms the timeout period in milliseconds
	  @param cert the X509 certificate to use for the connection, may be 0 if no certificate should be used
	  @param pkey the private key to use for the connection, may be 0 if no private key should be used
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @note the same as calling QoreSocket::connectINET() and then QoreSocket::upgradeClientToSSL()
	  @see QoreSocket::connect()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectINET()
	  @see QoreSocket::connectSSL()
	  @see QoreSocket::connectUNIXSSL()
	  @see QoreSocket::upgradeClientToSSL()
       */
      DLLEXPORT int connectINETSSL(const char *host, int prt, int timeout_ms, X509 *cert, EVP_PKEY *pkey, ExceptionSink *xsink);

      //! connects to a UNIX domain socket, negotiates an SSL connection, and returns a status code, Qore-language exceptions are raised in the case of any errors
      /** @param p the file name of the UNIX domain socket
	  @param cert the X509 certificate to use for the connection, may be 0 if no certificate should be used
	  @param pkey the private key to use for the connection, may be 0 if no private key should be used
	  @param xsink if not 0, if an error occurs, the Qore-language exception information will be added here
	  @return 0 for OK, -1 means that an error occured and a Qore-language exception was raised
	  @note the same as calling QoreSocket::connectUNIX() and then QoreSocket::upgradeClientToSSL()
	  @see QoreSocket::connect()
	  @see QoreSocket::connectINET()
	  @see QoreSocket::connectUNIX()
	  @see QoreSocket::connectSSL()
	  @see QoreSocket::connectINETSSL()
	  @see QoreSocket::upgradeClientToSSL()
       */
      DLLEXPORT int connectUNIXSSL(const char *p, X509 *cert, EVP_PKEY *pkey, ExceptionSink *xsink);

      //! binds to a UNIX domain socket or INET interface:port using TCP and returns a status code
      /** If "name" has a ':' in it; it's assumed to be a address:port specification for binding to an INET socket, 
	  otherwise "name" is assumed to be a file name for a UNIX domain socket.
	  @note a socket file will be created on the filesystem if a UNIX domain socket is opened.
	  @note the socket will be closed and reopened if necessary
	  @param name address:port or filename to bind to
	  @param reuseaddr if true then setsockopt() will be called with SO_REUSEADDR, allowing the bind to succeed even if the port is still in a TIME_WAIT state, for example
	  @return 0 for OK, not 0 for error
       */
      DLLEXPORT int bind(const char *name, bool reuseaddr = false);

      //! binds to a TCP INET port on all interfaces and returns a status code
      /** @note the socket will be closed and reopened if necessary
	  @param prt the port to bind to
	  @param reuseaddr if true then setsockopt() will be called with SO_REUSEADDR, allowing the bind to succeed even if the port is still in a TIME_WAIT state, for example
	  @return 0 for OK, not 0 for error
       */
      DLLEXPORT int bind(int prt, bool reuseaddr);

      //! binds to a TCP INET port on the given interface and returns a status code
      /** @note the socket will be closed and reopened if necessary
	  @param interface the interface to bind to (hostname or IP address)
	  @param prt the port to bind to
	  @param reuseaddr if true then setsockopt() will be called with SO_REUSEADDR, allowing the bind to succeed even if the port is still in a TIME_WAIT state, for example
	  @return 0 for OK, not 0 for error
       */
      DLLEXPORT int bind(const char *interface, int prt, bool reuseaddr = false);

      //! binds an INET TCP socket to a specific socket address
      /** @note the socket will be closed and reopened if necessary
	  @param addr the socket address to bind to
	  @param addr_size the size of the addr argument
	  @return 0 for OK, not 0 for error
       */
      DLLEXPORT int bind(const struct sockaddr *addr, int addr_size);

      //! returns the TCP port number, also assigns the interal port number if it must be discovered
      DLLEXPORT int getPort();

      //! accepts a new connection on a listening socket and returns a new QoreSocket object for the new connection
      /** the socket must be opened and in a listening state before making this call.
	  @param source source connection information will be written to this object if not 0
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return a new QoreSocket object for the new connection (or 0 if an error occured)
	  @see QoreSocket::listen()
	  @see QoreSocket::acceptSSL()
	  @see SocketSource
       */
      DLLEXPORT QoreSocket *accept(SocketSource *source, ExceptionSink *xsink);

      //! accepts a new connection on a listening socket, negotiates an SSL connection, and returns a new QoreSocket object for the new connection
      /** the socket must be opened and in a listening state before making this call.
	  @param source source connection information will be written to this object if not 0
	  @param cert the X509 certificate to use for the connection, may be 0 if no certificate should be used
	  @param pkey the private key to use for the connection, may be 0 if no private key should be used
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return a new QoreSocket object for the new connection (or 0 if an error occured)
	  @note the same as calling QoreSocket::accept() and then QoreSocket::upgradeServerToSSL() on the new socket
	  @see QoreSocket::listen()
	  @see SocketSource
       */
      DLLEXPORT QoreSocket *acceptSSL(SocketSource *source, X509 *cert, EVP_PKEY *pkey, ExceptionSink *xsink);

      //! accepts a new connection on a listening socket and replaces the current socket with the new connection
      /** the socket must be opened and in a listening state before making this call.
	  @param source source connection information will be written to this object if not 0
	  @return 0 for OK, not 0 if an error occured
	  @see QoreSocket::listen()
	  @see QoreSocket::accept()
	  @see QoreSocket::acceptSSL()
	  @see SocketSource
       */
      DLLEXPORT int acceptAndReplace(SocketSource *source);

      //! sets an open socket to the listening state
      /**
	  @return 0 for OK, not 0 if an error occured
       */
      DLLEXPORT int listen();

      //! sends binary data on a connected socket
      /**
	 @param buf the data to send
	 @param size the size of the data to send
	 @return 0 for OK, not 0 if an error occured
       */
      DLLEXPORT int send(const char *buf, qore_size_t size);
  
      //! sends string data on a connected socket, converts the string encoding to the socket's encoding if necessary
      /**
	 @param msg the string to send (must not be 0)
	 @param xsink if an error occurs in converting the string's character encoding, the Qore-language exception information will be added here
	 @return 0 for OK, not 0 if an error occured
       */
      DLLEXPORT int send(const QoreString *msg, ExceptionSink *xsink);

      //! sends binary data on a connected socket
      /**
	 @param msg the data to send
	 @return 0 for OK, not 0 if an error occured
       */
      DLLEXPORT int send(const BinaryNode *msg);

      //! sends untranslated data from an open file descriptor
      /**
	 @param fd a file descriptor, open for reading
	 @param size the number of bytes to send (-1 = send all until EOF)
	 @return 0 for OK, not 0 if an error occured
       */
      DLLEXPORT int send(int fd, qore_offset_t size = -1);

      //! sends a 1-byte binary integer data to a connected socket
      /** The socket must be connected before this call is made.
	  @param i the 1-byte integer to send through the socket
	  @return 0 for OK, not 0 if an error occured
       */
      DLLEXPORT int sendi1(char i);

      //! sends a 2-byte (16bit) binary integer in MSB (Most Significant Byte first, big endian, network) format through a connected socket
      /** The socket must be connected before this call is made.
	  @param i the integer to write to the file
	  @return 0 for OK, not 0 if an error occured
	  @see QoreSocket::sendi2LSB()
       */
      DLLEXPORT int sendi2(short i);

      //! sends a 4-byte (32bit) binary integer in MSB (Most Significant Byte first, big endian, network) format through a connected socket
      /** The socket must be connected before this call is made.
	  @param i the integer to write to the file
	  @return 0 for OK, not 0 if an error occured
	  @see QoreSocket::sendi4LSB()
       */
      DLLEXPORT int sendi4(int i);

      //! sends an 8-byte (64bit) binary integer in MSB (Most Significant Byte first, big endian, network) format through a connected socket
      /** The socket must be connected before this call is made.
	  @param i the integer to write to the file
	  @return 0 for OK, not 0 if an error occured
	  @see QoreSocket::sendi8LSB()
       */
      DLLEXPORT int sendi8(int64 i);

      //! sends a 2-byte (16bit) binary integer in LSB (Least Significant Byte first, little endian) format through a connected socket
      /** The socket must be connected before this call is made.
	  @note that this is not network byte order, normally QoreSocket::sendi2() should be used
	  @param i the integer to write to the file
	  @return 0 for OK, not 0 if an error occured
	  @see QoreSocket::sendi2()
       */
      DLLEXPORT int sendi2LSB(short i);

      //! sends a 4-byte (32bit) binary integer in LSB (Least Significant Byte first, little endian) format through a connected socket
      /** The socket must be connected before this call is made.
	  @note that this is not network byte order, normally QoreSocket::sendi4() should be used
	  @param i the integer to write to the file
	  @return 0 for OK, not 0 if an error occured
	  @see QoreSocket::sendi4()
       */
      DLLEXPORT int sendi4LSB(int i);

      //! sends an 8-byte (64bit) binary integer in LSB (Least Significant Byte first, little endian) format through a connected socket
      /** The socket must be connected before this call is made.
	  @note that this is not network byte order, normally QoreSocket::sendi8() should be used
	  @param i the integer to write to the file
	  @return 0 for OK, not 0 if an error occured
	  @see QoreSocket::sendi8()
       */
      DLLEXPORT int sendi8LSB(int64 i);

      //! reads a 1-byte signed integer from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvu1()
       **/
      DLLEXPORT int recvi1(int timeout, char *val);

      //! reads a 2-byte signed integer in MSB (Most Significant Byte first, big endian, network) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi2LSB()
	  @see QoreSocket::recvu2()
	  @see QoreSocket::recvu2LSB()
       **/
      DLLEXPORT int recvi2(int timeout, short *val);

      //! reads a 4-byte signed integer in MSB (Most Significant Byte first, big endian, network) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi4LSB()
	  @see QoreSocket::recvu4()
	  @see QoreSocket::recvu4LSB()
       **/
      DLLEXPORT int recvi4(int timeout, int *val);

      //! reads an 8-byte signed integer in MSB (Most Significant Byte first, big endian, network) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi8LSB()
	  @see QoreSocket::recvu8()
	  @see QoreSocket::recvu8LSB()
       **/
      DLLEXPORT int recvi8(int timeout, int64 *val);

      //! reads a 2-byte signed integer in LSB (Most Significant Byte first, little endian) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @note that this is not network byte order, normally QoreSocket::recvi2() should be used instead
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi2()
	  @see QoreSocket::recvu2()
	  @see QoreSocket::recvu2LSB()
       **/
      DLLEXPORT int recvi2LSB(int timeout, short *val);

      //! reads a 4-byte signed integer in LSB (Most Significant Byte first, little endian) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @note that this is not network byte order, normally QoreSocket::recvi4() should be used instead
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi4()
	  @see QoreSocket::recvu4()
	  @see QoreSocket::recvu4LSB()
       **/
      DLLEXPORT int recvi4LSB(int timeout, int *val);

      //! reads an 8-byte signed integer in LSB (Most Significant Byte first, little endian) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @note that this is not network byte order, normally QoreSocket::recvi8() should be used instead
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi8()
       **/
      DLLEXPORT int recvi8LSB(int timeout, int64 *val);

      //! reads a 1-byte unsigned integer from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi1()
       **/
      DLLEXPORT int recvu1(int timeout, unsigned char *val);

      //! reads a 2-byte unsigned integer in MSB (Most Significant Byte first, big endian, network) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi2()
	  @see QoreSocket::recvu2LSB()
	  @see QoreSocket::recvi2LSB()
       **/
      DLLEXPORT int recvu2(int timeout, unsigned short *val);

      //! reads a 4-byte unsigned integer in MSB (Most Significant Byte first, big endian, network) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvi4()
	  @see QoreSocket::recvu4LSB()
	  @see QoreSocket::recvi4LSB()
       **/
      DLLEXPORT int recvu4(int timeout, unsigned int *val);

      //! reads a 2-byte unsigned integer in LSB (Most Significant Byte first, little endian) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @note that this is not network byte order, normally QoreSocket::recvu2() should be used instead
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvu2()
	  @see QoreSocket::recvi2()
	  @see QoreSocket::recvi2LSB()
       **/
      DLLEXPORT int recvu2LSB(int timeout, unsigned short *val);

      //! reads a 4-byte unsigned integer in LSB (Most Significant Byte first, little endian) format from the socket with a timeout value and returns the value read as an output parameter
      /** The socket must be connected before this call is made.
	  @note that this is not network byte order, normally QoreSocket::recvu4() should be used instead
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param val output parameter: the integer value read from the file
	  @return 0 for OK, not 0 for error
	  @see QoreSocket::recvu4()
	  @see QoreSocket::recvi4()
	  @see QoreSocket::recvi4LSB()
       **/
      DLLEXPORT int recvu4LSB(int timeout, unsigned int *val);

      //! receive a certain number of bytes with a timeout value and return a QoreStringNode, caller owns the reference count returned
      /** The socket must be connected before this call is made.
	  @param bufsize number of bytes to read from the socket; if <= 0, read all data available from the socket until the socket is closed from the other side
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param prc output parameter: 0 for OK, not 0 for error
	  @return the data read as a QoreStringNode tagged with the socket's QoreEncoding, caller owns the reference count returned (0 if an error occurs)
	  @see QoreEncoding
       */
      DLLEXPORT QoreStringNode *recv(qore_offset_t bufsize, int timeout, int *prc);

      //! receive a certain number of bytes with a timeout value and return a BinaryNode, caller owns the reference count returned
      /** The socket must be connected before this call is made.
	  @param bufsize number of bytes to read from the socket; if <= 0, read all data available from the socket until the socket is closed from the other side
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param prc output parameter: 0 for OK, not 0 for error
	  @return the data read as a BinaryNode, caller owns the reference count returned (0 if an error occurs)
       */
      DLLEXPORT BinaryNode *recvBinary(qore_offset_t bufsize, int timeout, int *prc);

      //! receive with a timeout value and return a QoreStringNode, caller owns the reference count returned
      /** The socket must be connected before this call is made.
	  This call will read data, blocking according to the timeout value.  Then all data
	  available on the socket will be read and returned as a QoreStringNode.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param prc output parameter: 0 for OK, not 0 for error
	  @return the data read as a QoreStringNode tagged with the socket's QoreEncoding, caller owns the reference count returned (0 if an error occurs)
	  @see QoreEncoding
       */
      DLLEXPORT QoreStringNode *recv(int timeout, int *prc);

      //! receive all available data with a timeout value and return a BinaryNode, caller owns the reference count returned
      /** The socket must be connected before this call is made.
	  This call will read data, blocking according to the timeout value.  Then all data
	  available on the socket will be read and returned as a BinaryNode.  As soon as the 
	  first timeout occurs, the data will be returned immediately without blocking.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param prc output parameter: 0 for OK, not 0 for error
	  @return the data read as a BinaryNode, caller owns the reference count returned (0 if an error occurs)
       */
      DLLEXPORT BinaryNode *recvBinary(int timeout, int *prc);

      //! receive data on the socket and write it to a file descriptor
      /** The socket must be connected before this call is made.
	  @param fd the file descriptor to write to, must be already opened for writing
	  @param size the number of bytes to read from the socket, -1 to read until the socket is closed
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @return 0 for OK, not 0 for error
	  @note the timeout value applies to each read from the socket
       */
      DLLEXPORT int recv(int fd, qore_offset_t size, int timeout);

      //! send an HTTP request message on the socket
      /** The socket must be connected before this call is made.
	  @param method the method string to use in the header - no validity checking is made on this string
	  @param path the path string to use in the header, if the path is empty then '/' is sent
	  @param http_version should be either "1.0" or "1.1"
	  @param headers a hash of headers to send (key: value)
	  @param data optional message body to send (may be 0)
	  @param size the length of the message body (may be 0)
	  @param source the event source code for socket events
	  @return 0 for OK, not 0 for error
       */
      DLLEXPORT int sendHTTPMessage(const char *method, const char *path, const char *http_version, const QoreHashNode *headers, const void *data, qore_size_t size, int source = QORE_SOURCE_SOCKET);

      //! send an HTTP response message on the socket
      /** The socket must be connected before this call is made.
	  @param code the HTTP response code
	  @param desc the text description for the response code
	  @param http_version should be either "1.0" or "1.1"
	  @param headers a hash of headers to send (key: value)
	  @param data optional message body to send (may be 0)
	  @param size the length of the message body (may be 0)
	  @param source the event source code for socket events
	  @return 0 for OK, not 0 for error
       */
      DLLEXPORT int sendHTTPResponse(int code, const char *desc, const char *http_version, const QoreHashNode *headers, const void *data, qore_size_t size, int source = QORE_SOURCE_SOCKET);

      //! read and parse HTTP header, caller owns AbstractQoreNode reference count returned
      /** The socket must be connected before this call is made.
	  @note does not read the message body; message body must be read manually
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param prc output parameter: 0 or -2: remote end closed the connection, -1: receive error, -3: timeout
	  @param source the event source code for socket events
	  @return if 0 (and prc == 0), the socket was closed on the remote end without a response, if the type is NT_STRING, the response could not be parsed, if not 0, caller owns the reference count returned
       */
      DLLEXPORT AbstractQoreNode *readHTTPHeader(int timeout, int *prc, int source = QORE_SOURCE_SOCKET);

      //! receive a binary message in HTTP chunked transfer encoding, caller owns QoreHashNode reference count returned
      /** The socket must be connected before this call is made.
	  The message body is returned as a BinaryNode in the "body" key, any footers read after the body
	  are returned as the other hash keys in the hash.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @param source the event source code for socket events
	  @return the message body as the value of the "body" key and any footers read after the body as other keys (0 if an error occurs)
	  @see BinaryNode
       */
      DLLEXPORT QoreHashNode *readHTTPChunkedBodyBinary(int timeout, ExceptionSink *xsink, int source = QORE_SOURCE_SOCKET);

      //! receive a string message in HTTP chunked transfer encoding, caller owns QoreHashNode reference count returned
      /** The socket must be connected before this call is made.
	  The message body is returned as a QoreStringNode in the "body" key, any footers read after the body
	  are returned as the other hash keys in the hash.
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @param source the event source code for socket events
	  @return the message body as the value of the "body" key and any footers read after the body as other keys (0 if an error occurs)
	  @see QoreStringNode
       */
      DLLEXPORT QoreHashNode *readHTTPChunkedBody(int timeout, ExceptionSink *xsink, int source = QORE_SOURCE_SOCKET);

      //! set send timeout in milliseconds
      DLLEXPORT int setSendTimeout(int ms);

      //! set recv timeout in milliseconds
      DLLEXPORT int setRecvTimeout(int ms);

      //! get send timeout in milliseconds
      DLLEXPORT int getSendTimeout() const;

      //! get recv timeout in milliseconds
      DLLEXPORT int getRecvTimeout() const;

      //! returns true if data is available on the socket in the timeout period in milliseconds
      /** The socket must be connected before this call is made.
	  use a timeout of 0 to see if there is any data available on the socket
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @return true if data is available within the timeout period
       */
      DLLEXPORT bool isDataAvailable(int timeout = 0) const;

      //! closes the socket
      /** Deletes the socket file if it was a UNIX domain socket and was created with the QoreSocket::bind() call.
	  Also implicitly calls QoreSocket::shutdownSSL() if an SSL connection is active.
	  @return 0 if OK, not 0 on error
       */
      DLLEXPORT int close();

      //! calls shutdown on the socket
      /** shuts down the socket for reading and writing, after this call further sends and receives are
	  disallowed until the socket is reopened.
	  @note QoreSocket::shutdown() should normally be called before calling this function.
	  @return 0 if OK, not 0 on error
	  @see QoreSocket::shutdown()
       */
      DLLEXPORT int shutdown();

      //! shuts down an active SSL connection
      /** called implicitly by QoreSocket::close()
	  @return 0 if OK, not 0 on error
	  @see QoreSocket::close()
       */
      DLLEXPORT int shutdownSSL(ExceptionSink *xsink);

      //! returns the file descriptor associated with this socket
      /** @return the file descriptor associated with this socket
       */
      DLLEXPORT int getSocket() const;

      //! returns the character encoding associated with this socket
      /** @return the character encoding associated with this socket
       */
      DLLEXPORT const QoreEncoding *getEncoding() const;

      //! sets the character encoding for strings sent and received with this socket
      /** @param id the character encoding for strings sent and received with this socket
       */
      DLLEXPORT void setEncoding(const QoreEncoding *id);

      //! returns true if the socket is open
      /** @return true if the socket is open
       */
      DLLEXPORT bool isOpen() const;

      //! returns the name of the SSL Cipher for the currently-connected control connection, or 0 if there is none
      /** @return the name of the SSL Cipher for the currently-connected control connection, or 0 if there is none
       */
      DLLEXPORT const char *getSSLCipherName() const;

      //! returns the version string of the SSL Cipher for the currently-connected control connection, or 0 if there is none
      /** @return the version string of the SSL Cipher for the currently-connected control connection, or 0 if there is none
       */
      DLLEXPORT const char *getSSLCipherVersion() const;

      //! returns true if an SSL connection is active
      /** @return true if an SSL connection is active
       */
      DLLEXPORT bool isSecure() const;

      //! returns the peer certificate verification code if an SSL connection is in progress
      DLLEXPORT long verifyPeerCertificate() const;

      //! negotiates an SSL connection from the client side
      /** The socket must be connected before this call is made.
	  @param cert the X509 certificate to use for the connection, may be 0 if no certificate should be used
	  @param pkey the private key to use for the connection, may be 0 if no private key should be used
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return 0 if OK, not 0 on error	  
      */
      DLLEXPORT int upgradeClientToSSL(X509 *cert, EVP_PKEY *pkey, ExceptionSink *xsink);

      //! negotiates an SSL connection from the client side
      /** The socket must be connected before this call is made.
	  @param cert the X509 certificate to use for the connection, may be 0 if no certificate should be used
	  @param pkey the private key to use for the connection, may be 0 if no private key should be used
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return 0 if OK, not 0 on error	  
      */
      DLLEXPORT int upgradeServerToSSL(X509 *cert, EVP_PKEY *pkey, ExceptionSink *xsink);

      //! returns true if all write data has been written within the timeout period in milliseconds
      /** The socket must be connected before this call is made.
	  use a timeout of 0 to receive an answer immediately
	  @param timeout in milliseconds, -1=never timeout, 0=do not block, return immediately if there is no data waiting 
	  @return true if data is available within the timeout period
       */
      DLLEXPORT bool isWriteFinished(int timeout = 0) const;

      DLLLOCAL static void doException(int rc, const char *meth, ExceptionSink *xsink);

      //! sets the event queue (not part of the library's pubilc API), must be already referenced before call
      DLLLOCAL void setEventQueue(Queue *cbq, ExceptionSink *xsink);

      //! returns the event queue (not part of the library's public API)
      DLLLOCAL Queue *getQueue();

      //! returns a unique ID for the socket to be used in event messages
      DLLLOCAL int64 getObjectIDForEvents() const;

      //! posts deleted message and removes any event queue
      DLLLOCAL void cleanup(ExceptionSink *xsink);

      DLLLOCAL int setNoDelay(int nodelay);
      DLLLOCAL int getNoDelay() const;
};

#endif // _QORE_QORESOCKET_H
