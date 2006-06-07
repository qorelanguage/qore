/*
  TIBCO/tibco.cc

  TIBCO integration to QORE

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
#include <qore/support.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/Object.h>
#include <qore/Exception.h>
#include <qore/QoreString.h>
#include <qore/params.h>
#include <qore/QoreClass.h>

#include "tibco.h"
#include "QoreApp.h"

int CID_TIBCO;

static inline class QoreNode *map_minstance_to_node(const MInstance *min, ExceptionSink *xsink)
{
   Hash *h = new Hash();

   MEnumerator<MString, MData *> *me = min->newEnumerator();
   MString name;
   MData *val;

   while (me->next(name, val) && !xsink->isEvent())
      h->setKeyValue((char *)name.c_str(), map_mdata_to_node(val, xsink), xsink);

   delete me;
   if (xsink->isEvent())
   {
      delete h;
      return NULL;
   }
   return new QoreNode(h);
}

// maps a TIBCO sequence to a QORE list
static inline class QoreNode *map_msequence_to_node(const MSequence *ms, ExceptionSink *xsink)
{
   class QoreNode *rv = new QoreNode(NT_LIST);
   rv->val.list = new List();

   for (unsigned i = 0; i < ms->size() && !xsink->isEvent(); i++)
      rv->val.list->push(map_mdata_to_node((MData *)(*ms)[i], xsink));

   if (xsink->isEvent())
   {
      rv->deref(xsink);
      return NULL;
   }
   return rv;
}

// maps a TIBCO associative list to a QORE hash
static inline class QoreNode *map_massoclist_to_node(const MAssocList *mal, ExceptionSink *xsink)
{
   Hash *h = new Hash();

   MEnumerator<MString, MData *> *me = mal->newEnumerator();
   MString name;
   MData *val;

   while (me->next(name, val) && !xsink->isEvent())
      h->setKeyValue((char *)name.c_str(), map_mdata_to_node(val, xsink), xsink);

   delete me;
   if (xsink->isEvent())
   {
      delete h;
      return NULL;
   }
   return new QoreNode(h);
}

// maps a TIBCO union to a QORE hash
static inline class QoreNode *map_munion_to_node(const MUnion *mu, ExceptionSink *xsink)
{
   Hash *h = new Hash();

   MString MSkey = mu->getMemberName();
   h->setKeyValue((char *)MSkey.c_str(), map_mdata_to_node((MData *)mu->get(MSkey), xsink), xsink);
   
   if (xsink->isEvent())
   {
      delete h;
      return NULL;
   }
   return new QoreNode(h);
}

// maps a TIBCO MTree to a QORE node
class QoreNode *map_mdata_to_node(MData *md, ExceptionSink *xsink)
{
   const MInstance *min;
   const MAssocList *mal;
   const MSequence *ms;
   const MUnion *mu;
   const MInteger *mi;
   const MStringData *msd;
   const MReal *mr;
   const MDateTime *mdt;
   const MBool *mb;
   const MDate *mdate;

   // is it an MInstance?
   if ((min = MInstance::downCast(md)))
      return map_minstance_to_node(min, xsink);
   // is it an associative list?
   else if ((mal = MAssocList::downCast(md)))
      return map_massoclist_to_node(mal, xsink);
   // or is it a sequence?
   else if ((ms = MSequence::downCast(md)))
      return map_msequence_to_node(ms, xsink);
   // or is it a union?
   else if ((mu = MUnion::downCast(md)))
      return map_munion_to_node(mu, xsink);
   else if ((mi = MInteger::downCast(md)))
   {
      QoreNode *rv = new QoreNode(NT_INT);
      rv->val.intval = mi->getAsLong();
      return rv;
   }
   else if ((msd = MStringData::downCast(md)))
   {
      MString ms = msd->getAsString();
      return new QoreNode((char *)ms.c_str());
   }
   else if ((mr = MReal::downCast(md)))
      return new QoreNode(mr->getAsDouble());
   else if ((mdt = MDateTime::downCast(md)))
   {
      DateTime *dt = new DateTime;
      dt->year        = mdt->getYear();
      dt->month       = mdt->getMonth();
      dt->day         = mdt->getDay();
      dt->hour        = mdt->getHour();
      dt->minute      = mdt->getMinute();
      dt->second      = mdt->getSecond();
      dt->millisecond = mdt->getMicroSeconds() / 1000;
      dt->relative    = 0;
      return new QoreNode(dt);
   }
   else if ((mdate = MDate::downCast(md)))
   {
      DateTime *dt = new DateTime;
      dt->year        = mdate->getYear();
      dt->month       = mdate->getMonth();
      dt->day         = mdate->getDay();
      dt->hour        = 0;
      dt->minute      = 0;
      dt->second      = 0;
      dt->millisecond = 0;
      dt->relative    = 0;
      return new QoreNode(dt);
   }
   else if ((mb = MBool::downCast(md)))
   {
      QoreNode *rv = new QoreNode(NT_BOOLEAN);
      rv->val.boolval = mb->getAsBoolean();
      return rv;
   }
   xsink->raiseException("MAP-ERROR", "can't map MData element of class \"%s\" to QORE type",
	  md->getClassName());
   return NULL;
}

static inline void set_properties(MAppProperties *appProperties, Hash *h, ExceptionSink *xsink)
{
   tracein("set_properties()");

   HashIterator *hi = h->newIterator();
   while (hi->next())
   {
      char *key = hi->getKey();
      if (!hi->getValue())
      {
	 xsink->raiseException("TIBCO-INVALID-PROPERTIES-HASH", 
			"properties hash key \"%s\" has value = NOTHING",
			key);
	 delete hi;
	 return;
      }
      else if (hi->getValue()->type != NT_STRING)
      {
	 xsink->raiseException("TIBCO-INVALID-PROPERTIES-HASH",
			"properties hash has invalid type \"%s\" for key \"%s\" (must be string)",
			hi->getValue()->type->name, key);
	 delete hi;
	 return;
      }
      char *val = hi->getValue()->val.String->getBuffer();

      if (!strcmp(key, "AppVersion"))
	 appProperties->setAppVersion(val);
      else if (!strcmp(key, "AppInfo"))
	 appProperties->setAppInfo(val);
      else if (!strcmp(key, "AppName"))
	 appProperties->setAppName(val);
      else if (!strcmp(key, "RepoURL"))
	 appProperties->setRepoURL(val);
      else if (!strcmp(key, "ConfigURL"))
	 appProperties->setConfigURL(val);
#ifdef DEBUG
      else printe("ignoring properties member %s=%s\n", key, val);
#endif
   }
   delete hi;

   appProperties->setMultiThreaded(); 
   //appProperties->setDefaultStringEncoding(MEncoding::M_ASCII);
   //appProperties->setDefaultStringEncoding(MEncoding::M_LATIN_1);

   traceout("set_properties()");
}

/*
const char *MStringData::data(char const *type, unsigned int *p) const
{
   printd(0, "MStringData::data(%s, %08x) returning %s\n", type ? type : "(NULL)", p, "hello");
   return "hello";
}
*/

static inline void *getTA(void *obj)
{
   ((QoreApp *)obj)->ROreference();
   return obj;
}

// usage: new Tibco(session-name, properties, classlist, [, service, network, daemon])
class QoreNode *TIBCO_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBCO_constructor");

   char *session_name, *service = NULL, *network = NULL, *daemon = NULL;
   QoreNode *p0, *p1, *p2, *p;

   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_HASH, 1)))
   {
      xsink->raiseException("TIBCO-PARAMETER-ERROR", "invalid parameters passed to Tibco() constructor, expecting session name (string), properties (object), [classlist (object), service (string), network (string), daemon (string)]");
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      traceout("TIBCO_constructor");
      return NULL;
   }

   session_name = p0->val.String->getBuffer();
   Hash *classlist;
   if ((p2 = test_param(params, NT_HASH, 2)))
   {
      // FIXME: check that classlist hash has only String values!
      classlist = p2->val.hash->copy();
   }
   else
      classlist = NULL;

   if ((p = test_param(params, NT_STRING, 3)))
      service = p->val.String->getBuffer();

   if ((p = test_param(params, NT_STRING, 4)))
      network = p->val.String->getBuffer();

   if ((p = test_param(params, NT_STRING, 5)))
      daemon = p->val.String->getBuffer();

   // create adapter instance
   printd(1, "TIBCO_constructor() session=%s service=%s network=%s daemon=%s\n",
	  session_name, 
	  service ? service : "(null)", 
	  network ? network : "(null)", 
	  daemon  ? daemon  : "(null)");
   class QoreApp *myQoreApp = NULL;

   MAppProperties *appProps = new MAppProperties();

   //appProps->setAppName("coreDAS");
   //appProps->setAppVersion("1.0");
   //appProps->setAppInfo("hi");
   //appProps->setRepoURL("/home/david/src/qore/src/small.dat");
   //appProps->setConfigURL("/tibco/private/adapter/coreDAS");
   set_properties(appProps, p1->val.hash, xsink); 

   if (xsink->isEvent())
   {
      if (classlist)
      {
	 classlist->dereference(xsink);
	 delete classlist;
      }
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      return NULL;
   }
   try 
   {
      printd(4, "before QoreApp constructor\n");
      myQoreApp =
         new QoreApp(appProps, session_name, classlist, service, network, daemon);
      printd(4, "after QoreApp constructor (%08x)\n", myQoreApp);

      printd(4, "before start()\n");
      myQoreApp->start(Mfalse);
      printd(4, "after start()\n");
   }
   catch (MException &te)
   {
      xsink->raiseException("TIBCO-EXCEPTION", "Exception thrown in Tibco() constructor %s: %s",
			 te.getType().c_str(), te.getDescription().c_str());
      if (myQoreApp)
	 delete myQoreApp;
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      traceout("TIBCO_constructor");
      return NULL;
   }
   if (self->setPrivate(CID_TIBCO, myQoreApp, getTA))
      myQoreApp->deref(xsink);
   printd(5, "TIBCO_constructor() this=%08x myQoreApp=%08x\n", self, myQoreApp);
   traceout("TIBCO_constructor");
   return NULL;
}

class QoreNode *TIBCO_destructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBCO_destructor()");
   // set adapter paramter
   QoreApp *myQoreApp = (QoreApp *)self->getAndClearPrivateData(CID_TIBCO);
   if (myQoreApp)
      myQoreApp->deref(xsink);
   traceout("TIBCO_destructor()");
   return NULL;
}

static QoreNode *TIBCO_copy(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("TIBCO-ADAPTER-COPY-ERROR", "copying TibcoAdapter objects is curently not supported");
   return NULL;
}

// usage: Tibco::sendWithSyncReply(function_name, message[, timeout])
class QoreNode *TIBCO_sendWithSyncReply(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   int timeout = 0;
   class QoreNode *rv = NULL, *p0, *p1, *p2;
   char *fname;

   // check input parameters
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_HASH, 1)))
   {
      xsink->raiseException("TIBCO-SEND-WITH-SYNC-REPLY-PARAMETER-ERROR", "invalid parameters passed to tibco_sendWithSyncReply(), expecting function name (string), message (hash), [timeout (int)]");
      return NULL;
   }

   fname = p0->val.String->getBuffer();

   // set timeout paramter if present
   if ((p2 = get_param(params, 2)))
      timeout = p2->getAsInt();

   // set adapter paramter
   QoreApp *myQoreApp = (QoreApp *)self->getReferencedPrivateData(CID_TIBCO);

   if (myQoreApp)
   {
      // try to send message
      try
      {
	 rv = myQoreApp->sendWithSyncReply(fname, p1, timeout, xsink);
      }
      catch(MException &te)
      {
	 xsink->raiseException("TIBCO-EXCEPTION", 
			"Exception caught while sending \"%s\" with subject \"%s\": %s: %s", 
			fname, myQoreApp->get_subject(), te.getType().c_str(), te.getDescription().c_str());      
      }
      myQoreApp->deref(xsink);
   }
   else
      alreadyDeleted(xsink, "TibcoAdapter::sendWithSyncReply");

   return rv;
}

// usage: TIBCO_send(function_name, message)
class QoreNode *TIBCO_send(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   char *fname;
   class QoreNode *p0, *p1;

   // check input parameters
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_HASH, 1)))
   {
      xsink->raiseException("TIBCO-SEND-PARAMETER-ERROR", "invalid parameters passed to tibco_send(), expecting function name (string), message (hash)");
      return NULL;
   }

   fname = p0->val.String->getBuffer();

   // set adapter paramter
   QoreApp *myQoreApp = (QoreApp *)self->getReferencedPrivateData(CID_TIBCO);

   if (myQoreApp)
   {
      // try to send message
      try
      {
	 myQoreApp->send(fname, p1, xsink);
      }
      catch(MException &te)
      {
	 xsink->raiseException("TIBCO-EXCEPTION", 
			"Exception caught while sending \"%s\" with subject \"%s\": %s: %s", 
			fname, myQoreApp->get_subject(), te.getType().c_str(), te.getDescription().c_str());      
      }
      myQoreApp->deref(xsink);
   }
   else
      alreadyDeleted(xsink, "TibcoAdapter::send");
   return NULL;
}

// usage: TIBCO_sendSubject(subject, function_name, message)
class QoreNode *TIBCO_sendSubject(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p0, *p1, *p2;

   // check input parameters
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)) ||
       !(p2 = test_param(params, NT_HASH, 2)))
   {
      xsink->raiseException("TIBCO-SENDSUBJECT-PARAMETER-ERROR", "invalid parameters passed to TibcoAdapter::sendSubject(), expecting subject (string), function name (string), message (hash)");
      return NULL;
   }

   char *subject = p0->val.String->getBuffer();
   char *fname = p1->val.String->getBuffer();

   // set adapter paramter
   QoreApp *myQoreApp = (QoreApp *)self->getReferencedPrivateData(CID_TIBCO);

   if (myQoreApp)
   {
      // try to send message
      try
      {
	 myQoreApp->send(subject, fname, p2, xsink);
      }
      catch(MException &te)
      {
	 xsink->raiseException("TIBCO-EXCEPTION", 
			"Exception caught while sending \"%s\" with subject \"%s\": %s: %s", 
			fname, subject, te.getType().c_str(), te.getDescription().c_str());      
      }
      myQoreApp->deref(xsink);
   }
   else
      alreadyDeleted(xsink, "TibcoAdapter::sendSubject");
   return NULL;
}

// usage: TIBCO_setSubjectName(subject);
class QoreNode *TIBCO_setSubjectName(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("TIBCO-SET-SUBJECT-NAME-PARAMETER-ERROR", "invalid parameters passed to tibco_set_subject_name(), expecting subject name (string)");
      return NULL;
   }
   printd(1, "tibco_Set_subject_name() setting subject name to \"%s\"\n",
	  p0->val.String->getBuffer());

   // set adapter paramter
   QoreApp *myQoreApp = (QoreApp *)self->getReferencedPrivateData(CID_TIBCO);
   if (myQoreApp)
   {
      myQoreApp->set_subject_name(p0->val.String->getBuffer());
      myQoreApp->deref(xsink);
   }
   else
      alreadyDeleted(xsink, "TibcoAdapter::setSubjectName");

   return NULL;
}

// usage: Tibco::sendSubjectWithSyncReply(subject, function_name, message[, timeout])
class QoreNode *TIBCO_sendSubjectWithSyncReply(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   int timeout = 0;
   class QoreNode *rv = NULL, *p0, *p1, *p2, *p3;
   char *fname;

   // check input parameters
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)) ||
       !((p2 = test_param(params, NT_OBJECT, 2)) || (p2 = test_param(params, NT_HASH, 2))))
   {
      xsink->raiseException("TIBCO-SEND-WITH-SYNC-REPLY-PARAMETER-ERROR", "invalid parameters passed to tibco_send_with_sync_reply(), expecting subject (string), function name (string), message (object), [timeout (int)]");
      return NULL;
   }

   fname = p1->val.String->getBuffer();

   // set timeout paramter if present
   if ((p3 = get_param(params, 3)))
      timeout = p3->getAsInt();

   // set adapter paramter
   QoreApp *myQoreApp = (QoreApp *)self->getReferencedPrivateData(CID_TIBCO);

   if (myQoreApp)
   {
      // try to send message
      try
      {
	 printd(2, "TIBCO_sendSubjectWithSyncReply() calling sendWithSyncReply()\n");
	 rv = myQoreApp->sendWithSyncReply(p0->val.String->getBuffer(), fname, p2, timeout, xsink);
      }
      catch(MException &te)
      {
	 xsink->raiseException("TIBCO-EXCEPTION", 
			"Exception caught while sending \"%s\" with subject \"%s\": %s: %s", 
			fname, p0->val.String->getBuffer(), te.getType().c_str(), te.getDescription().c_str());      
      }
      myQoreApp->deref(xsink);
   }
   else
      alreadyDeleted(xsink, "TibcoAdapter::sendSubjectWithSyncReply");
   return rv;
}

// Tibco::receive(subject, [timeout])
class QoreNode *TIBCO_receive(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   unsigned long timeout = 0;
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("TIBCO-RECEIVE-PARAMETER-ERROR", "invalid parameters passed to tibco_receive(), expecting subject (string), [timeout (int)]");
      return NULL;
   }

   char *subject = p0->val.String->getBuffer();
   if ((p1 = get_param(params, 1)))
      timeout = p1->getAsInt();

   // set adapter paramter
   QoreApp *myQoreApp = (QoreApp *)self->getReferencedPrivateData(CID_TIBCO);
   QoreNode *rv = NULL;

   if (myQoreApp)
   {
      rv = myQoreApp->receive(subject, timeout, xsink);
      myQoreApp->deref(xsink);
   }
   else
      alreadyDeleted(xsink, "TibcoAdapter::receive");
   return rv;
}

class QoreClass *initTibcoAdapterClass()
{
   tracein("initTibcoAdapterClass()");

   class QoreClass *QC_TIBCO = new QoreClass(strdup("TibcoAdapter"));
   CID_TIBCO = QC_TIBCO->getID();
   QC_TIBCO->addMethod("constructor",              TIBCO_constructor);
   QC_TIBCO->addMethod("destructor",               TIBCO_destructor);
   QC_TIBCO->addMethod("copy",                     TIBCO_copy);
   QC_TIBCO->addMethod("receive",                  TIBCO_receive);
   //QC_TIBCO->addMethod("send",                     TIBCO_send);
   QC_TIBCO->addMethod("sendSubject",              TIBCO_sendSubject);
   //QC_TIBCO->addMethod("sendWithSyncReply",        TIBCO_sendWithSyncReply);
   QC_TIBCO->addMethod("sendSubjectWithSyncReply", TIBCO_sendSubjectWithSyncReply);
   //QC_TIBCO->addMethod("setSubjectName",           TIBCO_setSubjectName);
   traceout("initTibcoAdapterClass()");
   return QC_TIBCO;
}
