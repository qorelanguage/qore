/*
  TIBCO/tibae.cc

  TIBCO Active Enterprise integration to QORE

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

#include "tibae.h"
#include "TibCommandLine.h"
#include "QoreApp.h"

#include <memory>

static inline class AbstractQoreNode *map_minstance_to_node(const MInstance *min, ExceptionSink *xsink)
{
   std::auto_ptr<MEnumerator<MString, MData *> >me(min->newEnumerator());   
   MString name;
   MData *val;

   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);
   while (me->next(name, val)) {
      h->setKeyValue((char *)name.c_str(), map_mdata_to_node(val, xsink), xsink);
      if (*xsink)
	 return 0;
   }

   return h.release();
}

// maps a TIBCO sequence to a QORE list
static inline class AbstractQoreNode *map_msequence_to_node(const MSequence *ms, ExceptionSink *xsink)
{
   ReferenceHolder<QoreListNode> rv(new QoreListNode(), xsink);

   for (unsigned i = 0; i < ms->size() && !xsink->isEvent(); i++)
      rv->push(map_mdata_to_node((MData *)(*ms)[i], xsink));

   if (xsink->isEvent())
      return 0;

   return rv.release();
}

typedef MEnumerator<MString, MData *> ma_enumerator_t;

// maps a TIBCO associative list to a QORE hash
static inline class AbstractQoreNode *map_massoclist_to_node(const MAssocList *mal, ExceptionSink *xsink)
{
   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);

   std::auto_ptr<ma_enumerator_t> me(mal->newEnumerator());
   MString name;
   MData *val;

   while (me->next(name, val)) {
      h->setKeyValue((char *)name.c_str(), map_mdata_to_node(val, xsink), xsink);
      if (*xsink)
	 return 0;
   }

   return h.release();
}

// maps a TIBCO union to a QORE hash
static inline class AbstractQoreNode *map_munion_to_node(const MUnion *mu, ExceptionSink *xsink)
{
   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);

   MString MSkey = mu->getMemberName();
   h->setKeyValue((char *)MSkey.c_str(), map_mdata_to_node((MData *)mu->get(MSkey), xsink), xsink);
   
   if (*xsink)
      return 0;

   return h.release();
}

// maps a TIBCO MTree to a QORE node
class AbstractQoreNode *map_mdata_to_node(MData *md, ExceptionSink *xsink)
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
      return new QoreBigIntNode(mi->getMi8());
   
   const MStringData *msd;
   if ((msd = MStringData::downCast(md)))
   {
      MString ms = msd->getAsString();
      return new QoreStringNode((char *)ms.c_str());
   }
   
   const MReal *mr;
   if ((mr = MReal::downCast(md)))
      return new QoreFloatNode(mr->getAsDouble());
   
   const MDateTime *mdt;
   if ((mdt = MDateTime::downCast(md)))
      return new DateTimeNode(mdt->getYear(), mdt->getMonth(), mdt->getDay(), mdt->getHour(), mdt->getMinute(), mdt->getSecond(), mdt->getMicroSeconds() / 1000);
   
   const MDate *mdate;
   if ((mdate = MDate::downCast(md)))
      return new DateTimeNode(mdate->getYear(), mdate->getMonth(), mdate->getDay());
   
   const MBool *mb;
   if ((mb = MBool::downCast(md)))
      return get_bool_node((bool)mb->getAsBoolean());

   const MBinary *mbin;
   if ((mbin = MBinary::downCast(md)))
   {
      BinaryNode *b = new BinaryNode();
      b->append(mbin->getData(), mbin->size());
      return b;
   }

   const MInterval *mint;
   if ((mint = MInterval::downCast(md)))
      return new DateTimeNode(0, 0, 0, 0, 0, mint->getSeconds(), mint->getMicroSeconds() / 1000, true);

   const MTime *mtime;
   if ((mtime = MTime::downCast(md)))
      return new DateTimeNode(0, 0, 0, mtime->getHour(), mtime->getMinute(), mtime->getSecond(), mtime->getMicroSeconds() / 1000);

   xsink->raiseException("MAP-ERROR", "can't map MData element of class '%s' to QORE type", md->getClassName());
   return NULL;
}

void set_properties(MAppProperties *appProperties, const QoreHashNode *h, TibCommandLine &tcl, ExceptionSink *xsink)
{
   QORE_TRACE("set_properties()");

   // variable hash for overridding global variables
   const QoreHashNode *vh = NULL;

   ConstHashIterator hi(h);
   while (hi.next())
   {
      const char *key = hi.getKey();
      if (!hi.getValue())
      {
	 xsink->raiseException("TIBCO-INVALID-PROPERTIES-HASH", "properties hash key '%s' has value = NOTHING",	key);
	 return;
      }

      if (!strcmp(key, "Vars"))
      {
	 vh = dynamic_cast<const QoreHashNode *>(hi.getValue());
	 if (vh)
	    continue;
      }

      const AbstractQoreNode *v = hi.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(v);
      if (!str)
      {
	 xsink->raiseException("TIBCO-INVALID-PROPERTIES-HASH",
			       "properties hash has invalid type '%s' for key '%s' (must be string)",
			       v ? v->getTypeName() : "NOTHING", key);
	 return;
      }
      const char *val = str->getBuffer();

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
      ConstHashIterator vhi(vh);
      while (vhi.next())
      {
	 const char *key = vhi.getKey();
	 if (!key || !key[0])
	    continue;

	 const AbstractQoreNode *n = vhi.getValue();
	 const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
	 if (!str || !str->strlen())
	    continue;
	 //printd(5, "setting override for global variable %s=%s\n", key, str->getBuffer());
	 tcl.add(key, str->getBuffer());
      }
   }

   if (tcl.argc)
      appProperties->setCommandLine(tcl.argc, tcl.argv);

   appProperties->setMultiThreaded(); 
   //appProperties->setDefaultStringEncoding(MEncoding::M_ASCII);
   //appProperties->setDefaultStringEncoding(MEncoding::M_LATIN_1);


}
