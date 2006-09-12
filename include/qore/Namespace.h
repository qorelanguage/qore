/*
  Namespace.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  namespaces are children of a program object.  there is a parse
  lock per program object to ensure that objects are added (or backed out)
  atomically per program object.  All the objects referenced here should 
  be safe to read & copied at all times.  They will only be deleted when the
  program object is deleted (except the pending structures, which will be
  deleted any time there is a parse error, together with all other
  pending structures)

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

#ifndef QORE_NAMESPACE_H

#define QORE_NAMESPACE_H

#include <string.h>
#include <stdlib.h>

#define MAX_RECURSION_DEPTH 20

class Namespace *getRootNamespace(class Namespace **QoreNS);
int resolveSimpleConstant(class QoreNode **, int level);
int resolveScopedConstant(class QoreNode **, int level);
int parseInitConstantValue(class QoreNode **, int level);
int addMethodToClass(class NamedScope *name, class Method *qcmethod);
class QoreClass *parseFindScopedClass(class NamedScope *name);
class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name);
class QoreNode *findConstantValue(class NamedScope *name, int level);
static inline class QoreClass *parseFindClass(char *name);

class Namespace
{
      friend class NamespaceList;

  private:
      class ConstantList   *constant;
      class QoreClassList  *classList;
      class NamespaceList  *nsl;
      class Namespace      *next;

      // pending lists
      class ConstantList   *pendConstant;
      class QoreClassList  *pendClassList;
      class NamespaceList  *pendNSL;

      class QoreNode *parseMatchScopedConstantValue(class NamedScope *name, int *matched);
      class QoreClass *parseMatchScopedClass(class NamedScope *name, int *matched);
      //class QoreClass *parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched, class QoreClassList **plist, bool *is_pending);
      class QoreClass *parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched);
      class Namespace *parseMatchNamespace(class NamedScope *nscope, int *matched);
      inline class Namespace *findNamespace(char *name);
      inline class Namespace *resolveNameScope(class NamedScope *name);
      inline class QoreNode *getConstantValue(char *name);
      inline class Namespace *rootResolveNamespace(class NamedScope *nscope);
      void assimilate(class Namespace *ns);
      inline void init(char *n = NULL);

   public:
      char *name;

      inline Namespace(char *n = NULL);
      inline Namespace(char *n, QoreClassList *ocl, ConstantList *cl, NamespaceList *nnsl);
      inline ~Namespace();

      inline void addConstant(class NamedScope *name, class QoreNode *value);
      inline void addConstant(char *name, class QoreNode *value);
      inline void addSystemClass(class QoreClass *oc);
      void addClass(class QoreClass *oc);
      inline void addClass(class NamedScope *n, class QoreClass *oc);
      inline void addNamespace(class Namespace *ns);
      inline void addInitialNamespace(class Namespace *ns);
      inline class Namespace *copy(int po = 0);

      inline void parseInitConstants();
      inline void parseInit();
      inline void parseRollback();
      inline void parseCommit();
      inline class QoreClass *rootFindClass(char *name);
      inline class QoreClass *rootFindChangeClass(char *name);
      //inline class QoreClass *rootFindClass(char *name, class QoreClassList **plist, bool *is_pending);
      inline class QoreNode *rootFindConstantValue(char *name);
      inline class QoreNode *rootFindScopedConstantValue(class NamedScope *name, int *matched);
      inline class QoreClass *rootFindScopedClass(class NamedScope *name, int *matched);
      inline class QoreClass *rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched);
      //inline class QoreClass *rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched, class QoreClassList **plist, bool *is_pending);
      inline void rootAddClass(class NamedScope *name, class QoreClass *oc);
      inline void rootAddConstant(class NamedScope *name, class QoreNode *value);
      inline class Namespace *rootGetQoreNamespace();

      // info
      class Hash *getClassInfo();
      class Hash *getConstantInfo();
      class Hash *getInfo();
};

class NamespaceList
{
   private:
      inline void deleteAll();

   public:
      class Namespace *head, *tail;

      inline NamespaceList();
      inline ~NamespaceList();

      inline void add(class Namespace *ot);
      inline class Namespace *find(char *name);
      inline class NamespaceList *copy(int po);
      inline void parseInitConstants();
      inline void parseInit();
      inline void parseCommit(class NamespaceList *n);
      inline void parseRollback();
      inline void reset();
      inline void assimilate(class NamespaceList *n);

      class Namespace *parseResolveNamespace(class NamedScope *name, int *matched);
      class QoreNode *parseFindConstantValue(char *cname);
      class QoreNode *parseFindScopedConstantValue(class NamedScope *name, int *matched);
      //class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name, int *matched, class QoreClassList **plist, bool *is_pending);
      class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name, int *matched);
      class QoreClass *parseFindScopedClass(class NamedScope *name, int *matched);
      class QoreClass *parseFindClass(char *ocname);
      class QoreClass *parseFindChangeClass(char *ocname);
      //class QoreClass *parseFindClass(char *ocname, class QoreClassList **plist, bool *is_pending);
};

#include <qore/NamedScope.h>
#include <qore/QoreNode.h>
#include <qore/QoreClassList.h>
#include <qore/ConstantList.h>
#include <qore/support.h>
#include <qore/thread.h>
#include <qore/QoreProgram.h>

inline void Namespace::init(char *n)
{
   //printd(5, "Namespace::Namespace(%s)\n", n);
   next = NULL;
   if (n)
      name    = strdup(n);
   else
      name    = NULL;

   pendConstant  = new ConstantList();
   pendClassList = new QoreClassList();
   pendNSL       = new NamespaceList();
}

inline Namespace::Namespace(char *n)
{
   init(n);
   classList = new QoreClassList();
   constant   = new ConstantList();
   nsl        = new NamespaceList();
}

inline Namespace::Namespace(char *n, QoreClassList *ocl, ConstantList *cl, NamespaceList *nnsl)
{
   init(n);
   classList = ocl;
   constant   = cl;
   nsl        = nnsl;
}

inline Namespace::~Namespace()
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

inline class Namespace *Namespace::rootGetQoreNamespace()
{
   return nsl->find("Qore");
}

// private function
inline class Namespace *Namespace::resolveNameScope(class NamedScope *nscope)
{
   class Namespace *sns = this;

   // find namespace
   for (int i = 0; i < (nscope->elements - 1); i++)
      if (!(sns = sns->findNamespace(nscope->strlist[i])))
      {
	 parse_error("namespace '%s' cannot be resolved while evaluating '%s' in constant declaration",
		     nscope->strlist[i], nscope->ostr);
	 return NULL;
      }
   return sns;
}

// public: only called during Qore initialization to setup
// system constant types directly in Qore system namespaces
// FIXME: change to addInitConstant() to avoid confusion
inline void Namespace::addConstant(char *cname, class QoreNode *val)
{
   constant->add(cname, val);
}

// only called while parsing before addition to namespace tree, no locking needed
inline void Namespace::addConstant(class NamedScope *nscope, class QoreNode *value)
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

// private function
inline class QoreNode *Namespace::getConstantValue(char *cname)
{
   class QoreNode *rv = constant->find(cname);
   if (!rv)
      rv = pendConstant->find(cname);

   return rv ? rv : NULL;
}

// only called with RootNS
inline class QoreNode *Namespace::rootFindConstantValue(char *cname)
{
   class QoreNode *rv;
   if (!(rv = getConstantValue(cname))
       && (!(rv = nsl->parseFindConstantValue(cname))))
      rv = pendNSL->parseFindConstantValue(cname);
   return rv;
}

// public method
inline class Namespace *Namespace::copy(int po)
{
   //printd(0, "Namespace::copy() %s: %08p\n", name, this);
   return new Namespace(name, classList->copy(po), constant->copy(), nsl->copy(po));
}

// public, only called in single-threaded initialization
inline void Namespace::addSystemClass(class QoreClass *oc)
{
   tracein("Namespace::addSystemClass()");
#ifdef DEBUG
   if (classList->add(oc))
      run_time_error("Namespace::addSystemClass() %s %08p already exists in %s", oc->getName(), oc, name);
#else
   classList->add(oc);
#endif
   traceout("Namespace::addSystemClass()");
}

// public, only called when parsing for unattached namespaces
inline void Namespace::addClass(class NamedScope *n, class QoreClass *oc)
{
   //printd(5, "Namespace::addClass() adding ns=%s (%s, %08p)\n", n->ostr, oc->getName(), oc);
   class Namespace *sns = resolveNameScope(n);
   if (!sns)
      oc->deref();
   else
      if (sns->classList->find(oc->getName()))
      {
	 parse_error("class '%s' already exists in namespace '%s'", oc->getName(), name);
	 oc->deref();
      }
      else if (sns->pendClassList->add(oc))
      {
	 parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), name);
	 oc->deref();
      }
}

// only called with RootNS
inline void Namespace::rootAddClass(class NamedScope *nscope, class QoreClass *oc)
{
   tracein("Namespace::rootAddClass()");

   class Namespace *sns = rootResolveNamespace(nscope);

   if (sns)
   {
      printd(5, "Namespace::rootAddClass() '%s' adding %s:%08p to %s:%08p\n", nscope->ostr, 
	     oc->getName(), oc, sns->name, sns);
      sns->addClass(oc);
   }
   else
      oc->deref();

   traceout("Namespace::rootAddClass()");
}

inline void Namespace::rootAddConstant(class NamedScope *nscope, class QoreNode *value)
{
   class Namespace *sns = rootResolveNamespace(nscope);

   if (sns)
   {
      printd(5, "Namespace::rootAddConstant() %s: adding %s to %s (value=%08p type=%s)\n", nscope->ostr, 
	     nscope->getIdentifier(), sns->name, value, value ? value->type->name : "(none)");
      sns->pendConstant->add(nscope->strlist[nscope->elements - 1], value);
   }
   else
      value->deref(NULL);
}

// public
inline class QoreClass *Namespace::rootFindClass(char *ocname)
{
   tracein("Namespace::rootFindClass");
   QoreClass *oc;
   if (!(oc = classList->find(ocname))
       && !(oc = pendClassList->find(ocname))
       && !(oc = nsl->parseFindClass(ocname)))
      oc = pendNSL->parseFindClass(ocname);
   traceout("Namespace::rootFindClass");
   return oc;
}

inline class QoreClass *Namespace::rootFindChangeClass(char *ocname)
{
   tracein("Namespace::rootFindChangeClass");
   QoreClass *oc;
   if (!(oc = classList->findChange(ocname))
       && !(oc = pendClassList->find(ocname))
       && !(oc = nsl->parseFindChangeClass(ocname)))
      oc = pendNSL->parseFindClass(ocname);
   traceout("Namespace::rootFindChangeClass");
   return oc;
}

// public
/*
inline class QoreClass *Namespace::rootFindClass(char *ocname, class QoreClassList **plist, bool *is_pending)
{
   tracein("Namespace::rootFindClass()");

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

   traceout("Namespace::rootFindClass()");
   return oc;
}
*/
inline void Namespace::addInitialNamespace(class Namespace *ns)
{
   nsl->add(ns);
}

inline void Namespace::addNamespace(class Namespace *ns)
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

inline void Namespace::parseInitConstants()
{
   printd(5, "Namespace::parseInitConstants() %s\n", name);
   // do 2nd stage parse initialization on pending constants
   pendConstant->parseInit();

   pendNSL->parseInitConstants();
}

inline void Namespace::parseInit()
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

inline void Namespace::parseCommit()
{
   // merge pending constant list
   constant->assimilate(pendConstant);

   // merge pending classes and commit pending changes to committed classes
   classList->parseCommit(pendClassList);

   // merge pending namespaces and repeat for all subnamespaces
   nsl->parseCommit(pendNSL);
}

inline void Namespace::parseRollback()
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

inline NamespaceList::NamespaceList()
{
   head = tail = NULL;
}

inline void NamespaceList::deleteAll()
{
   while (head)
   {
      tail = head->next;
      delete head;
      head = tail;
   }
}

inline NamespaceList::~NamespaceList()
{
   deleteAll();
}

inline void NamespaceList::assimilate(class NamespaceList *n)
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

inline void NamespaceList::reset()
{
   deleteAll();
   head = tail = NULL;
}

inline void NamespaceList::add(class Namespace *ns)
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

inline class Namespace *NamespaceList::find(char *name)
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

inline class NamespaceList *NamespaceList::copy(int po)
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

inline void NamespaceList::parseInitConstants()
{
   class Namespace *w = head;

   while (w)
   {
      w->parseInitConstants();
      w = w->next;
   }
}

inline void NamespaceList::parseInit()
{
   class Namespace *w = head;

   while (w)
   {
      w->parseInit();
      w = w->next;
   }
}

inline void NamespaceList::parseCommit(class NamespaceList *l)
{
   assimilate(l);

   class Namespace *w = head;

   while (w)
   {
      w->parseCommit();
      w = w->next;
   }
}

inline void NamespaceList::parseRollback()
{
   class Namespace *w = head;

   while (w)
   {
      w->parseRollback();
      w = w->next;
   }
}

inline class Namespace *Namespace::findNamespace(char *nname)
{
   class Namespace *rv = nsl->find(nname);
   if (!rv)
      rv = pendNSL->find(nname);
   return rv;
}


// only called with RootNS, private
inline class Namespace *Namespace::rootResolveNamespace(class NamedScope *nscope)
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

// public
/*
inline class QoreClass *Namespace::rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched, class QoreClassList **plist, bool *is_pending)
{
   QoreClass *oc;

   if (!(oc = parseMatchScopedClassWithMethod(nscope, matched, plist, is_pending))
       && !(oc = nsl->parseFindScopedClassWithMethod(nscope, matched, plist, is_pending)))
      oc = pendNSL->parseFindScopedClassWithMethod(nscope, matched, plist, is_pending);
   return oc;
}
*/

// public
inline class QoreClass *Namespace::rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched)
{
   QoreClass *oc;

   if (!(oc = parseMatchScopedClassWithMethod(nscope, matched))
       && !(oc = nsl->parseFindScopedClassWithMethod(nscope, matched)))
      oc = pendNSL->parseFindScopedClassWithMethod(nscope, matched);
   return oc;
}

// public
// will always be called with a namespace (nscope->elements > 1)
inline class QoreClass *Namespace::rootFindScopedClass(class NamedScope *nscope, int *matched)
{
   QoreClass *oc = parseMatchScopedClass(nscope, matched);
   if (!oc && !(oc = nsl->parseFindScopedClass(nscope, matched)))
      oc = pendNSL->parseFindScopedClass(nscope, matched);
   return oc;
}

// public, will always be called with nscope->elements > 1
inline class QoreNode *Namespace::rootFindScopedConstantValue(class NamedScope *nscope, int *matched)
{
   class QoreNode *rv = parseMatchScopedConstantValue(nscope, matched);
   if (!rv && !(rv = nsl->parseFindScopedConstantValue(nscope, matched)))
      rv = pendNSL->parseFindScopedConstantValue(nscope, matched);
   return rv;
}

static inline void addConstant(class NamedScope *name, class QoreNode *value)
{
   getRootNS()->rootAddConstant(name, value);
}

static inline void addClass(class NamedScope *name, class QoreClass *oc)
{
   tracein("addClass()");
   getRootNS()->rootAddClass(name, oc);
   traceout("addClass()");
}

static inline class QoreNode *findConstantValue(char *name, int level)
{
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH)
   {
      parse_error("recursive constant definitions too deep resolving '%s'", name);
      return NULL;
   }

   class QoreNode *rv = getRootNS()->rootFindConstantValue(name);
   if (!rv)
      parse_error("constant '%s' cannot be resolved in any namespace", name);
   return rv;
}

static inline class QoreClass *parseFindClass(char *name)
{
   class QoreClass *c = getRootNS()->rootFindClass(name);
   if (!c)
      parse_error("reference to undefined class '%s'", name);

   return c;
}

#endif // QORE_NAMESPACE_H
