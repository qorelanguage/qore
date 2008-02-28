/*
  QoreHTTPClient.h

  Qore Programming Language

  Copyright (C) 2006, 2007 QoreTechnologies
  
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

#ifndef QORE_HTTP_CLIENT_H_
#define QORE_HTTP_CLIENT_H_

#include <qore/common.h>
#include <qore/AbstractPrivateData.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreSocket.h>

#define HTTPCLIENT_DEFAULT_PORT 80              //!< the default port number to use
#define HTTPCLIENT_DEFAULT_HOST "localhost"     //!< the default host name to use

#define HTTPCLIENT_DEFAULT_TIMEOUT 300000       //!< the default connection and response packet timeout to use (300,000 ms = 5m)

#define HTTPCLIENT_DEFAULT_MAX_REDIRECTS 5      //!< maximum number of HTTP redirects allowed

//! provides a way to communicate with HTTP servers using Qore data structures
/** thread-safe, uses QoreSocket for socket communication
 */
class QoreHTTPClient : public AbstractPrivateData, public QoreThreadLock
{
   private:
      //! private implementation of the class
      struct qore_qtc_private *priv;
      
      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int set_url_unlocked(const char *url, class ExceptionSink *xsink);
      // returns -1 if an exception was thrown, 0 for OK
      DLLEXPORT int set_proxy_url_unlocked(const char *url, class ExceptionSink *xsink);
      // returns -1 if an exception was thrown, 0 for OK
      DLLLOCAL int connect_unlocked(class ExceptionSink *xsink);
      DLLLOCAL void disconnect_unlocked();
      DLLLOCAL class QoreHashNode *send_internal(const char *meth, const char *mpath, const class QoreHashNode *headers, const void *data, unsigned size, bool getbody, class ExceptionSink *xsink);
      DLLLOCAL void setSocketPath();
      DLLLOCAL const char *getMsgPath(const char *mpath, class QoreString &pstr);
      DLLLOCAL class QoreHashNode *getResponseHeader(const char *meth, const char *mpath, class QoreHashNode &nh, const void *data, unsigned size, int &code, class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *getHostHeaderValue();

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreHTTPClient(const QoreHTTPClient&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreHTTPClient& operator=(const QoreHTTPClient&);

   public:
      //! creates the QoreHTTPClient object
      DLLEXPORT QoreHTTPClient();

      //! destroys the object and frees all associated memory
      DLLEXPORT virtual ~QoreHTTPClient();

      //! set options with a hash, returns -1 if an exception was thrown, 0 for OK
      /** options are:
	  - protocols: a hash where each key is a protocol name and the value must be set to a integer giving a port number or a hash having the following keys:
	    - port: giving the port number
	    - ssl: giving a boolean true or false value
	  - max_redirects: sets the max_redirects option
	  - default_port: sets the default port number
	  - proxy: sets the proxy URL
	  - url: sets the default connection URL
	  - default_path: sets the default path
	  - timeout: sets the connection or response packet timeout value in milliseconds
	  - http_version: either "1.0" or "1.1" to set the default HTTP version to use
	  @note this function is unlocked and designed only to be called with the constructor
	  @param opts the options to set for the object
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return -1 if an exception was thrown, 0 for OK
       */
      DLLEXPORT int setOptions(const class QoreHashNode *opts, ExceptionSink* xsink);

      //! sets the default port
      /** useful for c++ derived classes
       */
      DLLEXPORT void setDefaultPort(int prt);

      //! sets the default path
      /** useful for c++ derived classes
       */
      DLLEXPORT void setDefaultPath(const char *pth);

      //! adds a protocol
      /** useful for c++ derived classes
       */
      DLLEXPORT void addProtocol(const char *prot, int port, bool ssl = false);

      //! sets the connection and response packet timeout value in milliseconds
      DLLEXPORT void setTimeout(int to);

      //! returns the connection and response packet timeout value in milliseconds
      DLLEXPORT int getTimeout() const;

      //! sets the default encoding for the object
      DLLEXPORT void setEncoding(const class QoreEncoding *qe);

      //! returns the default encoding for the object
      DLLEXPORT const class QoreEncoding *getEncoding() const;
      
      //! sets the http version from a string
      /**
	 @param version either "1.0" or "1.1"
	 @param xsink if an error occurs, the Qore-language exception information will be added here	  
	 @return -1 if an exception was thrown, 0 for OK
      */
      DLLEXPORT int setHTTPVersion(const char* version, ExceptionSink* xsink);

      //! returns the http version as a string (either "1.0" or "1.1")
      /**
	 @return the http version as a string (either "1.0" or "1.1")
       */
      DLLEXPORT const char* getHTTPVersion() const;

      //! sets or clears HTTP 1.1 protocol compliance
      /**
	 @param h11 if true sets HTTP 1.1 protocol compliance, if false set 1.0
       */
      DLLEXPORT void setHTTP11(bool h11);

      //! returns true if HTTP 1.1 protocol compliance has been set
      DLLEXPORT bool isHTTP11() const;

      //! sets the connection URL
      /** @param url the URL to use for connection parameters
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return -1 if an exception was thrown, 0 for OK
       */
      DLLEXPORT int setURL(const char *url, class ExceptionSink *xsink);

      //! returns the connection parameters as a URL, caller owns the reference count returned
      /**
	 @return the connection parameters as a URL, caller owns the reference count returned
      */
      DLLEXPORT class QoreStringNode *getURL();

      //! sets the proxy URL
      /** @param url the URL to use for connection to the proxy
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return -1 if an exception was thrown, 0 for OK
       */
      DLLEXPORT int setProxyURL(const char *proxy, class ExceptionSink *xsink);

      //! returns the proxy connection parameters as a URL (or 0 if there is none), caller owns the reference count returned
      /**
	 @return the proxy connection parameters as a URL, caller owns the reference count returned
      */
      DLLEXPORT class QoreStringNode *getProxyURL();

      //! clears the proxy URL
      DLLEXPORT void clearProxyURL();

      //! sets the SSL flag for use in the next connection
      DLLEXPORT void setSecure(bool is_secure);

      //! returns the SSL connection parameter flag
      DLLEXPORT bool isSecure() const;

      //! sets the SSL flag for use in the next connection to the proxy
      DLLEXPORT void setProxySecure(bool is_secure);

      //! returns the SSL proxy connection parameter flag
      DLLEXPORT bool isProxySecure() const;

      //! sets the max_redirects option
      DLLEXPORT void setMaxRedirects(int max);

      //! returns the value of the max_redirects option
      DLLEXPORT int getMaxRedirects() const;

      //! returns the peer certificate verification code if an SSL connection is in progress
      DLLEXPORT long verifyPeerCertificate();

      //! returns the name of the SSL Cipher for the currently-connected control connection, or 0 if there is none
      /**
	 @return the name of the SSL Cipher for the currently-connected control connection, or 0 if there is none
       */
      DLLEXPORT const char *getSSLCipherName();

      //! returns the version string of the SSL Cipher for the currently-connected control connection, or 0 if there is none
      /**
	 @return the version string of the SSL Cipher for the currently-connected control connection, or 0 if there is none
       */
      DLLEXPORT const char *getSSLCipherVersion();

      //! opens a connection and returns a code giving the result
      /** @return -1 if an exception was thrown, 0 for OK
       */
      DLLEXPORT int connect(class ExceptionSink *xsink);

      //! disconnects from the remote server
      DLLEXPORT void disconnect();
      
      //! sends a message to the remote server and returns the entire response as a hash, caller owns the QoreHashNode reference returned
      /** possible errors: method not recognized, redirection errors, socket communication errors, timeout errors
	  @param meth the HTTP method name to send
	  @param path the path string to send in the header
	  @param headers a hash of headers to add to the message
	  @param data optional data to send (may be 0)
	  @param size the byte length of the data to send (if this is 0 then no data is sent)
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return the entire response as a hash, caller owns the QoreHashNode reference returned (0 if there was an error)
       */
      DLLEXPORT class QoreHashNode *send(const char *meth, const char *path, const class QoreHashNode *headers, const void *data, unsigned size, bool getbody, class ExceptionSink *xsink);

      //! sends an HTTP "GET" method and returns the value of the message body returned, the caller owns the AbstractQoreNode reference returned
      /** if you need to get all the headers received, then use QoreHTTPClient::send() instead
	  @param path the path string to send in the header
	  @param headers a hash of headers to add to the message
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return the body of the response message, caller owns the QoreHashNode reference returned (0 if there was an error or no body returned)
       */
      DLLEXPORT class AbstractQoreNode *get(const char *path, const class QoreHashNode *headers, class ExceptionSink *xsink);

      //! sends an HTTP "HEAD" method and returns the headers returned, the caller owns the QoreHashNode reference returned
      /** @param path the path string to send in the header
	  @param headers a hash of headers to add to the message
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return the entire response as a hash, caller owns the QoreHashNode reference returned (0 if there was an error)
       */
      DLLEXPORT class QoreHashNode *head(const char *path, const class QoreHashNode *headers, class ExceptionSink *xsink);

      //! sends an HTTP "POST" message to the remote server and returns the message body of the response, caller owns the AbstractQoreNode reference returned
      /** possible errors: method not recognized, redirection errors, socket communication errors, timeout errors
	  @param path the path string to send in the header
	  @param headers a hash of headers to add to the message
	  @param data optional data to send (should not be 0 for a POST)
	  @param size the byte length of the data to send (if this is 0 then no data is sent)
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return the body of the response message, caller owns the QoreHashNode reference returned (0 if there was an error or no body returned)
       */
      DLLEXPORT class AbstractQoreNode *post(const char *path, const class QoreHashNode *headers, const void *data, unsigned size, class ExceptionSink *xsink);

      //! sets the value of a default header to send with every outgoing message
      /**
	 @param header the name of the header to send
	 @param val the string value to use in the HTTP header
       */
      DLLEXPORT void setDefaultHeaderValue(const char *header, const char *val);

      DLLLOCAL static void static_init();
};

#endif 

// EOF

