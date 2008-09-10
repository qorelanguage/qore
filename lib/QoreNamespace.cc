/*
  Namespace.cc

  Qore Programming Language

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
#include <qore/intern/ErrnoConstants.h>
#include <qore/intern/TypeConstants.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/CallStack.h>
#include <qore/intern/QoreRegexBase.h>
#include <qore/intern/QoreNamespaceList.h>
#include <qore/intern/AutoNamespaceList.h>
#include <qore/intern/ssl_constants.h>
#include <qore/intern/ConstantList.h>
#include <qore/intern/QoreClassList.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/QoreSignal.h>

#include <qore/minitest.hpp>

// include files for default object classes
#include <qore/intern/QC_Socket.h>
#include <qore/intern/QC_SSLCertificate.h>
#include <qore/intern/QC_SSLPrivateKey.h>
#include <qore/intern/QC_Program.h>
#include <qore/intern/QC_File.h>
#include <qore/intern/QC_Dir.h>
#include <qore/intern/QC_GetOpt.h>
#include <qore/intern/QC_FtpClient.h>
#include <qore/intern/QC_HTTPClient.h>
#include <qore/intern/QC_XmlRpcClient.h>
#include <qore/intern/QC_JsonRpcClient.h>
#include <qore/intern/QC_AutoLock.h>
#include <qore/intern/QC_AutoGate.h>
#include <qore/intern/QC_AutoReadLock.h>
#include <qore/intern/QC_AutoWriteLock.h>

#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include <assert.h>
#include <sys/socket.h> // for AF_INET and AF_INET6

#ifdef DEBUG_TESTS
// the #include "test/Namespace_tests.cc" is on the bottom
#  include "tests/builtin_inheritance_tests.cc"
#endif

#define MAX_RECURSION_DEPTH 20

class AutoNamespaceList ANSL;

struct qore_ns_private {
      std::string name;

      QoreClassList  *classList;
      class ConstantList   *constant;
      class QoreNamespaceList  *nsl;

      // pending lists
      QoreClassList  *pendClassList;
      class ConstantList   *pendConstant;
      class QoreNamespaceList  *pendNSL;

      class QoreNamespace      *next;

      DLLLOCAL qore_ns_private(const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl, QoreClassList *pend_ocl, ConstantList *pend_cl, QoreNamespaceList *pend_nsl) :
	 name(n), 
	 classList(ocl), constant(cl), nsl(nnsl), 
	 pendClassList(pend_ocl), pendConstant(pend_cl), pendNSL(pend_nsl), next(0)
      {
	 assert(classList);
	 assert(constant);
	 assert(nsl);
	 assert(pendClassList);
	 assert(pendConstant);
	 assert(pendNSL);
      }
      DLLLOCAL qore_ns_private(const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl) :
	 name(n),
	 classList(ocl), constant(cl), nsl(nnsl), 
	 pendClassList(new QoreClassList), pendConstant(new ConstantList), pendNSL(new QoreNamespaceList), next(0)
      {
	 assert(classList);
	 assert(constant);
	 assert(nsl);
      }
      DLLLOCAL qore_ns_private(const char *n) :
	 name(n),
	 classList(new QoreClassList), constant(new ConstantList), nsl(new QoreNamespaceList), 
	 pendClassList(new QoreClassList), pendConstant(new ConstantList), pendNSL(new QoreNamespaceList), next(0)
      {
      }

      DLLLOCAL qore_ns_private() :
	 classList(new QoreClassList), constant(new ConstantList), nsl(new QoreNamespaceList), 
	 pendClassList(new QoreClassList), pendConstant(new ConstantList), pendNSL(new QoreNamespaceList), next(0)
      {
      }

      DLLLOCAL ~qore_ns_private()
      {
	 printd(5, "QoreNamespace::~QoreNamespace() this=%08p '%s'\n", this, name.c_str());
	 
	 delete constant;
	 delete classList;
	 delete nsl;
	 
	 delete pendConstant;
	 delete pendClassList;
	 delete pendNSL;
      }
};

QoreNamespace::QoreNamespace() : priv(new qore_ns_private)
{
}

QoreNamespace::QoreNamespace(const char *n) : priv(new qore_ns_private(n))
{
}

QoreNamespace::QoreNamespace(const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl, QoreClassList *pend_ocl, ConstantList *pend_cl, QoreNamespaceList *pend_nsl) : priv(new qore_ns_private(n, ocl, cl, nnsl, pend_ocl, pend_cl, pend_nsl))
{
}

/*
QoreNamespace::QoreNamespace(QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl) : priv(new qore_ns_private)
{
   priv->classList  = ocl;
   priv->constant   = cl;
   priv->nsl        = nnsl;
}
*/

QoreNamespace::~QoreNamespace()
{
   //QORE_TRACE("QoreNamespace::~QoreNamespace()");
   delete priv;

}

const char *QoreNamespace::getName() const
{
   return priv->name.c_str();
}

// private function
class QoreNamespace *QoreNamespace::resolveNameScope(class NamedScope *nscope) const
{
   const class QoreNamespace *sns = this;

   // find namespace
   for (int i = 0; i < (nscope->elements - 1); i++)
      if (!(sns = sns->findNamespace(nscope->strlist[i])))
      {
	 parse_error("namespace '%s' cannot be resolved while evaluating '%s' in constant declaration",
		     nscope->strlist[i], nscope->ostr);
	 return 0;
      }
   return (QoreNamespace *)sns;
}

// private function
AbstractQoreNode *QoreNamespace::getConstantValue(const char *cname) const
{
   AbstractQoreNode *rv = priv->constant->find(cname);
   if (!rv)
      rv = priv->pendConstant->find(cname);

   return rv ? rv : 0;
}

// only called while parsing before addition to namespace tree, no locking needed
void QoreNamespace::addConstant(class NamedScope *nscope, AbstractQoreNode *value)
{
   class QoreNamespace *sns = resolveNameScope(nscope);
   if (!sns)
      value->deref(0);
   else
   {
      const char *cname = nscope->strlist[nscope->elements - 1];
      if (sns->priv->constant->find(cname))
      {
	 parse_error("constant '%s' has already been defined", cname);
	 value->deref(0);
      }
      else 
	 sns->priv->pendConstant->add(cname, value);
   }
}

// public, only called in single-threaded initialization
void QoreNamespace::addSystemClass(QoreClass *oc)
{
   QORE_TRACE("QoreNamespace::addSystemClass()");

#ifdef DEBUG
   if (priv->classList->add(oc))
      assert(false);
#else
   priv->classList->add(oc);
#endif

}

// public, only called when parsing for unattached namespaces
void QoreNamespace::addClass(class NamedScope *n, QoreClass *oc)
{
   //printd(5, "QoreNamespace::addClass() adding ns=%s (%s, %08p)\n", n->ostr, oc->getName(), oc);
   class QoreNamespace *sns = resolveNameScope(n);
   if (!sns)
      delete oc;
   else
      if (sns->priv->classList->find(oc->getName()))
      {
	 parse_error("class '%s' already exists in namespace '%s'", oc->getName(), priv->name.c_str());
	 delete oc;
      }
      else if (sns->priv->pendClassList->add(oc))
      {
	 parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), priv->name.c_str());
	 delete oc;
      }
}

void QoreNamespace::addNamespace(class QoreNamespace *ns)
{
   // raise an exception if namespace collides with an object name
   if (priv->classList->find(ns->priv->name.c_str()))
   {
      parse_error("namespace name '%s' collides with previously-defined class '%s'", ns->priv->name.c_str(), ns->priv->name.c_str());
      delete ns;
      return;
   }
   if (priv->pendClassList->find(ns->priv->name.c_str()))
   {
      parse_error("namespace name '%s' collides with pending class '%s'", ns->priv->name.c_str(), ns->priv->name.c_str());
      delete ns;
      return;
   }
   priv->pendNSL->add(ns);
}

void QoreNamespace::parseInitConstants()
{
   printd(5, "QoreNamespace::parseInitConstants() %s\n", priv->name.c_str());
   // do 2nd stage parse initialization on pending constants
   priv->pendConstant->parseInit();

   priv->pendNSL->parseInitConstants();
}

void QoreNamespace::parseInit()
{
   printd(5, "QoreNamespace::parseInit() this=%08p\n", this);

   // do 2nd stage parse initialization on committed classes
   priv->classList->parseInit();
   
   // do 2nd stage parse initialization on pending classes
   priv->pendClassList->parseInit();

   // do 2nd stage parse initialization on pending classes in pending lists of subnamespaces
   priv->nsl->parseInit();

   // do 2nd stage parse initialization on pending namespaces
   priv->pendNSL->parseInit();
}

void QoreNamespace::parseCommit()
{
   // merge pending constant list
   priv->constant->assimilate(priv->pendConstant);

   // merge pending classes and commit pending changes to committed classes
   priv->classList->parseCommit(priv->pendClassList);

   // merge pending namespaces and repeat for all subnamespaces
   priv->nsl->parseCommit(priv->pendNSL);
}

void QoreNamespace::parseRollback()
{
   printd(5, "QoreNamespace::parseRollback() %s %08p\n", priv->name.c_str(), this);

   // delete pending constant list
   priv->pendConstant->reset();

   // delete pending changes to committed classes
   priv->classList->parseRollback();

   // delete pending classes
   priv->pendClassList->reset();

   // delete pending namespaces
   priv->pendNSL->reset();

   // do for all subnamespaces
   priv->nsl->parseRollback();
}

QoreNamespaceList::QoreNamespaceList()
{
   head = tail = 0;
}

void QoreNamespaceList::deleteAll()
{
   while (head)
   {
      tail = head->priv->next;
      delete head;
      head = tail;
   }
}

QoreNamespaceList::~QoreNamespaceList()
{
   deleteAll();
}

void QoreNamespaceList::assimilate(class QoreNamespaceList *n)
{
   // assimilate target list
   if (tail)
      tail->priv->next = n->head;
   else
      head = n->head;
   if (n->tail)
      tail = n->tail;
   
   // "zero" target list
   n->head = n->tail = 0;
}

void QoreNamespaceList::reset()
{
   deleteAll();
   head = tail = 0;
}

void QoreNamespaceList::add(class QoreNamespace *ns)
{
   // if namespace is already registered, then assimilate
   QoreNamespace *ons;
   if ((ons = find(ns->priv->name.c_str())))
   {
      ons->assimilate(ns);
      return;
   }
   // otherwise append to list
   if (tail)
      tail->priv->next = ns;
   else
      head = ns;
   tail = ns;
}

class QoreNamespace *QoreNamespaceList::find(const char *name)
{
   QORE_TRACE("QoreNamespaceList::find()");
   printd(5, "QoreNamespaceList::find(%s)\n", name);

   class QoreNamespace *w = head;

   while (w)
   {
      if (name == w->priv->name)
	 break;
      w = w->priv->next;
   }

   printd(5, "QoreNamespaceList::find(%s) returning %08p\n", name, w);

   return w;
}

class QoreNamespace *QoreNamespace::copy(int po) const
{
   return new QoreNamespace(priv->name.c_str(), priv->classList->copy(po), priv->constant->copy(), priv->nsl->copy(po), 
			    priv->pendClassList->copy(po), priv->pendConstant->copy(), priv->pendNSL->copy(po));
}

class QoreNamespaceList *QoreNamespaceList::copy(int po)
{
   class QoreNamespaceList *nsl = new QoreNamespaceList();

   class QoreNamespace *w = head;

   while (w)
   {
      nsl->add(w->copy(po));
      w = w->priv->next;
   }

   return nsl;
}

void QoreNamespaceList::parseInitConstants()
{
   class QoreNamespace *w = head;

   while (w)
   {
      w->parseInitConstants();
      w = w->priv->next;
   }
}

void QoreNamespaceList::parseInit()
{
   class QoreNamespace *w = head;

   while (w)
   {
      w->parseInit();
      w = w->priv->next;
   }
}

void QoreNamespaceList::parseCommit(class QoreNamespaceList *l)
{
   assimilate(l);

   class QoreNamespace *w = head;

   while (w)
   {
      w->parseCommit();
      w = w->priv->next;
   }
}

void QoreNamespaceList::parseRollback()
{
   class QoreNamespace *w = head;

   while (w)
   {
      w->parseRollback();
      w = w->priv->next;
   }
}

class QoreNamespace *QoreNamespace::findNamespace(const char *nname) const
{
   class QoreNamespace *rv = priv->nsl->find(nname);
   if (!rv)
      rv = priv->pendNSL->find(nname);
   return rv;
}

// public: only called during Qore initialization to setup
// system constant types directly in Qore system namespaces
// FIXME: change to addSystemConstant() to avoid confusion
void QoreNamespace::addConstant(const char *cname, AbstractQoreNode *val)
{
   priv->constant->add(cname, val);
}

void QoreNamespace::addInitialNamespace(class QoreNamespace *ns)
{
   priv->nsl->add(ns);
}

static int parseInitConstantHash(QoreHashNode *h, int level)
{
   RootQoreNamespace *rns = getRootNS();

   HashIterator hi(h);
   while (hi.next()) {
      const char *k = hi.getKey();
      AbstractQoreNode **value = hi.getValuePtr();

      if (rns->parseInitConstantValue(value, level))
	 return -1;

      ReferenceHolder<AbstractQoreNode> n(0);
      // resolve constant references in keys
      if (k[0] == HE_TAG_CONST || k[0] == HE_TAG_SCOPED_CONST) {
	 // FIXME: add new entry points to RootQoreNamespace so it's not necessary to create these temporary AbstractQoreNode values
	 if (k[0] == HE_TAG_CONST) {
	    n = new BarewordNode(strdup(k + 1));
	 }
	 else
	    n = new ConstantNode(strdup(k + 1));
	 if (rns->parseInitConstantValue(n.getPtrPtr(), level))
	    return -1;

	 if (n) {
	    QoreStringValueHelper str(*n);
	 
	    // reference value for new hash
	    (*value)->ref();
	    // not possible to have an exception here
	    // adds to end of hash key list so it won't invalidate up our iterator
	    // the string must be in QCS_DEFAULT
	    h->setKeyValue(str->getBuffer(), *value, 0);
	    // or here
	    hi.deleteKey(0);
	 }
      }
   }

   return 0;
}

// QoreNamespaceList::parseResolveNamespace()
// does a recursive breadth-first search to resolve a namespace declaration
QoreNamespace *QoreNamespaceList::parseResolveNamespace(class NamedScope *name, int *matched)
{
   QORE_TRACE("QoreNamespaceList::parseResolveNamespace()");

   class QoreNamespace *w = head, *ns = 0;

   // search first level of all sub namespaces
   while (w)
   {
      if ((ns = w->parseMatchNamespace(name, matched)))
	 break;
      w = w->priv->next;
   }
      //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);

   if (!ns)
   {
      // now search in all sub namespace lists
      w = head;
      while (w)
      {
	 if ((ns = w->priv->nsl->parseResolveNamespace(name, matched)))
	    break;
	 if ((ns = w->priv->pendNSL->parseResolveNamespace(name, matched)))
	    break;
	 //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);
	 w = w->priv->next;
      }
   }


   return ns;
}

// QoreNamespaceList::parseFindConstantValue()
AbstractQoreNode *QoreNamespaceList::parseFindConstantValue(const char *cname)
{
   QORE_TRACE("QoreNamespaceList::parseFindConstantValue()");
   
   AbstractQoreNode *rv = 0;
   class QoreNamespace *w = head;
   // see if a match can be found at the first level
   while (w)
   {
      if ((rv = w->getConstantValue(cname)))
	 break;
      w = w->priv->next;
   }

   if (!rv) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((rv = w->priv->nsl->parseFindConstantValue(cname)))
	    break;
	 if ((rv = w->priv->pendNSL->parseFindConstantValue(cname)))
	    break;
	 w = w->priv->next;
      }
   }


   return rv;
}

/*
static void showNSL(class QoreNamespaceList *nsl)
{
   printd(5, "showNSL() dumping %08p\n", nsl);
   for (int i = 0; i < nsl->num_namespaces; i++)
      printd(5, "showNSL()  %d: %08p %s (list: %08p)\n", i, nsl->nslist[i], nsl->nslist[i]->name, nsl->nslist[i]->nsl);
}
*/

// QoreNamespaceList::parseFindScopedConstantValue()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
AbstractQoreNode *QoreNamespaceList::parseFindScopedConstantValue(class NamedScope *name, int *matched)
{
   AbstractQoreNode *rv = 0;

   QORE_TRACE("QoreNamespaceList::parseFindScopedConstantValue()");
   printd(5, "QoreNamespaceList::parseFindScopedConstantValue(this=%08p) target: %s\n", this, name->ostr);

   //showNSL(this);
   // see if a complete match can be found at the first level
   QoreNamespace *w = head;
   while (w)
   {
      if ((rv = w->parseMatchScopedConstantValue(name, matched)))
	 break;
      w = w->priv->next;
   }

   if (!rv) // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((rv = w->priv->nsl->parseFindScopedConstantValue(name, matched)))
	    break;
	 if ((rv = w->priv->pendNSL->parseFindScopedConstantValue(name, matched)))
	    break;
	 w = w->priv->next;
      }
   }


   return rv;
}

/*
// parseFindOTInNSL()
QoreClass *parseFindOTInNSL(class QoreNamespaceList *nsl, const char *otname)
{
   QoreClass *ot;
   // see if a match can be found at the first level
   for (int i = 0; i < nsl->num_namespaces; i++)
      if ((ot = nsl->nslist[i]->priv->classList->find(otname)))
	 return ot;

   // check all levels
   for (int i = 0; i < nsl->num_namespaces; i++)
      if ((ot = findOTInNSL(nsl->nslist[i]->nsl, otname)))
	 return ot;

   parse_error("reference to undefined object type '%s'", otname);
   return 0;
}
*/

// QoreNamespaceList::parseFindScopedClassWithMethod()
// does a recursive breadth-first search to resolve a namespace containing the given class name
// note: is only called with a namespace specifier
QoreClass *QoreNamespaceList::parseFindScopedClassWithMethod(class NamedScope *name, int *matched) const
{
   QoreClass *oc = 0;

   // see if a complete match can be found at the first level
   QoreNamespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClassWithMethod(name, matched)))
	 break;
      w = w->priv->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->priv->pendNSL->parseFindScopedClassWithMethod(name, matched)))
	    break;
	 if ((oc = w->priv->nsl->parseFindScopedClassWithMethod(name, matched)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}

// QoreNamespaceList::parseFindScopedClassWithMethod()
// does a recursive breadth-first search to resolve a namespace containing the given class name
// note: is only called with a namespace specifier
/*
QoreClass *QoreNamespaceList::parseFindScopedClassWithMethod(class NamedScope *name, int *matched, QoreClassList **plist, bool *is_pending)
{
   QoreClass *oc = 0;

   // see if a complete match can be found at the first level
   QoreNamespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClassWithMethod(name, matched, plist, is_pending)))
	 break;
      w = w->priv->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->priv->pendNSL->parseFindScopedClassWithMethod(name, matched, plist, is_pending)))
	    break;
	 if ((oc = w->priv->nsl->parseFindScopedClassWithMethod(name, matched, plist, is_pending)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}
*/

/*
QoreClass *QoreNamespaceList::parseFindClass(const char *ocname, QoreClassList **plist, bool *is_pending)
{
   QoreClass *oc = 0;

   // see if a match can be found at the first level
   QoreNamespace *w = head;
   while (w)
   {
      // check pending classes
      if ((oc = w->priv->pendClassList->find(ocname)))
      {
	 (*plist) = w->priv->classList;
	 (*is_pending) = true;
	 break;
      }
      if ((oc = w->priv->classList->find(ocname)))
      {
	 (*plist) = w->priv->pendClassList;
	 break;
      }
      w = w->priv->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->priv->nsl->parseFindClass(ocname, plist, is_pending)))
	    break;
	 if ((oc = w->priv->pendNSL->parseFindClass(ocname, plist, is_pending)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}
*/

QoreClass *QoreNamespaceList::parseFindClass(const char *ocname)
{
   QoreClass *oc = 0;

   // see if a match can be found at the first level
   QoreNamespace *w = head;
   while (w)
   {
      if ((oc = w->priv->classList->find(ocname)))
	 break;
      // check pending classes
      if ((oc = w->priv->pendClassList->find(ocname)))
	 break;

      w = w->priv->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->priv->nsl->parseFindClass(ocname)))
	    break;
	 if ((oc = w->priv->pendNSL->parseFindClass(ocname)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}

QoreClass *QoreNamespaceList::parseFindChangeClass(const char *ocname)
{
   QoreClass *oc = 0;

   // see if a match can be found at the first level
   QoreNamespace *w = head;
   while (w)
   {
      if ((oc = w->priv->classList->findChange(ocname)))
	 break;
      // check pending classes
      if ((oc = w->priv->pendClassList->find(ocname)))
	 break;

      w = w->priv->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->priv->nsl->parseFindChangeClass(ocname)))
	    break;
	 if ((oc = w->priv->pendNSL->parseFindClass(ocname)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}

// QoreNamespaceList::parseFindScopedClass()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
QoreClass *QoreNamespaceList::parseFindScopedClass(class NamedScope *name, int *matched)
{
   QoreClass *oc = 0;

   // see if a complete match can be found at the first level
   QoreNamespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClass(name, matched)))
	 break;
      w = w->priv->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->priv->nsl->parseFindScopedClass(name, matched)))
	    break;
	 if ((oc = w->priv->pendNSL->parseFindScopedClass(name, matched)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}

QoreObject *get_file_constant(QoreClass *fc, int fd)
{
   return fc->execSystemConstructor(fd);
}

// returns 0 for success, non-zero return value means error
int RootQoreNamespace::addMethodToClass(class NamedScope *scname, class QoreMethod *qcmethod, class BCAList *bcal)
{
   // find class
   //QoreClassList *plist;
   //bool is_pending = false;
   QoreClass *oc;

   const char *cname  = scname->strlist[scname->elements - 2];
   const char *method = scname->strlist[scname->elements - 1];

   // if there is no namespace specified, then just find class
   if (scname->elements == 2)
   {
      oc = rootFindClass(cname);
      if (!oc)
      {
	 parse_error("reference to undefined class '%s' while trying to add method '%s'", cname, method);
	 return -1;
      }
   }
   else
   {
      int m = 0;
      oc = rootFindScopedClassWithMethod(scname, &m);
      if (!oc)
      {
	 if (m != (scname->elements - 2))
	    parse_error("cannot resolve namespace '%s' in '%s()'", scname->strlist[m], scname->ostr);
	 else
	    parse_error("class '%s' does not exist", cname);
	 return -1;
      }
   }

   oc->addMethod(qcmethod);
   // after the addMethod call, we can no longer return an error code if 
   // oc->parseAddBaseClassArgumentList() fails (because the caller will 
   // delete it if we return an error code), so we delete it here
   if (bcal && oc->parseAddBaseClassArgumentList(bcal))
      delete bcal;

   return 0;
}

QoreClass *RootQoreNamespace::parseFindClass(const char *cname) const
{
   QoreClass *oc = rootFindClass(cname);
   if (!oc)
      parse_error("reference to undefined class '%s'", cname);

   return oc;
}

QoreClass *RootQoreNamespace::parseFindScopedClass(class NamedScope *nscope) const
{
   QoreClass *oc;
   // if there is no namespace specified, then just find class
   if (nscope->elements == 1)
   {
      oc = rootFindClass(nscope->strlist[0]);
      if (!oc)
	 parse_error("reference to undefined class '%s'", nscope->ostr);
   }
   else
   {
      int m = 0;
      oc = rootFindScopedClass(nscope, &m);

      if (!oc) {
	 if (m != (nscope->elements - 1))
	    parse_error("cannot resolve namespace '%s' in '%s()'", nscope->strlist[m], nscope->ostr);
	 else
	 {
	    QoreString err;
	    err.sprintf("cannot find class '%s' in any namespace '", nscope->getIdentifier());
	    for (int i = 0; i < (nscope->elements - 1); i++)
	    {
	       err.concat(nscope->strlist[i]);
	       if (i != (nscope->elements - 2))
		  err.concat("::");
	    }
	    err.concat("'");
	    parse_error(err.getBuffer());
	 }
      }
   }
   printd(5, "RootQoreNamespace::parseFindScopedClass('%s') returning %08p\n", nscope->ostr, oc);
   return oc;
}

QoreClass *RootQoreNamespace::parseFindScopedClassWithMethod(class NamedScope *scname) const
{
   QoreClass *oc;

   int m = 0;
   oc = rootFindScopedClassWithMethod(scname, &m);
   
   if (!oc) {
      if (m != (scname->elements - 1))
	 parse_error("cannot resolve namespace '%s' in '%s()'", scname->strlist[m], scname->ostr);
      else
      {
	 QoreString err;
	 err.sprintf("cannot find class '%s' in any namespace '", scname->getIdentifier());
	 for (int i = 0; i < (scname->elements - 1); i++)
	 {
	    err.concat(scname->strlist[i]);
	    if (i != (scname->elements - 2))
	       err.concat("::");
	 }
	 err.concat("'");
	 parse_error(err.getBuffer());
      }
   }
   
   printd(5, "RootQoreNamespace::parseFindScopedClassWithMethod('%s') returning %08p\n", scname->ostr, oc);
   return oc;
}

// returns 0 for success, non-zero for error
int RootQoreNamespace::parseInitConstantValue(AbstractQoreNode **val, int level)
{
   if (!(*val))
      return 0;

   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH) {
      parse_error("maximum recursion level exceeded resolving constant definition");
      return -1;
   }

   //printd(5, "constant %08p resolving type '%s'\n", val, *val ? (*val)->getTypeName() : "null");

   while (true) {
      qore_type_t vtype = (*val)->getType();
      if (vtype == NT_BAREWORD) {
	 //printd(5, "constant %08p has recursive definition '%s'\n", val, reinterpret_cast<BarewordNode *>(*val)->str);
	 if (resolveSimpleConstant(val, level + 1))
	    return -1;
      }
      else if (vtype == NT_CONSTANT) {
	 //printd(5, "constant %08p has recursive definition '%s'\n", val, reinterpret_cast<ConstantNode *>(*val)->scoped_ref->ostr);
	 if (resolveScopedConstant(val, level + 1))
	    return -1;
      }
      else
	 break;
      //printd(5, "constant %08p resolved to type '%s'\n", val, *val ? (*val)->getTypeName() : "null");
   }

   qore_type_t vtype = (*val)->getType();
   if (vtype == NT_LIST) {
      QoreListNode *l = reinterpret_cast<QoreListNode *>(*val);
      for (unsigned i = 0; i < l->size(); i++) {
	 if (parseInitConstantValue(l->get_entry_ptr(i), level))
	    return -1;
      }
   }
   else if (vtype == NT_HASH) {
      QoreHashNode *h = reinterpret_cast<QoreHashNode *>(*val);
      if (parseInitConstantHash(h, level))
	 return -1;
   }
   else if (vtype == NT_TREE) {
      QoreTreeNode *tree =reinterpret_cast<QoreTreeNode *>(*val);
      if (parseInitConstantValue(&(tree->left), level))
	 return -1;
      if (tree->right)
	 if (parseInitConstantValue(&(tree->right), level))
	    return -1;
   }

   // if it's an expression or container type, then evaluate in case it contains immediate expressions
   if (vtype == NT_TREE || vtype == NT_LIST || vtype == NT_HASH) {
      //printd(5, "evaluating constant expression %08p\n", *val);
      ExceptionSink xsink;
      AbstractQoreNode *n = (*val)->eval(&xsink);
      (*val)->deref(&xsink);
      *val = n;
   }
   return 0;
}

// returns 0 for success, non-zero for error
int RootQoreNamespace::resolveSimpleConstant(AbstractQoreNode **node, int level) const
{
   assert(*node && (*node)->getType() == NT_BAREWORD);
   BarewordNode *b = reinterpret_cast<BarewordNode *>(*node);
   printd(5, "RootQoreNamespace::resolveSimpleConstant(%s, %d)\n", b->str, level);

   // if constant is not found, then a parse error will be raised
   AbstractQoreNode *rv = findConstantValue(b->str, level);
   if (!rv)
      return -1;

   printd(5, "RootQoreNamespace::resolveSimpleConstant(%s, %d) %08p %s-> %08p %s\n", 
	  b->str, level, *node, (*node)->getTypeName(), rv, rv->getTypeName());
   
   b->deref();
   *node = rv->refSelf();
   return 0;
}

int RootQoreNamespace::resolveScopedConstant(AbstractQoreNode **node, int level) const
{
   assert(node && (*node)->getType() == NT_CONSTANT);
   ConstantNode *c = reinterpret_cast<ConstantNode *>(*node);

   // if constant is not found, then a parse error will be raised
   AbstractQoreNode *rv = findConstantValue(c->scoped_ref, level);
   if (!rv) {
      //printd(5, "RootQoreNamespace::resolveScopedConstant(%s, %d) findConstantValue() returned 0\n", c->scoped_ref->ostr, level);
      return -1;
   }

   //printd(5, "RootQoreNamespace::resolveScopedConstant(%s, %d) %08p %s-> %08p %s\n", c->scoped_ref->ostr, level, *node, (*node)->getTypeName(), rv, rv->getTypeName());

   c->deref();
   *node = rv->refSelf();
   return 0;
}

AbstractQoreNode *RootQoreNamespace::findConstantValue(const char *cname, int level) const
{
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH)
   {
      parse_error("recursive constant definitions too deep resolving '%s'", cname);
      return 0;
   }

   AbstractQoreNode *rv = rootFindConstantValue(cname);
   if (!rv)
      parse_error("constant '%s' cannot be resolved in any namespace", cname);
   else {
      // here we enforce PO_NO_TERMINAL_IO for stdin, stdout, and stderr constants
      if (rv->getType() == NT_OBJECT && getProgram()->getParseOptions() & PO_NO_TERMINAL_IO) {
	 QoreObject *o = reinterpret_cast<QoreObject *>(rv);
	 if (o->isSystemObject() && o->validInstanceOf(CID_FILE))
	    parseException("ILLEGAL-CONSTANT-ACCESS", "File I/O constants cannot be accessed with PO_NO_TERMINAL_IO");
      }
   }
   return rv;
}

// called in 2nd stage of parsing to resolve constant references
AbstractQoreNode *RootQoreNamespace::findConstantValue(class NamedScope *scname, int level) const
{
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH)
   {
      parse_error("recursive constant definitions too deep resolving '%s'", scname->ostr);
      return 0;
   }

   AbstractQoreNode *rv;

   if (scname->elements == 1)
   {
      rv = rootFindConstantValue(scname->ostr);
      if (!rv)
      {
	 parse_error("constant '%s' cannot be resolved in any namespace", scname->ostr);
	 return 0;
      }
      return rv;
   }

   int m = 0;
   rv = rootFindScopedConstantValue(scname, &m);
   if (rv)
      return rv;

   if (m != (scname->elements - 1))
      parse_error("cannot resolve namespace '%s' in '%s'", scname->strlist[m], scname->ostr);
   else
   {
      QoreString err;
      err.sprintf("cannot find constant '%s' in any namespace '", scname->getIdentifier());
      for (int i = 0; i < (scname->elements - 1); i++)
      {
	 err.concat(scname->strlist[i]);
	 if (i != (scname->elements - 2))
	    err.concat("::");
      }
      err.concat("'");
      parse_error(err.getBuffer());
   }
   return 0;
}

// public, only called either in single-threaded initialization or
// while the program-level parse lock is held
void QoreNamespace::addClass(QoreClass *oc)
{
   QORE_TRACE("QoreNamespace::addClass()");
   //printd(5, "QoreNamespace::addClass() adding str=%s (%08p)\n", oc->name, oc);
   // raise an exception if object name collides with a namespace
   if (priv->nsl->find(oc->getName()))
   {
      parse_error("class name '%s' collides with previously-defined namespace '%s'", oc->getName(), oc->getName());
      delete oc;
   }
   else if (priv->pendNSL->find(oc->getName()))
   {
      parse_error("class name '%s' collides with pending namespace '%s'", oc->getName(), oc->getName());
      delete oc;
   }
   else if (priv->classList->find(oc->getName()))
   {
      parse_error("class '%s' already exists in namespace '%s'", oc->getName(), priv->name.c_str());
      delete oc;
   }
   else if (priv->pendClassList->add(oc))
   {
      parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), priv->name.c_str());
      delete oc;
   }

}

void QoreNamespace::assimilate(class QoreNamespace *ns)
{
   // assimilate pending constants
   // assimilate target list - if there were errors then the list will be deleted anyway
   priv->pendConstant->assimilate(ns->priv->pendConstant, priv->constant, priv->name.c_str());

   // assimilate classes
   priv->pendClassList->assimilate(ns->priv->pendClassList, priv->classList, priv->nsl, priv->pendNSL, priv->name.c_str());

   // assimilate sub namespaces
   QoreNamespace *nw = ns->priv->pendNSL->head;
   while (nw)
   {
      // throw parse exception if name is already defined
      if (priv->nsl->find(nw->priv->name.c_str()))
	 parse_error("subnamespace '%s' has already been defined in namespace '%s'",
		     nw->priv->name.c_str(), priv->name.c_str());
      else if (priv->pendNSL->find(nw->priv->name.c_str()))
	 parse_error("subnamespace '%s' is already pending in namespace '%s'",
		     nw->priv->name.c_str(), priv->name.c_str());
      else if (priv->classList->find(nw->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class has already been defined with this name",
		     nw->priv->name.c_str(), priv->name.c_str());
      else if (priv->pendClassList->find(nw->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class is already pending with this name",
		     nw->priv->name.c_str(), priv->name.c_str());

      nw = nw->priv->next;
   }
   // assimilate target list
   priv->pendNSL->assimilate(ns->priv->pendNSL);

   // delete source namespace
   delete ns;
}

// QoreNamespace::parseMatchNamespace()
// will only be called if there is a match with the name and nscope->elements > 1
class QoreNamespace *QoreNamespace::parseMatchNamespace(class NamedScope *nscope, int *matched) const
{
   // see if starting name matches this namespace
   if (!strcmp(nscope->strlist[0], priv->name.c_str()))
   {
      const class QoreNamespace *ns = this;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // check for a match of the structure in this namespace
      for (int i = 1; i < (nscope->elements - 1); i++)
      {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    break;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
      return (QoreNamespace *)ns;
   }

   return 0;
}

/*
QoreClass *QoreNamespace::parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched, QoreClassList **plist, bool *is_pending) const
{
   printd(5, "QoreNamespace::parseMatchScopedClassWithMethod(this=%08p) %s class=%s (%s)\n", this, priv->name.c_str(), nscope->strlist[nscope->elements - 2], nscope->ostr);

   QoreNamespace *ns = this;
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      // if first namespace doesn't match, then return 0
      if (strcmp(nscope->strlist[0], name))
	 return 0;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (int i = 1; i < (nscope->elements - 2); i++)
      {	 
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   // check last namespaces
   QoreClass *rv = ns->priv->pendClassList->find(nscope->strlist[nscope->elements - 2]);
   if (rv)
   {
      (*plist) = ns->priv->classList;
      (*is_pending) = true;
   }
   else if ((rv = ns->priv->classList->find(nscope->strlist[nscope->elements - 2])))
   {
      (*plist) = ns->priv->pendClassList;
   }
   return rv;
}
*/

QoreClass *QoreNamespace::parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched) const
{
   printd(5, "QoreNamespace::parseMatchScopedClassWithMethod(this=%08p) %s class=%s (%s)\n", this, priv->name.c_str(), nscope->strlist[nscope->elements - 2], nscope->ostr);

   const QoreNamespace *ns = this;
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      // if first namespace doesn't match, then return 0
      if (strcmp(nscope->strlist[0], priv->name.c_str()))
	 return 0;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (int i = 1; i < (nscope->elements - 2); i++)
      {	 
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   // check last namespaces
   QoreClass *rv = ns->priv->pendClassList->find(nscope->strlist[nscope->elements - 2]);
   if (!rv)
      rv = ns->priv->classList->find(nscope->strlist[nscope->elements - 2]);

   return rv;
}

QoreClass *QoreNamespace::parseMatchScopedClass(class NamedScope *nscope, int *matched) const
{
   if (strcmp(nscope->strlist[0], priv->name.c_str()))
      return 0;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   printd(5, "QoreNamespace::parseMatchScopedClass() matched %s in %s\n", priv->name.c_str(), nscope->ostr);

   const QoreNamespace *ns = this;
   
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      for (int i = 1; i < (nscope->elements - 1); i++)
      {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   QoreClass *rv = ns->priv->classList->find(nscope->strlist[nscope->elements - 1]);
   if (!rv)
      rv = ns->priv->pendClassList->find(nscope->strlist[nscope->elements - 1]);
   return rv;
}

AbstractQoreNode *QoreNamespace::parseMatchScopedConstantValue(class NamedScope *nscope, int *matched) const
{
   printd(5, "QoreNamespace::parseMatchScopedConstantValue() trying to find %s in %s (%08p)\n", 
	  nscope->getIdentifier(), priv->name.c_str(), getConstantValue(nscope->getIdentifier()));

   if (strcmp(nscope->strlist[0], priv->name.c_str()))
      return 0;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   const class QoreNamespace *ns = this;

   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
      for (int i = 1; i < (nscope->elements - 1); i++)
      {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }

   return ns->getConstantValue(nscope->getIdentifier());
}

QoreHashNode *QoreNamespace::getConstantInfo() const
{
   return priv->constant->getInfo();
}

QoreHashNode *QoreNamespace::getClassInfo() const
{
   return priv->classList->getInfo();
}

// returns a hash of namespace information
QoreHashNode *QoreNamespace::getInfo() const
{
   QoreHashNode *h = new QoreHashNode();

   h->setKeyValue("constants", getConstantInfo(), 0);
   h->setKeyValue("classes", getClassInfo(), 0);

   if (priv->nsl->head)
   {
      QoreHashNode *nsh = new QoreHashNode();
      
      class QoreNamespace *w = priv->nsl->head;
      while (w)
      {
	 nsh->setKeyValue(w->priv->name.c_str(), w->getInfo(), 0);
	 w = w->priv->next;
      }

      h->setKeyValue("subnamespaces", nsh, 0);
   }

   return h;
}

void QoreNamespace::setName(const char *nme)
{
   assert(priv->name.empty());
   priv->name = nme;
}

// only called with RootNS
AbstractQoreNode *RootQoreNamespace::rootFindConstantValue(const char *cname) const
{
   AbstractQoreNode *rv;
   if (!(rv = getConstantValue(cname))
       && (!(rv = priv->nsl->parseFindConstantValue(cname))))
      rv = priv->pendNSL->parseFindConstantValue(cname);
   return rv;
}

// only called with RootNS
void RootQoreNamespace::rootAddClass(class NamedScope *nscope, QoreClass *oc)
{
   QORE_TRACE("RootQoreNamespace::rootAddClass()");

   class QoreNamespace *sns = rootResolveNamespace(nscope);

   if (sns)
   {
      printd(5, "RootQoreNamespace::rootAddClass() '%s' adding %s:%08p to %s:%08p\n", nscope->ostr, 
	     oc->getName(), oc, sns->priv->name.c_str(), sns);
      sns->addClass(oc);
   }
   else
      delete oc;


}

void RootQoreNamespace::rootAddConstant(class NamedScope *nscope, AbstractQoreNode *value)
{
   class QoreNamespace *sns = rootResolveNamespace(nscope);

   if (sns)
   {
      printd(5, "RootQoreNamespace::rootAddConstant() %s: adding %s to %s (value=%08p type=%s)\n", nscope->ostr, 
	     nscope->getIdentifier(), sns->priv->name.c_str(), value, value ? value->getTypeName() : "(none)");
      sns->priv->pendConstant->add(nscope->strlist[nscope->elements - 1], value);
   }
   else
      value->deref(0);
}

// public
QoreClass *RootQoreNamespace::rootFindClass(const char *ocname) const
{
   QORE_TRACE("RootQoreNamespace::rootFindClass");
   QoreClass *oc;
   if (!(oc = priv->classList->find(ocname))
       && !(oc = priv->pendClassList->find(ocname))
       && !(oc = priv->nsl->parseFindClass(ocname)))
      oc = priv->pendNSL->parseFindClass(ocname);

   return oc;
}

QoreClass *RootQoreNamespace::rootFindChangeClass(const char *ocname)
{
   QORE_TRACE("RootQoreNamespace::rootFindChangeClass");
   QoreClass *oc;
   if (!(oc = priv->classList->findChange(ocname))
       && !(oc = priv->pendClassList->find(ocname))
       && !(oc = priv->nsl->parseFindChangeClass(ocname)))
      oc = priv->pendNSL->parseFindClass(ocname);

   return oc;
}

class QoreNamespace *RootQoreNamespace::rootResolveNamespace(class NamedScope *nscope)
{
   if (nscope->elements == 1)
      return this;

   QoreNamespace *ns;
   int match = 0;

   if (!(ns = parseMatchNamespace(nscope, &match))
       && !(ns = priv->nsl->parseResolveNamespace(nscope, &match))
       && !(ns = priv->pendNSL->parseResolveNamespace(nscope, &match)))

   if (!ns)
      parse_error("cannot resolve namespace '%s' in '%s'", nscope->strlist[match], nscope->ostr);

   return ns;
}

// private
QoreClass *RootQoreNamespace::rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched) const
{
   QoreClass *oc;

   if (!(oc = parseMatchScopedClassWithMethod(nscope, matched))
       && !(oc = priv->nsl->parseFindScopedClassWithMethod(nscope, matched)))
      oc = priv->pendNSL->parseFindScopedClassWithMethod(nscope, matched);
   return oc;
}

// private
// will always be called with a namespace (nscope->elements > 1)
QoreClass *RootQoreNamespace::rootFindScopedClass(class NamedScope *nscope, int *matched) const
{
   QoreClass *oc = parseMatchScopedClass(nscope, matched);
   if (!oc && !(oc = priv->nsl->parseFindScopedClass(nscope, matched)))
      oc = priv->pendNSL->parseFindScopedClass(nscope, matched);
   return oc;
}

// private, will always be called with nscope->elements > 1
AbstractQoreNode *RootQoreNamespace::rootFindScopedConstantValue(class NamedScope *nscope, int *matched) const
{
   AbstractQoreNode *rv = parseMatchScopedConstantValue(nscope, matched);
   if (!rv && !(rv = priv->nsl->parseFindScopedConstantValue(nscope, matched)))
      rv = priv->pendNSL->parseFindScopedConstantValue(nscope, matched);
   return rv;
}

void RootQoreNamespace::addQoreNamespace(class QoreNamespace *qns)
{
   addInitialNamespace(qns);
   qoreNS = qns;
}

// sets up the root namespace
RootQoreNamespace::RootQoreNamespace(class QoreNamespace **QoreNS) : QoreNamespace()
{
   QORE_TRACE("RootQoreNamespace::RootNamespace");

   priv->name = "";

   class QoreNamespace *qns = new QoreNamespace("Qore");

   QoreClass *File;
   // add system object types
   qns->addSystemClass(initSocketClass());
   qns->addSystemClass(initSSLCertificateClass());
   qns->addSystemClass(initSSLPrivateKeyClass());
   qns->addSystemClass(initProgramClass());
   qns->addSystemClass(File = initFileClass());
   qns->addSystemClass(initDirClass());
   qns->addSystemClass(initGetOptClass());
   qns->addSystemClass(initFtpClientClass());
   qns->addSystemClass(initAutoLockClass());
   qns->addSystemClass(initAutoGateClass());
   qns->addSystemClass(initAutoReadLockClass());
   qns->addSystemClass(initAutoWriteLockClass());

   // add HTTPClient namespace
   QoreClass *http_client_class;
   qns->addSystemClass((http_client_class = initHTTPClientClass()));
   qns->addSystemClass(initXmlRpcClientClass(http_client_class));
   qns->addSystemClass(initJsonRpcClientClass(http_client_class));

   // add signal constants
   QoreSignalManager::addSignalConstants(qns);

#ifdef DEBUG_TESTS
{ // tests
   QoreClass* base = initBuiltinInheritanceTestBaseClass();
   qns->addSystemClass(base);
   qns->addSystemClass(initBuiltinInheritanceTestDescendant1(base));
   // hierarchy with 3 levels
   QoreClass* desc2 = initBuiltinInheritanceTestDescendant2(base);
   qns->addSystemClass(desc2);
   QoreClass* desc3 = initBuiltinInheritanceTestDescendant3(desc2);
   qns->addSystemClass(desc3);
// BUGBUG : this fails. When desc2 is placed in the next line all is OK
   QoreClass* desc4 = initBuiltinInheritanceTestDescendant4(desc3);
   qns->addSystemClass(desc4);

   QoreClass* base2 = initBuiltinInheritanceTestBase2Class();
   qns->addSystemClass(base2);
// BUGBUG - the function actually fails to deal with two base classes, see the 
// code in tests/builtin_inheritance_tests.cc
   QoreClass* desc_multi = initBuiltinInheritanceTestDescendantMulti(base2, base);
   qns->addSystemClass(desc_multi);
}
#endif

   qns->addInitialNamespace(get_thread_ns());

   // add ssl socket constants
   addSSLConstants(qns);

   // add boolean constants for true and false
   qns->addConstant("True",          boolean_true());
   qns->addConstant("False",         boolean_false());

   // add File object constants for stdin (0), stdout (1), stderr (2)
   qns->addConstant("stdin",         File->execSystemConstructor(0));
   qns->addConstant("stdout",        File->execSystemConstructor(1));
   qns->addConstant("stderr",        File->execSystemConstructor(2));

   // add constants for exception types
   qns->addConstant("ET_System",     new QoreStringNode("System"));
   qns->addConstant("ET_User",       new QoreStringNode("User"));

   // create constants for call types
   qns->addConstant("CT_User",       new QoreBigIntNode(CT_USER));
   qns->addConstant("CT_Builtin",    new QoreBigIntNode(CT_BUILTIN));
   qns->addConstant("CT_NewThread",  new QoreBigIntNode(CT_NEWTHREAD));
   qns->addConstant("CT_Rethrow",    new QoreBigIntNode(CT_RETHROW));

   // create constants for version and platform information
   qns->addConstant("VersionString", new QoreStringNode(qore_version_string));
   qns->addConstant("VersionMajor",  new QoreBigIntNode(qore_version_major));
   qns->addConstant("VersionMinor",  new QoreBigIntNode(qore_version_minor));
   qns->addConstant("VersionSub",    new QoreBigIntNode(qore_version_sub));
   qns->addConstant("Build",         new QoreBigIntNode(qore_build_number));
   qns->addConstant("PlatformCPU",   new QoreStringNode(TARGET_ARCH));
   qns->addConstant("PlatformOS",    new QoreStringNode(TARGET_OS));

   // add constants for regex() function options
   qns->addConstant("RE_Caseless",   new QoreBigIntNode(PCRE_CASELESS));
   qns->addConstant("RE_DotAll",     new QoreBigIntNode(PCRE_DOTALL));
   qns->addConstant("RE_Extended",   new QoreBigIntNode(PCRE_EXTENDED));
   qns->addConstant("RE_MultiLine",  new QoreBigIntNode(PCRE_MULTILINE));
   // note that the following constant is > 32-bits so it can't collide with PCRE constants
   qns->addConstant("RE_Global",     new QoreBigIntNode(QRE_GLOBAL));

   // network constants
   qns->addConstant("AF_INET",       new QoreBigIntNode(AF_INET));
   qns->addConstant("AF_INET6",      new QoreBigIntNode(AF_INET6));

   // math constants
   qns->addConstant("M_PI",          new QoreFloatNode(3.14159265358979323846));

   // add constant for features found with configure
#ifdef HAVE_ROUND
   qns->addConstant("HAVE_ROUND",    &True);
#else
   qns->addConstant("HAVE_ROUND",    &False);
#endif

#ifdef HAVE_TIMEGM
   qns->addConstant("HAVE_TIMEGM",   &True);
#else
   qns->addConstant("HAVE_TIMEGM",   &False);
#endif

#ifdef HAVE_SETEUID
   qns->addConstant("HAVE_SETEUID",  &True);
#else
   qns->addConstant("HAVE_SETEUID",  &False);
#endif

#ifdef HAVE_SETEGID
   qns->addConstant("HAVE_SETEGID",  &True);
#else
   qns->addConstant("HAVE_SETEGID",  &False);
#endif

#ifdef HAVE_XMLTEXTREADERSETSCHEMA
   qns->addConstant("HAVE_PARSEXMLWITHSCHEMA",  &True);
#else
   qns->addConstant("HAVE_PARSEXMLWITHSCHEMA",  &False);
#endif

   // create Qore::SQL namespace
   qns->addInitialNamespace(getSQLNamespace());

   // create get Qore::Err namespace with ERRNO constants
   qns->addInitialNamespace(get_errno_ns());

   // create Qore::Type namespace with type constants
   qns->addInitialNamespace(get_type_ns());

   // add file constants
   addFileConstants(qns);

   // add parse option constants to Qore namespace
   addProgramConstants(qns);

   addQoreNamespace(qns);

   // add all changes in loaded modules
   ANSL.init(this, qns);

   *QoreNS = qns;

}

// private constructor
RootQoreNamespace::RootQoreNamespace(QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl, QoreClassList *pend_ocl, ConstantList *pend_cl, QoreNamespaceList *pend_nsl) : QoreNamespace("", ocl, cl, nnsl, pend_ocl, pend_cl, pend_nsl)
{
   qoreNS = priv->nsl->find("Qore");
}

RootQoreNamespace::~RootQoreNamespace()
{
}

QoreNamespace *RootQoreNamespace::rootGetQoreNamespace() const
{
   return qoreNS;
}

RootQoreNamespace *RootQoreNamespace::copy(int po) const
{
   return new RootQoreNamespace(priv->classList->copy(po), priv->constant->copy(), priv->nsl->copy(po), priv->pendClassList->copy(po), priv->pendConstant->copy(), priv->pendNSL->copy(po));
}

#ifdef DEBUG_TESTS
// moved down to allow to test internal classes
#  include "tests/Namespace_tests.cc"
#endif

