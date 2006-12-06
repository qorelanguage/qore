/*
  QoreApp.cc

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
#include <qore/common.h>
#include <qore/Object.h>
#include <qore/QoreString.h>

#include "QoreApp.h"

#include <unistd.h>

#ifdef TIBCO_MDT_BUG
#include <qore/LockedObject.h>
class LockedObject l_mdate_time;
#endif

QoreApp::~QoreApp()
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

class MData *QoreApp::instantiate_class(QoreNode *v, const MBaseClassDescription *mbcd, ExceptionSink *xsink)
{
   tracein("QoreApp::instantiate_class()");
   printd(5, "QoreApp::instantiate_class() mbcd=%08p %s: %08p (%s)\n", mbcd, mbcd->getFullName().c_str(), v, v ? v->type->name : "(null)");

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
#ifdef DEBUG
      // should never happen
      run_time_error("DEBUG:cannot instantiate class \"%s\"\n", mbcd->getFullName().c_str());
#endif
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
      run_time_error("DEBUG:unknown exception in QoreApp::onInitialization()");
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
