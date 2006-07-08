/*
  FtpClient.h
  
  thread-safe Qore FtpClient object
  
  Qore Programming Language
  
  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

class FtpResp
{
   private:
      class QoreString *str;

   public:
      inline FtpResp() : str(NULL) {}
      inline FtpResp(class QoreString *s)
      {
	 str = s;
      }
      inline ~FtpResp()
      {
	 if (str)
	    delete str;
      }
      inline class QoreString *assign(class QoreString *s)
      {
	 if (str)
	    delete str;
	 str = s;
	 return s;
      }
      inline char *getBuffer()
      {
	 return str->getBuffer();
      }
      inline class QoreString *getStr()
      {
	 return str;
      }
      
      inline int getCode()
      {
	 if (!str || str->strlen() < 3)
	    return -1;

	 char buf[4];
	 buf[0] = str->getBuffer()[0];
	 buf[1] = str->getBuffer()[1];
	 buf[2] = str->getBuffer()[2];
	 buf[3] = '\0';
	 return atoi(buf);
      }

      inline void stripEOL()
      {
	 if (!str || !str->strlen())
	    return;
	 if (str->getBuffer()[str->strlen() - 1] == '\n')
	    str->terminate(str->strlen() - 1);
	 if (str->getBuffer()[str->strlen() - 1] == '\r')
	    str->terminate(str->strlen() - 1);
      }
};

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

      class QoreString *getResponse(class ExceptionSink *xsink);
      inline class QoreString *sendMsg(char *cmd, char *arg, class ExceptionSink *xsink);
      inline void stripEOL(class QoreString *str);
      //int connectDataLongPassive(class ExceptionSink *xsink);
      int connectDataExtendedPassive(class ExceptionSink *xsink);
      int connectDataPassive(class ExceptionSink *xsink);
      int connectDataPort(class ExceptionSink *xsink);
      inline int connectData(class ExceptionSink *xsink);
      inline int acceptDataConnection(class ExceptionSink *xsink);
      inline int setBinaryMode(bool t, class ExceptionSink *xsink);
      inline int disconnectInternal();
      void setURLInternal(class QoreString *url, class ExceptionSink *xsink);
      inline int connectIntern(class FtpResp *resp, class ExceptionSink *xsink);
      inline int doAuth(class FtpResp *resp, class ExceptionSink *xsink);
      inline int doProt(class FtpResp *resp, class ExceptionSink *xsink);

   public:
      FtpClient(class QoreString *url, class ExceptionSink *xsink);
      inline ~FtpClient();
      int connect(class ExceptionSink *xsink);
      inline int disconnect();
      int cwd(char *dir, class ExceptionSink *xsink);
      class QoreString *pwd(class ExceptionSink *xsink);
      int put(char *localpath, char *remotename, class ExceptionSink *xsink);
      int get(char *remotepath, char *localname, class ExceptionSink *xsink);
      class QoreString *list(char *path, bool long_list, class ExceptionSink *xsink);
      int del(char *file, class ExceptionSink *xsink);
      //int cdup(class ExceptionSink *xsink);
      //int rename(char *old, char *name, class ExceptionSink *xsink);
      //int mkdir(char *dir, class ExceptionSink *xsink);
      //int rmdir(char *dir, class ExceptionSink *xsink);
      
      inline int getPort() { return port; }
      inline char *getUserName() { return user; }
      inline char *getPassword() { return pass; }
      inline char *getHostName() { return host; }
      inline void setURL(class QoreString *url, class ExceptionSink *xsink)
      {
	 lock();
	 setURLInternal(url, xsink);
	 unlock();
      }
      inline class QoreString *getURL()
      {
	 class QoreString *url = new QoreString("ftp://");
	 if (user)
	 {
	    url->concat(user);
	    if (pass)
	       url->sprintf(":%s", pass);
	    url->concat('@');
	 }
	 if (host)
	    url->concat(host);
	 if (port)
	    url->sprintf(":%d", port);
	 return url;
      }
      inline void setPort(int p) { port = p; }
      inline void setUserName(char *u) 
      { 
	 lock();
	 if (user) 
	    free(user); 
	 user = u ? strdup(u) : NULL;
	 unlock();
      }
      inline void setPassword(char *p) 
      { 
	 lock();
	 if (pass)
	    free(pass); 
	 pass = p ? strdup(p) : NULL;
	 unlock();
      }
      inline void setHostName(char *h) 
      { 
	 lock();
	 if (host) 
	    free(host); 
	 host = h ? strdup(h) : NULL;
	 unlock();
      }
      inline int setSecure()
      {
	 lock();
	 if (control_connected)
	 {
	    unlock();
	    return -1;
	 }
	 secure = secure_data = true;
	 unlock();
	 return 0;
      }
      inline int setInsecure()
      {
	 lock();
	 if (control_connected)
	 {
	    unlock();
	    return -1;
	 }
	 secure = secure_data = false;
	 unlock();
	 return 0;
      }
      inline int setInsecureData()
      {
	 lock();
	 if (control_connected)
	 {
	    unlock();
	    return -1;
	 }
	 secure_data = false;
	 unlock();
	 return 0;
      }
      // returns true if the control connection can only be established with a secure connection
      inline bool isSecure()
      {
	 return secure;
      }
      // returns true if data connections can only be established with a secure connection
      inline bool isDataSecure()
      {
	 return secure_data;
      }

      inline const char *getSSLCipherName()
      {
	 return control.getSSLCipherName();
      }

      inline const char *getSSLCipherVersion()
      {
	 return control.getSSLCipherVersion();
      }

      inline long verifyPeerCertificate()
      {
	 return control.verifyPeerCertificate();
      }	 

      inline void setModeAuto()
      {
	 lock();
	 mode = FTP_MODE_UNKNOWN;
	 unlock();
      }

      inline void setModeEPSV()
      {
	 lock();
	 mode = FTP_MODE_EPSV;
	 unlock();
      }

      inline void setModePASV()
      {
	 lock();
	 mode = FTP_MODE_PASV;
	 unlock();
      }

      inline void setModePORT()
      {
	 lock();
	 mode = FTP_MODE_PORT;
	 unlock();
      }
};

inline FtpClient::~FtpClient()
{
   // clear control buffer
   buffer.clear();
   disconnectInternal();
   if (host)
      free(host);
   if (user)
      free(user);
   if (pass)
      free(pass);
}

// private unlocked
static inline int getFTPCode(QoreString *str)
{
   QoreString *b = str->substr(0, 3);
   if (!b) return -1;
   int rc = atoi(b->getBuffer());
   delete b;
   return rc;
}

// private unlocked
inline class QoreString *FtpClient::sendMsg(char *cmd, char *arg, class ExceptionSink *xsink)
{
   QoreString c(cmd);
   if (arg)
   {
      c.concat(' ');
      c.concat(arg);
   }
   c.concat("\r\n");
   printd(FTPDEBUG, "FtpClient::sendMsg()> %s", c.getBuffer());
   if (control.send(c.getBuffer(), c.strlen()) < 0)
   {
      xsink->raiseException("FTP-SEND-ERROR", strerror(errno));
      return NULL;
   }

   QoreString *resp = getResponse(xsink);
   return resp;
}

// private unlocked
inline void FtpClient::stripEOL(class QoreString *str)
{
   if (!str || !str->strlen())
      return;
   if (str->getBuffer()[str->strlen() - 1] == '\n')
      str->terminate(str->strlen() - 1);
   if (str->getBuffer()[str->strlen() - 1] == '\r')
      str->terminate(str->strlen() - 1);
}

// private unlocked
inline int FtpClient::setBinaryMode(bool t, class ExceptionSink *xsink)
{
   // set transfer mode
   QoreString *resp = sendMsg("TYPE", (char *)(t ? "I" : "A"), xsink);
   if (xsink->isEvent())
      return -1;
   int code = getFTPCode(resp);
   if ((code / 100) != 2)
   {
      xsink->raiseException("FTP-ERROR", "can't set mode to '%c', FTP server responded: %s",
		     (t ? 'I' : 'A'), resp->getBuffer());
      delete resp;
      return -1;
   }
   delete resp;
   return 0;
}

// private unlocked
inline int FtpClient::acceptDataConnection(class ExceptionSink *xsink)
{
   if (data.acceptAndReplace(NULL))
   {
      data.close();
      xsink->raiseException("FTP-CONNECT-ERROR", "error accepting data connection: %s", 
		     strerror(errno));
      return -1;
   }
#ifdef DEBUG
   if (secure_data)
      printd(FTPDEBUG, "FtpClient::connectDataPort() negotiating client SSL connection\n");
#endif

   if (secure_data && data.upgradeClientToSSL(NULL, NULL, xsink))
      return -1;      

   printd(FTPDEBUG, "FtpClient::acceptDataConnection() accepted PORT data connection\n");
   return 0;
}

// private unlocked
inline int FtpClient::connectData(class ExceptionSink *xsink)
{
   switch (mode)
   {
      case FTP_MODE_UNKNOWN:
	 if (!connectDataExtendedPassive(xsink))
	    return 0;
	 if (xsink->isEvent())
	    return -1;
	 if (!connectDataPassive(xsink))
	    return 0;
	 if (xsink->isEvent())
	    return -1;
	 if (!connectDataPort(xsink))
	    return 0;

	 if (!xsink->isEvent())
	    xsink->raiseException("FTP-CONNECT-ERROR", "Could not negotiate data channel connection with FTP server");
	 return -1;
      case FTP_MODE_EPSV:
	 return connectDataExtendedPassive(xsink);
      case FTP_MODE_PASV:
	 return connectDataPassive(xsink);
      case FTP_MODE_PORT:
	 return connectDataPort(xsink);
   }
   return -1;
}

// private unlocked
inline int FtpClient::disconnectInternal()
{
   control.close();
   control_connected = false;
   mode = FTP_MODE_UNKNOWN;
   data.close();
   return 0;
}

// public locked
inline int FtpClient::disconnect()
{
   lock();
   int rc = disconnectInternal();
   unlock();
   return rc;
}

#endif // _QORE_OBJECTS_FTPCLIENT_H
