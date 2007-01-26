/*
  QoreApp.cc

  TIBCO integration to QORE

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/Object.h>
#include <qore/QoreString.h>
#include <qore/QoreNode.h>
#include <qore/LockedObject.h>
#include <qore/ScopeGuard.h>
#include <qore/charset.h>
#include <qore/DateTime.h>
#include <qore/Restrictions.h>
#include <qore/Gate.h>
#include <qore/List.h>

#include <memory>
#include <string>
#include <map>
#include <utility>
#include <assert.h>

#include "QoreApp.h"

#include <unistd.h>
#include <Maverick.h> // TIBCO operations

#ifdef TIBCO_MDT_BUG
#include <qore/LockedObject.h>
class LockedObject l_mdate_time;
#endif

static void remove_pending_calls(MApp* app);

static char *get_class(Hash *h)
{
   QoreNode *t;

   if (!(t = h->getKeyValue("^class^")))
      return NULL;
   if (t->type != NT_STRING)
      return NULL;
   return t->val.String->getBuffer();
}

QoreApp::QoreApp(MAppProperties *pMAP, char *name, Hash *clh,
    char *svc, char *net, char *dmn, char *sbj) 
: MApp(pMAP)
{
   appProps = pMAP;
   session_name = strdup(name ? name : "");
   rcv_subject = NULL;
   service = strdup(svc ? svc : "");
   network = strdup(net ? net : "");
   daemon  = strdup(dmn ? dmn : "");
   subject = strdup(sbj ? sbj : DEFAULT_SUBJECT);
   classlist = clh;
   myPublisher = NULL;
   mySubscriber = NULL;
   myEventHandler = NULL;
   mySession = NULL;
}

QoreApp::~QoreApp()
{
   tracein("QoreApp::~QoreApp()");

   remove_pending_calls(this); // pending operations async calls

#ifdef TIBCO_EXPLICIT_CREATE_SESSION
   if (mySession)
      delete mySession;
   if (myPublisher)
      delete myPublisher;
#endif
   
   // if the listener is active
   if (mySubscriber)
   {
      mySubscriber->removeListener(myEventHandler);
      delete myEventHandler;
      delete mySubscriber;
      free(rcv_subject);
   }
   
   free(session_name);
   free(service);
   free(network);
   free(daemon);
   free(subject);
   if (classlist)
      classlist->derefAndDelete(NULL);
   
   delete appProps;
   
   traceout("QoreApp::~QoreApp()");
}

void QoreApp::deref(ExceptionSink *xsink)
{
   //tracein("QoreApp::deref()");
   if (ROdereference())
   {
      try {
         //printd(5, "QoreApp::deref() %08p: about to call stop()\n", this);
         stop();
         //printd(5, "QoreApp::deref() %08p: about to delete\n", this);
         delete this;
         //printd(5, "QoreApp::deref() %08p: returned from delete\n", this);
      }
      catch (MException &te)
      {
         //xsink->raiseException("TIBCO-EXCEPTION", "Exception caught in TibcoAdapter::destructor(): %s: %s",
         //te.getType().c_str(), te.getDescription().c_str());
      }
   }
}

const MBaseClassDescription *QoreApp::find_class(char *cn, ExceptionSink *xsink)
{
   const MBaseClassDescription *mbcd = NULL;
   QoreNode *t = NULL;

   char *cdesc;
   if (!classlist || !(t = classlist->getKeyValue((char *)cn)) || t->type != NT_STRING)
   {
      /*
      cdesc = NULL;
      const MList<MString> *l = mcr->getClassNames();
      MListEnumerator<MString> *me = l->newEnumerator();
      MString elem;
      Mu4 index;
      while (me->next(index, elem))
      {
         printd(0, "QoreApp::find_class() %s\n", elem.c_str());
         if (strstr(elem.c_str(), cn))
         {
            cdesc = (char *)elem.c_str();
            printd(0, "QoreApp:find_class() found %s!\n", cdesc);
            break;
         }
      }
      delete me;
      */
      xsink->raiseException("TIBCO-CLASS-DESCRIPTION-NOT-FOUND", "no class description given for class \"%s\"", cn);
      return NULL;
   }
   else
      cdesc = t->val.String->getBuffer();

   if (!(mbcd = mcr->getClassDescription(cdesc)))
   {
      xsink->raiseException("TIBCO-CLASS-NOT-FOUND", "class name \"%s\" with description \"%s\" cannot be found in repository", cn, cdesc);
      return NULL;
   }
   //MListEnumerator<MString> *mle = classlist->newEnumerator();
   //MString name;
   //Mu4 i;

   //while (mle->next(i, name))
   //{
   //   const char *repo_class = name.c_str();
   //   printd(5, "list (%03d) %s\n", i, repo_class);
   //}
   //delete mle;
   return mbcd;
}


// FIXME: currently no comparison is done between the expected type and the provided type
MData *QoreApp::do_primitive_type(const MPrimitiveClassDescription *pcd, QoreNode *v, ExceptionSink *xsink)
{
   //tracein("QoreApp::do_primitive_type()");

   if (!v)
      return NULL;

   if (v->type == NT_BOOLEAN)
      return new MBool(v->val.boolval);

   if (v->type == NT_HASH)
   {
      // class instantiation (normally for TIBCO m_any type)
      char *cn;
      if (!(cn = get_class(v->val.hash)))
      {
         xsink->raiseException("TIBCO-MISSING-CLASS-NAME", "instantiating type \"%s\": can't instantiate class from object without \"^class^\" entry", pcd->getFullName().c_str());
         return NULL;
      }
      else
      {
         const MBaseClassDescription *mbcd = find_class(cn, xsink);
         if (xsink->isEvent())
            return NULL;
         QoreNode *val;
         if (!(val = v->val.hash->getKeyValue("^value^")))
         {
           xsink->raiseException("TIBCO-MISSING-VALUE", "instantiating type \"%s\": no \"^value^\" entry found in hash for class \"%s\"", pcd->getFullName().c_str(), cn);
            return NULL;
         }
         return instantiate_class(val, mbcd, xsink);
      }
   }
/*
   if (v->type == NT_OBJECT)
   {
      // class instantiation (normally for TIBCO m_any type)
      char *cn;
      if (!(cn = get_class(v->val.object->data)))
      {
         xsink->raiseException("TIBCO-MISSING-CLASS-NAME", "instantiating type \"%s\": can't instantiate class from object wi
thout \"^class^\" entry", pcd->getFullName().c_str());
         return NULL;
      }
      else
      {
         const MBaseClassDescription *mbcd = find_class(cn, xsink);
         if (xsink->isEvent())
            return NULL;
         QoreNode *val;
         if (!(val = v->val.object->retrieve_value("^value^")))
         {
            xsink->raiseException("TIBCO-MISSING-VALUE", "instantiating type \"%s\": no \"^value^\" entry found in hash for c
lass \"%s\"", pcd->getFullName().c_str(), cn);
            return NULL;
         }
         return instantiate_class(val, mbcd, xsink);
      }
   }
*/

   if (v->type == NT_STRING)
   {
      printd(3, "data=%08p val=\"%s\"\n", v->val.String->getBuffer(), v->val.String->getBuffer());
#if (TIBCO_SDK == 4)
      return new MStringData(v->val.String->getBuffer());
#else
      // it appears that all MString data must be UTF-8, no matter how we use the MStringData constructor
      // furthermore, it appears that we have to trick the SDK into thinking that the data is ASCII, so
      // no conversions are attempted
      QoreString *t = v->val.String;
      if (t->getEncoding() != QCS_UTF8)
      {
         t = t->convertEncoding(QCS_UTF8, xsink);
         if (xsink->isEvent())
            return NULL;
       //printd(5, "QoreApp::do_primitive_type() converted from %s to UTF-8 (%s)\n", t->getEncoding()->code, t->getBuffer());
      }
      //md = new MStringData(t->getBuffer(), MEncoding::M_UTF8);
      //md = new MStringData(t->getBuffer(), MEncoding::M_LATIN_1);
      class MData *md = new MStringData(t->getBuffer(), MEncoding::M_ASCII);
      if (t != v->val.String)
         delete t;
      return md;
      //printd(0, "1: MStringData encoding=%s\n", ms->getEncoding());
#endif
   }

   if (v->type == NT_INT)
      return new MInteger((int)v->val.intval);

   if (v->type == NT_FLOAT)
      return new MReal(v->val.floatval);

   if (v->type == NT_DATE)
   {
      class MData *md;
      const char *type = pcd->getShortName().c_str();
      if (!strcmp(type, "dateTime") || !strcmp(type, "any"))
      {
         // we have to use a string here in case the date is too large for a time_t value
         QoreString str;
         str.sprintf("%04d-%02d-%02dT%02d:%02d:%02d",
                     v->val.date_time->getYear(), v->val.date_time->getMonth(),
                     v->val.date_time->getDay(), v->val.date_time->getHour(),
                     v->val.date_time->getMinute(), v->val.date_time->getSecond());
         //printd(5, "QoreApp::do_primitive_type() creating date \"%s\"\n", str);
#ifdef TIBCO_MDT_BUG
         l_mdate_time.lock();
#endif
         md = new MDateTime(str.getBuffer());
#ifdef TIBCO_MDT_BUG
         l_mdate_time.unlock();
#endif
      }
      else if (!strcmp(type, "date"))
      {
#if 0
         char *str = (char *)malloc(sizeof(char) * 32);
         sprintf(str, "%04d-%02d-%02d",
                 v->val.date_time->getYear(), v->val.date_time->getMonth(),
                 v->val.date_time->getDay());
         //printd(5, "QoreApp::do_primitive_type() creating date \"%s\"\n", str);
         md = new MDate(str);
         free(str);
#else
         md = new MDate(v->val.date_time->getYear(), v->val.date_time->getMonth(), v->val.date_time->getDay());
#endif
      }
      else
      {
         xsink->raiseException("TIBCO-DATE-INSTANTIATION-ERROR", "cannot map from QORE type \"date\" to TIBCO type \"%s\"",
                               pcd->getShortName().c_str());
         return NULL;
      }
      return md;
   }

   if (v->type == NT_NOTHING || v->type == NT_NULL)
      return NULL;

   xsink->raiseException("TIBCO-UNSUPPORTED-TYPE", "unsupported QORE type \"%s\" (TIBCO type \"%s\")",
                  v->type->getName(), pcd->getShortName().c_str());

   //traceout("QoreApp::do_primitive_type()");
   return NULL;
}

MTree *QoreApp::make_MTree(char *class_name, QoreNode *value, ExceptionSink *xsink)
{
   tracein("QoreApp::make_MTree");

   const MBaseClassDescription *mbcd;

   mbcd = find_class(class_name, xsink);
   if (xsink->isEvent())
      return NULL;
   if (!mbcd)
   {
      xsink->raiseException("TIBCO-CLASS-DESCRIPTION-NOT-FOUND", "cannot find TIBCO class description for class name \"%s\"", class_name);
      return NULL;
   }
   MData *md = instantiate_class(value, mbcd, xsink);
   if (xsink->isEvent())
   {
      if (md)
         delete md;
      return NULL;
   }
   MInstance *mi;
   MSequence *ms;
   MUnion *mu;
   MAssocList* mal; // added by PV
   MTree *mt = new MTree();
   //mt->setRepresentationEncoding(M_PACKING_ENCODING_LATIN_1);
   if ((mi = MInstance::downCast(md)))
   {
      mi->serialize(*mt);
      //delete mi;
   }
   else if ((ms = MSequence::downCast(md)))
   {
      ms->serialize(*mt);
      //delete ms;
   }
   else if ((mu = MUnion::downCast(md)))
   {
      mu->serialize(*mt);
      //delete mu;
   } 
   else if ((mal = MAssocList::downCast(md))) // added by PV
   {
     mal->serialize(*mt);
   }
   //else
   if (md)
      delete md;

   traceout("QoreApp::make_MTree");
   return mt;
}

// maps a TIBCO MTree to a QORE node
class QoreNode *QoreApp::map_mtree_to_node(MTree *msg, ExceptionSink *xsink)
{
   class QoreNode *rv;

   tracein("QoreApp::map_mtree_to_node()");

   MDataFactory mdf(mcr);
   if (msg)
   {
      //printd(5, "msg=%08p name=%s data=%s\n", msg, msg->getName().c_str(), msg->toString().c_str());
      MData *md = mdf.create(*msg);
      //printd(5, "md=%08p\n", md);
      rv = map_mdata_to_node(md, xsink);
      delete md;
   }
   else
      rv = NULL;

   traceout("QoreApp::map_mtree_to_node()");
   return rv;
}

MData *QoreApp::instantiate_sequence(const MSequenceClassDescription *msd, QoreNode *v, ExceptionSink *xsink)
{
   // ensure value is a QORE list
   tracein("QoreApp::instantiate_sequence()");
   // check if class info embedded
   if (v && v->type == NT_HASH)
   {
      char *cn;
      if (!(cn = get_class(v->val.hash)))
      {
         xsink->raiseException("TIBCO-MISSING-CLASS-NAME",
                            "can't instantiate sequence of type \"%s\" from hash without \"^class^\" entry",
                            msd->getFullName().c_str());

         return NULL;
      }
      printd(1, "QoreApp::instantiate_sequence() \"%s\": ignoring class information provided (%s)\n",
             msd->getFullName().c_str(), cn);
      v = v->val.hash->getKeyValue("^value^");
   }
/*
   else if (v && v->type == NT_OBJECT)
   {
      char *cn;
      if (!(cn = get_class(v->val.object->data)))
      {
         xsink->raiseException("TIBCO-MISSING-CLASS-NAME",
                            "can't instantiate sequence of type \"%s\" from object without \"^class^\" entry",
                            msd->getFullName().c_str());

         return NULL;
      }
      printd(1, "QoreApp::instantiate_sequence() \"%s\": ignoring class information provided (%s)\n",
             msd->getFullName().c_str(), cn);
      v = v->val.object->retrieve_value("^value^");
   }
*/
   else if (is_nothing(v))
   {
      traceout("QoreApp::instantiate_sequence()");
      return new MSequence(mcr, msd->getFullName());
   }
   else if (v->type != NT_LIST)
   {
      xsink->raiseException("TIBCO-INVALID-TYPE-FOR-SEQUENCE",
                         "cannot instantiate TIBCO sequence \"%s\" from node type \"%s\"",
                         msd->getFullName().c_str(), v->type->getName());
      return NULL;
   }
   MSequence *seq = new MSequence(mcr, msd->getFullName());
   if (v)
      for (int i = 0; i < v->val.list->size(); i++)
      {
         QoreNode *ne = v->val.list->retrieve_entry(i);
         char *ecn;
         const MBaseClassDescription *mbcd = msd->getContainedClassDescription();
         ecn = (char *)mbcd->getFullName().c_str();
         printd(5, "instantiate_class() implicitly instantiating %s\n", ecn);
         MData *md;
         seq->append(md = instantiate_class(ne, mbcd, xsink));
         if (md)
            delete md;
      }
   traceout("QoreApp::instantiate_sequence()");
   return seq;
}

MData *QoreApp::instantiate_modeledclass(const MModeledClassDescription *mcd, QoreNode *v, ExceptionSink *xsink)
{
   tracein("QoreApp::instantiate_modeledclass()");
   if (is_nothing(v))
   {
      traceout("QoreApp::instantiate_modeledclass()");
      return new MInstance(mcr, mcd->getFullName());
   }
   Hash *h;
   if (v->type == NT_HASH)
      h = v->val.hash;
/*
   else if (v->type == NT_OBJECT)
      h = v->val.object->data;
*/
   else
   {
      xsink->raiseException("TIBCO-INVALID-TYPE-FOR-CLASS",
                         "cannot instantiate class \"%s\" from node type \"%s\"",
                         mcd->getFullName().c_str(), v->type->getName());
      return NULL;
   }

   // make MInstance for class instantiation
   MInstance *ma = new MInstance(mcr, mcd->getFullName());
   //printd(5, "QoreApp::instantiate_modeledclass() %s=%s\n", mcd->getShortName().c_str(), ma->getName().c_str());

   // get list of hash elements for object
   HashIterator hi(h);

   try {
      // instantiate each member
      while (hi.next())
      {
         char *key = hi.getKey();
         QoreNode *t = hi.getValue();

         const MAttributeDescription *mad;
         if (!(mad = mcd->getAttribute(key)))
         {
            xsink->raiseException("TIBCO-HASH-KEY-INVALID",
                           "error instantiating TIBCO class \"%s\", hash key \"%s\" is not an attribute of this class",
                           mcd->getFullName().c_str(), key);
            delete ma;
            traceout("QoreApp::instantiate_modeledclass()");
            return NULL;
         }
         printd(5, "QoreApp::instantiate_modeledclass(): instantiating %s member %s (%08p %s)\n",
                mcd->getFullName().c_str(), key, t, t ? t->type->getName() : "(null)");
         MData *md = instantiate_class(t, mad->getAttributeClassDescription(), xsink);
         printd(5, "QoreApp::instantiate_modeledclass(): setting key=%s to md=%08p\n", key, md);
         try {
            ma->set(key, md);
         }
         catch (MException &te)
         {
            if (md)
               delete md;
            throw te;
         }
         if (md)
            delete md;
      }
   }
   catch (MException &te)
   {
      delete ma;
      traceout("QoreApp::instantiate_modeledclass()");
      throw te;
   }

   traceout("QoreApp::instantiate_modeledclass()");
   return ma;
}

MData *QoreApp::instantiate_union(const MUnionDescription *mud, QoreNode *v, ExceptionSink *xsink)
{
   tracein("QoreApp::instantiate_union()");

   if (is_nothing(v))
   {
      traceout("QoreApp::instantiate_union()");
      return new MUnion(mcr, mud->getFullName());
   }
   Hash *h;
   if (v->type == NT_HASH)
      h = v->val.hash;
/*
   else if (v->type == NT_OBJECT)
      h = v->val.object->data;
*/
   else
   {
      xsink->raiseException("TIBCO-INVALID-TYPE-FOR-UNION",
                         "cannot instantiate TIBCO union \"%s\" from node type \"%s\"",
                         mud->getFullName().c_str(), v->type->getName());
      return NULL;
   }
   // ensure that Object does not have more than one key
   if (h->size() > 1)
   {
      xsink->raiseException("TIBCO-INVALID-MULTIPLE-KEYS-FOR-UNION",
                         "cannot instantiate TIBCO union \"%s\" from from hash with %d members!",
                         mud->getFullName().c_str(), v->val.object->size());
      return NULL;
   }
   MUnion *mu = new MUnion(mcr, mud->getFullName());
   if (h->size())
   {
      HashIterator hi(h);
      // get the first entry
      hi.next();
      char *key = hi.getKey();
      QoreNode *t = hi.getValue();

      const MMemberDescription *mmd;
      if (!(mmd = mud->getMember(key)))
      {
         xsink->raiseException("TIBCO-INVALID-KEY",
                            "error instantiating TIBCO union \"%s\", hash key \"%s\" is not a member of this union",
                        mud->getFullName().c_str(), key ? key : "(null)");
         delete mu;
         return NULL;
      }
      printd(3, "QoreApp::instantiate_union(): instantiating %s member %s (%08p %s)\n",
             mud->getFullName().c_str(), key, t, t ? t->type->getName() : "(null)");
      MData *md;
      mu->set(key, md = instantiate_class(t, mmd->getMemberClassDescription(), xsink));
      if (md)
         delete md;
   }

   traceout("QoreApp::instantiate_union()");
   return mu;
}

class MData *QoreApp::instantiate_class(QoreNode *v, const MBaseClassDescription *mbcd, ExceptionSink *xsink)
{
   tracein("QoreApp::instantiate_class()");
   printd(5, "QoreApp::instantiate_class() mbcd=%08p %s: %08p (%s)\n", mbcd, mbcd->getFullName().c_str(), v, v ? v->type->getName() : "(null)");

   const MPrimitiveClassDescription *pcd;
   const MModeledClassDescription *mcd;
   const MSequenceClassDescription *msd;
   const MUnionDescription *mud;

   class MData *rv;

   // find data type
   if ((pcd = MPrimitiveClassDescription::downCast(mbcd)))
      rv = do_primitive_type(pcd, v, xsink);
   else if ((mcd = MModeledClassDescription::downCast(mbcd)))
   {
      //printd(0, "QoreApp::instantiate_class() mbcd=%08p mcd=%08p\n", mbcd, mcd);
      rv = instantiate_modeledclass(mcd, v, xsink);
   }
   else if ((msd = MSequenceClassDescription::downCast(mbcd)))
      rv = instantiate_sequence(msd, v, xsink);
   else if ((mud = MUnionDescription::downCast(mbcd)))
      rv = instantiate_union(mud, v, xsink);      
   else
   {
      rv = NULL;
      assert(false);
   }

   traceout("QoreApp::instantiate_class()");
   return rv;
}

void QoreApp::onInitialization() throw (MException)
{
   tracein("QoreApp::onInitialization()");
   try
   {
#ifdef TIBCO_EXPLICIT_CREATE_SESSION
      mySession = new MRvSession(this, session_name, service, network, daemon);
      if (!(myPublisher = new MPublisher(this, "publisher", session_name, M_RV, subject, Mfalse)))
         throw MException("error", "could not create publisher!");
#else
      if (!(myPublisher = MPublisher::downCast(getComponentByName("Publisher"))))
         throw MException("error", "publisher not found in repository");
#endif
      mcr = getClassRegistry();
   }
   catch(MException &e)
   {
      throw (e);
   }
#ifdef DEBUG
   catch (...)
   {
      assert(false);
   }
#endif
   traceout("QoreApp::onInitialization()");
}

class QoreNode *QoreApp::sendWithSyncReply(char *function_name, QoreNode *value, int timeout, ExceptionSink *xsink)
{
   MTree *msg = make_MTree(function_name, value, xsink);
   if (xsink->isEvent())
   {
      if (msg)
	 delete msg;
      return NULL;
   }
   MTree reply;
   
   // add tracking info
   MTrackingInfo ti;
   ti.addApplicationInfo("QORE.sendWithSyncReply");
   msg->setTrackingInfo(ti);
   
   QoreNode *rv;
   printd(1, "calling myPublisher->sendWithSyncReply(timeout=%d)\n", timeout);
   //myPublisher->send(msg);
   //sendLock.lock();
   if (myPublisher->sendWithSyncReply(*msg, reply, timeout))
      rv = map_mtree_to_node(&reply, xsink);
   else 
      rv = NULL;
   //sendLock.unlock();
   printd(1, "returning from mpMPublisher->sendWithSyncReply()\n");
   delete msg;
   //something more to delete here?

   return rv;
}

void QoreApp::set_subject_name(char *sub)
{
   if (subject)
      free(subject);
   subject = strdup(sub);
   if (myPublisher)
#if (TIBCO_SDK == 4)
      myPublisher->setSubjectName(subject);
#else
      myPublisher->setDestinationName(subject);
#endif
}

// this version will create a new publisher and send the message with the subject name passed
class QoreNode *QoreApp::sendWithSyncReply(char *subj, char *function_name, QoreNode *value, int timeout, ExceptionSink *xsink)
{
   MTree *msg = make_MTree(function_name, value, xsink);
   if (xsink->isEvent())
   {
      if (msg)
	 delete msg;
      return NULL;
   }
   MTree reply;
   
   // add tracking info
   MTrackingInfo ti;
   ti.addApplicationInfo("QORE.sendWithSyncReply");
   msg->setTrackingInfo(ti);
   
   QoreNode *rv;

   // create new publisher
   MPublisher *tp;
   try 
   {
      QoreString s;
      s.sprintf("temp-publisher-%d", gettid());
      if (!(tp = new MPublisher(this, s.getBuffer(), session_name, M_RV, subj, Mfalse)))
	 throw MException("error", "could not create publisher!");
   }
   catch(MException &tib_e)
   {
      xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", 
		     tib_e.getType().c_str(), tib_e.getDescription().c_str());
      if (msg)
	 delete msg;
      return NULL;
   }

   printd(1, "calling tp->sendWithSyncReply(timeout=%d)\n", timeout);
   if (tp->sendWithSyncReply(*msg, reply, timeout))
      rv = map_mtree_to_node(&reply, xsink);
   else 
      rv = NULL;

   delete tp;
   printd(1, "returning from mpMPublisher->sendWithSyncReply()\n");
   delete msg;

   return rv;
}

void QoreApp::send(char *function_name, QoreNode *value, ExceptionSink *xsink)
{
   MTree *msg = make_MTree(function_name, value, xsink);
   if (xsink->isEvent())
   {
      if (msg)
	 delete msg;
      return;
   }
   MTree reply;

   // add tracking info
   MTrackingInfo ti;
   ti.addApplicationInfo("QORE.send");
   msg->setTrackingInfo(ti);

   printd(1, "calling mpMPublisher->send()\n");
   //sendLock.lock();
   myPublisher->send(*msg);
   //sendLock.unlock();
   printd(1, "returning from mpMPublisher->send()\n");

   delete msg;
   //something more to delete here?
}

void QoreApp::send(char *subj, char *function_name, QoreNode *value, ExceptionSink *xsink)
{
   MTree *msg = make_MTree(function_name, value, xsink);
   if (xsink->isEvent())
   {
      if (msg)
	 delete msg;
      return;
   }
   MTree reply;

   // add tracking info
   MTrackingInfo ti;
   ti.addApplicationInfo("QORE.send");
   msg->setTrackingInfo(ti);

   // create new publisher
   MPublisher *tp;
   try 
   {
      QoreString s;
      s.sprintf("temp-publisher-%d", gettid());
      if (!(tp = new MPublisher(this, s.getBuffer(), session_name, M_RV, subj, Mfalse)))
	 throw MException("error", "could not create publisher!");
   }
   catch(MException &tib_e)
   {
      xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", 
			    tib_e.getType().c_str(), tib_e.getDescription().c_str());
      delete msg;
      return;
   }

   printd(1, "calling tp->sendSubject()\n");
   tp->send(*msg);

   delete tp;

   delete msg;
   //something more to delete here?
}

class QoreNode *QoreApp::receive(char *subj, unsigned long timeout, ExceptionSink *xsink)
{
   QoreNode *rv = NULL;

   tracein("QoreApp::receive()");
   try {
      rcv_lock.lock();
      try {
	 if (mySubscriber && strcmp(subj, rcv_subject))
	 {
	    printd(5, "QoreApp::receive() new subject=\"%s\", destroying listener for \"%s\"\n", subj, rcv_subject);
	    mySubscriber->removeListener(myEventHandler);
	    delete myEventHandler;
	    delete mySubscriber;
	    mySubscriber = NULL;
	    free(rcv_subject);
	 }
	 if (!mySubscriber)
	 {
	    rcv_subject = strdup(subj);
	    printd(5, "QoreApp::receive() creating Subscriber for subject \"%s\"\n", subj);
	    mySubscriber = new MSubscriber(this, "subscriber", session_name, M_RV, subj, 0, true);
	    printd(5, "QoreApp::receive() creating event handler for this subscriber\n");
	    myEventHandler = new QoreEventHandler(this);
	    printd(5, "QoreApp::receive() event handler created, adding listener\n");
	    mySubscriber->addListener(myEventHandler);
	    printd(5, "QoreApp::receive() listener added (mySession=%08p)\n", mySession);
	 }
      }
      catch(MException &tib_e)
      {
	 rcv_lock.unlock();
	 throw tib_e;
      }
      rcv_lock.unlock();

      //printd(5, "QoreApp::receive() calling mySession->nextEvent()\n");
      if (timeout)
      {
	 if (!mySession->nextEvent(timeout))
	 {
	    traceout("QoreApp::receive()");
	    return NULL;
	 }
      }
      else
	 mySession->nextEvent();

      printd(5, "QoreApp::receive() returned from nextEvent()\n");
      if (!myEventHandler->xsink.isEvent())
      {
	 // build return value
	 Hash *h = new Hash();
	 
	 printd(5, "QoreApp::receive() msgNode=%08p\n", myEventHandler->msgNode);
	 // assign "msg" member
	 h->setKeyValue("msg", myEventHandler->msgNode, NULL);
	 myEventHandler->msgNode = NULL;

	 printd(5, "QoreApp::receive() subject=%s\n", myEventHandler->subject.c_str());
	 // assign "subject" member
	 h->setKeyValue("subject", new QoreNode(myEventHandler->subject.c_str()), NULL);

	 printd(5, "QoreApp::receive() replySubject=%s\n", myEventHandler->replySubject.c_str());
	 // assign "replySubject" member
	 h->setKeyValue("replySubject", new QoreNode(myEventHandler->replySubject.c_str()), NULL);
	 rv = new QoreNode(h);
      }
      else
	 xsink->assimilate(&myEventHandler->xsink);
   }
   catch(MException &tib_e)
   {
      xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", 
			    tib_e.getType().c_str(), tib_e.getDescription().c_str());
   }
   traceout("QoreApp::receive()");
   return rv;
}

void QoreEventHandler::onEvent(const MEvent &refEvent)
{
 tracein("QoreEventHandler::onEvent()");
 try {
  const MDataEvent *mde;
  const MExceptionEvent *mee;

  if ((mde = MDataEvent::downCast(&refEvent)))
  {
    printd(5, "QoreEventHandler::onEvent(): MDataEvent received\n");
#if (TIBCO_SDK == 4)
    replySubject = mde->getReplySubject();
    subject = mde->getSubject();
#else
    replySubject = mde->getReplyDestinationName();
    subject = mde->getDestinationName();
#endif
    printd(5, "QoreEventHandler::onEvent(): GUID=%s class=%s type=%s name=%s\n",
      mde->getGUID().c_str(),
      mde->getClassName(),
      mde->getType().c_str(),
      mde->getData()->getName().c_str());
      msgNode = myQoreApp->map_mtree_to_node((MTree *)mde->getData(), &xsink);
    }
    else if ((mee = MExceptionEvent::downCast(&refEvent)))
    {
      printd(5, "QoreEventHandler::onEvent(): MExceptionEvent received\n");
      const MException *me = mee->getException();

      xsink.raiseException("TIBCO-EXCEPTION-EVENT",
        "exception event received %s: %s: %s",
        me->getErrorCode().c_str(),
        me->getCategory().c_str(),
        me->getDescription().c_str());
    }
    else
    {
      xsink.raiseException("TIBCO-UNSUPPORTED-EVENT-TYPE",
        "cannot handle event type %s",
        refEvent.getType().c_str());
    }
  }
  catch(MException &tib_e)
  {
    xsink.raiseException("TIBCO-EXCEPTION", "%s, %s",
      tib_e.getType().c_str(), tib_e.getDescription().c_str());
  }
  traceout("QoreEventHandler::onEvent()");
}


QoreEventHandler::QoreEventHandler(QoreApp *pMApp) 
: MEventListener()
{
  myQoreApp = pMApp;
  count = 0;
  msgNode = NULL;
}

QoreEventHandler::~QoreEventHandler()
{
  printd(5,"QoreEventHandler::~QoreEventHandler() destructor called\n");
}

//------------------------------------------------------------------------------
// Tibco.Operations related functionality

//------------------------------------------------------------------------------
MData* QoreApp::QoreNode2MData(char* class_name, QoreNode* value, ExceptionSink* xsink)
{
   const MBaseClassDescription* mbcd = find_class(class_name, xsink);
   if (xsink->isEvent())
      return 0;
   if (!mbcd)
   {
      xsink->raiseException("TIBCO-CLASS-DESCRIPTION-NOT-FOUND", "cannot find TIBCO class description for class name \"%s\"", class_name);
      return 0;
   }
   MData *md = instantiate_class(value, mbcd, xsink);
   if (xsink->isEvent())
   {
     delete md;
     return 0;
   }
   return md;
}

//------------------------------------------------------------------------------
void QoreApp::setRequestParamaters(MOperationRequest& req, Hash* params, ExceptionSink* xsink)
{
  HashIterator hi(params);
  while (hi.next()) {
   char *key = hi.getKey();
   QoreNode *t = hi.getValue();

    std::auto_ptr<MData> data(QoreNode2MData(key, t, xsink));
    if (xsink->isException()) {
      return;
    }
    if (!data.get()) {
      xsink->raiseException("TIBCO-CLASS-DESCRIPTION-NOT-FOUND", "cannot find TIBCO class description for class name \"%s\"", key);
      return;
    }
    MString attribute_name(key);
    req.set(attribute_name, *data.get());
  }
}

//-----------------------------------------------------------------------------
namespace {

// used by operationsGetAsyncCallReply()
struct OperationsListener : public MOperationReplyListener
{
  QoreNode* m_returned_value;
  ExceptionSink m_xsink;
   
  OperationsListener(MApp* app) : MOperationReplyListener(app), m_returned_value(0) {}
  ~OperationsListener() { 
    if (m_returned_value) m_returned_value->deref(0); 
  }

  virtual void onReply(MOperationRequest* req) {
    try {
      // ownership of the returned pointer is not documented but I assume it is owned by the req
      MOperationReply* rply = req->getReply();
      const MData* returned_Tibco_data = rply->getReturnValue();
      if (!returned_Tibco_data) return;
      m_returned_value = map_mdata_to_node((MData*)returned_Tibco_data, &m_xsink);

    } catch (MOperationException& moe) {
      m_xsink.raiseException("TIBCO-REMOTE-EXCEPTION", "%s", moe.getRemoteExceptionName().c_str());
    } catch (MException& me) {
      m_xsink.raiseException("TIBCO-EXCEPTION", "%s, %s", me.getType().c_str(), me.getDescription().c_str());
    } catch (std::exception& exc) {
      m_xsink.raiseException("TIBCO-EXCEPTION", "a C++ exception caught");
    }
  }

  // returned pointer has ownership
  QoreNode* get_data(ExceptionSink* xsink) { 
    if (m_xsink.isException()) {
      xsink->assimilate(&m_xsink); // intention: copy the existing member sink to the one passed via parameter
      return 0;
    }
    QoreNode* res = m_returned_value; 
    m_returned_value = 0; 
    return res; 
  }
};

} // anonymous namespace

//------------------------------------------------------------------------------
QoreNode* QoreApp::operationsCallWithSyncResult(char* class_name, char* method_name, Hash* parameters, unsigned timeout, char* client_name, ExceptionSink* xsink)
{
  try {
    MOperationRequest req(this, class_name, method_name, client_name);
    setRequestParamaters(req, parameters, xsink);
    if (xsink->isException()) {
      return 0;
    }
    req.syncInvoke(timeout);

    MOperationReply* rply = req.getReply(); // the same unclear ownership as in listener class above
    const MData* returned_Tibco_data = rply->getReturnValue();
    if (!returned_Tibco_data) return 0;
    return map_mdata_to_node((MData*)returned_Tibco_data, xsink);

  } catch (MOperationException& moe) {
    xsink->raiseException("TIBCO-REMOTE-EXCEPTION", "%s", moe.getRemoteExceptionName().c_str());
  } catch (MException& e) {
    xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", e.getType().c_str(), e.getDescription().c_str());
  }
  return 0;
}

//------------------------------------------------------------------------------
void QoreApp::operationsOneWayCall(char* class_name, char* method_name, Hash* parameters, char* client_name, ExceptionSink* xsink)
{
  try {
    MOperationRequest req(this, class_name, method_name, client_name);
    setRequestParamaters(req, parameters, xsink);

    if (xsink->isException()) {
      return;
    }

    req.onewayInvoke();

  } catch (MOperationException& moe) {
    xsink->raiseException("TIBCO-REMOTE-EXCEPTION", "%s", moe.getRemoteExceptionName().c_str());
  } catch (MException& e) {
    xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", e.getType().c_str(), e.getDescription().c_str());
  }
}

//------------------------------------------------------------------------------
// The state for the async calls is stored here and protected by the mutex.
//
typedef struct pending_call_key {
  std::string m_class_name;
  std::string m_method_name;
  MApp* m_app;

  pending_call_key(char* class_name, char* method_name, MApp* app)
  : m_class_name(class_name), m_method_name(method_name), m_app(app) {}
};

inline bool operator<(const pending_call_key& lhs, const pending_call_key& rhs) {
  if (lhs.m_app < rhs.m_app) return true;
  if (lhs.m_method_name < rhs.m_method_name) return true;
  if (lhs.m_class_name < rhs.m_class_name) return true;
  return false;
}


// global map with not yet consumed (pending) async calls
typedef struct async_call_context_t
{
  async_call_context_t(MOperationRequest* req_, OperationsListener* listener_, MDispatcher* dispatcher_)
  : req(req_), listener(listener_), dispatcher(dispatcher_) {}
  async_call_context_t() : req(0), listener(0), dispatcher(0) {}

  bool isEmpty() const { return !req && !listener && !dispatcher; }
  void destroy() {
    if (dispatcher) dispatcher->stop();
    delete listener; listener = 0;
    delete req; req = 0;
    delete dispatcher; dispatcher = 0;
  }

  MOperationRequest* req; // owned
  OperationsListener* listener; // owned
  MDispatcher* dispatcher;  
};

typedef std::map<pending_call_key, async_call_context_t > pending_async_calls_t;
static pending_async_calls_t g_pending_async_calls;
static LockedObject g_mutex;


static void add_pending_call(char* class_name, char* method_name, MApp* app, async_call_context_t call_context)
{
  g_mutex.lock();
  ON_BLOCK_EXIT_OBJ(g_mutex, &LockedObject::unlock);
  g_pending_async_calls[pending_call_key(class_name, method_name, app)] = call_context;
}

static async_call_context_t extract_pending_call(char* class_name, char* method_name, MApp* app)
{
  g_mutex.lock();
  ON_BLOCK_EXIT_OBJ(g_mutex, &LockedObject::unlock);
  pending_async_calls_t::iterator it = g_pending_async_calls.find(pending_call_key(class_name, method_name, app));
  if (it == g_pending_async_calls.end()) {
    return async_call_context_t();
  }
  async_call_context_t result = it->second;
  g_pending_async_calls.erase(it);
  return result;
}

// called when the adaptor is destroyed
static void remove_pending_calls(MApp* app)
{
  g_mutex.lock();
  ON_BLOCK_EXIT_OBJ(g_mutex, &LockedObject::unlock);
  for (pending_async_calls_t::iterator it = g_pending_async_calls.begin(), end = g_pending_async_calls.end(); it != end;) {
    if (it->first.m_app == app) {
      pending_async_calls_t::iterator next = it;
      ++next;
      it->second.destroy();
      g_pending_async_calls.erase(it);
      it = next;
    } else {
      ++it;
    }
  }
}

//------------------------------------------------------------------------------
void QoreApp::operationsAsyncCall(char* class_name, char* method_name, Hash* parameters, unsigned timeout, char* client_name, ExceptionSink* xsink)
{
  try {
    std::auto_ptr<MOperationRequest> req(new MOperationRequest(this, class_name, method_name, client_name));
    setRequestParamaters(*req, parameters, xsink);

    if (xsink->isException()) {
      return;
    }

    std::auto_ptr<OperationsListener> listener(new OperationsListener(this));
    req->asyncInvoke(listener.get());
    std::auto_ptr<MDispatcher> dispatcher(new MDispatcher(this));

    async_call_context_t ctx(req.release(), listener.release(), dispatcher.release());
    add_pending_call(class_name, method_name, this, ctx);

  } catch (MOperationException& moe) {
    xsink->raiseException("TIBCO-REMOTE-EXCEPTION", "%s", moe.getRemoteExceptionName().c_str());
  } catch (MException& e) {
    xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", e.getType().c_str(), e.getDescription().c_str());
  }
}

//------------------------------------------------------------------------------
QoreNode* QoreApp::operationsGetAsyncCallResult(char* class_name, char* method_name, ExceptionSink* xsink)
{
  async_call_context_t context = extract_pending_call(class_name, method_name, this);
  if (context.isEmpty()) {
    xsink->raiseException("TIBCO-GET-ASYNC-CALL-RESULT", "No pending call for %s:%s found", class_name, method_name);
    return 0;
  }
  ON_BLOCK_EXIT_OBJ(context, &async_call_context_t::destroy);

  // UNFINISHED
  // This fails: the get_data() is called but listener's onReply() was not called
  // and the returned data are empty.
  // One part of the documentation suggest that it is client code (here) that should call
  // the onReply() but doing so Tibco exception is thrown.
  // The (probable) solution is to create worker threads that would retrieve the
  // reply data and asynchronously call the onReply(). MDispatcher is suggested 
  // as the 'thread provider'.
  //
  // The only example using MDispatcher is mt_rpc (in tra/5.3/examples/cpp).
  return context.listener->get_data(xsink); // <<== now always returns empty data
}

