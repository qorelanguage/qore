/*
  TIBCO/QoreApp.h

  TIBCO integration to QORE

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

#ifndef _QORE_TIBCO_QOREAPP_H

#define _QORE_TIBCO_QOREAPP_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/QoreType.h>
#include <qore/QoreString.h>
#include <qore/config.h>
#include <qore/ReferenceObject.h>
#include <qore/LockedObject.h>
#include <qore/Hash.h>

#include "tibae.h"

#include <Maverick.h>
#include <time.h>

#ifdef TIBCO_MDT_BUG
extern class LockedObject l_mdate_time;
#endif

class QoreApp : public MApp, public ReferenceObject
{
   private:
      char *session_name;
      char *service;
      char *network;
      char *daemon;
      char *subject;

      class MAppProperties *appProps;

      // for receive
      char *rcv_subject;
      class LockedObject rcv_lock;

      class Hash *classlist;

      class MPublisher *myPublisher;
      class MRvSession *mySession;
      class MSubscriber *mySubscriber;
      class QoreEventHandler *myEventHandler;
      class MClassRegistry *mcr;

      class MTree *make_MTree(char *class_name, QoreNode *value, ExceptionSink *xsink);
      class MData *instantiate_class(QoreNode *v, const MBaseClassDescription *mbcd, ExceptionSink *xsink);
      class MData *instantiate_sequence(const MSequenceClassDescription *msd, QoreNode *v, ExceptionSink *xsink);
      class MData *instantiate_modeledclass(const MModeledClassDescription *mcd, QoreNode *v, ExceptionSink *xsink);
      class MData *instantiate_union(const MUnionDescription *mud, QoreNode *v, ExceptionSink *xsink);
      class MData *do_primitive_type(const MPrimitiveClassDescription *pcd, QoreNode *v, ExceptionSink *xsink);
      const MBaseClassDescription *find_class(char *cn, ExceptionSink *xsink);
      int refs;

   protected:
      virtual void onInitialization() throw (MException);
      virtual void onTermination() throw (MException) {}

   public:
      inline QoreApp(MAppProperties *pMAP, char *name, class Hash *clh, 
		     char *svc = NULL, char *net = NULL, 
		     char *dmn = NULL, char *sbj = NULL);
      inline virtual ~QoreApp();
      class QoreNode *sendWithSyncReply(char *function_name, QoreNode *value, int timeout, ExceptionSink *xsink);
      class QoreNode *sendWithSyncReply(char *subject, char *function_name, QoreNode *value, int timeout, ExceptionSink *xsink);
      void send(char *function_name, QoreNode *value, ExceptionSink *xsink);
      void send(char *subject, char *function_name, QoreNode *value, ExceptionSink *xsink);
      class QoreNode *receive(char *subject, unsigned long timeout, ExceptionSink *xsink);
      void set_subject_name(char *sub)
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
      const char *get_subject() { return (const char *)subject; }
      class QoreNode *map_mtree_to_node(MTree *msg, ExceptionSink *xsink);
      inline void deref(class ExceptionSink *xsink);
};

class QoreEventHandler : public MEventListener
{
   private:
      QoreApp *myQoreApp; 
      int count;

      virtual void onEvent(const MEvent &refEvent)
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
      
   public:
      QoreNode *msgNode;
      MString replySubject;
      MString subject;
      ExceptionSink xsink;
      
      inline QoreEventHandler(QoreApp *pMApp) : MEventListener()
      {
	 myQoreApp = pMApp;
	 count = 0;
	 msgNode = NULL;
      }
      
      virtual ~QoreEventHandler()
      { 
	 printd(5,"QoreEventHandler::~QoreEventHandler() destructor called\n"); 
      }
      
}; // QoreEventHandler 

inline QoreApp::QoreApp(MAppProperties *pMAP, char *name, Hash *clh, 
			char *svc, char *net, char *dmn, char *sbj) : MApp(pMAP)
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

inline QoreApp::~QoreApp()
{
   tracein("QoreApp::~QoreApp()");
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

inline const MBaseClassDescription *QoreApp::find_class(char *cn, ExceptionSink *xsink)
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

static inline char *get_class(Hash *h)
{
   QoreNode *t;

   if (!(t = h->getKeyValue("^class^")))
      return NULL;
   if (t->type != NT_STRING)
      return NULL;
   return t->val.String->getBuffer();
}

// FIXME: currently no comparison is done between the expected type and the provided type
inline MData *QoreApp::do_primitive_type(const MPrimitiveClassDescription *pcd, QoreNode *v, ExceptionSink *xsink)
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
	 xsink->raiseException("TIBCO-MISSING-CLASS-NAME", "instantiating type \"%s\": can't instantiate class from object without \"^class^\" entry", pcd->getFullName().c_str());
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
	    xsink->raiseException("TIBCO-MISSING-VALUE", "instantiating type \"%s\": no \"^value^\" entry found in hash for class \"%s\"", pcd->getFullName().c_str(), cn);
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

   xsink->raiseException("DEBUG:TIBCO-UNSUPPORTED-TYPE", "unsupported QORE type \"%s\" (TIBCO type \"%s\")", 
		  v->type->name, pcd->getShortName().c_str());

   //traceout("QoreApp::do_primitive_type()");
   return NULL;
}

inline MData *QoreApp::instantiate_sequence(const MSequenceClassDescription *msd, QoreNode *v, ExceptionSink *xsink)
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
			 msd->getFullName().c_str(), v->type->name);
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

inline MData *QoreApp::instantiate_modeledclass(const MModeledClassDescription *mcd, QoreNode *v, ExceptionSink *xsink)
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
			 mcd->getFullName().c_str(), v->type->name);
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
		mcd->getFullName().c_str(), key, t, t ? t->type->name : "(null)");
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

inline MData *QoreApp::instantiate_union(const MUnionDescription *mud, QoreNode *v, ExceptionSink *xsink)
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
			 mud->getFullName().c_str(), v->type->name);
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
	     mud->getFullName().c_str(), key, t, t ? t->type->name : "(null)");
      MData *md;
      mu->set(key, md = instantiate_class(t, mmd->getMemberClassDescription(), xsink));
      if (md)
	 delete md;
   }
   
   traceout("QoreApp::instantiate_union()");
   return mu;
}

inline MTree *QoreApp::make_MTree(char *class_name, QoreNode *value, ExceptionSink *xsink)
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
   //else 
   if (md)
      delete md;

   traceout("QoreApp::make_MTree");
   return mt;
}

// maps a TIBCO MTree to a QORE node
inline class QoreNode *QoreApp::map_mtree_to_node(MTree *msg, ExceptionSink *xsink)
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

inline void QoreApp::deref(ExceptionSink *xsink)
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
   //traceout("QoreApp::deref()");
}

#endif // _QORE_TIBCO_QOREAPP_H
