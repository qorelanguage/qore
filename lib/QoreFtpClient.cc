/*
  QoreFtpClient.cc
  
  thread-safe QoreFtpClient object
  
  Qore Programming Language
  
  Copyright 2003 - 2009 David Nichols

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
#include <qore/QoreSocket.h>
#include <qore/intern/QC_Queue.h>

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

#define FTPDEBUG 5

//! to set the FTP mode
enum qore_ftp_mode {
   FTP_MODE_UNKNOWN,
   FTP_MODE_PORT,
   FTP_MODE_PASV,
   FTP_MODE_EPSV
   //FTP_MODE_LPSV
};

class FtpResp {
   private:
      QoreStringNode *str;
   
   public:
      DLLLOCAL inline FtpResp() : str(0) {}

      DLLLOCAL inline FtpResp(QoreStringNode *s) {
	 str = s;
      }

      DLLLOCAL inline ~FtpResp() {
	 if (str)
	    str->deref();
      }

      DLLLOCAL inline QoreStringNode *assign(QoreStringNode *s) {
	 if (str)
	    str->deref();
	 str = s;
	 return s;
      }

      DLLLOCAL inline const char *getBuffer() {
	 return str->getBuffer();
      }

      DLLLOCAL inline QoreStringNode *getStr() {
	 return str;
      }
};

struct qore_ftp_private {
      mutable QoreThreadLock m;
      // for when we read too much data on control connection
      QoreString buffer;
      QoreSocket control, data;
      char *host, *user, *pass;
      bool control_connected, loggedin;
      int mode, port;
      bool secure, secure_data;

      DLLLOCAL qore_ftp_private(const QoreString *url, ExceptionSink *xsink) {
	 control_connected = loggedin = false;
	 mode = FTP_MODE_UNKNOWN;
	 port = DEFAULT_FTP_CONTROL_PORT;
	 user = pass = host = 0;
	 secure = secure_data = false;

	 if (url)
	    setURLInternal(url, xsink);
      }

      DLLLOCAL ~qore_ftp_private() {
	 if (host)
	    free(host);
	 if (user)
	    free(user);
	 if (pass)
	    free(pass);
      }

      // private unlocked
      DLLLOCAL void setURLInternal(const QoreString *url_str, ExceptionSink *xsink) {
	 QoreURL url(url_str);
	 if (!url.getHost()) {
	    xsink->raiseException("FTP-URL-ERROR", "no hostname given in URL '%s'", url_str->getBuffer());
	    return;
	 }
	 
	 // verify protocol
	 if (url.getProtocol()) {
	    if (!url.getProtocol()->compare("ftps"))
	       secure = secure_data = true;
	    else if (url.getProtocol()->compare("ftp")) {
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

      void setControlEventQueue(Queue *cbq, ExceptionSink *xsink) {
	 AutoLocker al(m);
	 control.setEventQueue(cbq, xsink);
      }

      void setDataEventQueue(Queue *cbq, ExceptionSink *xsink) {
	 AutoLocker al(m);
	 data.setEventQueue(cbq, xsink);
      }

      void setEventQueue(Queue *cbq, ExceptionSink *xsink) {
	 AutoLocker al(m);
	 control.setEventQueue(cbq, xsink);
	 if (cbq)
	    cbq->ref();
	 data.setEventQueue(cbq, xsink);
      }

      void cleanup(ExceptionSink *xsink) {
	 AutoLocker al(m);
	 if (data.getQueue() && (data.getQueue() == control.getQueue())) {
	    data.cleanup(xsink);
	    control.setEventQueue(0, xsink);
	    return;
	 }

	 data.cleanup(xsink);
	 control.cleanup(xsink);
      }

      void do_event_send_msg(const char *cmd, const char *arg) {
	 Queue *q = control.getQueue();
	 if (q) {
	    QoreHashNode *h = new QoreHashNode;
	    h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_FTP_SEND_MESSAGE), 0);
	    h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FTPCLIENT), 0);
	    h->setKeyValue("id", new QoreBigIntNode(control.getObjectIDForEvents()), 0);
	    h->setKeyValue("command", new QoreStringNode(cmd), 0);
	    if (arg)
	       h->setKeyValue("arg", new QoreStringNode(arg), 0);
	    q->push_and_take_ref(h);
	 }
      }

      void do_event_msg_received(int code, const char *msg) {
	 Queue *q = control.getQueue();
	 if (q) {
	    QoreHashNode *h = new QoreHashNode;
	    h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_FTP_MESSAGE_RECEIVED), 0);
	    h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FTPCLIENT), 0);
	    h->setKeyValue("id", new QoreBigIntNode(control.getObjectIDForEvents()), 0);
	    h->setKeyValue("code", new QoreBigIntNode(code), 0);
	    h->setKeyValue("message", msg[0] ? new QoreStringNode(msg) : 0, 0);
	    q->push_and_take_ref(h);
	 }
      }
};

QoreFtpClient::QoreFtpClient(const QoreString *url, ExceptionSink *xsink) : priv(new qore_ftp_private(url, xsink)) {
}

QoreFtpClient::~QoreFtpClient() {
   disconnectInternal();
   delete priv;
}

void QoreFtpClient::lock() {
   priv->m.lock();
}

void QoreFtpClient::unlock() {
   priv->m.unlock();
}

// private unlocked
static inline int getFTPCode(QoreString *str) {
   if (str->strlen() < 3)
      return -1;
   const char *b = str->getBuffer();
   return (b[0] - 48) * 100 + (b[1] - 48) * 10 + (b[0] - 48);
}

// private unlocked
QoreStringNode *QoreFtpClient::sendMsg(int &code, const char *cmd, const char *arg, ExceptionSink *xsink) {
   priv->do_event_send_msg(cmd, arg);

   QoreString c(cmd);
   if (arg) {
      c.concat(' ');
      c.concat(arg);
   }
   c.concat("\r\n");
   printd(FTPDEBUG, "QoreFtpClient::sendMsg() %s", c.getBuffer());
   if (priv->control.send(c.getBuffer(), c.strlen()) < 0) {
      xsink->raiseException("FTP-SEND-ERROR", strerror(errno));
      return 0;
   }
   
   QoreStringNode *rsp = getResponse(code, xsink);
   return rsp;
}

// private unlocked
int QoreFtpClient::setBinaryMode(bool t, ExceptionSink *xsink) {
   // set transfer mode
   int code;
   QoreStringNodeHolder resp(sendMsg(code, "TYPE", (char *)(t ? "I" : "A"), xsink));
   if (xsink->isEvent())
      return -1;

   if ((code / 100) != 2) {
      xsink->raiseException("FTP-ERROR", "can't set mode to '%c', FTP server responded: %s", (t ? 'I' : 'A'), resp->getBuffer());
      return -1;
   }
   return 0;
}

// private unlocked
int QoreFtpClient::acceptDataConnection(ExceptionSink *xsink) {
   if (priv->data.acceptAndReplace(0)) {
      priv->data.close();
      xsink->raiseException("FTP-CONNECT-ERROR", "error accepting data connection: %s", 
			    strerror(errno));
      return -1;
   }
#ifdef DEBUG
   if (priv->secure_data)
      printd(FTPDEBUG, "QoreFtpClient::connectDataPort() negotiating client SSL connection\n");
#endif
   
   if (priv->secure_data && priv->data.upgradeClientToSSL(0, 0, xsink))
      return -1;      
   
   printd(FTPDEBUG, "QoreFtpClient::acceptDataConnection() accepted PORT data connection\n");
   return 0;
}

// private unlocked
int QoreFtpClient::connectData(ExceptionSink *xsink)
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
QoreStringNode *QoreFtpClient::getResponse(int &code, ExceptionSink *xsink) {
   QoreStringNodeHolder resp(0);
   int rc;
   // if there is data in the buffer, then take it, otherwise read
   if (!priv->buffer.strlen())
      resp = priv->control.recv(-1, &rc);
   else {
      qore_size_t len = priv->buffer.strlen();
      resp = new QoreStringNode(priv->buffer.giveBuffer(), len, len + 1, priv->buffer.getEncoding());
   }
   // see if we got the whole response
   if (resp && resp->getBuffer()) {
      const char *start = resp->getBuffer();
      const char *p = start;
      while (true) {
	 if ((*p) == '\n') {
	    if (p > (start + 3)) {
	       // if we got the whole response
	       if (isdigit(*start) && isdigit(start[1]) && isdigit(start[2]) && start[3] == ' ') {
		  code = ((*start - 48) * 100) + ((start[1] - 48) * 10) + start[2] - 48;
		  // if we read more data, then store it in the buffer
		  if (p[1] != '\0') {
		     priv->buffer.set(&p[1]);
		     resp->terminate(p - resp->getBuffer() + 1); 
		  }
		  break;
	       }
	    }
	    start = p + 1;
	 }
	 // if we have not got the whole message
	 else if (*p == '\0') {
	    QoreStringNodeHolder r(priv->control.recv(-1, &rc));
	    if (!r) {
	       xsink->raiseException("FTP-RECEIVE-ERROR", "short message received on control port");
	       return 0;
	    }
	    //printd(FTPDEBUG, "QoreFtpClient::getResponse() read %s\n", r->getBuffer());
	    // in case the buffer gets reallocated
	    int pos = p - resp->getBuffer();
	    resp->concat(*r);
	    p = resp->getBuffer() + pos;
	 }
	 p++;
      }
   }
   printd(FTPDEBUG, "QoreFtpClient::getResponse() %s", resp ? resp->getBuffer() : "NULL");
   resp->chomp();
   if (resp)
      priv->do_event_msg_received(code, resp->getBuffer() + 4);
   return resp.release();
}

/*
// RFC 1639 Long Passive Mode
int QoreFtpClient::connectDataLongPassive(ExceptionSink *xsink)
{
   // try long passive mode
   int code;
   QoreString *resp = sendMsg(code, "LPSV", 0, xsink);
   if ((getFTPCode(resp) / 100) != 2) {
      delete resp;
      return -1;
   }

   // ex: 228 Entering Long Passive Mode (6,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,247,239)
   // get port for data connection
}
*/

// private unlocked
// RFC 2428 Extended Passive Mode
int QoreFtpClient::connectDataExtendedPassive(ExceptionSink *xsink)
{
   // try extended passive mode
   int code;
   FtpResp resp(sendMsg(code, "EPSV", 0, xsink));
   if ((code / 100) != 2)
      return -1;

   // ex: 229 Entering Extended Passive Mode (|||63519|)
   // get port for data connection
   const char *s = strstr(resp.getBuffer(), "|||");
   if (!s) {
      xsink->raiseException("FTP-RESPONSE-ERROR", "cannot find port in EPSV response: %s", resp.getBuffer());
      return -1;
   }
   s += 3;
   char *end = (char *)strchr(s, '|');
   if (!end) {
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
int QoreFtpClient::connectDataPassive(ExceptionSink *xsink) {
   // try passive mode
   int code;
   FtpResp resp(sendMsg(code, "PASV", 0, xsink));
   if ((code / 100) != 2)
      return -1;

   // reply ex: 227 Entering passive mode (127,0,0,1,28,46)  
   // get port for data connection
   const char *s = strstr(resp.getBuffer(), "(");
   if (!s) {
      xsink->raiseException("FTP-RESPONSE-ERROR", "cannot parse PASV response: %s", resp.getBuffer());
      return -1;
   }
   int num[5];
   s++;
   const char *comma;
   for (int i = 0; i < 5; i++) {
      comma = strchr(s, ',');
      if (!comma) {
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

   if (priv->secure_data && priv->data.upgradeClientToSSL(0, 0, xsink))
      return -1;      

   priv->mode = FTP_MODE_PASV;
   return 0;
}

// private unlocked
int QoreFtpClient::connectDataPort(ExceptionSink *xsink) {
   // get address for interface of control connection
   struct sockaddr_in add;
   socklen_t socksize = sizeof(struct sockaddr_in);
   
   if (getsockname(priv->control.getSocket(), (struct sockaddr *)&add, &socksize) < 0) {
      xsink->raiseException("FTP-CONNECT-ERROR", "cannot determine local interface address for data port connection: %s",
		     strerror(errno));
      return -1;
   }
   // bind to any port on local interface
   add.sin_port = 0;
   if (priv->data.bind((struct sockaddr *)&add, sizeof (struct sockaddr_in))) {
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
   int code;
   FtpResp resp(sendMsg(code, "PORT", pconn.getBuffer(), xsink));
   if (xsink->isEvent()) {
      priv->data.close();
      return -1;
   }

   // ex: 200 PORT command successful.
   if ((code / 100) != 2) {
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
int QoreFtpClient::connectIntern(FtpResp *resp, ExceptionSink *xsink) {
   // connect to FTP port on remote machine
   if (priv->control.connectINET(priv->host, priv->port)) {
      if (priv->port != DEFAULT_FTP_CONTROL_PORT)
	 xsink->raiseException("FTP-CONNECT-ERROR", "could not connect to ftp%s://%s:%d", priv->secure ? "s" : "", priv->host, priv->port);
      else
	 xsink->raiseException("FTP-CONNECT-ERROR", "could not connect to ftp%s://%s", priv->secure ? "s" : "", priv->host);

      return -1;
   }

   priv->control_connected = 1;

   int code;
   resp->assign(getResponse(code, xsink));

   /*
   int rc;
   resp->assign(priv->control.recv(-1, &rc));
   */

   if (xsink->isEvent())
      return -1;

   printd(FTPDEBUG, "QoreFtpClient::connectIntern() %s", resp->getBuffer());

   // ex: 220 (vsFTPd 2.0.1)
   // ex: 220 localhost FTP server (tnftpd 20040810) ready.
   // etc
   if ((code / 100) != 2) {
      xsink->raiseException("FTP-CONNECT-ERROR", "FTP server reported the following error: %s",
			    resp->getBuffer());
      return -1;
   }

   return 0;
}

// do PBSZ and PROT commands
int QoreFtpClient::doProt(class FtpResp *resp, ExceptionSink *xsink)
{
   int code;
   // RFC-4217: PBSZ 0 for streaming data
   resp->assign(sendMsg(code, "PBSZ", "0", xsink));
   if (xsink->isEvent())
      return -1;
   if (code != 200) {
      xsink->raiseException("FTPS-SECURE-DATA-ERROR", "response from FTP server to PBSZ 0 command: %s", resp->getBuffer());
      return -1;
   }

   resp->assign(sendMsg(code, "PROT", "P", xsink));
   if (xsink->isEvent())
      return -1;
   if (code != 200) {
      xsink->raiseException("FTPS-SECURE-DATA-ERROR", "response from FTP server to PROT P command: %s", resp->getBuffer());
      return -1;
   }

   return 0;
}

// private unlocked
int QoreFtpClient::doAuth(class FtpResp *resp, ExceptionSink *xsink)
{
   int code;
   resp->assign(sendMsg(code, "AUTH", "TLS", xsink));
   if (xsink->isEvent())
      return -1;

   if (code != 234) {
      // RFC-2228 ADAT exchange not supported
      if (code == 334)
	 xsink->raiseException("FTPS-AUTH-ERROR", "server requires unsupported ADAT exchange");
      else {
	 xsink->raiseException("FTPS-AUTH-ERROR", "response from FTP server: %s", resp->getBuffer());
      }
      return -1;
   }
   
   if (priv->control.upgradeClientToSSL(0, 0, xsink))
      return -1;

   if (priv->secure_data)
      return doProt(resp, xsink);

   return 0;
}

// public locked
int QoreFtpClient::connect(ExceptionSink *xsink)
{
   SafeLocker sl(priv->m);

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

   int code;
   resp.assign(sendMsg(code, "USER", priv->user ? priv->user : (char *)DEFAULT_USERNAME, xsink));
   if (xsink->isEvent())
      return -1;

   // if user not logged in immediately, continue
   if ((code / 100) != 2) {
      // if there is an error, then exit
      if (code != 331) {
	 xsink->raiseException("FTP-LOGIN-ERROR", "response from FTP server: %s", resp.getBuffer());
      }

      // send password
      resp.assign(sendMsg(code, "PASS", priv->pass ? priv->pass : (char *)DEFAULT_PASSWORD, xsink));
      if (xsink->isEvent())
	 return -1;

      // if user not logged in for whatever reason, then exit
      if ((code / 100) != 2) {
	 xsink->raiseException("FTP-LOGIN-ERROR", "response from FTP server: %s", resp.getBuffer());
	 return -1;
      }
   }

   priv->loggedin = true;

   return 0;
}

// public locked
QoreStringNode *QoreFtpClient::list(const char *path, bool long_list, ExceptionSink *xsink)
{
   SafeLocker sl(priv->m);
   if (!priv->loggedin) {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before QoreFtpClient::%s()",
		     (long_list ? "list" : "nlst"));
      return 0;
   }

   if (setBinaryMode(false, xsink) || connectData(xsink))
      return 0;

   int code;
   FtpResp resp(sendMsg(code, (long_list ? "LIST" : "NLST"), path, xsink));
   if (xsink->isEvent())
      return 0;

   //printf("LIST: %s", resp->getBuffer());
   // file not found or similar
   if ((code / 100 == 5)) {
      priv->data.close();
      return 0;
   }

   if ((code / 100 != 1)) {
      priv->data.close();
      xsink->raiseException("FTP-LIST-ERROR", "FTP server returned an error to the %s command: %s",
			    (long_list ? "LIST" : "NLST"), resp.getBuffer());
      return 0;
   }

   if ((priv->mode == FTP_MODE_PORT && acceptDataConnection(xsink)) || xsink->isEvent()) {
      priv->data.close();
      return 0;
   }
   else if (priv->secure_data && priv->data.upgradeClientToSSL(0, 0, xsink))
      return 0;

   QoreStringNodeHolder l(new QoreStringNode());

   // read until done
   while (true) {
      int rc;
      if (!resp.assign(priv->data.recv(-1, &rc)))
	 break;
      //printf("%s", resp->getBuffer());
      l->concat(resp.getStr());
   }
   priv->data.close();
   resp.assign(getResponse(code, xsink));
   sl.unlock();
   if (xsink->isEvent())
      return 0;

   //printf("LIST: %s", resp->getBuffer());
   if ((code / 100 != 2)) {
      xsink->raiseException("FTP-LIST-ERROR", "FTP server returned an error to the %s command: %s", 
			    (long_list ? "LIST" : "NLST"), resp.getBuffer());
      return 0;
   }
   return l.release();
}

// public locked
int QoreFtpClient::put(const char *localpath, const char *remotename, ExceptionSink *xsink) {
   printd(5, "QoreFtpClient::put(%s, %s)\n", localpath, remotename ? remotename : "NULL");

   SafeLocker sl(priv->m);
   if (!priv->loggedin) {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::put()");
      return -1;
   }

   int fd = open(localpath, O_RDONLY, 0);
   if (fd < 0) {
      xsink->raiseException("FTP-FILE-OPEN-ERROR", "%s: %s", localpath, strerror(errno));
      return -1;
   }

   // set binary mode and establish data connection
   if (setBinaryMode(true, xsink) || connectData(xsink)) {
      close(fd);
      return -1;
   }

   // get file size
   struct stat file_info;
   if (fstat(fd, &file_info) == -1) {
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
   int code;
   FtpResp resp(sendMsg(code, "STOR", rn, xsink));
   if (rn != remotename)
      free(rn);
   if (xsink->isEvent()) {
      priv->data.close();
      close(fd);
      return -1;
   }
   //printf("%s", resp->getBuffer());

   if ((code / 100) != 1) {
      priv->data.close();
      xsink->raiseException("FTP-PUT-ERROR", "could not put file, FTP server replied: %s", 
			    resp.getBuffer());
      close(fd);
      return -1;
   }

   if ((priv->mode == FTP_MODE_PORT && acceptDataConnection(xsink)) || xsink->isEvent()) {
      priv->data.close();
      close(fd);
      return -1;
   }
   else if (priv->secure_data && priv->data.upgradeClientToSSL(0, 0, xsink))
      return -1;      

   int rc = priv->data.send(fd, file_info.st_size ? file_info.st_size : -1);
   priv->data.close();
   close(fd);

   resp.assign(getResponse(code, xsink));
   sl.unlock();
   if (xsink->isEvent())
      return -1;

   //printf("PUT: %s", resp->getBuffer());
   if ((code / 100 != 2)) {
      xsink->raiseException("FTP-PUT-ERROR", "FTP server returned an error to the PUT command: %s", resp.getBuffer());
      return -1;
   }   

   if (rc) {
      xsink->raiseException("FTP-PUT-ERROR", "error sending file, may not be complete on target");
      return -1;
   }
   return 0;
}

// sets up a data connection and requests to retrieve a file
// returns -1=error, 0=OK
// private unlocked
int QoreFtpClient::pre_get(FtpResp &resp, const char *remotepath, ExceptionSink *xsink) {
   // set binary mode and establish data connection
   if (setBinaryMode(true, xsink) || connectData(xsink))
      return -1;

   // setup the file transfer on the data channel
   int code;
   resp.assign(sendMsg(code, "RETR", remotepath, xsink));
   if (xsink->isEvent()) {
      priv->data.close();
      return -1;
   }
   //printf("%s", resp->getBuffer());

   if ((code / 100) != 1) {
      priv->data.close();
      xsink->raiseException("FTP-GET-ERROR", "could not retrieve file, FTP server replied: %s", resp.getBuffer());
      return -1;
   }

   if ((priv->mode == FTP_MODE_PORT && acceptDataConnection(xsink)) || xsink->isEvent()) {
      priv->data.close();
      return -1;
   }
   else if (priv->secure_data && priv->data.upgradeClientToSSL(0, 0, xsink)) {
      priv->data.close();
      return -1;      
   }

   return 0;
}

// public locked
int QoreFtpClient::get(const char *remotepath, const char *localname, ExceptionSink *xsink) {
   printd(5, "QoreFtpClient::get(%s, %s)\n", remotepath, localname ? localname : "NULL");

   SafeLocker sl(priv->m);
   if (!priv->loggedin) {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::get()");
      return -1;
   }

   // get local file name
   char *ln = localname ? (char *)localname : q_basename(remotepath);

   printd(FTPDEBUG, "QoreFtpClient::get(%s) %s\n", remotepath, ln);
   // open local file
   int fd = open(ln, O_WRONLY|O_CREAT, 0644);
   if (fd < 0) {
      xsink->raiseException("FTP-FILE-OPEN-ERROR", "%s: %s", ln, strerror(errno));
      if (ln != localname)
	 free(ln);
      return -1;
   }

   FtpResp resp;
   if (pre_get(resp, remotepath, xsink)) {
      // delete temporary file
      unlink(ln);
      if (ln != localname)
	 free(ln);
      close(fd);
      return -1;
   }

   if (ln != localname)
      free(ln);

   priv->data.recv(fd, -1, -1);
   priv->data.close();
   close(fd);

   int code;
   resp.assign(getResponse(code, xsink));
   sl.unlock();
   if (xsink->isEvent())
      return -1;

   //printf("PUT: %s", resp->getBuffer());
   if ((code / 100 != 2)) {
      xsink->raiseException("FTP-GET-ERROR", "FTP server returned an error to the RETR command: %s", 
			    resp.getBuffer());
      return -1;
   }
   return 0;
}

static void doFtpSocketException(int rc, ExceptionSink *xsink) {
   // -2 = socket not open - should not be possible
   assert(rc != -2);

   if (!rc)
      xsink->raiseException("DATA-SOCKET-CLOSED", "remote end closed the data connection");
   else if (rc == -1)   // recv() error
      xsink->raiseException("DATA-SOCKET-RECV-ERROR", strerror(errno));
   else if (rc == -3)
      xsink->raiseException("TIMEOUT", "the transfer exceeded the timeout period");
}

// public locked
QoreStringNode *QoreFtpClient::getAsString(const char *remotepath, ExceptionSink *xsink) {
   printd(5, "QoreFtpClient::getAsString(%s)\n", remotepath);

   SafeLocker sl(priv->m);
   if (!priv->loggedin) {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::getAsString()");
      return 0;
   }

   printd(FTPDEBUG, "QoreFtpClient::getAsString(%s)\n", remotepath);

   FtpResp resp;
   if (pre_get(resp, remotepath, xsink))
      return 0;

   int rc;
   SimpleRefHolder<QoreStringNode> rv(priv->data.recv(-1, -1, &rc));
   priv->data.close();

   int code;
   resp.assign(getResponse(code, xsink));
   sl.unlock();

   if (xsink->isEvent())
      return 0;
   if (rc <= 0) {
      doFtpSocketException(rc, xsink);
      return 0;
   }

   //printf("PUT: %s", resp->getBuffer());
   if ((code / 100 != 2)) {
      xsink->raiseException("FTP-GETASSTRING-ERROR", "FTP server returned an error to the RETR command: %s", 
			    resp.getBuffer());
      return 0;
   }
   return rv.release();
}

// public locked
BinaryNode *QoreFtpClient::getAsBinary(const char *remotepath, ExceptionSink *xsink) {
   printd(5, "QoreFtpClient::getAsBinary(%s)\n", remotepath);

   SafeLocker sl(priv->m);
   if (!priv->loggedin) {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::getAsBinary()");
      return 0;
   }

   printd(FTPDEBUG, "QoreFtpClient::getAsBinary(%s)\n", remotepath);

   FtpResp resp;
   if (pre_get(resp, remotepath, xsink))
      return 0;

   int rc;
   SimpleRefHolder<BinaryNode> rv(priv->data.recvBinary(-1, -1, &rc));
   priv->data.close();

   int code;
   resp.assign(getResponse(code, xsink));
   sl.unlock();

   if (xsink->isEvent())
      return 0;
   if (rc <= 0) {
      doFtpSocketException(rc, xsink);
      return 0;
   }

   //printf("PUT: %s", resp->getBuffer());
   if ((code / 100 != 2)) {
      xsink->raiseException("FTP-GETASBINARY-ERROR", "FTP server returned an error to the RETR command: %s", 
			    resp.getBuffer());
      return 0;
   }
   return rv.release();
}

// public locked
int QoreFtpClient::cwd(const char *dir, ExceptionSink *xsink)
{
   SafeLocker sl(priv->m);
   if (!priv->loggedin) {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::cwd()");
      return -1;
   }
   int code;
   QoreStringNodeHolder p(sendMsg(code, "CWD", dir, xsink));

   sl.unlock();
   if (xsink->isEvent())
      return -1;

   if ((code / 100) == 2)
      return 0;

   p->chomp();
   xsink->raiseException("FTP-CWD-ERROR", "FTP server returned an error to the CWD command: %s", p->getBuffer());
   return -1;
}

// public locked
QoreStringNode *QoreFtpClient::pwd(ExceptionSink *xsink) {
   SafeLocker sl(priv->m);
   if (!priv->loggedin) {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::pwd()");
      return 0;
   }

   int code;
   QoreStringNodeHolder p(sendMsg(code, "PWD", 0, xsink));
   sl.unlock();
   if ((getFTPCode(*p) / 100) == 2) {
      QoreStringNode *rv = p->substr(4, xsink);
      assert(!*xsink); // not possible to have an exception here
      rv->chomp();
      return rv;
   }
   p->chomp();
   xsink->raiseException("FTP-PWD-ERROR", "FTP server returned an error response to the PWD command: %s", p->getBuffer());
   return 0;
}

// public locked
int QoreFtpClient::del(const char *file, ExceptionSink *xsink) {
   SafeLocker sl(priv->m);
   if (!priv->loggedin) {
      xsink->raiseException("FTP-NOT-CONNECTED", "QoreFtpClient::connect() must be called before the QoreFtpClient::delete()");
      return -1;
   }

   int code;
   QoreStringNodeHolder p(sendMsg(code, "DELE", file, xsink));
   sl.unlock();
   if (xsink->isEvent())
      return -1;

   if ((code / 100) == 2)
      return 0;

   p->chomp();
   xsink->raiseException("FTP-DELETE-ERROR", "FTP server returned an error to the DELE command: %s", p->getBuffer());
   return -1;
}

void QoreFtpClient::setURL(const QoreString *url, ExceptionSink *xsink)
{
   lock();
   priv->setURLInternal(url, xsink);
   unlock();
}

QoreStringNode *QoreFtpClient::getURL() const
{
   QoreStringNode *url = new QoreStringNode("ftp://");
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
   priv->user = u ? strdup(u) : 0;
   unlock();
}

void QoreFtpClient::setPassword(const char *p) 
{ 
   lock();
   if (priv->pass)
      free(priv->pass); 
   priv->pass = p ? strdup(p) : 0;
   unlock();
}

void QoreFtpClient::setHostName(const char *h) 
{ 
   lock();
   if (priv->host) 
      free(priv->host); 
   priv->host = h ? strdup(h) : 0;
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

void QoreFtpClient::setEventQueue(Queue *cbq, ExceptionSink *xsink) {
   priv->setEventQueue(cbq, xsink);
}

void QoreFtpClient::setControlEventQueue(Queue *cbq, ExceptionSink *xsink) {
   priv->setControlEventQueue(cbq, xsink);
}

void QoreFtpClient::setDataEventQueue(Queue *cbq, ExceptionSink *xsink) {
   priv->setDataEventQueue(cbq, xsink);
}

void QoreFtpClient::cleanup(ExceptionSink *xsink) {
   priv->cleanup(xsink);
}

