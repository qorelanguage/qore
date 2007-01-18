/*
  FtpClient.h
  
  thread-safe Qore FtpClient object
  
  Qore Programming Language
  
  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

  is "auto" mode, tries the following data modes in order:
  * EPSV mode (RFC 2428) 
  * PASV mode (RFC 959)
  * then PORT 

  references: 
  RFC-959: FTP
  RFC-2428: EPSV mode only (no IPv6 support yet)
  RFC-4217 (supercedes RFC-2228):
   * AUTH TLS: secure authentication
   * PBSZ 0 and PROT P: secure data connections
 
  (!RFC-1639: LPSV mode not implemented yet)

  tested with:
  * tnftpd 20040810 (Darwin/OS X 10.3.8) EPSV, PASV, PORT
  * vsFTPd 2.0.1 (Fedora Core 3) EPSV
  * proFTPd 1.3.0 (Darwin/OS X 10.4.7) EPSV, PORT, AUTH TLS, PBSZ 0, PROT P

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

#ifndef _QORE_FTPCLIENT_H

#define _QORE_FTPCLIENT_H

#define DEFAULT_FTP_CONTROL_PORT  21
#define DEFAULT_FTP_DATA_PORT     20

#define FTPDEBUG 5

#include <qore/QoreSocket.h>
#include <qore/QoreString.h>
#include <qore/Exception.h>
#include <qore/QoreNet.h>
#include <qore/LockedObject.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define FTP_MODE_UNKNOWN 0
#define FTP_MODE_PORT    1
#define FTP_MODE_PASV    2
#define FTP_MODE_EPSV    3
//#define FTP_MODE_LPSV    4

#define DEFAULT_USERNAME "anonymous"
#define DEFAULT_PASSWORD "qore@nohost.com"

class FtpClient : public LockedObject
{
   private:
      // for when we read too much data on control connection
      class QoreString buffer;
      class QoreSocket control, data;
      char *host, *user, *pass, *transfer_mode;
      bool control_connected, loggedin;
      int mode, port;
      bool secure, secure_data;

      DLLLOCAL class QoreString *getResponse(class ExceptionSink *xsink);
      DLLLOCAL class QoreString *sendMsg(char *cmd, char *arg, class ExceptionSink *xsink);
      DLLLOCAL void stripEOL(class QoreString *str);
      //int connectDataLongPassive(class ExceptionSink *xsink);
      DLLLOCAL int connectDataExtendedPassive(class ExceptionSink *xsink);
      DLLLOCAL int connectDataPassive(class ExceptionSink *xsink);
      DLLLOCAL int connectDataPort(class ExceptionSink *xsink);
      DLLLOCAL int connectData(class ExceptionSink *xsink);
      DLLLOCAL int acceptDataConnection(class ExceptionSink *xsink);
      DLLLOCAL int setBinaryMode(bool t, class ExceptionSink *xsink);
      DLLLOCAL int disconnectInternal();
      DLLLOCAL void setURLInternal(class QoreString *url, class ExceptionSink *xsink);
      DLLLOCAL int connectIntern(class FtpResp *resp, class ExceptionSink *xsink);
      DLLLOCAL int doAuth(class FtpResp *resp, class ExceptionSink *xsink);
      DLLLOCAL int doProt(class FtpResp *resp, class ExceptionSink *xsink);

   public:
      DLLEXPORT FtpClient(class QoreString *url, class ExceptionSink *xsink);
      DLLEXPORT ~FtpClient();
      DLLEXPORT int connect(class ExceptionSink *xsink);
      DLLEXPORT int disconnect();
      DLLEXPORT int cwd(char *dir, class ExceptionSink *xsink);
      DLLEXPORT class QoreString *pwd(class ExceptionSink *xsink);
      DLLEXPORT int put(char *localpath, char *remotename, class ExceptionSink *xsink);
      DLLEXPORT int get(char *remotepath, char *localname, class ExceptionSink *xsink);
      DLLEXPORT class QoreString *list(char *path, bool long_list, class ExceptionSink *xsink);
      DLLEXPORT int del(char *file, class ExceptionSink *xsink);
      //DLLEXPORT int cdup(class ExceptionSink *xsink);
      //DLLEXPORT int rename(char *old, char *name, class ExceptionSink *xsink);
      //DLLEXPORT int mkdir(char *dir, class ExceptionSink *xsink);
      //DLLEXPORT int rmdir(char *dir, class ExceptionSink *xsink);
      DLLEXPORT int getPort() const;
      DLLEXPORT char *getUserName() const;
      DLLEXPORT char *getPassword() const;
      DLLEXPORT char *getHostName() const;
      DLLEXPORT void setURL(class QoreString *url, class ExceptionSink *xsink);
      DLLEXPORT class QoreString *getURL() const;
      DLLEXPORT void setPort(int p);
      DLLEXPORT void setUserName(char *u); 
      DLLEXPORT void setPassword(char *p);
      DLLEXPORT void setHostName(char *h); 
      DLLEXPORT int setSecure();
      DLLEXPORT int setInsecure();
      DLLEXPORT int setInsecureData();
      // returns true if the control connection can only be established with a secure connection
      DLLEXPORT bool isSecure() const;
      // returns true if data connections can only be established with a secure connection
      DLLEXPORT bool isDataSecure() const;
      DLLEXPORT const char *getSSLCipherName() const;
      DLLEXPORT const char *getSSLCipherVersion() const;
      DLLEXPORT long verifyPeerCertificate() const;
      DLLEXPORT void setModeAuto();
      DLLEXPORT void setModeEPSV();
      DLLEXPORT void setModePASV();
      DLLEXPORT void setModePORT();
};

#endif // _QORE_OBJECTS_FTPCLIENT_H
