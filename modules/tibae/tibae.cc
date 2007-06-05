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

   Hash* h = new Hash;
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
      h->derefAndDelete(xsink);
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
      h->derefAndDelete(xsink);
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
      return new QoreNode((int64)mi->getMi8());
   else if ((msd = MStringData::downCast(md)))
   {
      MString ms = msd->getAsString();
      return new QoreNode((char *)ms.c_str());
   }
   else if ((mr = MReal::downCast(md)))
      return new QoreNode(mr->getAsDouble());
   else if ((mdt = MDateTime::downCast(md)))
      return new QoreNode(new DateTime(mdt->getYear(), mdt->getMonth(), mdt->getDay(), mdt->getHour(), mdt->getMinute(), mdt->getSecond(), mdt->getMicroSeconds() / 1000));
   else if ((mdate = MDate::downCast(md)))
      return new QoreNode(new DateTime(mdate->getYear(), mdate->getMonth(), mdate->getDay()));
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

void set_properties(MAppProperties *appProperties, Hash *h, TibCommandLine &tcl, ExceptionSink *xsink)
{
   tracein("set_properties()");

   // variable hash for overridding global variables
   class Hash *vh = NULL;

   HashIterator hi(h);
   while (hi.next())
   {
      const char *key = hi.getKey();
      if (!hi.getValue())
      {
	 xsink->raiseException("TIBCO-INVALID-PROPERTIES-HASH", 
			"properties hash key \"%s\" has value = NOTHING",
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
			"properties hash has invalid type \"%s\" for key \"%s\" (must be string)",
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
