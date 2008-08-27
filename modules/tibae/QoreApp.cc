/*
  QoreApp.cc

  TIBCO integration to QORE

  Copyright 2003 - 2008 David Nichols

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

#include "QoreApp.h"

#include <Maverick.h>

#include <memory>
#include <string>
#include <map>
#include <utility>

#include <assert.h>
#include <unistd.h>


static void remove_pending_calls(MApp* app);

static const char *get_class(const QoreHashNode *h)
{
   const AbstractQoreNode *t;

   if (!(t = h->getKeyValue("^class^")))
      return NULL;
   
   const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(t);
   if (!str)
      return NULL;
   return str->getBuffer();
}

QoreApp::QoreApp(MAppProperties *pMAP, const char *name, QoreHashNode *clh,
		 const char *svc, const char *net, const char *dmn, const char *sbj) : MApp(pMAP)
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
   printd(5, "QoreApp::QoreApp() this=%08p session=%s\n", this, session_name);
}

QoreApp::~QoreApp()
{
   QORE_TRACE("QoreApp::~QoreApp()");
   printd(5, "QoreApp::~QoreApp() this=%08p session=%s\n", this, session_name);

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
      classlist->deref(NULL);
   
   delete appProps;
   

}

void QoreApp::deref(ExceptionSink *xsink)
{
   //printd(5, "QoreApp::deref() this=%08p session=%s, refs=%d -> %d\n", this, session_name, reference_count(), reference_count() - 1);
   //QORE_TRACE("QoreApp::deref()");
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

const MBaseClassDescription *QoreApp::find_class(const char *cn, ExceptionSink *xsink)
{
   const MBaseClassDescription *mbcd = NULL;
   AbstractQoreNode *t = NULL;

   const char *cdesc;
   if (classlist && (t = classlist->getKeyValue(cn)) && t->getType() == NT_STRING)
      cdesc = (reinterpret_cast<QoreStringNode *>(t))->getBuffer();
   else
      cdesc = cn;

   if (!(mbcd = mcr->getClassDescription(cdesc)))
   {
      xsink->raiseException("TIBCO-CLASS-NOT-FOUND", "class name '%s' with description '%s' cannot be found in repository", cn, cdesc);
      return NULL;
   }
   return mbcd;
}

DLLLOCAL MDateTime *get_mdatetime(const DateTime *d)
{
   // we have to use a string here in case the date is too large for a time_t value
   QoreString str;
   str.sprintf("%04d-%02d-%02dT%02d:%02d:%02d.%03d", d->getYear(), d->getMonth(), d->getDay(), 
	       d->getHour(), d->getMinute(), d->getSecond(), d->getMillisecond());
   return new MDateTime(str.getBuffer());

#if 0
   // we can't use the DateTime::getEpochSeconds() because that gives seconds in GMT
   // and we need local time!
   MDateTimeStruct mdts;
   mdts.setTime_t(d.getEpochSeconds());
   mdts.setMicroSeconds(d.getMillisecond() * 1000);
   return new MDateTime(mdts);
#endif
}

class MData *QoreApp::do_type(int type_code, const AbstractQoreNode *v, ExceptionSink *xsink)
{
   switch (type_code)
   {
      case TIBAE_BOOLEAN:
	 return new MBool(v ? v->getAsBool() : false);

      case TIBAE_I1:
	 return new MInteger((char)(v ? v->getAsInt() : 0));

      case TIBAE_I2:
	 return new MInteger((short)(v ? v->getAsInt() : 0));

      case TIBAE_I4:
	 return new MInteger((int)(v ? v->getAsInt() : 0));

      case TIBAE_I8:
	 return new MInteger(v ? v->getAsBigInt() : 0ll);

      case TIBAE_U1:
	 return new MInteger((unsigned char)(v ? v->getAsInt() : 0));

      case TIBAE_U2:
	 return new MInteger((unsigned short)(v ? v->getAsInt() : 0));

      case TIBAE_U4:
	 return new MInteger((unsigned int)(v ? v->getAsInt() : 0));

      case TIBAE_U8:
	 return new MInteger((unsigned long long)(v ? v->getAsBigInt() : 0ll));

      case TIBAE_R4:
	 return new MReal((float)(v ? v->getAsFloat() : 0.0));

      case TIBAE_R8:
	 return new MReal(v ? v->getAsFloat() : 0.0);

      case TIBAE_DATETIME:
      {
	 DateTimeValueHelper d(v);

	 return get_mdatetime(*d);
      }

      case TIBAE_DATE:
      {
	 DateTimeValueHelper d(v);
         return new MDate(d->getYear(), d->getMonth(), d->getDay());
      }

      case TIBAE_STRING:
      {
	 QoreStringValueHelper t(v, QCS_UTF8, xsink);
	 if (*xsink)
	    return 0;

	 return new MStringData(t->getBuffer(), MEncoding::M_ASCII);
      }

      case TIBAE_BINARY:
      {
	 const BinaryNode *b = dynamic_cast<const BinaryNode *>(v);
	 if (!b)
	 {
	    xsink->raiseException("TIBCO-TYPE-ERROR", "expecting binary object to serialize as TIBCO_BINARY, got '%s'", 
				  v ? v->getTypeName() : "NOTHING");
	    return 0;
	 }


	 return new MBinary(b->getPtr(), b->size());
      }

      case TIBAE_INTERVAL:
      {
	 DateTimeValueHelper d(v);
	 int64 ms = d->getRelativeMilliseconds();
	 float seconds = (float)ms / (float)1000.0; 

	 try {
	    return new MInterval(0, 0, 0, seconds);
	 }
	 catch (MException &tib_e)
	 {
	    xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", tib_e.getType().c_str(), tib_e.getDescription().c_str());
	    return 0;
	 }
      }

      case TIBAE_TIME:
      {
	 DateTimeValueHelper d(v);
	 float seconds = (float)d->getSecond() + ((float)d->getMillisecond()) / (float)1000.0; 

	 try {
	    return new MTime(d->getHour(), d->getMinute(), seconds);
	 }
	 catch (MException &tib_e)
	 {
	    xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", tib_e.getType().c_str(), tib_e.getDescription().c_str());
	    return 0;
	 }
      }

      case TIBAE_BYTE:
      case TIBAE_CHAR:
      case TIBAE_FIXED:

      default:
	 xsink->raiseException("TIBCO-TYPE-ERROR", "Do not know how to serialize to type code %d", type_code);
   }

   return 0;
}

// FIXME: currently no comparison is done between the expected type and the provided type
MData *QoreApp::do_primitive_type(const MPrimitiveClassDescription *pcd, const AbstractQoreNode *v, ExceptionSink *xsink)
{
   //QORE_TRACE("QoreApp::do_primitive_type()");

   if (!v)
      return NULL;

   qore_type_t ntype = v->getType();

   if (ntype == NT_HASH) {
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(v);
      // check to see if type is specified
      const AbstractQoreNode *t = h->getKeyValue("^type^");
      if (!is_nothing(t))
	 return do_type(t->getAsInt(), h->getKeyValue("^value^"), xsink);
      
      // class instantiation (normally for TIBCO m_any type)
      const char *cn;
      if (!(cn = get_class(h)))
      {
	 xsink->raiseException("TIBCO-MISSING-CLASS-NAME", "instantiating type '%s': can't instantiate class from object without '^class^' key", pcd->getFullName().c_str());
	 return NULL;
      }
      
      const MBaseClassDescription *mbcd = find_class(cn, xsink);
      if (xsink->isEvent())
	 return NULL;
      const AbstractQoreNode *val;
      if (!(val = h->getKeyValue("^value^")))
      {
	 xsink->raiseException("TIBCO-MISSING-VALUE", "instantiating type '%s': no '^value^' entry found in hash for class '%s'", pcd->getFullName().c_str(), cn);
	 return NULL;
      }
      return instantiate_class(val, mbcd, xsink);
   }

/*
   if (ntype == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(v);
      // class instantiation (normally for TIBCO m_any type)
      const char *cn;
      if (!(cn = get_class(o->data)))
      {
	 xsink->raiseException("TIBCO-MISSING-CLASS-NAME", "instantiating type '%s': can't instantiate class from object wi
thout '^class^' entry", pcd->getFullName().c_str());
	 return NULL;
      }
      else
      {
	 const MBaseClassDescription *mbcd = find_class(cn, xsink);
	 if (xsink->isEvent())
	    return NULL;
	 AbstractQoreNode *val;
	 if (!(val = o->retrieve_value("^value^")))
	 {
	    xsink->raiseException("TIBCO-MISSING-VALUE", "instantiating type '%s': no '^value^' entry found in hash for c
lass '%s'", pcd->getFullName().c_str(), cn);
	    return NULL;
	 }
	 return instantiate_class(val, mbcd, xsink);
      }
   }
*/

   if (ntype == NT_BOOLEAN) {
      return new MBool(reinterpret_cast<const QoreBoolNode *>(v)->getValue());
   }

   if (ntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(v);
      printd(3, "data=%08p val='%s'\n", str->getBuffer(), str->getBuffer());
#if (TIBCO_SDK == 4)
      return new MStringData(str->getBuffer());
#else
      // it appears that all MString data must be UTF-8, no matter how we use the MStringData constructor
      // furthermore, it appears that we have to trick the SDK into thinking that the data is ASCII, so
      // no conversions are attempted
      TempEncodingHelper t(str, QCS_UTF8, xsink);
      if (!t)
	 return 0;
      
      //md = new MStringData(t->getBuffer(), MEncoding::M_UTF8);
      return new MStringData(t->getBuffer(), MEncoding::M_ASCII);
#endif
   }

   if (ntype == NT_INT)
   {
      const QoreBigIntNode *b = reinterpret_cast<const QoreBigIntNode *>(v);
      int64 i = b->val;
      // see if it's a 32-bit integer
      if ((int)i == i)
	 return new MInteger((int)i);
      // otherwise return i8
      return new MInteger(b->val);
   }

   if (ntype == NT_FLOAT) {
      return new MReal(reinterpret_cast<const QoreFloatNode *>(v)->f);
   }

   if (ntype == NT_DATE) {
      const DateTimeNode *date = reinterpret_cast<const DateTimeNode *>(v);
      const char *type = pcd->getShortName().c_str();
      if (!strcmp(type, "dateTime") || !strcmp(type, "any"))
	 return get_mdatetime(date);
      else if (!strcmp(type, "date"))
	 return new MDate(date->getYear(), date->getMonth(), date->getDay());
	 
      xsink->raiseException("TIBCO-DATE-INSTANTIATION-ERROR", "cannot map from QORE type 'date' to TIBCO type '%s'",
			    pcd->getShortName().c_str());
      return 0;
   }

   if (ntype == NT_BINARY) {
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(v);
      return new MBinary(b->getPtr(), b->size());
   }

   if (ntype == NT_NOTHING || ntype == NT_NULL)
      return NULL;

   xsink->raiseException("TIBCO-UNSUPPORTED-TYPE", "unsupported QORE type '%s' (TIBCO type '%s')", v->getTypeName(), pcd->getShortName().c_str());


   return 0;
}

MTree *QoreApp::make_MTree(const char *class_name, const AbstractQoreNode *value, ExceptionSink *xsink)
{
   QORE_TRACE("QoreApp::make_MTree");

   const MBaseClassDescription *mbcd;

   mbcd = find_class(class_name, xsink);
   if (xsink->isEvent())
      return NULL;
   if (!mbcd)
   {
      xsink->raiseException("TIBCO-CLASS-DESCRIPTION-NOT-FOUND", "cannot find TIBCO class description for class name '%s'", class_name);
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


   return mt;
}

// maps a TIBCO MTree to a QORE node
class AbstractQoreNode *QoreApp::map_mtree_to_node(MTree *msg, ExceptionSink *xsink)
{
   class AbstractQoreNode *rv;

   QORE_TRACE("QoreApp::map_mtree_to_node()");

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


   return rv;
}

MData *QoreApp::instantiate_sequence(const MSequenceClassDescription *msd, const AbstractQoreNode *v, ExceptionSink *xsink)
{
   // ensure value is a QORE list
   // check if class info embedded
   if (v && v->getType() == NT_HASH)
   {
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(v);
      const char *cn;
      if (!(cn = get_class(h)))
      {
         xsink->raiseException("TIBCO-MISSING-CLASS-NAME",
			       "can't instantiate sequence of type '%s' from hash without '^class^' entry",
			       msd->getFullName().c_str());

         return 0;
      }
      printd(1, "QoreApp::instantiate_sequence() '%s': ignoring class information provided (%s)\n", msd->getFullName().c_str(), cn);
      v = h->getKeyValue("^value^");
   }

   if (is_nothing(v))
      return new MSequence(mcr, msd->getFullName());

   const QoreListNode *l = dynamic_cast<const QoreListNode *>(v);

   if (!l)
   {
      xsink->raiseException("TIBCO-INVALID-TYPE-FOR-SEQUENCE", "cannot instantiate TIBCO sequence '%s' from node type '%s'",
			    msd->getFullName().c_str(), v->getTypeName());
      return 0;
   }

   MSequence *seq = new MSequence(mcr, msd->getFullName());
   for (int i = 0; i < l->size(); i++)
   {
      const AbstractQoreNode *ne = l->retrieve_entry(i);
      const MBaseClassDescription *mbcd = msd->getContainedClassDescription();
      printd(5, "instantiate_class() implicitly instantiating %s\n", mbcd->getFullName().c_str());
      MData *md;
      seq->append(md = instantiate_class(ne, mbcd, xsink));
      if (md)
	 delete md;
   }
   return seq;
}

MData *QoreApp::instantiate_modeledclass(const MModeledClassDescription *mcd, const AbstractQoreNode *v, ExceptionSink *xsink)
{
   QORE_TRACE("QoreApp::instantiate_modeledclass()");
   if (is_nothing(v))
   {

      return new MInstance(mcr, mcd->getFullName());
   }
   const QoreHashNode *h;
   if (v->getType() == NT_HASH)
      h = reinterpret_cast<const QoreHashNode *>(v);
   else
   {
      xsink->raiseException("TIBCO-INVALID-TYPE-FOR-CLASS",
                         "cannot instantiate class '%s' from node type '%s'",
                         mcd->getFullName().c_str(), v->getTypeName());
      return NULL;
   }

   // make MInstance for class instantiation
   MInstance *ma = new MInstance(mcr, mcd->getFullName());
   //printd(5, "QoreApp::instantiate_modeledclass() %s=%s\n", mcd->getShortName().c_str(), ma->getName().c_str());

   // get list of hash elements for object
   ConstHashIterator hi(h);

   try {
      // instantiate each member
      while (hi.next())
      {
         const char *key = hi.getKey();
         const AbstractQoreNode *t = hi.getValue();

         const MAttributeDescription *mad;
         if (!(mad = mcd->getAttribute(key)))
         {
            xsink->raiseException("TIBCO-HASH-KEY-INVALID",
				  "error instantiating TIBCO class '%s', hash key '%s' is not an attribute of this class",
				  mcd->getFullName().c_str(), key);
            delete ma;

            return NULL;
         }
         printd(5, "QoreApp::instantiate_modeledclass(): instantiating %s member %s (%08p %s)\n",
                mcd->getFullName().c_str(), key, t, t ? t->getTypeName() : "(null)");
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

      throw te;
   }


   return ma;
}

MData *QoreApp::instantiate_union(const MUnionDescription *mud, const AbstractQoreNode *v, ExceptionSink *xsink)
{
   QORE_TRACE("QoreApp::instantiate_union()");

   if (is_nothing(v))
   {

      return new MUnion(mcr, mud->getFullName());
   }
   const QoreHashNode *h;
   if (v->getType() == NT_HASH)
      h = reinterpret_cast<const QoreHashNode *>(v);
   else
   {
      xsink->raiseException("TIBCO-INVALID-TYPE-FOR-UNION",
                         "cannot instantiate TIBCO union '%s' from node type '%s'",
                         mud->getFullName().c_str(), v->getTypeName());
      return NULL;
   }
   // ensure that QoreObject does not have more than one key
   if (h->size() > 1)
   {
      xsink->raiseException("TIBCO-INVALID-MULTIPLE-KEYS-FOR-UNION",
			    "cannot instantiate TIBCO union '%s' from from hash with %d members!",
			    mud->getFullName().c_str(), h->size());
      return NULL;
   }
   MUnion *mu = new MUnion(mcr, mud->getFullName());
   if (h->size())
   {
      ConstHashIterator hi(h);
      // get the first entry
      hi.next();
      const char *key = hi.getKey();
      const AbstractQoreNode *t = hi.getValue();

      const MMemberDescription *mmd;
      if (!(mmd = mud->getMember(key)))
      {
         xsink->raiseException("TIBCO-INVALID-KEY",
                            "error instantiating TIBCO union '%s', hash key '%s' is not a member of this union",
                        mud->getFullName().c_str(), key ? key : "(null)");
         delete mu;
         return NULL;
      }
      printd(3, "QoreApp::instantiate_union(): instantiating %s member %s (%08p %s)\n",
             mud->getFullName().c_str(), key, t, t ? t->getTypeName() : "(null)");
      MData *md;
      mu->set(key, md = instantiate_class(t, mmd->getMemberClassDescription(), xsink));
      if (md)
         delete md;
   }


   return mu;
}

class MData *QoreApp::instantiate_class(const AbstractQoreNode *v, const MBaseClassDescription *mbcd, ExceptionSink *xsink)
{
   QORE_TRACE("QoreApp::instantiate_class()");
   printd(5, "QoreApp::instantiate_class() mbcd=%08p %s: %08p (%s)\n", mbcd, mbcd->getFullName().c_str(), v, v ? v->getTypeName() : "(null)");

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


   return rv;
}

void QoreApp::onInitialization() throw (MException)
{
   QORE_TRACE("QoreApp::onInitialization()");
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

}

class AbstractQoreNode *QoreApp::sendWithSyncReply(const char *function_name, const AbstractQoreNode *value, int timeout, ExceptionSink *xsink)
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
   
   AbstractQoreNode *rv;
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

void QoreApp::set_subject_name(const char *sub)
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
class AbstractQoreNode *QoreApp::sendWithSyncReply(const char *subj, const char *function_name, const AbstractQoreNode *value, int timeout, ExceptionSink *xsink)
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
   
   AbstractQoreNode *rv;

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

void QoreApp::send(const char *function_name, const AbstractQoreNode *value, ExceptionSink *xsink)
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

void QoreApp::send(const char *subj, const char *function_name, const AbstractQoreNode *value, ExceptionSink *xsink)
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

QoreHashNode *QoreApp::receive(const char *subj, unsigned long timeout, ExceptionSink *xsink)
{
   QoreHashNode *rv = NULL;

   QORE_TRACE("QoreApp::receive()");
   try {
      rcv_lock.lock();
      try {
	 if (mySubscriber && strcmp(subj, rcv_subject))
	 {
	    printd(5, "QoreApp::receive() new subject='%s', destroying listener for '%s'\n", subj, rcv_subject);
	    mySubscriber->removeListener(myEventHandler);
	    delete myEventHandler;
	    delete mySubscriber;
	    mySubscriber = NULL;
	    free(rcv_subject);
	 }
	 if (!mySubscriber)
	 {
	    rcv_subject = strdup(subj);
	    printd(5, "QoreApp::receive() creating Subscriber for subject '%s'\n", subj);
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

	    return NULL;
	 }
      }
      else
	 mySession->nextEvent();

      printd(5, "QoreApp::receive() returned from nextEvent()\n");
      if (!myEventHandler->xsink.isEvent())
      {
	 // build return value
	 QoreHashNode *h = new QoreHashNode();
	 
	 printd(5, "QoreApp::receive() msgNode=%08p\n", myEventHandler->msgNode);
	 // assign "msg" member
	 h->setKeyValue("msg", myEventHandler->msgNode, NULL);
	 myEventHandler->msgNode = NULL;

	 printd(5, "QoreApp::receive() subject=%s\n", myEventHandler->subject.c_str());
	 // assign "subject" member
	 h->setKeyValue("subject", new QoreStringNode(myEventHandler->subject.c_str()), NULL);

	 printd(5, "QoreApp::receive() replySubject=%s\n", myEventHandler->replySubject.c_str());
	 // assign "replySubject" member
	 h->setKeyValue("replySubject", new QoreStringNode(myEventHandler->replySubject.c_str()), NULL);
	 rv = h;
      }
      else
	 xsink->assimilate(&myEventHandler->xsink);
   }
   catch(MException &tib_e)
   {
      xsink->raiseException("TIBCO-EXCEPTION", "%s, %s", 
			    tib_e.getType().c_str(), tib_e.getDescription().c_str());
   }

   return rv;
}

void QoreEventHandler::onEvent(const MEvent &refEvent)
{
 QORE_TRACE("QoreEventHandler::onEvent()");
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
void QoreApp::setRequestParameters(MOperationRequest& req, const QoreHashNode *params, ExceptionSink* xsink)
{
   const MOperationDescription *mod = req.getOperationDescription();

   ConstHashIterator hi(params);
   while (hi.next()) {
      const char *key = hi.getKey();
      const AbstractQoreNode *t = hi.getValue();

      const MOperationParameterDescription *mopd = mod->getParameter(key);
      if (!mopd)
      {
	 xsink->raiseException("TIBCO-PARAMETER-DESCRIPTION-NOT-FOUND", "cannot find parameter description for '%s'", key);
	 return;
      }
      const MBaseClassDescription* mbdc = mopd->getMemberClassDescription();
           
      //std::auto_ptr<MData> data(AbstractQoreNode2MData(key, t, xsink));
      std::auto_ptr<MData> data(instantiate_class(t, mbdc, xsink));
      if (xsink->isException()) {
	 return;
      }
      if (!data.get()) {
	 xsink->raiseException("TIBCO-CLASS-DESCRIPTION-NOT-FOUND", "cannot find TIBCO class description for class name '%s'", key);
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
  AbstractQoreNode* m_returned_value;
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
  AbstractQoreNode* get_data(ExceptionSink* xsink) { 
    if (m_xsink.isException()) {
      xsink->assimilate(&m_xsink); // intention: copy the existing member sink to the one passed via parameter
      return 0;
    }
    AbstractQoreNode* res = m_returned_value; 
    m_returned_value = 0; 
    return res; 
  }
};

} // anonymous namespace

//------------------------------------------------------------------------------
AbstractQoreNode* QoreApp::operationsCallWithSyncResult(const char *class_name, const char *method_name, const QoreHashNode *parameters, unsigned timeout, const char *client_name, ExceptionSink* xsink)
{
  try {
    MOperationRequest req(this, class_name, method_name, client_name);
    setRequestParameters(req, parameters, xsink);
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
void QoreApp::operationsOneWayCall(const char *class_name, const char *method_name, const QoreHashNode *parameters, const char *client_name, ExceptionSink* xsink)
{
  try {
    MOperationRequest req(this, class_name, method_name, client_name);
    setRequestParameters(req, parameters, xsink);

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
typedef struct s_pending_call_key {
      std::string m_class_name;
      std::string m_method_name;
      MApp* m_app;
      
      s_pending_call_key(const char *class_name, const char *method_name, MApp* app)
	 : m_class_name(class_name), m_method_name(method_name), m_app(app) 
      {
      }
} pending_call_key;

inline bool operator<(const pending_call_key& lhs, const pending_call_key& rhs) {
  if (lhs.m_app < rhs.m_app) return true;
  if (lhs.m_method_name < rhs.m_method_name) return true;
  if (lhs.m_class_name < rhs.m_class_name) return true;
  return false;
}

// global map with not yet consumed (pending) async calls
typedef struct s_async_call_context
{
  s_async_call_context(MOperationRequest* req_, OperationsListener* listener_, MDispatcher* dispatcher_)
  : req(req_), listener(listener_), dispatcher(dispatcher_) {}
  s_async_call_context() : req(0), listener(0), dispatcher(0) {}

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
} async_call_context_t;

typedef std::map<pending_call_key, async_call_context_t > pending_async_calls_t;
static pending_async_calls_t g_pending_async_calls;
static QoreThreadLock g_mutex;


static void add_pending_call(const char *class_name, const char *method_name, MApp* app, async_call_context_t call_context)
{
  g_mutex.lock();
  ON_BLOCK_EXIT_OBJ(g_mutex, &QoreThreadLock::unlock);
  g_pending_async_calls[pending_call_key(class_name, method_name, app)] = call_context;
}

static async_call_context_t extract_pending_call(const char *class_name, const char *method_name, MApp* app)
{
  g_mutex.lock();
  ON_BLOCK_EXIT_OBJ(g_mutex, &QoreThreadLock::unlock);
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
  ON_BLOCK_EXIT_OBJ(g_mutex, &QoreThreadLock::unlock);
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
void QoreApp::operationsAsyncCall(const char *class_name, const char *method_name, const QoreHashNode *parameters, unsigned timeout, const char *client_name, ExceptionSink* xsink)
{
  try {
    std::auto_ptr<MOperationRequest> req(new MOperationRequest(this, class_name, method_name, client_name));
    setRequestParameters(*req, parameters, xsink);

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
AbstractQoreNode* QoreApp::operationsGetAsyncCallResult(const char *class_name, const char *method_name, ExceptionSink* xsink)
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

