/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreFtpClient.h
  
  thread-safe Qore QoreFtpClient object
  
  Qore Programming Language
  
  Copyright (C) 2003 - 2014 David Nichols

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

#ifndef _QORE_QOREFTPCLIENT_H

#define _QORE_QOREFTPCLIENT_H

#define DEFAULT_FTP_CONTROL_PORT  21
#define DEFAULT_FTP_DATA_PORT     20

#define DEFAULT_USERNAME "anonymous"
#define DEFAULT_PASSWORD "qore@nohost.com"

class FtpResp;
class Queue;

//! provides thread-safe access to FTP servers through Qore data structures
/**
   is "auto" mode, tries the following data modes in order:
   - EPSV mode (RFC 2428) 
   - PASV mode (RFC 959)
   - then PORT 

   references: 
   - RFC 959: FTP
   - RFC 2428: EPSV mode only (no IPv6 support yet)
   - RFC 4217 (supercedes RFC 2228):
   - AUTH TLS: secure authentication
   - PBSZ 0 and PROT P: secure data connections

   @note RFC 1639: LPSV mode not implemented yet

   tested with:
   - tnftpd 20040810 (Darwin/OS X 10.3.8) EPSV, PASV, PORT
   - vsFTPd 2.0.1 (Fedora Core 3) EPSV
   - proFTPd 1.3.0 (Darwin/OS X 10.4.7) EPSV, PORT, AUTH TLS, PBSZ 0, PROT P
*/
class QoreFtpClient {
protected:
   //! private implementation of the object
   struct qore_ftp_private *priv;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreFtpClient(const QoreFtpClient&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreFtpClient& operator=(const QoreFtpClient&);

public:
   //! creates the object and sets connection parameters based on the url passed
   /** a Qore-language exception will be raised if the URL is invalid (protocol is not "ftp" 
       or "ftps") or the hostname is missing.
       @param url the URL string to use to set connection parameters
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT QoreFtpClient(const QoreString *url, ExceptionSink *xsink);

   //! creates the object with no connection parameters
   DLLEXPORT QoreFtpClient();

   //! disconnects from the host if necessary and frees all memory associated with the object
   DLLEXPORT ~QoreFtpClient();

   //! connects to the remote host and logs in
   /** if there are any connection or authentication errors, Qore-language exceptions are raised
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)
   */
   DLLEXPORT int connect(ExceptionSink *xsink);

   //! disconnects from the remote host if connected
   /**
      @return 0 for OK, non-zero for error (currently always returns 0)
   */
   DLLEXPORT int disconnect();

   //! changes the working directory on the remote host
   /** if there are any errors (if no connection has been previously established, it's an error),
       Qore-language exceptions are raised.
       @param dir the directory to change to
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)
   */
   DLLEXPORT int cwd(const char *dir, ExceptionSink *xsink);

   //! returns the working directory on the remote host (caller owns the reference count returned)
   /** the connection must be already established before this function is called or an error will be raised.
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return a string giving the working directory on the remote host (caller owns the reference count returned), 0 if an error occured
   */
   DLLEXPORT QoreStringNode *pwd(ExceptionSink *xsink);

   //! sends a file from the local filesystem to the remote server
   /** the connection must be already established before this function is called or an error will be raised.
       @param localpath the local path of the file to send
       @param remotename the name of the file on the remote server
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)
   */
   DLLEXPORT int put(const char *localpath, const char *remotename, ExceptionSink *xsink);

   //! gets a file from the remote server and saves it on the local filesystem
   /** the connection must be already established before this function is called or an error will be raised.
       @param remotepath the path of the file on the remote server
       @param localname the local name of the file
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)

       @see QoreFtpClient::getAsString()
       @see QoreFtpCleint::getAsBinary()
   */
   DLLEXPORT int get(const char *remotepath, const char *localname, ExceptionSink *xsink);

   //! sends a file data io the remote server
   /** the connection must be already established before this function is called or an error will be raised.
       @param data the data to send
       @param len the length of the data to send (if 0, a Qore-language exception will be raised)
       @param remotename the name of the file on the remote server
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)
   */
   DLLEXPORT int putData(const void *data, qore_size_t len, const char *remotename, ExceptionSink *xsink);

   //! gets a file from the remote server and returns it as a string
   /** the connection must be already established before this function is called or an error will be raised.
       @param remotepath the path of the file on the remote server
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return file data received as a string, otherwise 0 = error (meaning that an exception has been raised)
   */
   DLLEXPORT QoreStringNode *getAsString(const char *remotepath, ExceptionSink *xsink);

   //! gets a file from the remote server and returns it as a binary node
   /** the connection must be already established before this function is called or an error will be raised.
       @param remotepath the path of the file on the remote server
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return file data received as a binary node, otherwise 0 = error (meaning that an exception has been raised)
   */
   DLLEXPORT BinaryNode *getAsBinary(const char *remotepath, ExceptionSink *xsink);

   //! renames/moves a file on the remote server
   /** the connection must be already established before this function is called or an error will be raised.
       @param from the original file path on the remote server
       @param to the new file path on the remote server
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)
   */
   DLLEXPORT int rename(const char *from, const char *to, ExceptionSink *xsink);

   //! returns a string listing the directory contents on the remote host (caller owns the reference count returned)
   /** the connection must be already established before this function is called or an error will be raised.
       @param path the path to list
       @param long_list if true then a "long list" is made
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return a string giving the directory listing on the remote host (caller owns the reference count returned), 0 if an error occured
   */
   DLLEXPORT QoreStringNode *list(const char *path, bool long_list, ExceptionSink *xsink);

   //! deletes the given file on the remote server
   /** the connection must be already established before this function is called or an error will be raised.
       @param file the filename to delete
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)
   */
   DLLEXPORT int del(const char *file, ExceptionSink *xsink);

   //! creates a directory on the remote server
   /** the connection must be already established before this function is called or an error will be raised.
       @param remotepath the path of the directory on the remote server
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)
   */
   DLLEXPORT int mkdir(const char *remotepath, ExceptionSink *xsink);

   //! removes a directory on the remote server
   /** the connection must be already established before this function is called or an error will be raised.
       @param remotepath the path of the directory on the remote server
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 0 for OK, non-zero for error (meaning that an exception has been raised)
   */
   DLLEXPORT int rmdir(const char *remotepath, ExceptionSink *xsink);
 
   //! returns the port number connection parameter
   DLLEXPORT int getPort() const;

   //! returns the user name connection parameter
   DLLEXPORT const char *getUserName() const;

   //! returns the password connection parameter
   DLLEXPORT const char *getPassword() const;

   //! returns the hostname connection parameter
   DLLEXPORT const char *getHostName() const;

   //! sets the connection parameters from a URL
   /** a Qore-language exception will be raised if the URL is invalid (protocol is not "ftp" 
       or "ftps") or the hostname is missing.
       @param url the URL string to use to set connection parameters
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void setURL(const QoreString *url, ExceptionSink *xsink);

   //! returns a URL string representing the current connection parameters, caller owns the reference count returned
   /** this function always returns a value
       @return a URL string representing the current connection parameters, caller owns the reference count returned
   */
   DLLEXPORT QoreStringNode *getURL() const;

   //! sets the port connection parameter
   DLLEXPORT void setPort(int p);

   //! sets the user name connection parameter
   DLLEXPORT void setUserName(const char *u); 

   //! sets the password connection parameter
   DLLEXPORT void setPassword(const char *p);

   //! sets the host name connection parameter
   DLLEXPORT void setHostName(const char *h); 

   //! sets the secure connection parameter flag (to use the FTPS protocol)
   /** @return 0 for OK, -1 for error, not set (if a connection is currently established, then this flag cannot be changed)
    */
   DLLEXPORT int setSecure();

   //! unsets the secure connection parameter flag (to use the FTP protocol)
   /** @return 0 for OK, -1 for error, not set (if a connection is currently established, then this flag cannot be changed)
    */
   DLLEXPORT int setInsecure();

   //! sets the secure data connection parameter flag
   /** after calling QoreFtpClient::setSecure(), this function can be set to indicate that
       data connection should not be encrypted (while logins will be encrypted)
       @return 0 for OK, -1 for error, not set (if a connection is currently established, then this flag cannot be changed)
   */
   DLLEXPORT int setInsecureData();

   //! returns the secure connection parameter flag
   /** true indicates that current control connection (if any) is encrypted, or that the next control connection can only be established with a secure connection
       @return the secure connection parameter flag
   */
   DLLEXPORT bool isSecure() const;

   //! returns the secure data connection parameter flag
   /** true indicates that the current data connection (if any) is encrypted, or that the next data connection can only be established with a secure connection
       @return the secure dataconnection parameter flag
   */
   DLLEXPORT bool isDataSecure() const;

   //! returns the name of the SSL Cipher for the currently-connected control connection, or 0 if there is none
   /**
      @return the name of the SSL Cipher for the currently-connected control connection, or 0 if there is none
   */
   DLLEXPORT const char *getSSLCipherName() const;

   //! returns the version string of the SSL Cipher for the currently-connected control connection, or 0 if there is none
   /**
      @return the version string of the SSL Cipher for the currently-connected control connection, or 0 if there is none
   */
   DLLEXPORT const char *getSSLCipherVersion() const;

   //! returns the peer certificate verification code
   DLLEXPORT long verifyPeerCertificate() const;

   //! sets the connection mode for the next connection to "auto"
   DLLEXPORT void setModeAuto();

   //! sets the connection mode for the next connection to "EPSV" (extended passive mode)
   DLLEXPORT void setModeEPSV();

   //! sets the connection mode for the next connection to "PASV" (passive mode)
   DLLEXPORT void setModePASV();

   //! sets the connection mode for the next connection to "PORT"
   DLLEXPORT void setModePORT();

   DLLEXPORT void clearWarningQueue(ExceptionSink* xsink);
   DLLEXPORT void setWarningQueue(ExceptionSink* xsink, int64 warning_ms, int64 warning_bs, class Queue* wq, AbstractQoreNode* arg, int64 min_ms = 1000);
   DLLEXPORT QoreHashNode* getUsageInfo() const;
   DLLEXPORT void clearStats();

   //! sets the same event queue for data and control sockets
   DLLLOCAL void setEventQueue(Queue *cbq, ExceptionSink *xsink);

   //! sets the event queue for the data socket
   DLLLOCAL void setDataEventQueue(Queue *cbq, ExceptionSink *xsink);

   //! sets the event queue for the control socket
   DLLLOCAL void setControlEventQueue(Queue *cbq, ExceptionSink *xsink);

   DLLLOCAL void cleanup(ExceptionSink *xsink);
};

#endif // _QORE_OBJECTS_FTPCLIENT_H
