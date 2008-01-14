/*
  QoreFtpClient.cc
  
  thread-safe QoreFtpClient object
  
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
#include <qore/QoreFtpClient.h>
#include <qore/QoreURL.h>

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

class FtpResp
{
   private:
      class QoreStringNode *str;
   
   public:
      DLLLOCAL inline FtpResp() : str(0) {}
      DLLLOCAL inline FtpResp(class QoreStringNode *s)
      {
	 str = s;
      }
      DLLLOCAL inline ~FtpResp()
      {
	 if (str)
	    str->deref();
      }
      DLLLOCAL inline class QoreStringNode *assign(class QoreStringNode *s)
      {
	 if (str)
	    str->deref();
	 str = s;
	 return s;
      }
      DLLLOCAL inline const char *getBuffer()
      {
	 return str->getBuffer();
      }
      DLLLOCAL inline class QoreStringNode *getStr()
      {
	 return str;
      }
      DLLLOCAL inline int getCode()
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
      DLLLOCAL inline void stripEOL()
      {
	 if (!str || !str->strlen())
	    return;
	 if (str->getBuffer()[str->strlen() - 1] == '\n')
	    str->terminate(str->strlen() - 1);
	 if (str->getBuffer()[str->strlen() - 1] == '\r')
	    str->terminate(str->strlen() - 1);
      }
};

struct qore_ftp_private {
      // for when we read too much data on control connection
      class QoreString buffer;
      class QoreSocket control, data;
      char *host, *user, *pass;
      bool control_connected, loggedin;
      int mode, port;
      bool secure, secure_data;

      DLLLOCAL qore_ftp_private(const QoreString *url, class ExceptionSink *xsink)
      {
	 control_connected = loggedin = false;
	 mode = FTP_MODE_UNKNOWN;
	 port = DEFAULT_FTP_CONTROL_PORT;
	 user = pass = host = NULL;
	 secure = secure_data = false;

	 if (url)
	    setURLInternal(url, xsink);
      }

      DLLLOCAL ~qore_ftp_private()
      {
	 if (host)
	    free(host);
	 if (user)
	    free(user);
	 if (pass)
	    free(pass);
      }

      // private unlocked
      DLLLOCAL void setURLInternal(const QoreString *url_str, class ExceptionSink *xsink)
      {
	 QoreURL url(url_str);
	 if (!url.isValid())
	 {
	    xsink->raiseException("FTP-URL-ERROR", "no hostname given in URL '%s'", url_str->getBuffer());
	    return;
	 }
	 
	 // verify protocol
	 if (url.getProtocol())
	 {
	    if (!url.getProtocol()->compare("ftps"))
	       secure = secure_data = true;
	    else if (url.getProtocol()->compare("ftp"))
	    {
	       xsink->raiseException("UNSUPPORTED-PROTOCOL", "'%s' not supported (expected 'ftp' or 'ftps')", url.getProtocol()->getBuffer());
	       return;
	    }
	 }
	 
	 // set username
	 user = url.take_username();   
	 // set password
	 pass = url.take_password();
	 // set host
	 host = url.take_host();
	 port = url.getPort() ? url.getPort() : DEFAULT_FTP_CONTROL_PORT;
      }
};

QoreFtpClient::QoreFtpClient(const QoreString *url, class ExceptionSink *xsink) : priv(new qore_ftp_private(url, xsink))
{
}

QoreFtpClient::~QoreFtpClient()
{
   disconnectInternal();
   delete priv;
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
class QoreStringNode *QoreFtpClient::sendMsg(const char *cmd, const char *arg, class ExceptionSink *xsink)
{
   QoreString c(cmd);
   if (arg)
   {
      c.concat(' ');
      c.concat(arg);
   }
   c.concat("\r\n");
   printd(FTPDEBUG, "QoreFtpClient::sendMsg()> %s", c.getBuffer());
   if (priv->control.send(c.getBuffer(), c.strlen()) < 0)
   {
      xsink->raiseException("FTP-SEND-ERROR", strerror(errno));
      return NULL;
   }
   
   return getResponse(xsink);
}

// private unlocked
void QoreFtpClient::stripEOL(class QoreString *str)
{
   if (!str || !str->strlen())
      return;
   if (str->getBuffer()[str->strlen() - 1] == '\n')
      str->terminate(str->strlen() - 1);
   if (str->getBuffer()[str->strlen() - 1] == '\r')
      str->terminate(str->strlen() - 1);
}

// private unlocked
int QoreFtpClient::setBinaryMode(bool t, class ExceptionSink *xsink)
{
   // set transfer mode
   TempQoreStringNode resp(sendMsg("TYPE", (char *)(t ? "I" : "A"), xsink));
   if (xsink->isEvent())
      return -1;
   int code = getFTPCode(*resp);
   if ((code / 100) != 2)
   {
      xsink->raiseException("FTP-ERROR", "can't set mode to '%c', FTP server responded: %s", (t ? 'I' : 'A'), resp->getBuffer());
      return -1;
   }
   return 0;
}

// private unlocked
int QoreFtpClient::acceptDataConnection(class ExceptionSink *xsink)
{
   if (priv->data.acceptAndReplace(NULL))
   {
      priv->data.close();
      xsink->raiseException("FTP-CONNECT-ERROR", "error accepting data connection: %s", 
			    strerror(errno));
      return -1;
   }
#ifdef DEBUG
   if (priv->secure_data)
      printd(FTPDEBUG, "QoreFtpClient::connectDataPort() negotiating client SSL connection\n");
#endif
   
   if (priv->secure_data && priv->data.upgradeClientToSSL(NULL, NULL, xsink))
      return -1;      
   
   printd(FTPDEBUG, "QoreFtpClient::acceptDataConnection() accepted PORT data connection\n");
   return 0;
}

// private unlocked
int QoreFtpClient::connectData(class ExceptionSink *xsink)
{
   switch (priv->mode)
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
int QoreFtpClient::disconnectInternal()
{
   priv->control.close();
   priv->control_connected = false;
   priv->mode = FTP_MODE_UNKNOWN;
   priv->data.close();
   return 0;
}

// public locked
int QoreFtpClient::disconnect()
{
   lock();
   int rc = disconnectInternal();
   unlock();
   return rc;
}

// private unlocked
class QoreStringNode *QoreFtpClient::getResponse(class ExceptionSink *xsink)
{
   TempQoreStringNode resp(0);
   int rc;
   // if there is data in the buffer, then take it, otherwise read
   if (!priv->buffer.strlen())
      resp = priv->control.recv(-1, &rc);
   else
   {
      resp = new QoreStringNode(priv->buffer.giveBuffer());
      priv->buffer.clear();
   }
   // see if we got the whole response
   if (resp && resp->getBuffer())
   {
      const char *start = resp->getBuffer();
      const char *p = start;
      while (1)
      {
	 if ((*p) == '\n')
	 {
	    if (p > (start + 3))
	    {
	       // if we got the whole response
	       if (isdigit(*start) && isdigit(start[1]) && isdigit(start[2]) && start[3] == ' ')
	       {
		  // if we read more data, then store it in the buffer
		  if (p[1] != '\0')
		  {
		     priv->buffer.set(&p[1]);
		     resp->terminate(p - resp->getBuffer() + 1); 
		  }
		  break;
	       }
	    }
	    start = p + 1;
	 }
	 // if we have not got the whole message
	 else if (*p == '\0')
	 {
	    TempQoreStringNode r(priv->control.recv(-1, &rc));
	    if (!r)
	    {
	       xsink->raiseException("FTP-RECEIVE-ERROR", "short message received on control port");
	       return NULL;
	    }
	    // in case the buffer gets reallocated
	    int pos = p - resp->getBuffer();
	    resp->concat(r);
	    p = resp->getBuffer() + pos;
	 }
	 p++;
      }
   }
   printd(FTPDEBUG, "QoreFtpClient::getResponse() %s", resp ? resp->getBuffer() : "NULL");
   return resp.release();
}

/*
// RFC 1639 Long Passive Mode
int QoreFtpClient::connectDataLongPassive(class ExceptionSink *xsink)
{
   // try extended passive mode
   class QoreString *resp = sendMsg("LPSV", NULL, xsink);
   if ((getFTPCode(resp) / 100) != 2)
   {
      delete resp;
      return -1;
   }

   // ex: 228 Entering Long Passive Mode (6,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,247,239)
   // get port for data connection
}
*/

// private unlocked
// RFC 2428 Extended Passive Mode
int QoreFtpClient::connectDataExtendedPassive(class ExceptionSink *xsink)
{
   // try extended passive mode
   class FtpResp resp(sendMsg("EPSV", NULL, xsink));
   if ((resp.getCode() / 100) != 2)
      return -1;

   // ex: 229 Entering Extended Passive Mode (|||63519|)
   // get port for data connection
   const char *s = strstr(resp.getBuffer(), "|||");
   if (!s)
   {
      resp.stripEOL();
      xsink->raiseException("FTP-RESPONSE-ERROR", "cannot find port in EPSV response: %s", resp.getBuffer());
      return -1;
   }
   s += 3;
   char *end = (char *)strchr(s, '|');
   if (!end)
   {
      resp.stripEOL();
      xsink->raiseException("FTP-RESPONSE-ERROR", "cannot find port in EPSV response: %s", resp.getBuffer());
      return -1;
   }
   *end = '\0';

   int data_port = atoi(s);
   if (priv->data.connectINET(priv->host, data_port))
   {
      xsink->raiseException("FTP-CONNECT-ERROR", "could not connect to passive data port (%s:%d): %s", priv->host, data_port,
			    strerror(errno));
      return -1;
   }
   printd(FTPDEBUG, "EPSV connected to %s:%d\n", priv->host, data_port);

   priv->mode = FTP_MODE_EPSV;
   return 0;
}

// private unlocked
int QoreFtpClient::connectDataPassive(class ExceptionSink *xsink)
{
   // try passive mode
   class FtpResp resp;
   resp.assign(sendMsg("PASV", NULL, xsink));
   if ((resp.getCode() / 100) != 2)
      return -1;

   // reply ex: 227 Entering passive mode (127,0,0,1,28,46)  
   // get port for data connection
   const char *s = strstr(resp.getBuffer(), "(");
   if (!s)
   {
      resp.stripEOL();
      xsink->raiseException("FTP-RESPONSE-ERROR", "cannot parse PASV response: %s", resp.getBuffer());
      return -1;
   }
   int num[5];
   s++;
   const char *comma;
   for (int i = 0; i < 5; i++)
   {
      comma = strchr(s, ',');
      if (!comma)
      {
	 resp.stripEOL();
	 xsink->raiseException("FTP-RESPONSE-ERROR", "cannot parse PASV response: %s", resp.getBuffer());
	 return -1;
      }
      num[i] = atoi(s);
      s = comma + 1;
   }
   int dataport = (num[4] << 8) + atoi(s);
   class QoreString ip;
   ip.sprintf("%d.%d.%d.%d", num[0], num[1], num[2], num[3]);
   printd(FTPDEBUG,"QoreFtpClient::connectPassive() address: %s:%d\n", ip.getBuffer(), dataport);

   if (priv->data.connectINET(ip.getBuffer(), dataport))
   {
      xsink->raiseException("FTP-CONNECT-ERROR", "could not connect to passive data port (%s:%d): %s", 
			    ip.getBuffer(), dataport, strerror(errno));
      return -1;
   }

   if (priv->secure_data && priv->data.upgradeClientToSSL(NULL, NULL, xsink))
      return -1;      

   priv->mode = FTP_MODE_PASV;
   return 0;
}

// private unlocked
int QoreFtpClient::connectDataPort(class ExceptionSink *xsink)
{
   // get address for interface of control connection
   struct sockaddr_in add;
#ifdef HPUX_ACC_SOCKLEN_HACK
   int socksize = sizeof(struct sockaddr_in);
#else
   socklen_t socksize = sizeof(struct sockaddr_in);
#endif
   
   if (getsockname(priv->control.getSocket(), (struct sockaddr *)&add, &socksize) < 0)
   {
      xsink->raiseException("FTP-CONNECT-ERROR", "cannot determine local interface address for data port connection: %s",
		     strerror(errno));
      return -1;
   }
   // bind to any port on local interface
   add.sin_port = 0;
   if (priv->data.bind((struct sockaddr *)&add, sizeof (struct sockaddr_in)))
   {
      xsink->raiseException("FTP-CONNECT-ERROR", "could not bind to any port on local interface: %s", 
		     strerror(errno));
      return -1;
   }
   // get port number
   int dataport = priv->data.getPort();

   // get ip address
   char ifname[80];
   if (!inet_ntop(AF_INET, &((struct sockaddr_in *)&add)->sin_addr, ifname, sizeof(ifname)))
   {
      priv->data.close();
      xsink->raiseException("FTP-CONNECT-ERROR", "cannot determine local interface address for data port connection: %s",
		     strerror(errno));
      return -1;
   }
   printd(FTPDEBUG, "QoreFtpClient::connectDataPort() requesting connection to %s:%d\n", ifname, dataport);
   // change dots to commas for PORT message
   for (int i = 0; ifname[i]; i++)
      if (ifname[i] == '.')
	 ifname[i] = ',';

   QoreString pconn;
   pconn.sprintf("%s,%d,%d", ifname, dataport >> 8, dataport & 255);
   FtpResp resp(sendMsg("PORT", pconn.getBuffer(), xsink));
   if (xsink->isEvent())
   {
      priv->data.close();
      return -1;
   }

   // ex: 200 PORT command successful.
   if ((resp.getCode() / 100) != 2)
   {
      priv->data.close();
      return -1;
   }
   
   if (priv->data.listen())
   {
      priv->data.close();
      xsink->raiseException("FTP-CONNECT-ERROR", "error listening on data connection: %s", 
			    strerror(errno));
      return -1;
   }
   printd(FTPDEBUG, "QoreFtpClient::connectDataPort() listening on port %d\n", dataport);

   priv->mode = FTP_MODE_PORT;
   return 0;
}

// private unlocked
int QoreFtpClient::connectIntern(class FtpResp *resp, class ExceptionSink *xsink)
{
   // connect to FTP port on remote machine
   if (priv->control.connectINET(priv->host, priv->port))
   {
      if (priv->port != DEFAULT_FTP_CONTROL_PORT)
	 xsink->raiseException("FTP-CONNECT-ERROR", "could not connect to ftp%s://%s:%d", priv->secure ? "s" : "", priv->host, priv->port);
      else
	 xsink->raiseException("FTP-CONNECT-ERROR", "could not connect to ftp%s://%s", priv->secure ? "s" : "", priv->host);

      return -1;
   }

   priv->control_connected = 1;

   int rc;
   resp->assign(priv->control.recv(-1, &rc));
   if (xsink->isEvent())
      return -1;

   printd(FTPDEBUG, "QoreFtpClient::connectIntern() %s", resp->getBuffer());

   // ex: 220 (vsFTPd 2.0.1)
   // ex: 220 localhost FTP server (tnftpd 20040810) ready.
   // etc
   if ((resp->getCode() / 100) != 2)
   {
      resp->stripEOL();
      xsink->raiseException("FTP-CONNECT-ERROR", "FTP server reported the following error: %s",
			    resp->getBuffer());
      return -1;
   }

   return 0;
}

// do PBSZ and PROT commands
int QoreFtpClient::doProt(class FtpResp *resp, class ExceptionSink *xsink)
{
   // RFC-4217: PBSZ 0 for streaming data
   resp->assign(sendMsg("PBSZ", "0", xsink));
   if (xsink->isEvent())
      return -1;
   int code = resp->getCode();
   if (code != 200)
   {
      resp->stripEOL();
      xsink->raiseException("FTPS-SECURE-DATA-ERROR", "response from FTP server to PBSZ 0 command: %s", resp->getBuffer());
      return -1;
   }

   resp->assign(sendMsg("PROT", "P", xsink));
   if (xsink->isEvent())
      return -1;
   code = resp->getCode();
   if (code != 200)
   {
      resp->stripEOL();
      xsink->raiseException("FTPS-SECURE-DATA-ERROR", "response from FTP server to PROT P command: %s", resp->getBuffer());
      return -1;
   }

   return 0;
}

// private unlocked
int QoreFtpClient::doAuth(class FtpResp *resp, class ExceptionSink *xsink)
{
   resp->assign(sendMsg("AUTH", "TLS", xsink));
   if (xsink->isEvent())
      return -1;
   int code = resp->getCode();

   if (code != 234)
   {
      // RFC-2228 ADAT exchange not supported
      if (code == 334)
	 xsink->raiseException("FTPS-AUTH-ERROR", "server requires unsupported ADAT exchange");
      else
      {
	 resp->stripEOL();
	 xsink->raiseException("FTPS-AUTH-ERROR", "response from FTP server: %s", resp->getBuffer());
      }
      return -1;
   }
   
   if (priv->control.upgradeClientToSSL(NULL, NULL, xsink))
      return -1;

   if (priv->secure_data)
      return doProt(resp, xsink);

   return 0;
}

// public locked
int QoreFtpClient::connect(class ExceptionSink *xsink)
{
   SafeLocker sl(this);

   disconnectInternal();

   if (!priv->host)
   {
      xsink->raiseException("FTP-CONNECT-ERROR", "no hostname set");
      return -1;
   }

   FtpResp resp;
   if (connectIntern(&resp, xsink))
      return -1;

   if (priv->secure && doAuth(&resp, xsink))
      return -1;

   resp.assign(sendMsg("USER", priv->user ? priv->user : (char *)DEFAULT_USERNAME, xsink));
   if (xsink->isEvent())
      return -1;

   int code = resp.getCode();

   // if user not logged in immediately, continue
   if ((code / 100) != 2)
   {
      // if there is an error, then exit
      if (code != 331)
      {
	 resp.stripEOL();
	 xsink->raiseException("FTP-LOGIN-ERROR", "response from FTP server: %s", resp.getBuffer());
      }

      // send password
      resp.assign(sendMsg("PASS", priv->pass ? priv->pass : (char *)DEFAULT_PASSWORD, xsink));
      if (xsink->isEvent())
	 return -1;

      code = resp.getCode();

      // if user not logged in for whatever reason, then exit
      if ((code / 100) != 2)
      {
	 resp.stripEOL();
	 xsink->raiseException("FTP-LOGIN-ERROR", "response from FTP server: %s", resp.getBuffer());
	 return -1;
      }
   }

   priv->loggedin = true;

   return 0;
}

// public locked
class QoreStringNode *QoreFtpClient::list(const char *path, bool long_list, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   if (!priv->loggedin)
   {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before QoreFtpClient::%s()",
		     (long_list ? "list" : "nlst"));
      return NULL;
   }

   if (setBinaryMode(false, xsink) || connectData(xsink))
      return NULL;

   FtpResp resp(sendMsg((long_list ? "LIST" : "NLST"), path, xsink));
   if (xsink->isEvent())
      return NULL;

   int code = resp.getCode();
   //printf("LIST: %s", resp->getBuffer());
   // file not found or similar
   if ((code / 100 == 5))
   {
      priv->data.close();
      return NULL;
   }

   if ((code / 100 != 1))
   {
      priv->data.close();
      resp.stripEOL();
      xsink->raiseException("FTP-LIST-ERROR", "FTP server returned an error to the %s command: %s",
			    (long_list ? "LIST" : "NLST"), resp.getBuffer());
      return NULL;
   }

   if ((priv->mode == FTP_MODE_PORT && acceptDataConnection(xsink)) || xsink->isEvent())
   {
      priv->data.close();
      return NULL;
   }
   else if (priv->secure_data && priv->data.upgradeClientToSSL(NULL, NULL, xsink))
      return NULL;

   TempQoreStringNode l(new QoreStringNode());

   // read until done
   while (true)
   {
      int rc;
      if (!resp.assign(priv->data.recv(-1, &rc)))
	 break;
      //printf("%s", resp->getBuffer());
      l->concat(resp.getStr());
   }
   priv->data.close();
   resp.assign(getResponse(xsink));
   sl.unlock();
   if (xsink->isEvent())
      return NULL;

   code = resp.getCode();

   //printf("LIST: %s", resp->getBuffer());
   if ((code / 100 != 2))
   {
      resp.stripEOL();
      xsink->raiseException("FTP-LIST-ERROR", "FTP server returned an error to the %s command: %s", 
			    (long_list ? "LIST" : "NLST"), resp.getBuffer());
      return NULL;
   }
   return l.release();
}

// public locked
int QoreFtpClient::put(const char *localpath, const char *remotename, class ExceptionSink *xsink)
{
   printd(5, "QoreFtpClient::put(%s, %s)\n", localpath, remotename ? remotename : "NULL");

   SafeLocker sl(this);
   if (!priv->loggedin)
   {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::put()");
      return -1;
   }

   int fd = open(localpath, O_RDONLY, 0);
   if (fd < 0)
   {
      xsink->raiseException("FTP-FILE-OPEN-ERROR", "%s: %s", localpath, strerror(errno));
      return -1;
   }

   // set binary mode and establish data connection
   if (setBinaryMode(true, xsink) || connectData(xsink))
   {
      close(fd);
      return -1;
   }

   // get file size
   struct stat file_info;
   if (fstat(fd, &file_info) == -1)
   {
      close(fd);
      xsink->raiseException("FTP-FILE-PUT-ERROR", "could not get file size: %s", strerror(errno));
      return -1;
   }

   // get remote file name
   char *rn;
   if (remotename)
      rn = (char *)remotename;
   else
      rn = q_basename(localpath);

   // transfer file
   FtpResp resp(sendMsg("STOR", rn, xsink));
   if (rn != remotename)
      free(rn);
   if (xsink->isEvent())
   {
      priv->data.close();
      close(fd);
      return -1;
   }
   //printf("%s", resp->getBuffer());

   if ((resp.getCode() / 100) != 1)
   {
      priv->data.close();
      resp.stripEOL();
      xsink->raiseException("FTP-PUT-ERROR", "could not put file, FTP server replied: %s", 
			    resp.getBuffer());
      close(fd);
      return -1;
   }

   if ((priv->mode == FTP_MODE_PORT && acceptDataConnection(xsink)) || xsink->isEvent())
   {
      priv->data.close();
      close(fd);
      return -1;
   }
   else if (priv->secure_data && priv->data.upgradeClientToSSL(NULL, NULL, xsink))
      return -1;      

   int rc = priv->data.send(fd, file_info.st_size ? file_info.st_size : -1);
   priv->data.close();
   close(fd);

   resp.assign(getResponse(xsink));
   sl.unlock();
   if (xsink->isEvent())
      return -1;

   //printf("PUT: %s", resp->getBuffer());
   if ((resp.getCode() / 100 != 2))
   {
      resp.stripEOL();
      xsink->raiseException("FTP-PUT-ERROR", "FTP server returned an error to the PUT command: %s", resp.getBuffer());
      return -1;
   }   

   if (rc)
   {
      xsink->raiseException("FTP-PUT-ERROR", "error sending file, may not be complete on target");
      return -1;
   }
   return 0;
}

// public locked
int QoreFtpClient::get(const char *remotepath, const char *localname, class ExceptionSink *xsink)
{
   printd(5, "QoreFtpClient::get(%s, %s)\n", remotepath, localname ? localname : "NULL");

   SafeLocker sl(this);
   if (!priv->loggedin)
   {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::get()");
      return -1;
   }

   // get local file name
   char *ln;
   if (localname)
      ln = (char *)localname;
   else
      ln = q_basename(remotepath);

   printd(FTPDEBUG, "QoreFtpClient::get(%s) %s\n", remotepath, ln);
   // open local file
   int fd = open(ln, O_WRONLY|O_CREAT, 0644);
   if (fd < 0)
   {
      xsink->raiseException("FTP-FILE-OPEN-ERROR", "%s: %s", ln, strerror(errno));
      if (ln != localname)
	 free(ln);
      return -1;
   }

   // set binary mode and establish data connection
   if (setBinaryMode(true, xsink) || connectData(xsink))
   {
      // delete temporary file
      unlink(ln);
      if (ln != localname)
	 free(ln);
      close(fd);
      return -1;
   }

   // transfer file
   FtpResp resp(sendMsg("RETR", remotepath, xsink));
   if (xsink->isEvent())
   {
      // delete temporary file
      unlink(ln);
      if (ln != localname)
	 free(ln);
      priv->data.close();
      close(fd);
      return -1;
   }
   //printf("%s", resp->getBuffer());

   if ((resp.getCode() / 100) != 1)
   {
      // delete temporary file
      unlink(ln);
      if (ln != localname)
	 free(ln);
      priv->data.close();
      close(fd);
      resp.stripEOL();
      xsink->raiseException("FTP-GET-ERROR", "could not retrieve file, FTP server replied: %s", 
			    resp.getBuffer());
      return -1;
   }

   if ((priv->mode == FTP_MODE_PORT && acceptDataConnection(xsink)) || xsink->isEvent())
   {
      // delete temporary file
      unlink(ln);
      if (ln != localname)
	 free(ln);
      priv->data.close();
      close(fd);
      return -1;
   }
   else if (priv->secure_data && priv->data.upgradeClientToSSL(NULL, NULL, xsink))
      return -1;      

   if (ln != localname)
      free(ln);

   priv->data.recv(fd, -1, -1);
   priv->data.close();
   close(fd);

   resp.assign(getResponse(xsink));
   sl.unlock();
   if (xsink->isEvent())
      return -1;

   //printf("PUT: %s", resp->getBuffer());
   if ((resp.getCode() / 100 != 2))
   {
      resp.stripEOL();
      xsink->raiseException("FTP-GET-ERROR", "FTP server returned an error to the RETR command: %s", 
			    resp.getBuffer());
      return -1;
   }
   return 0;
}

// public locked
int QoreFtpClient::cwd(const char *dir, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   if (!priv->loggedin)
   {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::cwd()");
      return -1;
   }
   TempQoreStringNode p(sendMsg("CWD", dir, xsink));

   sl.unlock();
   if (xsink->isEvent())
      return -1;

   if ((getFTPCode(*p) / 100) == 2)
      return 0;

   stripEOL(*p);
   xsink->raiseException("FTP-CWD-ERROR", "FTP server returned an error to the CWD command: %s", p->getBuffer());
   return -1;
}

// public locked
class QoreStringNode *QoreFtpClient::pwd(class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   if (!priv->loggedin)
   {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::pwd()");
      return NULL;
   }

   TempQoreStringNode p(sendMsg("PWD", NULL, xsink));
   sl.unlock();
   if ((getFTPCode(*p) / 100) == 2)
   {
      QoreStringNode *rv = p->substr(4);
      // FIXME use trim or something instead
      stripEOL(rv);
      return rv;
   }
   stripEOL(*p);
   xsink->raiseException("FTP-PWD-ERROR", "FTP server returned an error response to the PWD command: %s", p->getBuffer());
   return NULL;
}

// public locked
int QoreFtpClient::del(const char *file, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   if (!priv->loggedin)
   {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::delete()");
      return -1;
   }

   TempQoreStringNode p(sendMsg("DELE", file, xsink));
   sl.unlock();
   if (xsink->isEvent())
      return -1;

   if ((getFTPCode(*p) / 100) == 2)
      return 0;

   stripEOL(*p);
   xsink->raiseException("FTP-DELETE-ERROR", "FTP server returned an error to the DELE command: %s", p->getBuffer());
   return -1;
}

void QoreFtpClient::setURL(const QoreString *url, class ExceptionSink *xsink)
{
   lock();
   priv->setURLInternal(url, xsink);
   unlock();
}

class QoreStringNode *QoreFtpClient::getURL() const
{
   class QoreStringNode *url = new QoreStringNode("ftp://");
   if (priv->user)
   {
      url->concat(priv->user);
      if (priv->pass)
	 url->sprintf(":%s", priv->pass);
      url->concat('@');
   }
   if (priv->host)
      url->concat(priv->host);
   if (priv->port)
      url->sprintf(":%d", priv->port);
   return url;
}

void QoreFtpClient::setPort(int p)
{ 
   priv->port = p; 
}

void QoreFtpClient::setUserName(const char *u) 
{ 
   lock();
   if (priv->user) 
      free(priv->user); 
   priv->user = u ? strdup(u) : NULL;
   unlock();
}

void QoreFtpClient::setPassword(const char *p) 
{ 
   lock();
   if (priv->pass)
      free(priv->pass); 
   priv->pass = p ? strdup(p) : NULL;
   unlock();
}

void QoreFtpClient::setHostName(const char *h) 
{ 
   lock();
   if (priv->host) 
      free(priv->host); 
   priv->host = h ? strdup(h) : NULL;
   unlock();
}

int QoreFtpClient::setSecure()
{
   lock();
   if (priv->control_connected)
   {
      unlock();
      return -1;
   }
   priv->secure = priv->secure_data = true;
   unlock();
   return 0;
}

int QoreFtpClient::setInsecure()
{
   lock();
   if (priv->control_connected)
   {
      unlock();
      return -1;
   }
   priv->secure = priv->secure_data = false;
   unlock();
   return 0;
}

int QoreFtpClient::setInsecureData()
{
   lock();
   if (priv->control_connected)
   {
      unlock();
      return -1;
   }
   priv->secure_data = false;
   unlock();
   return 0;
}

// returns true if the control connection can only be established with a secure connection
bool QoreFtpClient::isSecure() const
{
   return priv->secure;
}

// returns true if data connections can only be established with a secure connection
bool QoreFtpClient::isDataSecure() const
{
   return priv->secure_data;
}

const char *QoreFtpClient::getSSLCipherName() const
{
   return priv->control.getSSLCipherName();
}

const char *QoreFtpClient::getSSLCipherVersion() const
{
   return priv->control.getSSLCipherVersion();
}

long QoreFtpClient::verifyPeerCertificate() const
{
   return priv->control.verifyPeerCertificate();
}	 

void QoreFtpClient::setModeAuto()
{
   lock();
   priv->mode = FTP_MODE_UNKNOWN;
   unlock();
}

void QoreFtpClient::setModeEPSV()
{
   lock();
   priv->mode = FTP_MODE_EPSV;
   unlock();
}

void QoreFtpClient::setModePASV()
{
   lock();
   priv->mode = FTP_MODE_PASV;
   unlock();
}

void QoreFtpClient::setModePORT()
{
   lock();
   priv->mode = FTP_MODE_PORT;
   unlock();
}

int QoreFtpClient::getPort() const 
{
   return priv->port; 
}

const char *QoreFtpClient::getUserName() const 
{ 
   return priv->user;
}

const char *QoreFtpClient::getPassword() const 
{ 
   return priv->pass; 
}

const char *QoreFtpClient::getHostName() const 
{ 
   return priv->host; 
}
