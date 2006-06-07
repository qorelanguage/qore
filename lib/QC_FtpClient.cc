/*
  QC_FtpClient.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/thread.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>
#include <qore/Exception.h>

#include "QC_FtpClient.h"

int CID_FTPCLIENT;

static inline void *getFtpClient(void *obj)
{
   ((QoreFtpClient *)obj)->ROreference();
   return obj;
}

class QoreNode *FC_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = test_param(params, NT_STRING, 0);
   class QoreString *url;
   if (p0)
      url = p0->val.String;
   else
      url = NULL;

   class QoreFtpClient *f;

   if (self->setPrivate(CID_FTPCLIENT, f = new QoreFtpClient(url, xsink), getFtpClient) || xsink->isEvent())
      f->deref();
   return NULL;
}

class QoreNode *FC_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getAndClearPrivateData(CID_FTPCLIENT);
   if (f)
      f->deref();
   return NULL;
}

class QoreNode *FC_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("FTPCLIENT-COPY-ERROR", "FtpClient objects cannot be copied.");
   return NULL;
}

class QoreNode *FC_connect(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   class QoreNode *rv;
   if (f)
   {
      int rc = f->connect(xsink);
      f->deref();
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
   }
   else
   {
      alreadyDeleted(xsink, "FtpClient::connect");
      rv = NULL;
   }
   return rv;
}

class QoreNode *FC_disconnect(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      int rc = f->disconnect();
      f->deref();
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::disconnect");
   }
   return rv;
}

class QoreNode *FC_list(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   char *path;
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (p0)
      path = p0->val.String->getBuffer();
   else
      path = NULL;

   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      QoreString *l = f->list(path, true, xsink);
      if (l)
	 rv = new QoreNode(l); 
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::list");
   }
   return rv;
}

class QoreNode *FC_nlst(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   char *path;
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (p0)
      path = p0->val.String->getBuffer();
   else
      path = NULL;

   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      QoreString *l = f->list(path, false, xsink);
      if (l)
	 rv = new QoreNode(l); 
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::nlst");
   }
   return rv;
}

class QoreNode *FC_pwd(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      QoreString *l = f->pwd(xsink);
      if (l)
	 rv = new QoreNode(l); 
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::pwd");
   }
   return rv;
}

class QoreNode *FC_cwd(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0 || !p0->val.String->strlen())
   {
      xsink->raiseException("FTPCLIENT-CWD-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::cwd()");
      return NULL;
   }

   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      int rc = f->cwd(p0->val.String->getBuffer(), xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::cwd");
   }
   return rv;
}

class QoreNode *FC_put(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0 || !p0->val.String->strlen())
   {
      xsink->raiseException("FTPCLIENT-PUT-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::put()");
      return NULL;
   }
   char *rn;
   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (p1)
      rn = p1->val.String->getBuffer();
   else
      rn = NULL;

   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      int rc = f->put(p0->val.String->getBuffer(), rn, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::put");
   }
   return rv;
}

class QoreNode *FC_get(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0 || !p0->val.String->strlen())
   {
      xsink->raiseException("FTPCLIENT-GET-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::get()");
      return NULL;
   }
   char *ln;
   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (p1)
      ln = p1->val.String->getBuffer();
   else
      ln = NULL;

   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      int rc = f->get(p0->val.String->getBuffer(), ln, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::get");
   }
   return rv;
}

class QoreNode *FC_del(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0 || !p0->val.String->strlen())
   {
      xsink->raiseException("FTPCLIENT-DEL-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::del()");
      return NULL;
   }

   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      int rc = f->del(p0->val.String->getBuffer(), xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::del");
   }
   return rv;
}

class QoreNode *FC_setUserName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETUSERNAME-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::setUserName()");
      return NULL;
   }
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   if (f)
   {
      f->setUserName(p0->val.String->getBuffer());
      f->deref();
   }
   else
      alreadyDeleted(xsink, "FtpClient::setUserName");
   return NULL;
}

class QoreNode *FC_setPassword(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETPASSWORD-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::setPassword()");
      return NULL;
   }
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   if (f)
   {
      f->setPassword(p0->val.String->getBuffer());
      f->deref();
   }
   else
      alreadyDeleted(xsink, "FtpClient::setPassword");
   return NULL;
}

class QoreNode *FC_setHostName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETHOSTNAME-PARAMETER-ERROR", "expecting name(string) as first parameter of FtpClient::setHostName()");
      return NULL;
   }
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   if (f)
   {
      f->setHostName(p0->val.String->getBuffer());
      f->deref();
   }
   else
      alreadyDeleted(xsink, "FtpClient::setHostName");
   return NULL;
}

class QoreNode *FC_setPort(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   int port;
   if (p0)
      port = p0->getAsInt();
   else
      port = 0;
   if (!port)
   {
      xsink->raiseException("FTPCLIENT-SETPORT-PARAMETER-ERROR", "expecting non-zero port(int) as first parameter of FtpClient::setPort()");
      return NULL;
   }
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   if (f)
   {
      f->setPort(port);
      f->deref();
   }
   else
      alreadyDeleted(xsink, "FtpClient::setPort");
   return NULL;
}

class QoreNode *FC_setURL(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETURL-PARAMETER-ERROR", "expecting url(string) as first parameter of FtpClient::setURL()");
      return NULL;
   }
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   if (f)
   {
      f->setURL(p0->val.String, xsink);
      f->deref();
   }
   else
      alreadyDeleted(xsink, "FtpClient::setURL");
   return NULL;
}

class QoreNode *FC_getUserName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      char *u = f->getUserName();
      if (u)
	 rv = new QoreNode(u); 
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::getUserName");
   }
   return rv;
}

class QoreNode *FC_getPassword(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      char *p = f->getPassword();
      if (p)
	 rv = new QoreNode(p); 
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::getPassword");
   }
   return rv;
}

class QoreNode *FC_getHostName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      char *h = f->getHostName();
      if (h)
	 rv = new QoreNode(h); 
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::getHostName");
   }
   return rv;
}

class QoreNode *FC_getPort(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      rv = new QoreNode(NT_INT, f->getPort());
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::getPort");
   }
   return rv;
}

class QoreNode *FC_getURL(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreFtpClient *f = (QoreFtpClient *)self->getReferencedPrivateData(CID_FTPCLIENT);
   QoreNode *rv;
   if (f)
   {
      class QoreString *u = f->getURL();
      if (u)
	 rv = new QoreNode(u); 
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "FtpClient::getURL");
   }
   return rv;
}

class QoreClass *initFtpClientClass()
{
   tracein("initFtpClientClass()");

   class QoreClass *QC_FTPCLIENT = new QoreClass(strdup("FtpClient"));
   CID_FTPCLIENT = QC_FTPCLIENT->getID();
   QC_FTPCLIENT->addMethod("constructor",   FC_constructor);
   QC_FTPCLIENT->addMethod("destructor",    FC_destructor);
   QC_FTPCLIENT->addMethod("copy",          FC_copy);
   QC_FTPCLIENT->addMethod("connect",       FC_connect);
   QC_FTPCLIENT->addMethod("disconnect",    FC_disconnect);
   QC_FTPCLIENT->addMethod("list",          FC_list);
   QC_FTPCLIENT->addMethod("nlst",          FC_nlst);
   QC_FTPCLIENT->addMethod("pwd",           FC_pwd);
   QC_FTPCLIENT->addMethod("cwd",           FC_cwd);
   QC_FTPCLIENT->addMethod("get",           FC_get);
   QC_FTPCLIENT->addMethod("put",           FC_put);
   QC_FTPCLIENT->addMethod("del",           FC_del);
   QC_FTPCLIENT->addMethod("setUserName",   FC_setUserName);
   QC_FTPCLIENT->addMethod("setPassword",   FC_setPassword);
   QC_FTPCLIENT->addMethod("setHostName",   FC_setHostName);
   QC_FTPCLIENT->addMethod("setPort",       FC_setPort);
   QC_FTPCLIENT->addMethod("setURL",        FC_setURL);
   QC_FTPCLIENT->addMethod("getUserName",   FC_getUserName);
   QC_FTPCLIENT->addMethod("getPassword",   FC_getPassword);
   QC_FTPCLIENT->addMethod("getHostName",   FC_getHostName);
   QC_FTPCLIENT->addMethod("getPort",       FC_getPort);
   QC_FTPCLIENT->addMethod("getURL",        FC_getURL);

   traceout("initFtpClientClass()");
   return QC_FTPCLIENT;
}
