/*
  QoreClass.cc

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
#include <qore/Sequence.h>
#include <qore/BuiltinMethod.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

// global class ID sequence
DLLLOCAL class Sequence classIDSeq;

// BCEANode
// base class constructor evaluated argument node
// created locally at run time
class BCEANode
{
public:
   class QoreNode *args;
   bool execed;
   
   DLLLOCAL inline BCEANode(class QoreNode *arg);
   DLLLOCAL inline BCEANode();
};

struct ltqc
{
   bool operator()(const class QoreClass *qc1, const class QoreClass *qc2) const
   {
      return qc1 < qc2;
   }
};

#include <map>
typedef std::map<class QoreClass *, class BCEANode *, ltqc> bceamap_t;

/*
 BCEAList
 base class constructor evaluated argument list
 */
class BCEAList : public bceamap_t
{
   protected:
      DLLLOCAL inline ~BCEAList() { }
   
   public:
      DLLLOCAL inline void deref(class ExceptionSink *xsink);
      // evaluates arguments, returns -1 if an exception was thrown
      DLLLOCAL inline int add(class QoreClass *qc, class QoreNode *arg, class ExceptionSink *xsink);
      DLLLOCAL inline class QoreNode *findArgs(class QoreClass *qc, bool *aexeced);
};

inline BCEANode::BCEANode(class QoreNode *arg)
{
   args = arg;
   execed = false;
}

inline BCEANode::BCEANode()
{
   args = NULL;
   execed = true;
}

inline class QoreNode *BCEAList::findArgs(class QoreClass *qc, bool *aexeced)
{
   bceamap_t::iterator i = find(qc);
   if (i != end())
   {
      if (i->second->execed)
      {
	 *aexeced = true;
	 return NULL;
      }
      *aexeced = false;
      i->second->execed = true;
      return i->second->args;
   }

   insert(std::make_pair(qc, new BCEANode()));
   *aexeced = false;
   return NULL;
}

inline int BCEAList::add(class QoreClass *qc, class QoreNode *arg, class ExceptionSink *xsink)
{
   // see if class already exists in the list
   bceamap_t::iterator i = find(qc);
   if (i != end())
      return 0;

   // evaluate and save arguments
   insert(std::make_pair(qc, new BCEANode(arg ? arg->eval(xsink) : NULL)));
   if (xsink->isEvent())
      return -1;
   return 0;
}

inline void BCEAList::deref(class ExceptionSink *xsink)
{
   bceamap_t::iterator i;
   while ((i = begin()) != end())
   {
      class BCEANode *n = i->second;
      erase(i);
      
      if (n->args)
	 n->args->deref(xsink);
      delete n;
   }
   delete this;
}

BCANode::~BCANode()
{
   if (ns)
      delete ns;
   if (name)
      free(name);
   if (argexp)
      argexp->deref(NULL);
}

inline void BCANode::resolve()
{
   if (ns)
   {
      sclass = getRootNS()->parseFindScopedClass(ns);
      printd(5, "BCANode::resolve() this=%08p resolved named scoped %s -> %08p\n", this, ns->ostr, sclass);
      delete ns;
      ns = NULL;
   }
   else
   {
      sclass = getRootNS()->parseFindClass(name);
      printd(5, "BCANode::resolve() this=%08p resolved %s -> %08p\n", this, name, sclass);
      free(name);
      name = NULL;
   }   
}

BCNode::~BCNode()
{
   if (cname)
      delete cname;
   if (cstr)
      free(cstr);
   if (args)
      args->deref(NULL);
}

BCList::BCList(class BCNode *n) : init(false)
{
   push_back(n);
}

inline BCList::BCList() : init(false)
{
}

inline BCList::~BCList()
{
   bclist_t::iterator i;
   while ((i = begin()) != end())
   {
      delete *i;
      // erase() is constant time as long as i == begin()
      erase(i);
   }
}

inline void BCList::ref()
{
   ROreference();
}

void BCList::deref()
{
   if (ROdereference())
      delete this;
}

inline void BCList::parseInit(class QoreClass *cls, class BCAList *bcal)
{
   if (init)
      return;

   init = true;

   printd(5, "BCList::parseInit(%s) this=%08p empty=%d, bcal=%08p\n", cls->getName(), this, empty(), bcal);
   for (bclist_t::iterator i = begin(); i != end(); i++)
   {
      if (!(*i)->sclass)
	 if ((*i)->cname)
	 {
	    (*i)->sclass = getRootNS()->parseFindScopedClass((*i)->cname);
	    printd(5, "BCList::parseInit() %s inheriting %s (%08p)\n", cls->getName(), (*i)->cname->ostr, (*i)->sclass);
	    delete (*i)->cname;
	    (*i)->cname = NULL;
	 }
	 else
	 {
	    (*i)->sclass = getRootNS()->parseFindClass((*i)->cstr);
	    printd(5, "BCList::parseInit() %s inheriting %s (%08p)\n", cls->getName(), (*i)->cstr, (*i)->sclass);
	    free((*i)->cstr);
	    (*i)->cstr = NULL;
	 }
      // recursively add base classes to special method list
      if ((*i)->sclass)
      {
         (*i)->sclass->addBaseClassesToSubclass(cls);
	 // include all subclass domains in this class' domain
	 cls->addDomain((*i)->sclass->getDomain());
      }
   }

   // compare each class in the list to ensure that there are no duplicates
   for (bclist_t::iterator i = begin(); i != end(); i++)
   {
      if ((*i)->sclass)
      {
	 bclist_t::iterator j = i;
	 while (++j != end())
	    if ((*i)->sclass == (*j)->sclass)
	       parse_error("class '%s' cannot inherit '%s' more than once", cls->getName(), (*i)->sclass->getName());
      }	 
   }

   // if there is a base class constructor list, resolve all classes and 
   // ensure that all classes referenced are base classes of this class
   if (bcal)
   {
      for (bcalist_t::iterator i = bcal->begin(); i != bcal->end(); i++)
      {
	 (*i)->resolve();
	 if ((*i)->sclass && !match(*i))
	    parse_error("%s in base class constructor argument list is not a base class of %s", (*i)->sclass->getName(), cls->getName());
      }
   }
}

inline class Method *BCList::findMethod(char *name)
{
   for (bclist_t::iterator i = begin(); i != end(); i++)
   {
      if ((*i)->sclass)
      {
	 if ((*i)->sclass->scl)
	    (*i)->sclass->scl->parseInit((*i)->sclass, (*i)->sclass->bcal);
	 class Method *m;
	 if ((m = (*i)->sclass->findMethod(name)))
	    return m;
      }
   }
   return NULL;
}

// only called at run-time
inline class Method *BCList::findMethod(char *name, bool *priv)
{
   for (bclist_t::iterator i = begin(); i != end(); i++)
   {
      if ((*i)->sclass)
      {
	 class Method *m;
	 if ((m = (*i)->sclass->findMethod(name, priv)))
	 {
	    if (!*priv && (*i)->priv)
	       (*priv) = (*i)->priv;
	    return m;
	 }
      }
   }
   return NULL;
}

inline bool BCList::match(class BCANode *bca)
{
   for (bclist_t::iterator i = begin(); i != end(); i++)
   {
      if (bca->sclass == (*i)->sclass)
      {
	 (*i)->args = bca->argexp;
	 bca->argexp = NULL;
	 (*i)->hasargs = true;
	 return true;
      }
   }
   return false;
}

inline bool BCList::isPrivateMember(char *str) const
{
   for (bclist_t::const_iterator i = begin(); i != end(); i++)
      if ((*i)->sclass->isPrivateMember(str))
	 return true;
   return false;
}

inline class Method *BCList::resolveSelfMethod(char *name)
{
   for (bclist_t::iterator i = begin(); i != end(); i++)
   {
      if ((*i)->sclass)
      {
	 if ((*i)->sclass->scl)
	    (*i)->sclass->scl->parseInit((*i)->sclass, (*i)->sclass->bcal);
	 class Method *m;
	 if ((m = (*i)->sclass->resolveSelfMethodIntern(name)))
	    return m;
      }
   }
   return NULL;
}

inline void BCList::execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink)
{
   for (bclist_t::iterator i = begin(); i != end(); i++)
   {
      printd(5, "BCList::execConstructors() %s::constructor() o=%08p (for subclass %s)\n", (*i)->sclass->getName(), o, o->getClass()->getName()); 

      (*i)->sclass->execSubclassConstructor(o, bceal, xsink);
      if (xsink->isEvent())
	 break;
   }
}

void BCList::execConstructorsWithArgs(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink)
{
   // if there are base class constructor arguments that haven't already been overridden
   // by a base class constructor argument specification in a subclass, evaluate them now
   for (bclist_t::iterator i = begin(); i != end(); i++)
      if ((*i)->hasargs && bceal->add((*i)->sclass, (*i)->args, xsink))
	    return;
   execConstructors(o, bceal, xsink);
}

inline void BCList::execSystemConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink)
{
   for (bclist_t::iterator i = begin(); i != end(); i++)
   {
      printd(5, "BCList::execSystemConstructors() %s::constructor() o=%08p (for subclass %s)\n", (*i)->sclass->getName(), o, o->getClass()->getName()); 
      (*i)->sclass->execSubclassSystemConstructor(o, bceal, xsink);
      if (xsink->isEvent())
	 break;
   }
}

BCAList::BCAList(class BCANode *n)
{
   push_back(n);
}

BCAList::~BCAList()
{
   bcalist_t::iterator i;
   while ((i = begin()) != end())
   {
      delete *i;
      erase(i);
   }
}

inline void BuiltinMethod::deref()
{
   if (ROdereference())
      delete this;
}

inline void QoreClass::checkSpecialIntern(class Method *m)
{
   // set quick pointers
   if (!methodGate && !strcmp(m->getName(), "methodGate"))
      methodGate = m;
   else if (!memberGate && !strcmp(m->getName(), "memberGate"))
      memberGate = m;
}

inline void QoreClass::checkSpecial(class Method *m)
{
   // set quick pointers
   if (!constructor && !strcmp(m->getName(), "constructor"))
      constructor = m;
   else if (!destructor && !strcmp(m->getName(), "destructor"))
      destructor = m;
   else if (!copyMethod && !strcmp(m->getName(), "copy"))
      copyMethod = m;
   else 
      checkSpecialIntern(m);
}

class Method *QoreClass::findLocalMethod(char *nme)
{
   hm_method_t::iterator i = hm.find(nme);
   if (i != hm.end())
      return i->second;

   return NULL;
}

class Method *QoreClass::findMethod(char *nme)
{
   class Method *w;
   if (!(w = findLocalMethod(nme)))
   {
      // search superclasses
      if (scl)
	 w = scl->findMethod(nme);
   }
   return w;
}

class Method *QoreClass::findMethod(char *nme, bool *priv)
{
   class Method *w;
   if (!(w = findLocalMethod(nme)))
   {
      // search superclasses
      if (scl)
	 w = scl->findMethod(nme, priv);
   }
   return w;
}

// only called when parsing
void QoreClass::setName(char *n)
{
   assert(!name);
   name = n;
}

bool QoreClass::is_unique() const
{
   return nref.is_unique();
}

class QoreClass *QoreClass::getReference()
{
   //printd(5, "QoreClass::getReference() %08x %s %d -> %d\n", this, name, nref.reference_count(), nref.reference_count() + 1);
   nref.ROreference();
   return this;
}

void QoreClass::nderef()
{
   //printd(5, "QoreClass::nderef() %08p %s %d -> %d\n", this, name, nref.reference_count(), nref.reference_count() - 1);
   if (nref.ROdereference())
      delete this;
}

bool QoreClass::hasCopy() const
{
   return copyMethod ? true : false; 
}

int QoreClass::getID() const
{ 
   return classID; 
}

int QoreClass::getIDForMethod() const
{ 
   return methodID;
}

bool QoreClass::isSystem() const
{ 
   return sys;
}

bool QoreClass::hasMemberGate() const
{
   return memberGate != NULL;
}

int QoreClass::getDomain() const
{
   return domain;
}

char *QoreClass::getName() const 
{ 
   return name; 
}

int QoreClass::numMethods() const
{
   return hm.size();
}

inline Method *QoreClass::parseFindMethod(char *nme)
{
   class Method *m;
   if ((m = findLocalMethod(nme)))
      return m;

   // look in pending methods
   hm_method_t::iterator i = hm_pending.find(nme);
   if (i != hm_pending.end())
      return i->second;

   return NULL;
}

void QoreClass::addBuiltinBaseClass(class QoreClass *qc, class QoreNode *xargs)
{
   if (!scl)
      scl = new BCList();
   scl->push_back(new BCNode(qc, xargs));
}

void QoreClass::addDefaultBuiltinBaseClass(class QoreClass *qc, class QoreNode *xargs)
{
   addBuiltinBaseClass(qc, xargs);
   // make sure no methodID has already been assigned
   assert(methodID == classID);
   methodID = qc->classID;
}

void QoreClass::setSystemConstructor(q_constructor_t m)
{
   sys = true;
   system_constructor = new Method(new BuiltinMethod(this, m));
}

// deletes all pending user methods
void QoreClass::parseRollback()
{
   delete_pending_methods();
}

inline void Method::userInit(UserFunction *u, int p)
{
   name = strdup(u->name);
   type = OTF_USER;
   func.userFunc = u;
   priv = p;
}

inline Method::Method()
{ 
}

Method::Method(UserFunction *u, int p)
{
   userInit(u, p);
}

inline Method::Method(BuiltinMethod *b)
{
   name = b->name;
   type = OTF_BUILTIN;
   func.builtin = b;
   priv = 0;
}

Method::~Method()
{
   if (name && type != OTF_BUILTIN)
      free(name);
   if (type == OTF_USER)
      func.userFunc->deref();
   else
      func.builtin->deref();
}

inline bool Method::isSynchronized() const
{
   if (type == OTF_BUILTIN)
      return false;
   return func.userFunc->isSynchronized();
}

inline bool Method::inMethod(class Object *self) const
{
   if (type == OTF_USER)
      return ::inMethod(func.userFunc->name, self);
   return ::inMethod(func.builtin->name, self);
}

inline void Method::evalSystemConstructor(Object *self, QoreNode *args, class BCList *bcl, class BCEAList *bceal, ExceptionSink *xsink)
{
   // type must be OTF_BUILTIN
   func.builtin->evalSystemConstructor(self, args, bcl, bceal, xsink);
}

inline void Method::evalSystemDestructor(class Object *self, class ExceptionSink *xsink)
{
   // get pointer to private data object from class ID of base type
   AbstractPrivateData *ptr = self->getAndClearPrivateData(func.builtin->myclass->getID(), xsink);
   //printd(5, "Method::evalSystemDestructor() class=%s (%08p) id=%d ptr=%08p\n", func.builtin->myclass->getName(), func.builtin->myclass, func.builtin->myclass->getID(), ptr);
   // NOTE: ptr may be null for builtin subclasses without private data
   if (ptr)
      func.builtin->evalSystemDestructor(self, ptr, xsink);
}

inline void Method::parseInit()
{
   // must be called even if func.userFunc->statements is NULL
   func.userFunc->statements->parseInit(func.userFunc->params, NULL);
}

inline void Method::parseInitConstructor(class BCList *bcl)
{
   // must be called even if func.userFunc->statements is NULL
   func.userFunc->statements->parseInit(func.userFunc->params, bcl);
}

inline class Method *Method::copy() const
{
   class Method *nof;
   if (type == OTF_USER)
   {
      func.userFunc->ROreference();
      nof = new Method;
      nof->userInit(func.userFunc, priv);
   }
   else
   {
      func.builtin->ROreference();
      nof = new Method(func.builtin);
   }
   return nof;
}

static inline class QoreClass *getStackClass()
{
   class Object *obj = getStackObject();
   if (obj)
      return obj->getClass();
   return NULL;
}

void QoreClass::addPrivateMember(char *nme)
{
   if (pmm.find(nme) == pmm.end())
   {
      if (pending_pmm.find(nme) == pending_pmm.end())
      {
	 //printd(5, "QoreClass::addPrivateMember() this=%08p %s adding %08p %s\n", this, name, nme, nme);
	 pending_pmm.insert(nme);
      }
      else
      {
	 if (name)
	    parse_error("private member '%s' already pending in class %s", nme, name);
	 else
	    parse_error("private member '%s' already pending in class", nme);
	 free(nme);
      }
   }
   else
   {
      parse_error("private member '%s' already declared in class %s", nme, name);
      free(nme);
   }
}

inline bool BCSMList::isBaseClass(class QoreClass *qc) const
{
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      if (qc == *i)
	 return true;
      i++;
   }
   return false;
}

inline void BCSMList::addBaseClassesToSubclass(class QoreClass *thisclass, class QoreClass *sc)
{
   //printd(5, "BCSMList::addBaseClassesToSubclass(this=%s, sc=%s) size=%d\n", thisclass->getName(), sc->getName());
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      sc->scl->sml.add(thisclass, *i);
      i++;
   }
}

inline void BCSMList::add(class QoreClass *thisclass, class QoreClass *qc)
{
   if (thisclass == qc)
   {
      parse_error("class '%s' cannot inherit itself", qc->getName());
      return;
   }
   // see if class already exists in list
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      if (*i == qc)
         return;
      if (*i == thisclass)
      {
      	 parse_error("circular reference in class hierarchy, '%s' is an ancestor of itself", thisclass->getName());
      	 return;
      }
      i++;
   }

   // append to the end of the doubly-linked list
   push_back(qc);
}

inline void BCSMList::execDestructors(class Object *o, class ExceptionSink *xsink)
{
   class_list_t::reverse_iterator i = rbegin();
   // cast below required by g++ 3.2 at least
   while (i != rend())
   {
      printd(5, "BCSMList::execDestructors() %s::destructor() o=%08p (subclass %s)\n", (*i)->getName(), o, o->getClass()->getName());
      (*i)->execSubclassDestructor(o, xsink);
      i++;
   }
}

inline void BCSMList::execSystemDestructors(class Object *o, class ExceptionSink *xsink)
{
   class_list_t::reverse_iterator i = rbegin();
   while (i != rend())
   {
      printd(5, "BCSMList::execSystemDestructors() %s::destructor() o=%08p (subclass %s)\n", (*i)->getName(), o, o->getClass()->getName());
      (*i)->execSubclassSystemDestructor(o, xsink);
      i++;
   }
}

inline void BCSMList::execCopyMethods(class Object *self, class Object *old, class ExceptionSink *xsink)
{
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      (*i)->execSubclassCopy(self, old, xsink);
      if (xsink->isEvent())
	 break;
      i++;
   }
}

inline class QoreClass *BCSMList::getClass(int cid) const
{
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      if ((*i)->getID() == cid)
	 return *i;
      i++;
   }
   return NULL;
}

inline void QoreClass::init(char *nme, int dom)
{
   initialized = false;
   domain = dom;
   scl = NULL;
   name = nme;
   sys  = false;
   bcal = NULL;

   // quick pointers
   constructor = NULL;
   destructor  = NULL;
   copyMethod  = NULL;
   methodGate  = NULL;
   memberGate  = NULL;

   system_constructor = NULL;
}

QoreClass::QoreClass(int dom, char *nme)
{
   init(nme, dom);

   classID = methodID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating '%s' ID:%d (this=%08p)\n", name, classID, this);
}

QoreClass::QoreClass(char *nme)
{
   init(nme);

   classID = methodID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating '%s' ID:%d (this=%08p)\n", name, classID, this);
}

QoreClass::QoreClass()
{
   init(NULL);

   classID = methodID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating unnamed class ID:%d (this=%08p)\n", classID, this);
}

QoreClass::QoreClass(char *nme, int id)
{
   init(strdup(nme));

   classID = id;
   printd(5, "QoreClass::QoreClass() creating copy of '%s' ID:%d (this=%08p)\n", name, classID, this);
}

QoreClass::~QoreClass()
{
   //printd(5, "QoreClass::~QoreClass() deleting %08p %s\n", this, name);
   hm_method_t::iterator i;
   while ((i = hm.begin()) != hm.end())
   {
      class Method *m = i->second;
      //printd(5, "QoreClass::~QoreClass() deleting method %08p %s::%s()\n", m, name, m->getName());
      hm.erase(i);
      delete m;
   }   
   // delete private member list
   strset_t::iterator j;
   while ((j = pmm.begin()) != pmm.end())
   {
      char *n = *j;
      pmm.erase(j);
      //printd(5, "QoreClass::~QoreClass() freeing private member %08p '%s'\n", n, n);
      free(n);
   }
   while ((j = pending_pmm.begin()) != pending_pmm.end())
   {
      char *n = *j;
      pending_pmm.erase(j);
      //printd(5, "QoreClass::~QoreClass() freeing pending private member %08p '%s'\n", n, n);
      free(n);
   }
   // delete any pending methods
   delete_pending_methods();
   free(name);
   if (scl)
      scl->deref();
   if (bcal)
      delete bcal;
   if (system_constructor)
      delete system_constructor;
}

inline void QoreClass::delete_pending_methods()
{
   hm_method_t::iterator i;
   while ((i = hm_pending.begin()) != hm_pending.end())
   {
      class Method *m = i->second;
      //printd(5, "QoreClass::~QoreClass() deleting pending method %08p %s::%s()\n", m, name, m->getName());
      hm_pending.erase(i);
      delete m;
   }
}

class QoreClass *QoreClass::getClass(int cid) const
{
   if (cid == classID)
      return (QoreClass *)this;
   return scl ? scl->sml.getClass(cid) : NULL;
}

class QoreNode *Method::eval(Object *self, QoreNode *args, ExceptionSink *xsink)
{
   QoreNode *rv = NULL;

   tracein("Method::eval()");
#ifdef DEBUG
   char *oname = self->getClass()->getName();
   printd(5, "Method::eval() %s::%s() (object=%08p, pgm=%08p)\n", oname, name, self, self->getProgram());
#endif

   int need_deref = 0;

   // need to evaluate arguments before pushing object context
   QoreNode *new_args;
   if (args)
   {
      if (args->val.list->needsEval())
      {
	 printd(5, "Method::eval() about to evaluate args=%08p (%s)\n", args, args->type->getName());
	 new_args = args->eval(xsink);
	 printd(5, "Method::eval() args=%08p (%s) new_args=%08p (%s)\n", args, args->type->getName(), new_args, new_args ? new_args->type->getName() : "NONE");
	 if (xsink->isEvent())
	 {
	    if (new_args)
	       new_args->deref(xsink);
	    traceout("Method::eval()");
	    return NULL;
	 }
	 need_deref = 1;
      }
      else
	 new_args = args;
   }
   else
      new_args = NULL;

   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      rv = func.userFunc->eval(new_args, self, xsink);
   else
      rv = self->evalBuiltinMethodWithPrivateData(func.builtin, new_args, xsink);

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();

   if (new_args && need_deref)
      new_args->deref(xsink);
#ifdef DEBUG
   printd(5, "Method::eval() %s::%s() returning %08p (type=%s, refs=%d)\n",
	  oname, name, rv, rv ? rv->type->getName() : "(null)", rv ? rv->reference_count() : 0);
#endif
   traceout("Method::eval()");
   return rv;
}

void Method::evalConstructor(Object *self, QoreNode *args, class BCList *bcl, class BCEAList *bceal, ExceptionSink *xsink)
{
   tracein("Method::evalConstructor()");
#ifdef DEBUG
   char *oname = self->getClass()->getName();
   printd(5, "Method::evalConstructor() %s::%s() (object=%08p, pgm=%08p)\n", oname, name, self, self->getProgram());
#endif

   int need_deref = 0;

   // need to evaluate arguments before pushing object context
   QoreNode *new_args;
   if (args)
   {
      if (args->val.list->needsEval())
      {
	 printd(5, "Method::evalConstructor() about to evaluate args=%08p (%s)\n", args, args->type->getName());
	 new_args = args->eval(xsink);
	 printd(5, "Method::evalConstructor() args=%08p (%s) new_args=%08p (%s)\n",
		args, args->type->getName(), new_args, new_args ? new_args->type->getName() : "NONE");
	 if (xsink->isEvent())
	 {
	    if (new_args)
	       new_args->deref(xsink);
	    traceout("Method::evalConstructor()");
	    return;
	 }
	 need_deref = 1;
      }
      else
	 new_args = args;
   }
   else
      new_args = NULL;

   if (!xsink->isEvent())
   {
      if (type == OTF_USER)
      {
	 class QoreNode *rv = func.userFunc->evalConstructor(new_args, self, bcl, bceal, xsink);
	 if (rv)
	    rv->deref(xsink);
      }
      else
      {
	 // switch to new program for imported objects
	 QoreProgram *cpgm;
	 QoreProgram *opgm = self->getProgram();
	 if (opgm)
	 {
	    cpgm = getProgram();
	    if (opgm && cpgm != opgm)
	       pushProgram(opgm);
	 }
	 else
	    cpgm = NULL;

	 func.builtin->evalConstructor(self, new_args, bcl, bceal, xsink);

	 // switch back to original program if necessary
	 if (opgm && cpgm != opgm)
	    popProgram();
      }
   }

   if (new_args && need_deref)
      new_args->deref(xsink);
#ifdef DEBUG
   printd(5, "Method::evalConstructor() %s::%s() done\n", oname, name);
#endif
   traceout("Method::evalConstructor()");
}

void Method::evalCopy(Object *self, Object *old, ExceptionSink *xsink)
{
   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      func.userFunc->evalCopy(old, self, xsink);
   else // builtin function
      old->evalCopyMethodWithPrivateData(func.builtin, self, xsink);

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();
}

void Method::evalDestructor(Object *self, ExceptionSink *xsink)
{
   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      func.userFunc->eval(NULL, self, xsink);
   else // builtin function
   {
      AbstractPrivateData *ptr = self->getAndClearPrivateData(func.builtin->myclass->getID(), xsink);
      if (ptr)
	 func.builtin->evalDestructor(self, ptr, xsink);
      else if (!xsink->isException() && func.builtin->myclass->getID() == func.builtin->myclass->getIDForMethod()) // do not throw an exception if the class has no private data
      {
	 if (self->getClass() == func.builtin->myclass)
	    xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::destructor() cannot be executed because the object has already been deleted", self->getClass()->getName());
	 else
	    xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::destructor() (base class of '%s') cannot be executed because the object has already been deleted", func.builtin->myclass->getName(), self->getClass()->getName());
      }
   }

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();
}

class QoreClass *QoreClass::copyAndDeref()
{
   tracein("QoreClass::copyAndDeref");
   class QoreClass *noc = new QoreClass(name, classID);
   noc->methodID = methodID;

   printd(5, "QoreClass::copyAndDeref() name=%s (%08p) new name=%s (%08p)\n", name, name, noc->name, noc->name);

   // set up function list

   for (hm_method_t::iterator i = hm.begin(); i != hm.end(); i++)
   {
      class Method *nf = i->second->copy();

      noc->hm[nf->getName()] = nf;
      if (i->second == constructor)
	 noc->constructor  = nf;
      else if (i->second == destructor)
	 noc->destructor   = nf;
      else if (i->second == copyMethod)
	 noc->copyMethod   = nf;
      else if (i->second == methodGate)
	 noc->methodGate   = nf;
      else if (i->second == memberGate)
	 noc->memberGate   = nf;
   }
   // copy private member list
   for (strset_t::iterator i = pmm.begin(); i != pmm.end(); i++)
      noc->pmm.insert(strdup(*i));

   if (scl)
   {
      scl->ref();
      noc->scl = scl;
   }

   nderef();
   traceout("QoreClass::copyAndDeref");
   return noc;
}

inline void QoreClass::insertMethod(Method *m)
{
   //printd(5, "QoreClass::insertMethod() %s::%s() size=%d\n", name, m->getName(), numMethods());
   hm[m->getName()] = m;
}      

inline void QoreClass::addDomain(int dom)
{
   domain |= dom;
}

class QoreNode *QoreClass::evalMethod(Object *self, char *nme, QoreNode *args, class ExceptionSink *xsink)
{
   tracein("QoreClass::evalMethod()");
   Method *w;
   int external = (this != getStackClass());
   printd(5, "QoreClass::evalMethod() %s::%s() %s call attempted\n", name, nme, external ? "external" : "internal" );

   if (!strcmp(nme, "copy"))
   {
      traceout("QoreClass::evalMethod()");
      return execCopy(self, xsink);
   }

   bool priv = false;
   if (!(w = findMethod(nme, &priv)))
   {
      if (methodGate && !methodGate->inMethod(self)) // call methodGate with unknown method name and arguments
	 return evalMethodGate(self, nme, args, xsink);
      // otherwise return an exception
      xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() has been defined", name, nme);
      traceout("QoreClass::evalMethod()");
      return NULL;
   }
   // check for illegal explicit call
   if (//w == copyMethod || 
       w == constructor || w == destructor)
   {
      xsink->raiseException("ILLEGAL-EXPLICIT-METHOD-CALL", "explicit calls to ::%s() methods are not allowed", nme);
      traceout("QoreClass::evalMethod()");
      return NULL;      
   }

   if (external)
      if (w->isPrivate())
      {
	 xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s() is private and cannot be accessed externally", name, nme);
	 traceout("QoreClass::evalMethod()");
	 return NULL;
      }
      else if (priv)
      {
	 xsink->raiseException("BASE-CLASS-IS-PRIVATE", "%s() is a method of a privately-inherited class of %s", nme, name);
	 traceout("QoreClass::evalMethod()");
	 return NULL;
      }
   traceout("QoreClass::evalMethod()");
   return w->eval(self, args, xsink);
}

class QoreNode *QoreClass::evalMethodGate(Object *self, char *nme, QoreNode *args, ExceptionSink *xsink)
{
   tracein("QoreClass::evalMethodGate()");
   printd(5, "QoreClass::evalMethodGate() method=%s args=%08p\n", nme, args);
   // build new argument list
   if (args)
   {
      args = args->eval(xsink);
      if (xsink->isEvent())
      {
	 args->deref(xsink);
	 traceout("QoreClass::evalMethodGate()");
	 return NULL;
      }
      args->val.list->insert(new QoreNode(nme));
   }
   else
   {
      args = new QoreNode(new List());
      args->val.list->push(new QoreNode(nme));
   }
   QoreNode *rv = methodGate->eval(self, args, xsink);
   args->deref(xsink);
   
   traceout("QoreClass::evalMethodGate()");
   return rv;
}

bool QoreClass::isPrivateMember(char *str) const
{
   strset_t::const_iterator i = pmm.find(str);
   if (i != pmm.end())
      return true;

   if (scl)
      return scl->isPrivateMember(str);
   return false;
}

class QoreNode *QoreClass::evalMemberGate(class Object *self, class QoreNode *nme, class ExceptionSink *xsink)
{
   tracein("QoreClass::evalMembeGatre()");
   printd(5, "QoreClass::evalMemberGate() member=%s\n", nme->val.String->getBuffer());
   // do not run memberGate method if we are already in it...
   if (!memberGate || memberGate->inMethod(self))
   {
      traceout("QoreClass::evalMemberGate()");
      return NULL;
   }
   class QoreNode *args = new QoreNode(new List());
   args->val.list->push(nme->RefSelf());
   class QoreNode *rv = memberGate->eval(self, args, xsink);
   args->deref(xsink);
   traceout("QoreClass::evalMemberGate()");
   return rv;
}

class QoreNode *QoreClass::execConstructor(QoreNode *args, ExceptionSink *xsink)
{
   // create new object
   class Object *o = new Object(this, getProgram());
   class BCEAList *bceal;
   if (scl)
      bceal = new BCEAList();
   else
      bceal = NULL;

   printd(5, "QoreClass::execConstructor() %s::constructor() o=%08p\n", name, o);

   if (!constructor)
   {
      if (scl) // execute superclass constructors if any
	 scl->execConstructors(o, bceal, xsink);
   }
   else // no lock is sent with constructor, because no variable has been assigned yet
      constructor->evalConstructor(o, args, scl, bceal, xsink);

   if (bceal)
      bceal->deref(xsink);

   if (xsink->isEvent())
   {
      // instead of executing the destructors for the superclasses that were already executed we call Object::obliterate()
      // which will clear out all the private data by running their dereference methods which should generally be OK
      o->obliterate(xsink);
      printd(5, "QoreClass::execConstructor() %s::constructor() o=%08p, exception in constructor, dereferencing object and returning NULL\n", name, o);
      return NULL;
   }

   QoreNode *rv = new QoreNode(o);
   printd(5, "QoreClass::execConstructor() %s::constructor() o=%08p, returning %08p\n", name, o, rv);
   return rv;
}

class QoreNode *QoreClass::execSystemConstructor(QoreNode *args, class ExceptionSink *xsink)
{
   // create new object
   class Object *o = new Object(this, NULL);
   class BCEAList *bceal;
   if (scl)
      bceal = new BCEAList();
   else
      bceal = NULL;

   printd(5, "QoreClass::execSystemConstructor() %s::constructor() o=%08p\n", name, o);

   if (!constructor)
   {
      if (scl) // execute superclass constructors if any
	 scl->execSystemConstructors(o, bceal, xsink);
   }
   else // no lock is sent with constructor, because no variable has been assigned yet
      system_constructor->evalSystemConstructor(o, args, scl, bceal, xsink);

   if (bceal)
      bceal->deref(xsink);

   // should never happen!
   assert(!xsink->isEvent());

   QoreNode *rv = new QoreNode(o);
   printd(5, "QoreClass::execSystemConstructor() %s::constructor() o=%08p, returning %08p\n", name, o, rv);
   return rv;
}

inline void QoreClass::execSubclassConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink)
{
   if (!constructor)
   {
      if (scl) // execute superclass constructors if any
	 scl->execConstructors(self, bceal, xsink);
   }
   else // no lock is sent with constructor, because no variable has been assigned yet
   {
      bool already_executed;
      class QoreNode *args = bceal->findArgs(this, &already_executed);
      if (!already_executed)
	 constructor->evalConstructor(self, args, scl, bceal, xsink);
   }
}

inline void QoreClass::execSubclassSystemConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink)
{
   if (!constructor)
   {
      if (scl) // execute superclass constructors if any
	 scl->execSystemConstructors(self, bceal, xsink);
   }
   else // no lock is sent with constructor, because no variable has been assigned yet
   {
      bool already_executed;
      class QoreNode *args = bceal->findArgs(this, &already_executed);
      if (!already_executed)
	 system_constructor->evalSystemConstructor(self, args, scl, bceal, xsink);
   }
}

void QoreClass::execDestructor(Object *self, ExceptionSink *xsink)
{
   printd(5, "QoreClass::execDestructor() %s::destructor() o=%08p\n", name, self);

   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   class ExceptionSink de;

   if (self->isSystemObject())
   {
      if (destructor)
	 destructor->evalSystemDestructor(self, &de);
      else
	 self->defaultSystemDestructor(classID, &de);

      // execute superclass destructors
      if (scl)
	 scl->sml.execSystemDestructors(self, &de);
   }
   else
   {
      if (destructor)
	 destructor->evalDestructor(self, &de);
      else if (sys)
	 self->defaultSystemDestructor(classID, &de);

      // execute superclass destructors
      if (scl)
	 scl->sml.execDestructors(self, &de);
   }

   xsink->assimilate(&de);
}

inline void QoreClass::execSubclassDestructor(Object *self, ExceptionSink *xsink)
{
   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   class ExceptionSink de;
   if (destructor)
      destructor->evalDestructor(self, &de);
   else if (sys)
      self->defaultSystemDestructor(classID, &de);

   xsink->assimilate(&de);
}

inline void QoreClass::execSubclassSystemDestructor(Object *self, ExceptionSink *xsink)
{
   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   class ExceptionSink de;
   if (destructor)
      destructor->evalSystemDestructor(self, &de);
   else if (sys)
      self->defaultSystemDestructor(classID, &de);

   xsink->assimilate(&de);
}

class QoreNode *QoreClass::execCopy(Object *old, ExceptionSink *xsink)
{
   class Hash *h = old->evalData(xsink);
   if (xsink->isEvent())
      return NULL;

   class Object *self = new Object(this, getProgram(), h);

   // execute superclass copy methods
   if (scl)
      scl->sml.execCopyMethods(self, old, xsink);

   if (copyMethod && !xsink->isEvent())
      copyMethod->evalCopy(self, old, xsink);

   if (!xsink->isEvent())
      return new QoreNode(self);

   return NULL;
}

inline void QoreClass::execSubclassCopy(Object *self, Object *old, ExceptionSink *xsink)
{
   if (copyMethod)
      copyMethod->evalCopy(self, old, xsink);
}

void QoreClass::addBaseClassesToSubclass(class QoreClass *sc)
{      
   if (scl)
   {
      // initialize list, just in case
      scl->parseInit(this, bcal);
      scl->sml.addBaseClassesToSubclass(this, sc);
   }
   sc->scl->sml.add(sc, this);
}

// private, called from subclasses only
inline Method *QoreClass::resolveSelfMethodIntern(char *nme)
{
   class Method *m = parseFindMethod(nme);

   // if still not found now look in superclass methods
   if (!m && scl)
      m = scl->resolveSelfMethod(nme);

   return m;
}

Method *QoreClass::resolveSelfMethod(char *nme)
{
   class Method *m = findLocalMethod(nme);
#ifdef DEBUG
   if (m)
      printd(5, "QoreClass::resolveSelfMethod(%s) resolved to %s::%s() %08p\n", nme, name, nme, m);
#endif
   bool err = false;
   if (m && (//m == copyMethod || 
	  m == constructor || m == destructor))
      err = true;

   // look in pending methods
   if (!m)
   {
      // pending methods are not set to the quick pointers, so we have to compare the strings...
      if (//!!strcmp(nme, "copy") || 
	 !strcmp(nme, "constructor") || !strcmp(nme, "destructor"))
	 err = true;
      else
      {
	 hm_method_t::iterator i = hm_pending.find(nme);
	 if (i != hm_pending.end())
	 {
	    m = i->second;
	    printd(5, "QoreClass::resolveSelfMethod(%s) resolved to pending method %s::%s() %08p\n", nme, name, nme, m);
	 }
      }
   }

   // if still not found now look in superclass methods
   if (!err && !m && scl)
   {
      m = scl->resolveSelfMethod(nme);
#ifdef DEBUG
      if (m)
	 printd(5, "QoreClass::resolveSelfMethod(%s) resolved to <base class>::%s() %08p\n", nme, nme, m);
#endif
   }

   if (err)
   {
      parse_error("explicit calls to ::%s() methods are not allowed", nme);
      m = NULL;
   }
   else if (!m)
      parse_error("no method %s::%s() has been defined", name, nme);

   return m;
}

Method *QoreClass::resolveSelfMethod(class NamedScope *nme)
{
   // first find class
   class QoreClass *qc = getRootNS()->parseFindScopedClassWithMethod(nme);
   if (!qc)
      return NULL;

   // see if class is base class of this class
   if (qc != this && !scl->sml.isBaseClass(qc))
   {
      parse_error("'%s' is not a base class of '%s'", qc->getName(), name);
      return NULL;
   }

   char *nstr = nme->getIdentifier();
   class Method *m = qc->findMethod(nstr);
   bool err = false;
   if (m && (//m == copyMethod || 
	     m == constructor || m == destructor))
      err = true;

   // look in pending methods
   if (!m)
   {
      // pending methods are not set to the quick pointers, so we have to compare the strings...
      if (//!strcmp(nstr, "copy") || 
	  !strcmp(nstr, "constructor") || !strcmp(nstr, "destructor"))
	 err = true;
      else
      {
	 hm_method_t::iterator i = qc->hm_pending.find(nstr);
	 if (i != qc->hm_pending.end())
	    m = i->second;
      }
   }

   if (err)
   {
      parse_error("explicit calls to ::%s() methods are not allowed", nstr);
      m = NULL;
   }
   else if (!m)
      parse_error("no method %s::%s() has been defined", qc->getName(), nstr);

   return m;
}

void QoreClass::addMethod(Method *m)
{
   printd(5, "QoreClass::addMethod(%08p) %s.%s() (this=%08p)\n", m, name ? name : "<pending>", m->getName(), this);

   bool dst = !strcmp(m->getName(), "destructor");
   // check for illegal private constructor or destructor
   if (!strcmp(m->getName(), "constructor") || dst)
   {
      if (m->isPrivate())
	 parseException("ILLEGAL-PRIVATE-METHOD", "%s methods cannot be private", m->getName());
   }

   // if the method already exists or the user is trying to define a user destructor on a system object
   // (system objects without explicit destructors have an implicit default system destructor that cannot be overridden)
   if (parseFindMethod(m->getName()) || (sys && dst))
   {
      parse_error("method '%s::%s()' has already been defined", name, m->getName());
      delete m;
   }
   else
   {
      // insert in pending list for parse init
      hm_pending[m->getName()] = m;
   }
}

int QoreClass::parseAddBaseClassArgumentList(class BCAList *new_bcal)
{
   // if the constructor is being defined after the class has already been initialized, then throw a parse exception
   if (numMethods())
   {
      parse_error("constructors giving explicit arguments to base class constructors must be defined when the class is defined");
      return -1;
   }
   else if (bcal)
   {
      parse_error("a constructor with a base class argument list has already been defined");
      return -1;
   }
   bcal = new_bcal;
   return 0;
}

// adds a builtin method to the class - no duplicate checking is made
void QoreClass::addMethod(char *nme, q_method_t m)
{
   assert(strcmp(nme, "constructor"));
   assert(strcmp(nme, "destructor"));
   assert(strcmp(nme, "copy"));

   sys = true;
   BuiltinMethod *b = new BuiltinMethod(this, nme, m);
   Method *o = new Method(b);
   insertMethod(o);
   // check for special methods (except constructor and destructor)
   checkSpecialIntern(o);
}

// sets a builtin function as class constructor - no duplicate checking is made
void QoreClass::setConstructor(q_constructor_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   constructor = o;
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor(q_destructor_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   destructor = o;
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy(q_copy_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   copyMethod = o;
}

List *QoreClass::getMethodList() const
{
   List *l = new List();

   for (hm_method_t::const_iterator i = hm.begin(); i != hm.end(); i++)
      l->push(new QoreNode(i->first));
   return l;
}

// initializes all user methods
void QoreClass::parseInit()
{
   setParseClass(this);
   if (!initialized)
   {
      printd(5, "QoreClass::parseInit() %s this=%08p pending size=%d, scl=%08p, bcal=%08p\n", name, this, hm_pending.size(), scl, bcal);
      if (scl)
	 scl->parseInit(this, bcal);

      if (!sys && domain & getProgram()->getParseOptions())
	 parseException("ILLEGAL-CLASS-DEFINITION", "class '%s' inherits functionality from base classes that is restricted by current parse options", name);
      initialized = true;
   }

   for (hm_method_t::iterator i = hm_pending.begin(); i != hm_pending.end(); i++)
   {
      // initialize method
      if (!strcmp(i->second->getName(), "constructor"))
	 i->second->parseInitConstructor(scl);
      else
	 i->second->parseInit();
   }

   if (bcal)
   {
      if (!scl)
      {
	 parse_error("base class constructor arguments given for a class that has no parent classes");
      }
      delete bcal;
      bcal = NULL;
   }
}

// commits all pending user methods and pending private members
void QoreClass::parseCommit()
{
   printd(5, "QoreClass::parseCommit() %s this=%08p size=%d\n", name, this, hm_pending.size());

   hm_method_t::iterator i;
   while ((i = hm_pending.begin()) != hm_pending.end())
   {
      class Method *m = i->second;
      hm_pending.erase(i);
      insertMethod(m);
      checkSpecial(m);
   }

   // add all pending private members
   strset_t::iterator j;
   while ((j = pending_pmm.begin()) != pending_pmm.end())
   { 
      //printd(5, "QoreClass::parseCommit() %s committing private member %08p %s\n", name, *j, *j);
      pmm.insert(*j);
      pending_pmm.erase(j);
   }
}

void QoreClass::parseSetBaseClassList(class BCList *bcl)
{
   assert(!scl);
   scl = bcl;
}
