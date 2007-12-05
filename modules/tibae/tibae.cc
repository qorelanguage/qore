/*
  TIBCO/tibae.cc

  TIBCO Active Enterprise integration to QORE

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

#include "tibae.h"
#include "TibCommandLine.h"
#include "QoreApp.h"

#include <memory>

static inline class QoreNode *map_minstance_to_node(const MInstance *min, ExceptionSink *xsink)
{
   std::auto_ptr<MEnumerator<MString, MData *> >me(min->newEnumerator());   
   MString name;
   MData *val;

   QoreHash* h = new QoreHash;
   while (me->next(name, val) && !xsink->isEvent())
      h->setKeyValue((char *)name.c_str(), map_mdata_to_node(val, xsink), xsink);

   if (xsink->isEvent())
   {
      h->derefAndDelete(xsink);
      return NULL;
   }
   return new QoreNode(h);
}

// maps a TIBCO sequence to a QORE list
static inline class QoreNode *map_msequence_to_node(const MSequence *ms, ExceptionSink *xsink)
{
   class QoreNode *rv = new QoreNode(NT_LIST);
   rv->val.list = new QoreList();

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
   QoreHash *h = new QoreHash();

   MEnumerator<MString, MData *> *me = mal->newEnumerator();
   MString name;
   MData *val;

   while (me->next(name, val) && !xsink->isEvent())
      h->setKeyValue((char *)name.c_str(), map_mdata_to_node(val, xsink), xsink);

   delete me;
   if (xsink->isEvent())
   {
      h->derefAndDelete(xsink);
      return NULL;
   }
   return new QoreNode(h);
}

// maps a TIBCO union to a QORE hash
static inline class QoreNode *map_munion_to_node(const MUnion *mu, ExceptionSink *xsink)
{
   QoreHash *h = new QoreHash();

   MString MSkey = mu->getMemberName();
   h->setKeyValue((char *)MSkey.c_str(), map_mdata_to_node((MData *)mu->get(MSkey), xsink), xsink);
   
   if (xsink->isEvent())
   {
      h->derefAndDelete(xsink);
      return NULL;
   }
   return new QoreNode(h);
}

// maps a TIBCO MTree to a QORE node
class QoreNode *map_mdata_to_node(MData *md, ExceptionSink *xsink)
{
   const MInstance *min;

   // is it an MInstance?
   if ((min = MInstance::downCast(md)))
      return map_minstance_to_node(min, xsink);

   const MAssocList *mal;
   // is it an associative list?
   if ((mal = MAssocList::downCast(md)))
      return map_massoclist_to_node(mal, xsink);

   const MSequence *ms;
   // or is it a sequence?
   if ((ms = MSequence::downCast(md)))
      return map_msequence_to_node(ms, xsink);

   const MUnion *mu;
   // or is it a union?
   if ((mu = MUnion::downCast(md)))
      return map_munion_to_node(mu, xsink);
   
   const MInteger *mi;
   if ((mi = MInteger::downCast(md)))
      return new QoreNode((int64)mi->getMi8());
   
   const MStringData *msd;
   if ((msd = MStringData::downCast(md)))
   {
      MString ms = msd->getAsString();
      return new QoreNode((char *)ms.c_str());
   }
   
   const MReal *mr;
   if ((mr = MReal::downCast(md)))
      return new QoreNode(mr->getAsDouble());
   
   const MDateTime *mdt;
   if ((mdt = MDateTime::downCast(md)))
      return new QoreNode(new DateTime(mdt->getYear(), mdt->getMonth(), mdt->getDay(), mdt->getHour(), mdt->getMinute(), mdt->getSecond(), mdt->getMicroSeconds() / 1000));
   
   const MDate *mdate;
   if ((mdate = MDate::downCast(md)))
      return new QoreNode(new DateTime(mdate->getYear(), mdate->getMonth(), mdate->getDay()));
   
   const MBool *mb;
   if ((mb = MBool::downCast(md)))
      return new QoreNode((bool)mb->getAsBoolean());

   const MBinary *mbin;
   if ((mbin = MBinary::downCast(md)))
   {
      BinaryObject *b = new BinaryObject();
      b->append(mbin->getData(), mbin->size());
      return new QoreNode(b);
   }

   const MInterval *mint;
   if ((mint = MInterval::downCast(md)))
   {
      class DateTime *d = new DateTime(0, 0, 0, 0, 0, mint->getSeconds(), mint->getMicroSeconds() / 1000, true);
      return new QoreNode(d);
   }

   const MTime *mtime;
   if ((mtime = MTime::downCast(md)))
   {
      class DateTime *d = new DateTime(0, 0, 0, mtime->getHour(), mtime->getMinute(), mtime->getSecond(), mtime->getMicroSeconds() / 1000);
      return new QoreNode(d);
   }

   xsink->raiseException("MAP-ERROR", "can't map MData element of class '%s' to QORE type", md->getClassName());
   return NULL;
}

void set_properties(MAppProperties *appProperties, QoreHash *h, TibCommandLine &tcl, ExceptionSink *xsink)
{
   tracein("set_properties()");

   // variable hash for overridding global variables
   class QoreHash *vh = NULL;

   HashIterator hi(h);
   while (hi.next())
   {
      const char *key = hi.getKey();
      if (!hi.getValue())
      {
	 xsink->raiseException("TIBCO-INVALID-PROPERTIES-HASH", 
			"properties hash key '%s' has value = NOTHING",
			key);
	 return;
      }

      if (!strcmp(key, "Vars") && hi.getValue() && hi.getValue()->type == NT_HASH)
      {
	 vh = hi.getValue()->val.hash;
	 continue;
      }

      if (hi.getValue()->type != NT_STRING)
      {
	 xsink->raiseException("TIBCO-INVALID-PROPERTIES-HASH",
			"properties hash has invalid type '%s' for key '%s' (must be string)",
			hi.getValue()->type->getName(), key);
	 return;
      }
      const char *val = hi.getValue()->val.String->getBuffer();

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

   if (vh)
   {
      HashIterator vhi(vh);
      while (vhi.next())
      {
	 const char *key = vhi.getKey();
	 if (!key || !key[0])
	    continue;

	 class QoreNode *n = vhi.getValue();
	 if (!n || n->type != NT_STRING || !n->val.String->strlen())
	    continue;
	 //printd(5, "setting override for global variable %s=%s\n", key, n->val.String->getBuffer());
	 tcl.add(key, n->val.String->getBuffer());
      }
   }

   if (tcl.argc)
      appProperties->setCommandLine(tcl.argc, tcl.argv);

   appProperties->setMultiThreaded(); 
   //appProperties->setDefaultStringEncoding(MEncoding::M_ASCII);
   //appProperties->setDefaultStringEncoding(MEncoding::M_LATIN_1);

   traceout("set_properties()");
}
