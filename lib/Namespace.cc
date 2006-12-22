/*
  Namespace.cc

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
#include <qore/Qore.h>
#include <qore/Namespace.h>
#include <qore/support.h>
#include <qore/common.h>
#include <qore/QoreClass.h>
#include <qore/Object.h>
#include <qore/qore_thread.h>
#include <qore/params.h>
#include <qore/ErrnoConstants.h>
#include <qore/TypeConstants.h>
#include <qore/QoreProgram.h>
#include <qore/ParserSupport.h>
#include <qore/Function.h>
#include <qore/CallStack.h>
#include <qore/QoreRegexBase.h>
#include <qore/DBI.h>
#include <qore/AutoNamespaceList.h>
#include <qore/ssl_constants.h>
#include <qore/NamedScope.h>
#include <qore/QoreNode.h>
#include <qore/QoreFile.h>
#include <qore/hash_map.h>
#include <qore/minitest.hpp>
#include <qore/QoreType.h>
#include <qore/ConstantList.h>
#include <qore/QoreClassList.h>

// include files for default object classes
#include <qore/QC_Socket.h>
#include <qore/QC_SSLCertificate.h>
#include <qore/QC_SSLPrivateKey.h>
#include <qore/QC_Program.h>
#include <qore/QC_File.h>
#include <qore/QC_GetOpt.h>
#include <qore/QC_FtpClient.h>
#include <qore/QC_HTTPClient.h>

#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include <assert.h>

#ifdef DEBUG
// the #include "test/Namespace_tests.cc" is on the bottom
#  include "tests/builtin_inheritance_tests.cc"
#endif

#define MAX_RECURSION_DEPTH 20

class AutoNamespaceList ANSL;

// this class is entirely private to the rest of the library
class NamespaceList
{
   private:
      DLLLOCAL void deleteAll();

   public:
      class Namespace *head, *tail;

      DLLLOCAL NamespaceList();
      DLLLOCAL ~NamespaceList();
      DLLLOCAL class Namespace *find(char *name);
      DLLLOCAL void add(class Namespace *ot);
      DLLLOCAL class NamespaceList *copy(int po);
      DLLLOCAL void parseInitConstants();
      DLLLOCAL void parseInit();
      DLLLOCAL void parseCommit(class NamespaceList *n);
      DLLLOCAL void parseRollback();
      DLLLOCAL void reset();
      DLLLOCAL void assimilate(class NamespaceList *n);
      DLLLOCAL class Namespace *parseResolveNamespace(class NamedScope *name, int *matched);
      DLLLOCAL class QoreNode *parseFindConstantValue(char *cname);
      DLLLOCAL class QoreNode *parseFindScopedConstantValue(class NamedScope *name, int *matched);
      DLLLOCAL class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *parseFindScopedClass(class NamedScope *name, int *matched);
      DLLLOCAL class QoreClass *parseFindClass(char *ocname);
      DLLLOCAL class QoreClass *parseFindChangeClass(char *ocname);
};


void Namespace::init()
{
   next = NULL;
   pendConstant = new ConstantList();
   pendClassList = new QoreClassList();
   pendNSL = new NamespaceList();
}

Namespace::Namespace()
{
   init();
   name       = NULL;
   classList  = new QoreClassList();
   constant   = new ConstantList();
   nsl        = new NamespaceList();
}

Namespace::Namespace(char *n)
{
   init();
   name       = strdup(n);
   classList  = new QoreClassList();
   constant   = new ConstantList();
   nsl        = new NamespaceList();
}

Namespace::Namespace(char *n, QoreClassList *ocl, ConstantList *cl, NamespaceList *nnsl)
{
   init();
   name       = strdup(n);
   classList  = ocl;
   constant   = cl;
   nsl        = nnsl;
}

Namespace::Namespace(QoreClassList *ocl, ConstantList *cl, NamespaceList *nnsl)
{
   init();
   name       = "";
   classList  = ocl;
   constant   = cl;
   nsl        = nnsl;
}

Namespace::~Namespace()
{
   //tracein("Namespace::~Namespace()");
   printd(5, "Namespace::~Namespace() deleting NS '%s'\n", name);
   if (name)
      free(name);

   delete constant;
   delete classList;
   delete nsl;

   delete pendConstant;
   delete pendClassList;
   delete pendNSL;

   //traceout("Namespace::~Namespace()");
}

char *Namespace::getName() const
{
   return name;
}

// private function
class Namespace *Namespace::resolveNameScope(class NamedScope *nscope) const
{
   const class Namespace *sns = this;

   // find namespace
   for (int i = 0; i < (nscope->elements - 1); i++)
      if (!(sns = sns->findNamespace(nscope->strlist[i])))
      {
	 parse_error("namespace '%s' cannot be resolved while evaluating '%s' in constant declaration",
		     nscope->strlist[i], nscope->ostr);
	 return NULL;
      }
   return (Namespace *)sns;
}

// private function
class QoreNode *Namespace::getConstantValue(char *cname) const
{
   class QoreNode *rv = constant->find(cname);
   if (!rv)
      rv = pendConstant->find(cname);

   return rv ? rv : NULL;
}

// only called while parsing before addition to namespace tree, no locking needed
void Namespace::addConstant(class NamedScope *nscope, class QoreNode *value)
{
   class Namespace *sns = resolveNameScope(nscope);
   if (!sns)
      value->deref(NULL);
   else
   {
      char *cname = nscope->strlist[nscope->elements - 1];
      if (sns->constant->find(cname))
      {
	 parse_error("constant '%s' has already been defined", cname);
	 value->deref(NULL);
      }
      else 
	 sns->pendConstant->add(cname, value);
   }
}

// public, only called in single-threaded initialization
void Namespace::addSystemClass(class QoreClass *oc)
{
   tracein("Namespace::addSystemClass()");

#ifdef DEBUG
   if (classList->add(oc))
      assert(false);
#else
   classList->add(oc);
#endif
   traceout("Namespace::addSystemClass()");
}

// public, only called when parsing for unattached namespaces
void Namespace::addClass(class NamedScope *n, class QoreClass *oc)
{
   //printd(5, "Namespace::addClass() adding ns=%s (%s, %08p)\n", n->ostr, oc->getName(), oc);
   class Namespace *sns = resolveNameScope(n);
   if (!sns)
      delete oc;
   else
      if (sns->classList->find(oc->getName()))
      {
	 parse_error("class '%s' already exists in namespace '%s'", oc->getName(), name);
	 delete oc;
      }
      else if (sns->pendClassList->add(oc))
      {
	 parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), name);
	 delete oc;
      }
}

void Namespace::addNamespace(class Namespace *ns)
{
   // raise an exception if namespace collides with an object name
   if (classList->find(ns->name))
   {
      parse_error("namespace name '%s' collides with previously-defined class '%s'", 
		  ns->name, ns->name);
      delete ns;
      return;
   }
   if (pendClassList->find(ns->name))
   {
      parse_error("namespace name '%s' collides with pending class '%s'", 
		  ns->name, ns->name);
      delete ns;
      return;
   }
   pendNSL->add(ns);
}

void Namespace::parseInitConstants()
{
   printd(5, "Namespace::parseInitConstants() %s\n", name);
   // do 2nd stage parse initialization on pending constants
   pendConstant->parseInit();

   pendNSL->parseInitConstants();
}

void Namespace::parseInit()
{
   printd(5, "Namespace::parseInit() this=%08p\n", this);

   // do 2nd stage parse initialization on committed classes
   classList->parseInit();
   
   // do 2nd stage parse initialization on pending classes
   pendClassList->parseInit();

   // do 2nd stage parse initialization on pending classes in pending lists of subnamespaces
   nsl->parseInit();

   // do 2nd stage parse initialization on pending namespaces
   pendNSL->parseInit();
}

void Namespace::parseCommit()
{
   // merge pending constant list
   constant->assimilate(pendConstant);

   // merge pending classes and commit pending changes to committed classes
   classList->parseCommit(pendClassList);

   // merge pending namespaces and repeat for all subnamespaces
   nsl->parseCommit(pendNSL);
}

void Namespace::parseRollback()
{
   printd(5, "Namespace::parseRollback() %s %08p\n", name, this);

   // delete pending constant list
   pendConstant->reset();

   // delete pending changes to committed classes
   classList->parseRollback();

   // delete pending classes
   pendClassList->reset();

   // delete pending namespaces
   pendNSL->reset();

   // do for all subnamespaces
   nsl->parseRollback();
}

NamespaceList::NamespaceList()
{
   head = tail = NULL;
}

void NamespaceList::deleteAll()
{
   while (head)
   {
      tail = head->next;
      delete head;
      head = tail;
   }
}

NamespaceList::~NamespaceList()
{
   deleteAll();
}

void NamespaceList::assimilate(class NamespaceList *n)
{
   // assimilate target list
   if (tail)
      tail->next = n->head;
   else
      head = n->head;
   if (n->tail)
      tail = n->tail;
   
   // "zero" target list
   n->head = n->tail = NULL;
}

void NamespaceList::reset()
{
   deleteAll();
   head = tail = NULL;
}

void NamespaceList::add(class Namespace *ns)
{
   // if namespace is already registered, then assimilate
   Namespace *ons;
   if ((ons = find(ns->name)))
   {
      ons->assimilate(ns);
      return;
   }
   // otherwise append to list
   if (tail)
      tail->next = ns;
   else
      head = ns;
   tail = ns;
}

class Namespace *NamespaceList::find(char *name)
{
   tracein("NamespaceList::find()");
   printd(5, "NamespaceList::find(%s)\n", name);

   class Namespace *w = head;

   while (w)
   {
      if (!strcmp(name, w->name))
	 break;
      w = w->next;
   }

   printd(5, "NamespaceList::find(%s) returning %08p\n", name, w);
   traceout("NamespaceList::find()");
   return w;
}

class Namespace *Namespace::copy(int po) const
{
   return new Namespace(name, classList->copy(po), constant->copy(), nsl->copy(po));
}

class NamespaceList *NamespaceList::copy(int po)
{
   class NamespaceList *nsl = new NamespaceList();

   class Namespace *w = head;

   while (w)
   {
      nsl->add(w->copy(po));
      w = w->next;
   }

   return nsl;
}

void NamespaceList::parseInitConstants()
{
   class Namespace *w = head;

   while (w)
   {
      w->parseInitConstants();
      w = w->next;
   }
}

void NamespaceList::parseInit()
{
   class Namespace *w = head;

   while (w)
   {
      w->parseInit();
      w = w->next;
   }
}

void NamespaceList::parseCommit(class NamespaceList *l)
{
   assimilate(l);

   class Namespace *w = head;

   while (w)
   {
      w->parseCommit();
      w = w->next;
   }
}

void NamespaceList::parseRollback()
{
   class Namespace *w = head;

   while (w)
   {
      w->parseRollback();
      w = w->next;
   }
}

class Namespace *Namespace::findNamespace(char *nname) const
{
   class Namespace *rv = nsl->find(nname);
   if (!rv)
      rv = pendNSL->find(nname);
   return rv;
}

// public: only called during Qore initialization to setup
// system constant types directly in Qore system namespaces
// FIXME: change to addSystemConstant() to avoid confusion
void Namespace::addConstant(char *cname, class QoreNode *val)
{
   constant->add(cname, val);
}

void Namespace::addInitialNamespace(class Namespace *ns)
{
   nsl->add(ns);
}

int parseInitConstantHash(class Hash *h, int level)
{
   // cannot use an iterator here because we change the hash
   List *keys = h->getKeys();
   class RootNamespace *rns = getRootNS();
   for (int i = 0; i < keys->size(); i++)
   {
      char *k = keys->retrieve_entry(i)->val.String->getBuffer();

      class QoreNode **value = h->getKeyValuePtr(k);

      if (rns->parseInitConstantValue(value, level + 1))
	 return -1;

      // resolve constant references in keys
      if (k[0] == HE_TAG_CONST || k[0] == HE_TAG_SCOPED_CONST)
      {
	 QoreNode *n;
	 if (k[0] == HE_TAG_CONST)
	 {
	    n = new QoreNode(NT_BAREWORD);
	    n->val.c_str = strdup(k + 1);
	 }
	 else
	    n = new QoreNode(new NamedScope(strdup(k + 1)));
	 if (rns->parseInitConstantValue(&n, level + 1))
	 {
	    if (n)
	       n->deref(NULL);
	    return -1;
	 }

	 if (n)
	 {
	    if (n->type != NT_STRING)
	    {
	       QoreNode *t = n;
	       n = n->convert(NT_STRING);
	       // not possible to have an exception here
	       t->deref(NULL);
	    }
	 
	    // reference value for new hash
	    (*value)->ref();
	    // not possible to have an exception here
	    h->setKeyValue(n->val.String->getBuffer(), *value, NULL);
	    // or here
	    h->deleteKey(k, NULL);
	    // or here
	    n->deref(NULL);
	 }
      }
   }
   keys->derefAndDelete(NULL);
   return 0;
}

// NamespaceList::parseResolveNamespace()
// does a recursive breadth-first search to resolve a namespace declaration
class Namespace *NamespaceList::parseResolveNamespace(class NamedScope *name, int *matched)
{
   tracein("NamespaceList::parseResolveNamespace()");

   class Namespace *w = head, *ns = NULL;

   // search first level of all sub namespaces
   while (w)
   {
      if ((ns = w->parseMatchNamespace(name, matched)))
	 break;
      w = w->next;
   }
      //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);

   if (!ns)
   {
      // now search in all sub namespace lists
      w = head;
      while (w)
      {
	 if ((ns = w->nsl->parseResolveNamespace(name, matched)))
	    break;
	 if ((ns = w->pendNSL->parseResolveNamespace(name, matched)))
	    break;
	 //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);
	 w = w->next;
      }
   }

   traceout("NamespaceList::parseResolveNamespace()");
   return ns;
}

// NamespaceList::parseFindConstantValue()
class QoreNode *NamespaceList::parseFindConstantValue(char *cname)
{
   tracein("NamespaceList::parseFindConstantValue()");
   
   class QoreNode *rv = NULL;
   class Namespace *w = head;
   // see if a match can be found at the first level
   while (w)
   {
      if ((rv = w->getConstantValue(cname)))
	 break;
      w = w->next;
   }

   if (!rv) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((rv = w->nsl->parseFindConstantValue(cname)))
	    break;
	 if ((rv = w->pendNSL->parseFindConstantValue(cname)))
	    break;
	 w = w->next;
      }
   }

   traceout("NamespaceList::parseFindConstantValue()");
   return rv;
}

/*
static void showNSL(class NamespaceList *nsl)
{
   printd(5, "showNSL() dumping %08p\n", nsl);
   for (int i = 0; i < nsl->num_namespaces; i++)
      printd(5, "showNSL()  %d: %08p %s (list: %08p)\n", i, nsl->nslist[i], nsl->nslist[i]->name, nsl->nslist[i]->nsl);
}
*/

// NamespaceList::parseFindScopedConstantValue()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
class QoreNode *NamespaceList::parseFindScopedConstantValue(class NamedScope *name, int *matched)
{
   class QoreNode *rv = NULL;

   tracein("NamespaceList::parseFindScopedConstantValue()");
   printd(5, "NamespaceList::parseFindScopedConstantValue(this=%08p) target: %s\n", this, name->ostr);

   //showNSL(this);
   // see if a complete match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((rv = w->parseMatchScopedConstantValue(name, matched)))
	 break;
      w = w->next;
   }

   if (!rv) // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((rv = w->nsl->parseFindScopedConstantValue(name, matched)))
	    break;
	 if ((rv = w->pendNSL->parseFindScopedConstantValue(name, matched)))
	    break;
	 w = w->next;
      }
   }

   traceout("NamespaceList::parseFindScopedConstantValue()");
   return rv;
}

/*
// parseFindOTInNSL()
class QoreClass *parseFindOTInNSL(class NamespaceList *nsl, char *otname)
{
   class QoreClass *ot;
   // see if a match can be found at the first level
   for (int i = 0; i < nsl->num_namespaces; i++)
      if ((ot = nsl->nslist[i]->classList->find(otname)))
	 return ot;

   // check all levels
   for (int i = 0; i < nsl->num_namespaces; i++)
      if ((ot = findOTInNSL(nsl->nslist[i]->nsl, otname)))
	 return ot;

   parse_error("reference to undefined object type '%s'", otname);
   return NULL;
}
*/

// NamespaceList::parseFindScopedClassWithMethod()
// does a recursive breadth-first search to resolve a namespace containing the given class name
// note: is only called with a namespace specifier
class QoreClass *NamespaceList::parseFindScopedClassWithMethod(class NamedScope *name, int *matched) const
{
   QoreClass *oc = NULL;

   // see if a complete match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClassWithMethod(name, matched)))
	 break;
      w = w->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->pendNSL->parseFindScopedClassWithMethod(name, matched)))
	    break;
	 if ((oc = w->nsl->parseFindScopedClassWithMethod(name, matched)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}

// NamespaceList::parseFindScopedClassWithMethod()
// does a recursive breadth-first search to resolve a namespace containing the given class name
// note: is only called with a namespace specifier
/*
class QoreClass *NamespaceList::parseFindScopedClassWithMethod(class NamedScope *name, int *matched, class QoreClassList **plist, bool *is_pending)
{
   QoreClass *oc = NULL;

   // see if a complete match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClassWithMethod(name, matched, plist, is_pending)))
	 break;
      w = w->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->pendNSL->parseFindScopedClassWithMethod(name, matched, plist, is_pending)))
	    break;
	 if ((oc = w->nsl->parseFindScopedClassWithMethod(name, matched, plist, is_pending)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}
*/

/*
class QoreClass *NamespaceList::parseFindClass(char *ocname, class QoreClassList **plist, bool *is_pending)
{
   class QoreClass *oc = NULL;

   // see if a match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      // check pending classes
      if ((oc = w->pendClassList->find(ocname)))
      {
	 (*plist) = w->classList;
	 (*is_pending) = true;
	 break;
      }
      if ((oc = w->classList->find(ocname)))
      {
	 (*plist) = w->pendClassList;
	 break;
      }
      w = w->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->nsl->parseFindClass(ocname, plist, is_pending)))
	    break;
	 if ((oc = w->pendNSL->parseFindClass(ocname, plist, is_pending)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}
*/

class QoreClass *NamespaceList::parseFindClass(char *ocname)
{
   class QoreClass *oc = NULL;

   // see if a match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->classList->find(ocname)))
	 break;
      // check pending classes
      if ((oc = w->pendClassList->find(ocname)))
	 break;

      w = w->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->nsl->parseFindClass(ocname)))
	    break;
	 if ((oc = w->pendNSL->parseFindClass(ocname)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}

class QoreClass *NamespaceList::parseFindChangeClass(char *ocname)
{
   class QoreClass *oc = NULL;

   // see if a match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->classList->findChange(ocname)))
	 break;
      // check pending classes
      if ((oc = w->pendClassList->find(ocname)))
	 break;

      w = w->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->nsl->parseFindChangeClass(ocname)))
	    break;
	 if ((oc = w->pendNSL->parseFindClass(ocname)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}

// NamespaceList::parseFindScopedClass()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
class QoreClass *NamespaceList::parseFindScopedClass(class NamedScope *name, int *matched)
{
   class QoreClass *oc = NULL;

   // see if a complete match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClass(name, matched)))
	 break;
      w = w->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->nsl->parseFindScopedClass(name, matched)))
	    break;
	 if ((oc = w->pendNSL->parseFindScopedClass(name, matched)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}

class QoreNode *get_file_constant(class QoreClass *fc, int fd)
{
   class ExceptionSink xsink;

   class QoreNode *rv = fc->execSystemConstructor(NULL, &xsink);
   class File *f = (File *)rv->val.object->getReferencedPrivateData(CID_FILE);
   f->makeSpecial(fd);
   f->deref();

   return rv;
}

// returns 0 for success, non-zero return value means error
int RootNamespace::addMethodToClass(class NamedScope *name, class Method *qcmethod, class BCAList *bcal)
{
   // find class
   //class QoreClassList *plist;
   //bool is_pending = false;
   class QoreClass *oc;

   char *cname  = name->strlist[name->elements - 2];
   char *method = name->strlist[name->elements - 1];

   // if there is no namespace specified, then just find class
   if (name->elements == 2)
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
      oc = rootFindScopedClassWithMethod(name, &m);
      if (!oc)
      {
	 if (m != (name->elements - 2))
	    parse_error("cannot resolve namespace '%s' in '%s()'", name->strlist[m], name->ostr);
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

class QoreClass *RootNamespace::parseFindClass(char *name) const
{
   class QoreClass *oc = rootFindClass(name);
   if (!oc)
      parse_error("reference to undefined class '%s'", name);

   return oc;
}

class QoreClass *RootNamespace::parseFindScopedClass(class NamedScope *name) const
{
   class QoreClass *oc;
   // if there is no namespace specified, then just find class
   if (name->elements == 1)
   {
      oc = rootFindClass(name->strlist[0]);
      if (!oc)
	 parse_error("reference to undefined class '%s'", name->ostr);
   }
   else
   {
      int m = 0;
      oc = rootFindScopedClass(name, &m);

      if (!oc)
	 if (m != (name->elements - 1))
	    parse_error("cannot resolve namespace '%s' in '%s()'", name->strlist[m], name->ostr);
	 else
	 {
	    QoreString err;
	    err.sprintf("cannot find class '%s' in any namespace '", name->getIdentifier());
	    for (int i = 0; i < (name->elements - 1); i++)
	    {
	       err.concat(name->strlist[i]);
	       if (i != (name->elements - 2))
		  err.concat("::");
	    }
	    err.concat("'");
	    parse_error(err.getBuffer());
	 }
   }
   printd(5, "RootNamespace::parseFindScopedClass('%s') returning %08p\n", name->ostr, oc);
   return oc;
}

class QoreClass *RootNamespace::parseFindScopedClassWithMethod(class NamedScope *name) const
{
   class QoreClass *oc;

   int m = 0;
   oc = rootFindScopedClassWithMethod(name, &m);
   
   if (!oc)
      if (m != (name->elements - 1))
	 parse_error("cannot resolve namespace '%s' in '%s()'", name->strlist[m], name->ostr);
      else
      {
	 QoreString err;
	 err.sprintf("cannot find class '%s' in any namespace '", name->getIdentifier());
	 for (int i = 0; i < (name->elements - 1); i++)
	 {
	    err.concat(name->strlist[i]);
	    if (i != (name->elements - 2))
	       err.concat("::");
	 }
	 err.concat("'");
	 parse_error(err.getBuffer());
      }
   
   printd(5, "RootNamespace::parseFindScopedClassWithMethod('%s') returning %08p\n", name->ostr, oc);
   return oc;
}

// returns 0 for success, non-zero for error
int RootNamespace::parseInitConstantValue(class QoreNode **val, int level)
{
   if (!(*val))
      return 0;

   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH)
   {
      parse_error("maximum recursion level exceeded resolving constant definition");
      return -1;
   }

   while (true)
   {
      if ((*val)->type == NT_BAREWORD)
      {
	 if (resolveSimpleConstant(val, level + 1))
	    return -1;
      }
      else if ((*val)->type == NT_CONSTANT)
      {
	 if (resolveScopedConstant(val, level + 1))
	    return -1;
      }
      else
	 break;
   }
   if ((*val)->type == NT_LIST)
      for (int i = 0; i < (*val)->val.list->size(); i++)
      {
	 if (parseInitConstantValue((*val)->val.list->get_entry_ptr(i), level + 1))
	    return -1;
      }
   else if ((*val)->type == NT_HASH)
   {
      if (parseInitConstantHash((*val)->val.hash, level))
	 return -1;
   }
   else if ((*val)->type == NT_TREE)
   {
      if (parseInitConstantValue(&((*val)->val.tree->left), level + 1))
	 return -1;
      if ((*val)->val.tree->right)
	 if (parseInitConstantValue(&((*val)->val.tree->right), level + 1))
	    return -1;
   }
   // if it's an expression or container type, then evaluate in case it contains immediate expressions
   if ((*val)->type == NT_TREE || (*val)->type == NT_LIST || (*val)->type == NT_HASH)
   {
      class ExceptionSink xsink;
      class QoreNode *n = (*val)->eval(&xsink);
      (*val)->deref(&xsink);
      *val = n;
   }
   return 0;
}

// returns 0 for success, non-zero for error
int RootNamespace::resolveSimpleConstant(class QoreNode **node, int level) const
{
   printd(5, "RootNamespace::resolveSimpleConstant(%s, %d)\n", (*node)->val.c_str, level);

   // if constant is not found, then a parse error will be raised
   class QoreNode *rv = findConstantValue((*node)->val.c_str, level);
   if (!rv)
      return -1;

   printd(5, "RootNamespace::resolveSimpleConstant(%s, %d) %08p %s-> %08p %s\n", 
	  (*node)->val.c_str, level, *node, (*node)->type->getName(), rv, rv->type->getName());
   
   (*node)->deref(NULL);
   *node = rv->RefSelf();
   return 0;
}

int RootNamespace::resolveScopedConstant(class QoreNode **node, int level) const
{
   printd(5, "resolveScopedConstant(%s, %d)\n", (*node)->val.scoped_ref->ostr, level);

   // if constant is not found, then a parse error will be raised
   class QoreNode *rv = findConstantValue((*node)->val.scoped_ref, level);
   if (!rv)
      return -1;

   (*node)->deref(NULL);
   *node = rv->RefSelf();
   return 0;
}

class QoreNode *RootNamespace::findConstantValue(char *name, int level) const
{
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH)
   {
      parse_error("recursive constant definitions too deep resolving '%s'", name);
      return NULL;
   }

   class QoreNode *rv = rootFindConstantValue(name);
   if (!rv)
      parse_error("constant '%s' cannot be resolved in any namespace", name);
   return rv;
}

// called in 2nd stage of parsing to resolve constant references
class QoreNode *RootNamespace::findConstantValue(class NamedScope *name, int level) const
{
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH)
   {
      parse_error("recursive constant definitions too deep resolving '%s'", name->ostr);
      return NULL;
   }

   class QoreNode *rv;

   if (name->elements == 1)
   {
      rv = rootFindConstantValue(name->ostr);
      if (!rv)
      {
	 parse_error("constant '%s' cannot be resolved in any namespace", name->ostr);
	 return NULL;
      }
   }
   else
   {
      int m = 0;
      rv = rootFindScopedConstantValue(name, &m);
      if (!rv)
      {
	 if (m != (name->elements - 1))
	    parse_error("cannot resolve namespace '%s' in '%s'", name->strlist[m], name->ostr);
	 else
	 {
	    QoreString err;
	    err.sprintf("cannot find constant '%s' in any namespace '", name->getIdentifier());
	    for (int i = 0; i < (name->elements - 1); i++)
	    {
	       err.concat(name->strlist[i]);
	       if (i != (name->elements - 2))
		  err.concat("::");
	    }
	    err.concat("'");
	    parse_error(err.getBuffer());
	 }
	 return NULL;
      }
   }
   return rv;
}

// public, only called either in single-threaded initialization or
// while the program-level parse lock is held
void Namespace::addClass(class QoreClass *oc)
{
   tracein("Namespace::addClass()");
   //printd(5, "Namespace::addClass() adding str=%s (%08p)\n", oc->name, oc);
   // raise an exception if object name collides with a namespace
   if (nsl->find(oc->getName()))
   {
      parse_error("class name '%s' collides with previously-defined namespace '%s'", oc->getName(), oc->getName());
      delete oc;
   }
   else if (pendNSL->find(oc->getName()))
   {
      parse_error("class name '%s' collides with pending namespace '%s'", oc->getName(), oc->getName());
      delete oc;
   }
   else if (classList->find(oc->getName()))
   {
      parse_error("class '%s' already exists in namespace '%s'", oc->getName(), name);
      delete oc;
   }
   else if (pendClassList->add(oc))
   {
      parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), name);
      delete oc;
   }
   traceout("Namespace::addClass()");
}

void Namespace::assimilate(class Namespace *ns)
{
   // assimilate pending constants
   // assimilate target list - if there were errors then the list will be deleted anyway
   pendConstant->assimilate(ns->pendConstant, constant, name);

   // assimilate classes
   pendClassList->assimilate(ns->pendClassList, classList, nsl, pendNSL, name);

   // assimilate sub namespaces
   Namespace *nw = ns->pendNSL->head;
   while (nw)
   {
      // throw parse exception if name is already defined
      if (nsl->find(nw->name))
	 parse_error("subnamespace '%s' has already been defined in namespace '%s'",
		     nw->name, name);
      else if (pendNSL->find(nw->name))
	 parse_error("subnamespace '%s' is already pending in namespace '%s'",
		     nw->name, name);
      else if (classList->find(nw->name))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class has already been defined with this name",
		     nw->name, name);
      else if (pendClassList->find(nw->name))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class is already pending with this name",
		     nw->name, name);

      nw = nw->next;
   }
   // assimilate target list
   pendNSL->assimilate(ns->pendNSL);

   // delete source namespace
   delete ns;
}

// Namespace::parseMatchNamespace()
// will only be called if there is a match with the name and nscope->elements > 1
class Namespace *Namespace::parseMatchNamespace(class NamedScope *nscope, int *matched) const
{
   // see if starting name matches this namespace
   if (!strcmp(nscope->strlist[0], name))
   {
      const class Namespace *ns = this;

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
      return (Namespace *)ns;
   }

   return NULL;
}

/*
class QoreClass *Namespace::parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched, class QoreClassList **plist, bool *is_pending) const
{
   printd(5, "Namespace::parseMatchScopedClassWithMethod(this=%08p) %s class=%s (%s)\n", this, name, nscope->strlist[nscope->elements - 2], nscope->ostr);

   Namespace *ns = this;
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      // if first namespace doesn't match, then return NULL
      if (strcmp(nscope->strlist[0], name))
	 return NULL;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (int i = 1; i < (nscope->elements - 2); i++)
      {	 
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return NULL;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   // check last namespaces
   class QoreClass *rv = ns->pendClassList->find(nscope->strlist[nscope->elements - 2]);
   if (rv)
   {
      (*plist) = ns->classList;
      (*is_pending) = true;
   }
   else if ((rv = ns->classList->find(nscope->strlist[nscope->elements - 2])))
   {
      (*plist) = ns->pendClassList;
   }
   return rv;
}
*/

class QoreClass *Namespace::parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched) const
{
   printd(5, "Namespace::parseMatchScopedClassWithMethod(this=%08p) %s class=%s (%s)\n", this, name, nscope->strlist[nscope->elements - 2], nscope->ostr);

   const Namespace *ns = this;
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      // if first namespace doesn't match, then return NULL
      if (strcmp(nscope->strlist[0], name))
	 return NULL;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (int i = 1; i < (nscope->elements - 2); i++)
      {	 
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return NULL;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   // check last namespaces
   class QoreClass *rv = ns->pendClassList->find(nscope->strlist[nscope->elements - 2]);
   if (!rv)
      rv = ns->classList->find(nscope->strlist[nscope->elements - 2]);

   return rv;
}

class QoreClass *Namespace::parseMatchScopedClass(class NamedScope *nscope, int *matched) const
{
   if (strcmp(nscope->strlist[0], name))
      return NULL;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   printd(5, "Namespace::parseMatchScopedClass() matched %s in %s\n", name, nscope->ostr);

   const Namespace *ns = this;
   
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      for (int i = 1; i < (nscope->elements - 1); i++)
      {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return NULL;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   class QoreClass *rv = ns->classList->find(nscope->strlist[nscope->elements - 1]);
   if (!rv)
      rv = ns->pendClassList->find(nscope->strlist[nscope->elements - 1]);
   return rv;
}

class QoreNode *Namespace::parseMatchScopedConstantValue(class NamedScope *nscope, int *matched) const
{
   printd(5, "Namespace::parseMatchScopedConstantValue() trying to find %s in %s (%08p)\n", 
	  nscope->getIdentifier(), name, getConstantValue(nscope->getIdentifier()));

   if (strcmp(nscope->strlist[0], name))
      return NULL;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   const class Namespace *ns = this;

   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
      for (int i = 1; i < (nscope->elements - 1); i++)
      {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return NULL;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }

   return ns->getConstantValue(nscope->getIdentifier());
}

class Hash *Namespace::getConstantInfo() const
{
   return constant->getInfo();
}

class Hash *Namespace::getClassInfo() const
{
   return classList->getInfo();
}

// returns a hash of namespace information
class Hash *Namespace::getInfo() const
{
   class Hash *h = new Hash();

   h->setKeyValue("constants", new QoreNode(getConstantInfo()), NULL);
   h->setKeyValue("classes", new QoreNode(getClassInfo()), NULL);

   if (nsl->head)
   {
      class Hash *nsh = new Hash();
      
      class Namespace *w = nsl->head;
      while (w)
      {
	 nsh->setKeyValue(w->name, new QoreNode(w->getInfo()), NULL);
	 w = w->next;
      }

      h->setKeyValue("subnamespaces", new QoreNode(nsh), NULL);
   }

   return h;
}

void Namespace::setName(char *nme)
{
   assert(!name);
   name = nme;
}

// only called with RootNS
class QoreNode *RootNamespace::rootFindConstantValue(char *cname) const
{
   class QoreNode *rv;
   if (!(rv = getConstantValue(cname))
       && (!(rv = nsl->parseFindConstantValue(cname))))
      rv = pendNSL->parseFindConstantValue(cname);
   return rv;
}

// only called with RootNS
void RootNamespace::rootAddClass(class NamedScope *nscope, class QoreClass *oc)
{
   tracein("RootNamespace::rootAddClass()");

   class Namespace *sns = rootResolveNamespace(nscope);

   if (sns)
   {
      printd(5, "RootNamespace::rootAddClass() '%s' adding %s:%08p to %s:%08p\n", nscope->ostr, 
	     oc->getName(), oc, sns->name, sns);
      sns->addClass(oc);
   }
   else
      delete oc;

   traceout("RootNamespace::rootAddClass()");
}

void RootNamespace::rootAddConstant(class NamedScope *nscope, class QoreNode *value)
{
   class Namespace *sns = rootResolveNamespace(nscope);

   if (sns)
   {
      printd(5, "RootNamespace::rootAddConstant() %s: adding %s to %s (value=%08p type=%s)\n", nscope->ostr, 
	     nscope->getIdentifier(), sns->name, value, value ? value->type->getName() : "(none)");
      sns->pendConstant->add(nscope->strlist[nscope->elements - 1], value);
   }
   else
      value->deref(NULL);
}

// public
class QoreClass *RootNamespace::rootFindClass(char *ocname) const
{
   tracein("RootNamespace::rootFindClass");
   QoreClass *oc;
   if (!(oc = classList->find(ocname))
       && !(oc = pendClassList->find(ocname))
       && !(oc = nsl->parseFindClass(ocname)))
      oc = pendNSL->parseFindClass(ocname);
   traceout("RootNamespace::rootFindClass");
   return oc;
}

class QoreClass *RootNamespace::rootFindChangeClass(char *ocname)
{
   tracein("RootNamespace::rootFindChangeClass");
   QoreClass *oc;
   if (!(oc = classList->findChange(ocname))
       && !(oc = pendClassList->find(ocname))
       && !(oc = nsl->parseFindChangeClass(ocname)))
      oc = pendNSL->parseFindClass(ocname);
   traceout("RootNamespace::rootFindChangeClass");
   return oc;
}

/*
// public
class QoreClass *RootNamespace::rootFindClass(char *ocname, class QoreClassList **plist, bool *is_pending)
{
   tracein("RootNamespace::rootFindClass()");

   QoreClass *oc;
   if ((oc = pendClassList->find(ocname)))
   {
      (*plist) = classList;
      (*is_pending) = true;
   }
   else if ((oc = classList->find(ocname)))
      (*plist) = pendClassList;
   else if (!(oc = nsl->parseFindClass(ocname, plist, is_pending)))
      oc = pendNSL->parseFindClass(ocname, plist, is_pending);

   traceout("RootNamespace::rootFindClass()");
   return oc;
}
*/

class Namespace *RootNamespace::rootResolveNamespace(class NamedScope *nscope)
{
   if (nscope->elements == 1)
      return this;

   Namespace *ns;
   int match = 0;

   if (!(ns = parseMatchNamespace(nscope, &match))
       && !(ns = nsl->parseResolveNamespace(nscope, &match))
       && !(ns = pendNSL->parseResolveNamespace(nscope, &match)))

   if (!ns)
      parse_error("cannot resolve namespace '%s' in '%s'", nscope->strlist[match], nscope->ostr);

   return ns;
}

/*
// public
class QoreClass *RootNamespace::rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched, class QoreClassList **plist, bool *is_pending)
{
   QoreClass *oc;

   if (!(oc = parseMatchScopedClassWithMethod(nscope, matched, plist, is_pending))
       && !(oc = nsl->parseFindScopedClassWithMethod(nscope, matched, plist, is_pending)))
      oc = pendNSL->parseFindScopedClassWithMethod(nscope, matched, plist, is_pending);
   return oc;
}
*/

// public
class QoreClass *RootNamespace::rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched) const
{
   QoreClass *oc;

   if (!(oc = parseMatchScopedClassWithMethod(nscope, matched))
       && !(oc = nsl->parseFindScopedClassWithMethod(nscope, matched)))
      oc = pendNSL->parseFindScopedClassWithMethod(nscope, matched);
   return oc;
}

// public
// will always be called with a namespace (nscope->elements > 1)
class QoreClass *RootNamespace::rootFindScopedClass(class NamedScope *nscope, int *matched) const
{
   QoreClass *oc = parseMatchScopedClass(nscope, matched);
   if (!oc && !(oc = nsl->parseFindScopedClass(nscope, matched)))
      oc = pendNSL->parseFindScopedClass(nscope, matched);
   return oc;
}

// public, will always be called with nscope->elements > 1
class QoreNode *RootNamespace::rootFindScopedConstantValue(class NamedScope *nscope, int *matched) const
{
   class QoreNode *rv = parseMatchScopedConstantValue(nscope, matched);
   if (!rv && !(rv = nsl->parseFindScopedConstantValue(nscope, matched)))
      rv = pendNSL->parseFindScopedConstantValue(nscope, matched);
   return rv;
}

void RootNamespace::addQoreNamespace(class Namespace *qns)
{
   addInitialNamespace(qns);
   qoreNS = qns;
}

// sets up the root namespace
RootNamespace::RootNamespace(class Namespace **QoreNS) : Namespace()
{
   tracein("RootNamespace::RootNamespace");

   name = "";

   class Namespace *qns = new Namespace("Qore");

   class QoreClass *File;
   // add system object types
   qns->addSystemClass(initSocketClass());
   qns->addSystemClass(initSSLCertificateClass());
   qns->addSystemClass(initSSLPrivateKeyClass());
   qns->addSystemClass(initProgramClass());
   qns->addSystemClass(File = initFileClass());
   qns->addSystemClass(initGetOptClass());
   qns->addSystemClass(initFtpClientClass());
   //qns->addSystemClass(initHTTPClientClass());

#ifdef DEBUG
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
   qns->addConstant("stdin",         get_file_constant(File, 0));
   qns->addConstant("stdout",        get_file_constant(File, 1));
   qns->addConstant("stderr",        get_file_constant(File, 2));

   // add constants for exception types
   qns->addConstant("ET_System",     new QoreNode("System"));
   qns->addConstant("ET_User",       new QoreNode("User"));

   // create constants for call types
   qns->addConstant("CT_User",       new QoreNode((int64)CT_USER));
   qns->addConstant("CT_Builtin",    new QoreNode((int64)CT_BUILTIN));
   qns->addConstant("CT_NewThread",  new QoreNode((int64)CT_NEWTHREAD));
   qns->addConstant("CT_Rethrow",    new QoreNode((int64)CT_RETHROW));

   // create constants for version and platform information
   qns->addConstant("VersionString", new QoreNode(qore_version_string));
   qns->addConstant("VersionMajor",  new QoreNode((int64)qore_version_major));
   qns->addConstant("VersionMinor",  new QoreNode((int64)qore_version_minor));
   qns->addConstant("VersionSub",    new QoreNode((int64)qore_version_sub));
   qns->addConstant("Build",         new QoreNode((int64)qore_build_number));
   qns->addConstant("PlatformCPU",   new QoreNode(TARGET_ARCH));
   qns->addConstant("PlatformOS",    new QoreNode(TARGET_OS));

   // add constants for regex() function options
   qns->addConstant("RE_Caseless",   new QoreNode((int64)PCRE_CASELESS));
   qns->addConstant("RE_DotAll",     new QoreNode((int64)PCRE_DOTALL));
   qns->addConstant("RE_Extended",   new QoreNode((int64)PCRE_EXTENDED));
   qns->addConstant("RE_MultiLine",  new QoreNode((int64)PCRE_MULTILINE));
   // note that the following constant is > 32-bits so it can't collide with PCRE constants
   qns->addConstant("RE_Global",     new QoreNode((int64)QRE_GLOBAL));

   // create Qore::SQL namespace
   qns->addInitialNamespace(getSQLNamespace());

   // create get Qore::Err namespace with ERRNO constants
   qns->addInitialNamespace(get_errno_ns());

   // create Qore::Type namespace with type constants
   qns->addInitialNamespace(get_type_ns());
   // add HTTPClient constants
   //qns->addInitialNamespace(addHTTPClientNamespace());

   // add file constants
   addFileConstants(qns);

   // add parse option constants to Qore namespace
   addProgramConstants(qns);


   addQoreNamespace(qns);

   // add all changes in loaded modules
   ANSL.init(this, qns);

   *QoreNS = qns;
   traceout("RootNamespace::RootNamespace");
}

// private constructor
RootNamespace::RootNamespace(QoreClassList *ocl, ConstantList *cl, NamespaceList *nnsl) : Namespace(ocl, cl, nnsl)
{
   qoreNS = nsl->find("Qore");
}

RootNamespace::~RootNamespace()
{
   name = NULL;
}

class Namespace *RootNamespace::rootGetQoreNamespace() const
{
   return qoreNS;
}

class RootNamespace *RootNamespace::copy(int po) const
{
   return new RootNamespace(classList->copy(po), constant->copy(), nsl->copy(po));
}

#ifdef DEBUG
// moved down to allow to test internal classes
#  include "tests/Namespace_tests.cc"
#endif

