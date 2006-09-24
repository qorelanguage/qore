/*
  QoreClass.h

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

#ifndef _QORE_QORECLASS_H

#define _QORE_QORECLASS_H

//#include <qore/LockedObject.h>
#include <qore/ReferenceObject.h>
#include <qore/support.h>

#include <stdlib.h>
#include <string.h>

#ifdef HAVE_QORE_HASH_MAP
#include <qore/hash_map.h>
#endif

#define OTF_USER    0
#define OTF_BUILTIN 1

class QoreNode *internalObjectVarRef(class QoreNode *n, class ExceptionSink *xsink);
void initObjects();
void deleteObjects();

class Method {
   private:
      int type;
      union {
	    class UserFunction *userFunc;
	    class BuiltinMethod *builtin;
      } func;
      bool priv;

      inline Method() { bcal = NULL; }
      inline void userInit(UserFunction *u, int p);

   protected:

   public:
      char *name;
      class Method *next;
      class BCAList *bcal; // for subclass constructors only

      inline Method(class UserFunction *u, int p, class BCAList *b);
      inline Method(class BuiltinMethod *b);
      inline ~Method();
      inline bool inMethod(class Object *self);
      class QoreNode *eval(class Object *self, class QoreNode *args, class ExceptionSink *xsink);
      void evalConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink);
      void evalDestructor(class Object *self, class ExceptionSink *xsink);
      inline void evalSystemConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink);
      inline void evalSystemDestructor(class Object *self, class ExceptionSink *xsink);
      void evalCopy(class Object *self, class Object *old, class ExceptionSink *xsink);
      inline class Method *copy();
      inline void parseInit();
      inline void parseInitConstructor(class BCList *bcl);
      inline int getType() { return type; }
      inline bool isPrivate() { return priv; }
      inline char *getName() { return name; }
      // only called when method is user
      inline bool isSynchronized();
};

/*
  BCANode
  base class constructor argument node
*/
class BCANode
{
   public:
      class QoreClass *sclass;
      class NamedScope *ns;
      char *name;
      class QoreNode *argexp;
      class BCANode *next;

      inline BCANode(class NamedScope *n, class QoreNode *arg)
      {
	 ns = n;
	 name = NULL;
	 argexp = arg;
      }
      inline BCANode(char *n, class QoreNode *arg)
      {
	 ns = NULL;
	 name = n;
	 argexp = arg;
      }
      inline ~BCANode();
      inline void resolve();
};

/*
  BCAList
  base class constructor argument list
  this data structure will not be modified even if the class is copied
  to a subprogram object
*/
class BCAList : public ReferenceObject
{
   protected:
      inline ~BCAList()
      {
	 while (head)
	 {
	    class BCANode *w = head->next;
	    delete head;
	    head = w;
	 }
      }

   public:
      class BCANode *head;

      inline BCAList(class BCANode *n)
      {
	 head = n;
	 n->next = NULL;
      }
      inline void add(class BCANode *n)
      {
	 n->next = head;
	 head = n;
      }
      inline void ref()
      {
	 ROreference();
      }
      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

/*
  BCEANode
  base class constructor evaluated argument node
  created locally at run time
*/
class BCEANode
{
   public:
      class QoreClass *sclass;
      class QoreNode *args;
      class BCEANode *next;
      bool execed;

      inline BCEANode(class QoreClass *qc, class QoreNode *arg)
      {
	 sclass = qc;
	 args = arg;
	 execed = false;
      }
};

/*
  BCEAList
  base class constructor evaluated argument list
*/
class BCEAList
{
   protected:
      inline ~BCEAList() { }

   public:
      class BCEANode *head;

      inline BCEAList()
      {
	 head = NULL;
      }
      inline void deref(class ExceptionSink *xsink);
      inline void add(class QoreClass *qc, class QoreNode *arg, class ExceptionSink *xsink);
      inline void addDefault(class QoreClass *qc)
      {
	 class BCEANode *n = new BCEANode(qc, NULL);
	 n->next = head;
	 head = n;
	 n->execed = true;
      }
      inline class QoreNode *findArgs(class QoreClass *qc, bool *aexeced)
      {
	 class BCEANode *w = head;
	 while (w)
	 {
	    if (qc == w->sclass)
	    {
	       if (w->execed)
	       {
		  *aexeced = true;
		  return NULL;
	       }
	       *aexeced = false;
	       w->execed = true;
	       return w->args;
	    }
	    w = w->next;
	 }
	 addDefault(qc);
	 *aexeced = false;
	 return NULL;
      }
};

/*
  BCSMNode
  special method superclass node: unique list of base classes for a class hierarchy
  to ensure that "special" methods, constructor(), destructor(), copy() - are executed
  only once
*/
class BCSMNode 
{
   public:
      class QoreClass *sclass;
      class BCSMNode *next;
      class BCSMNode *prev;

      BCSMNode(class QoreClass *qc)
      {
	 sclass = qc;
	 next = NULL;
      }
};

// BCSMList
class BCSMList 
{
   public:
      class BCSMNode *head, *tail;

      inline BCSMList()
      {
	 head = tail = NULL;
      }
      inline ~BCSMList()
      {
	 while (head)
	 {
	    tail = head->next;
	    delete head;
	    head = tail;
	 }
      }
      inline void add(class QoreClass *thisclass, class QoreClass *qc);
      inline void addBaseClassesToSubclass(class QoreClass *thisclass, class QoreClass *sc);
      inline bool isBaseClass(class QoreClass *qc)
      {
	 class BCSMNode *w = head;
	 while (w)
	 {
	    if (qc == w->sclass)
	       return true;
	    w = w->next;
	 }
	 return false;
      }
      inline class QoreClass *getClass(int cid);

      //inline void execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      inline void execDestructors(class Object *o, class ExceptionSink *xsink);
      inline void execSystemDestructors(class Object *o, class ExceptionSink *xsink);
      inline void execCopyMethods(class Object *self, class Object *old, class ExceptionSink *xsink);
};

/*
  BCNode 
  base class pointer
*/
class BCNode
{
   public:
      class NamedScope *cname;
      char *cstr;
      class QoreClass *sclass;
      class BCNode *next;
      class QoreNode *args;
      bool hasargs;
      bool priv;
      
      inline BCNode(class NamedScope *c, bool p)
      {
	 cname = c;
	 cstr = NULL;
	 sclass = NULL;
	 next = NULL;
	 args = NULL;
	 hasargs = false;
	 priv = p;
      }
      inline BCNode(char *str, bool p)
      {
	 cname = NULL;
	 cstr = str;
	 sclass = NULL;
	 next = NULL;
	 args = NULL;
	 hasargs = false;
	 priv = p;
      }
      inline ~BCNode();
};

/*
  BCList
  doubly-linked list of base classes, constructors called head->tail, 
  destructors called in reverse order (tail->head)
  note that this data structure cannot be modified even if the class is
  copied to a subprogram object and extended
*/
class BCList : public ReferenceObject
{
   private:
      bool init;

   protected:
      inline ~BCList()
      {
	 while (head)
	 {
	    class BCNode *n = head->next;
	    delete head;
	    head = n;
	 }
      }

   public:
      // head and tail pointers to maintain insertion order
      class BCNode *head, *tail;
      // special method (constructor, destructor, copy) list for superclasses 
      class BCSMList sml;

      inline BCList(class BCNode *n)
      {
	 head = tail = n;
	 init = false;
      }
      inline void add(class BCNode *n);
      inline void parseInit(class QoreClass *thisclass, class BCAList *bcal);
      inline class Method *resolveSelfMethod(char *name);
      inline class Method *findMethod(char *name);
      inline class Method *findMethod(char *name, bool *p);
      inline bool match(class BCANode *bca);
      inline void execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      inline void execSystemConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      inline bool isPrivateMember(char *str);
      inline void ref()
      {
	 ROreference();
      }
      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

class Member {
   public:
      char *name;
      class Member *next;

      inline Member(char *n)
      {
	 name = n;
      }

      inline ~Member()
      {
	 if (name)
	    free(name);
      }
};

class MemberList {
   public:
      class Member *head;

      inline MemberList()
      {
	 head = NULL;
      }
      inline MemberList(char *name)
      {
	 head = new Member(name);
	 head->next = NULL;
      }
      inline ~MemberList()
      {
	 while (head)
	 {
	    class Member *w = head->next;
	    delete head;
	    head = w;
	 }
      }
      class MemberList *copy()
      {
	 class MemberList *nl = new MemberList();
	 class Member *w = head;
	 while (w)
	 {
	    nl->add(strdup(w->name));
	    w = w->next;
	 }
	 return nl;
      }
      inline int add(char *name);
      inline void add(class Member *w)
      {
	 w->next = head;
	 head = w;
      }
      inline bool inlist(char *name)
      {
	 class Member *w = head;
	 while (w)
	 {
	    if (!strcmp(name, w->name))
	       return true;
	    w = w->next;
	 }
	 return false;
      }
};

/*
  QoreClass

  the class is a ReferenceObject, because objects instantiated from this class
  may exist longer than the parent object, and we don't want the class to
  disappear while there are still objects in existance that have been instantiated
  from this class. The ref() and deref() functions are called from the Object class
  when it is created or destroyed
*/
class QoreClass : public ReferenceObject //, public LockedObject
{
   private:
      char *name;
#ifdef HAVE_QORE_HASH_MAP
      hm_method_t hm;
      hm_qn_t pmm, pending_pmm;
#else
      class MemberList *privateMemberList, *pending_privateMemberList;
      class Method *methodlist_head, *methodlist_tail;
      int numFuncs;
#endif
      class Method *pending_head, *system_constructor;
      class Method *constructor, *destructor, *copyMethod, *methodGate, *memberGate;
      int classID;
      bool sys, initialized;
      int domain;            // capabilities of builtin class to use in the context of parse restrictions
      class ReferenceObject nref;  // namespace references

      inline void init(char *nme, int dom = 0);
      inline class Method *parseFindMethod(char *name);
      void insertMethod(class Method *o);
      // checks for all special methods except constructor & destructor
      inline void checkSpecialIntern(class Method *m);
      // checks for all special methods
      inline void checkSpecial(class Method *m);
      class QoreNode *evalMethodGate(class Object *self, char *nme, class QoreNode *args, class ExceptionSink *xsink);
      inline void delete_pending_methods()
      {
	 Method *w = pending_head;

	 printd(5, "QoreClass::delete_pending_methods() %s this=%08p start=%08p\n", name, this, w);
	 while (w)
	 {
	    class Method *n = w->next;
	    delete w;
	    w = n;
	 }

	 pending_head = NULL;
      }

   protected:
      inline ~QoreClass();

   public:
      class BCAList *bcal;         // base class constructor argument list
      class BCList *scl;           // base class list

      inline QoreClass(int dom, char *nme);
      inline QoreClass(char *nme = 0);
      inline QoreClass(char *nme, int id);

      inline void addMethod(class Method *f);
      inline void addMethod(char *nme, q_method_t m);
      inline void setDestructor(q_destructor_t m);
      inline void setConstructor(q_constructor_t m);
      inline void setCopy(q_copy_t m);

      inline void addPrivateMember(char *name);
      inline void mergePrivateMembers(class MemberList *n);
      bool isPrivateMember(char *str);

      class QoreNode *evalMethod(class Object *self, char *nme, class QoreNode *args, class ExceptionSink *xsink);
      inline class QoreNode *evalMemberGate(class Object *self, class QoreNode *name, class ExceptionSink *xsink);
      inline class QoreNode *execConstructor(class QoreNode *args, class ExceptionSink *xsink);
      inline class QoreNode *execSystemConstructor(class QoreNode *args, class ExceptionSink *xsink);
      inline void execSubclassConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink);
      inline void execSubclassSystemConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink);
      inline void execDestructor(class Object *self, class ExceptionSink *xsink);
      inline void execSubclassDestructor(class Object *self, class ExceptionSink *xsink);
      inline void execSubclassSystemDestructor(class Object *self, class ExceptionSink *xsink);
      inline class QoreNode *execCopy(class Object *old, class ExceptionSink *xsink);
      inline void execSubclassCopy(class Object *self, class Object *old, class ExceptionSink *xsink);

      inline class Method *findMethod(char *nme);
      inline class Method *findMethod(char *nme, bool *priv);
      inline class Method *findLocalMethod(char *name);
      inline class Method *resolveSelfMethodIntern(char *nme);
      inline class Method *resolveSelfMethod(char *nme);
      inline class Method *resolveSelfMethod(class NamedScope *nme);
      inline void setSystemConstructor(q_constructor_t m);
      inline class List *getMethodList();
      inline int numMethods() 
      {
#ifdef HAVE_QORE_HASH_MAP
	 return hm.size();
#else
	 return numFuncs;
#endif
      }
      inline int hasCopy() { return copyMethod ? 1 : 0; }
      class QoreClass *copyAndDeref();
      inline int getID() { return classID; }
      inline void parseInit();
      inline void parseCommit();
      inline void parseRollback();
      inline bool isSystem() { return sys; }
      inline void addBaseClassesToSubclass(class QoreClass *sc);
      inline class QoreClass *getClass(int cid)
      {
	 if (cid == classID)
	    return this;
	 return scl ? scl->sml.getClass(cid) : NULL;
      }
      inline void deref();
      inline bool hasMemberGate()
      {
	 return memberGate != NULL;
      }
      inline void ref()
      {
	 //printd(5, "QoreClass::ref() %08x %s %d -> %d\n", this, name, reference_count(), reference_count() + 1);
	 ROreference();
      }
      inline class QoreClass *getReference()
      {
	 //printd(5, "QoreClass::getReference() %08x %s %d -> %d\n", this, name, nref.reference_count(), nref.reference_count() + 1);
	 nref.ROreference();
	 return this;
      }
      inline void nderef();
      inline bool is_unique()
      {
	 return nref.is_unique();
      }
      inline void addDomain(int dom)
      {
	 domain |= dom;
      }
      inline int getDomain()
      {
	 return domain;
      }
      inline char *getName() { return name; }
      inline void setName(char *n)
      {
#ifdef DEBUG
	 if (name)
	    run_time_error("QoreClass::setName(%08p '%s') name already set to %08p '%s'", n, n, name, name);
#endif
	 name = n;
      }
      //inline void merge(class QoreClass *oc);
};

static inline class QoreClass *getStackClass();
static inline class QoreNode *evalStackObjectValue(char *name, class ExceptionSink *xsink);
static inline class QoreNode **getStackObjectValuePtr(char *name, class VLock *vl, ExceptionSink *xsink);
static inline class QoreNode *getStackObjectValue(char *name, class VLock *vl, ExceptionSink *xsink);

#include <qore/common.h>
#include <qore/Exception.h>
#include <qore/thread.h>
#include <qore/Function.h>
#include <qore/List.h>
#include <qore/Statement.h>
#include <qore/Object.h>
#include <qore/QoreType.h>
#include <qore/Variable.h>
#include <qore/Sequence.h>
#include <qore/Namespace.h>

#include <string.h>

extern class Sequence classIDSeq;

inline void QoreClass::nderef()
{
   //printd(5, "QoreClass::nderef() %08p %s %d -> %d\n", this, name, nref.reference_count(), nref.reference_count() - 1);
   if (nref.ROdereference())
      deref();
}

inline void QoreClass::deref()
{
   //printd(5, "QoreClass::deref() %08p %s %d -> %d\n", this, name, reference_count(), reference_count() - 1);
   if (ROdereference())
      delete this;
}

inline void BCEAList::add(class QoreClass *qc, class QoreNode *arg, class ExceptionSink *xsink)
{
   // see if class already exists in the list
   class BCEANode *w = head;
   while (w)
   {
      if (w->sclass == qc)
	 return;
      w = w->next;
   }
   // evaluate and save arguments
   class BCEANode *n = new BCEANode(qc, arg ? arg->eval(xsink) : NULL);
   n->next = head;
   head = n;
}

inline void BCEAList::deref(class ExceptionSink *xsink)
{
   while (head)
   {
      class BCEANode *w = head->next;
      if (head->args)
	 head->args->deref(xsink);
      delete head;
      head = w;
   }
   delete this;
}

inline BCANode::~BCANode()
{
   if (ns)
      delete ns;
   if (name)
      free(name);
}

inline void BCANode::resolve()
{
   if (ns)
   {
      sclass = parseFindScopedClass(ns);
      printd(5, "BCANode::resolve() this=%08p resolved named scoped %s -> %08p\n", this, ns->ostr, sclass);
      delete ns;
      ns = NULL;
   }
   else
   {
      sclass = parseFindClass(name);
      printd(5, "BCANode::resolve() this=%08p resolved %s -> %08p\n", this, name, sclass);
      free(name);
      name = NULL;
   }   
}

inline void BCSMList::addBaseClassesToSubclass(class QoreClass *thisclass, class QoreClass *sc)
{
   class BCSMNode *w = head;
   while (w)
   {
      sc->scl->sml.add(thisclass, w->sclass);
      w = w->next;
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
   class BCSMNode *w = head;
   while (w)
   {
      if (w->sclass == qc)
         return;
      if (w->sclass == thisclass)
      {
      	 parse_error("circular reference in class hierarchy, '%s' is an ancestor of itself", thisclass->getName());
      	 return;
      }
      w = w->next;
   }
   // append to the end of the doubly-linked list
   w = new BCSMNode(qc);
   if (tail)
      tail->next = w;
   else
      head = w;
   w->prev = tail;
   tail = w;
}

/*
inline void BCSMList::execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink)
{
   class BCSMNode *w = head;
   while (w)
   {
      class QoreNode *args = bceal->findArgs(w->sclass);
      printd(5, "BCSMList::execConstructors() %s (%08p) base class %s args=%08p\n", o->getClass()->getName(), o, w->sclass->getName(), args);
      w->sclass->execSubclassConstructor(o, args, xsink);
      if (xsink->isEvent())
	 break;
      w = w->next;
   }
}
*/

inline void BCSMList::execDestructors(class Object *o, class ExceptionSink *xsink)
{
   class BCSMNode *w = tail;
   while (w)
   {
      printd(5, "BCSMList::execDestructors() %s::destructor() o=%08p (subclass %s)\n", w->sclass->getName(), o, o->getClass()->getName());
      w->sclass->execSubclassDestructor(o, xsink);
      w = w->prev;
   }
}

inline void BCSMList::execSystemDestructors(class Object *o, class ExceptionSink *xsink)
{
   class BCSMNode *w = tail;
   while (w)
   {
      printd(5, "BCSMList::execSystemDestructors() %s::destructor() o=%08p (subclass %s)\n", w->sclass->getName(), o, o->getClass()->getName());
      w->sclass->execSubclassSystemDestructor(o, xsink);
      w = w->prev;
   }
}

inline void BCSMList::execCopyMethods(class Object *self, class Object *old, class ExceptionSink *xsink)
{
   class BCSMNode *w = head;
   while (w)
   {
      w->sclass->execSubclassCopy(self, old, xsink);
      if (xsink->isEvent())
	 break;
      w = w->next;
   }
}

inline class QoreClass *BCSMList::getClass(int cid)
{
   class BCSMNode *w = head;
   while (w)
   {
      if (w->sclass->getID() == cid)
	 return w->sclass;
      w = w->next;
   }
   return NULL;
}

inline BCNode::~BCNode()
{
   if (cname)
      delete cname;
   if (cstr)
      free(cstr);
   if (args)
      args->deref(NULL);
}

inline void BCList::execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink)
{
   class BCNode *w = head;
   while (w)
   {
      printd(5, "BCList::execConstructors() %s::constructor() o=%08p (for subclass %s)\n", w->sclass->getName(), o, o->getClass()->getName()); 
      w->sclass->execSubclassConstructor(o, bceal, xsink);
      if (xsink->isEvent())
	 break;
      w = w->next;
   }
}

inline void BCList::execSystemConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink)
{
   class BCNode *w = head;
   while (w)
   {
      printd(5, "BCList::execSystemConstructors() %s::constructor() o=%08p (for subclass %s)\n", w->sclass->getName(), o, o->getClass()->getName()); 
      w->sclass->execSubclassSystemConstructor(o, bceal, xsink);
      if (xsink->isEvent())
	 break;
      w = w->next;
   }
}

inline void BCList::add(class BCNode *n)
{
   tail->next = n;
   tail = n;
}

inline void BCList::parseInit(class QoreClass *cls, class BCAList *bcal)
{
   if (init)
      return;

   init = true;

   class BCNode *w = head;
   while (w)
   {
      if (w->cname)
      {
	 w->sclass = parseFindScopedClass(w->cname);
	 printd(5, "BCList::parseInit() %s inheriting %s (%08p)\n", cls->getName(), w->cname->ostr, w->sclass);
      }
      else
      {
	 w->sclass = parseFindClass(w->cstr);
	 printd(5, "BCList::parseInit() %s inheriting %s (%08p)\n", cls->getName(), w->cstr, w->sclass);
      }
      // recursively add base classes to special method list
      if (w->sclass)
      {
         w->sclass->addBaseClassesToSubclass(cls);
	 // include all subclass domains in this class' domain
	 cls->addDomain(w->sclass->getDomain());
      }

      w = w->next;
   }

   // compare each class in the list to ensure that there are no duplicates
   w = head;
   while (w)
   {
      if (w->sclass)
      {
	 class BCNode *n = w->next;
	 while (n)
	 {
	    if (w->sclass == n->sclass)
	       parse_error("class '%s' cannot inherit '%s' more than once", cls->getName(), w->sclass->getName());

	    n = n->next;
	 }
      }	 
      w = w->next;
   }

   // if there is a base class constructor list, resolve all classes and 
   // ensure that all classes referenced are base classes of this class
   if (bcal)
   {
      class BCANode *w = bcal->head;
      while (w)
      {
	 w->resolve();
	 if (w->sclass && !match(w))
	    parse_error("%s in base class constructor argument list is not a base class of %s", w->sclass->getName(), cls->getName());
	 w = w->next;
      }
   }
}

inline class Method *BCList::resolveSelfMethod(char *name)
{
   class Method *m;
   class BCNode *w = head;
   while (w)
   {
      if (w->sclass)
      {
	 if (w->sclass->scl)
	    w->sclass->scl->parseInit(w->sclass, w->sclass->bcal);
	 if ((m = w->sclass->resolveSelfMethodIntern(name)))
	    return m;
      }
      w = w->next;
   }
   return NULL;
}

inline class Method *BCList::findMethod(char *name)
{
   class Method *m;
   class BCNode *w = head;
   while (w)
   {
      if (w->sclass)
      {
	 if (w->sclass->scl)
	    w->sclass->scl->parseInit(w->sclass, w->sclass->bcal);
	 if ((m = w->sclass->findMethod(name)))
	    return m;
      }
      w = w->next;
   }
   return NULL;
}

// only called at run-time
inline class Method *BCList::findMethod(char *name, bool *priv)
{
   class Method *m;
   class BCNode *w = head;
   while (w)
   {
      if (w->sclass)
      {
	 if ((m = w->sclass->findMethod(name, priv)))
	 {
	    if (!*priv && w->priv)
	       (*priv) = w->priv;
	    return m;
	 }
      }
      w = w->next;
   }
   return NULL;
}

inline bool BCList::match(class BCANode *bca)
{
   class BCNode *w = head;
   while (w)
   {
      if (bca->sclass == w->sclass)
      {
	 w->args = bca->argexp;
	 w->hasargs = true;
	 return true;
      }
      w = w->next;
   }
   bca->argexp->deref(NULL);
   return false;
}

inline bool BCList::isPrivateMember(char *str)
{
   class BCNode *w = head;
   while (w)
   {
      if (w->sclass->isPrivateMember(str))
	 return true;
      w = w->next;
   }
   return false;
}

class BuiltinMethod : public BuiltinFunction, public ReferenceObject
{
   protected:
      inline ~BuiltinMethod() {}

   public:
      class QoreClass *myclass;

      inline BuiltinMethod(class QoreClass *c, char *nme, q_method_t m) : BuiltinFunction(nme, m, QDOM_DEFAULT), myclass(c) {}
      inline BuiltinMethod(class QoreClass *c, q_constructor_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      inline BuiltinMethod(class QoreClass *c, q_destructor_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      inline BuiltinMethod(class QoreClass *c, q_copy_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      inline void deref();
};

inline void BuiltinMethod::deref()
{
   if (ROdereference())
      delete this;
}

inline void QoreClass::init(char *nme, int dom)
{
   initialized = false;
   domain = dom;
   scl = NULL;
   name = nme;
   sys  = false;
   pending_head = NULL;
   bcal = NULL;
#ifndef HAVE_QORE_HASH_MAP
   methodlist_head = NULL;
   methodlist_tail = NULL;
   numFuncs = 0;
   privateMemberList = new MemberList();
   pending_privateMemberList = new MemberList();
#endif

   // quick pointers
   constructor = NULL;
   destructor  = NULL;
   copyMethod  = NULL;
   methodGate  = NULL;
   memberGate  = NULL;

   system_constructor = NULL;
}

inline QoreClass::QoreClass(int dom, char *nme)
{
   init(nme, dom);

   classID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating '%s' ID:%d (this=%08p)\n", name, classID, this);
}

inline QoreClass::QoreClass(char *nme)
{
   init(nme);

   classID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating '%s' ID:%d (this=%08p)\n", name, classID, this);
}

inline QoreClass::QoreClass(char *nme, int id)
{
   init(strdup(nme));

   classID = id;
   printd(5, "QoreClass::QoreClass() creating copy of '%s' ID:%d (this=%08p)\n", name, classID, this);
}

inline QoreClass::~QoreClass()
{
   //printd(5, "QoreClass::~QoreClass() deleting %08p %s\n", this, name);
#ifdef HAVE_QORE_HASH_MAP
   hm_method_t::iterator i;
   while ((i = hm.begin()) != hm.end())
   {
      class Method *m = i->second;
      //printd(5, "QoreClass::~QoreClass() deleting method %08p %s::%s()\n", m, name, m->name);
      hm.erase(i);
      delete m;
   }   
   // delete private member list
   hm_qn_t::iterator j;
   while ((j = pmm.begin()) != pmm.end())
   {
      char *n = j->first;
      pmm.erase(j);
      free(n);
   }
   while ((j = pending_pmm.begin()) != pending_pmm.end())
   {
      char *n = j->first;
      pending_pmm.erase(j);
      free(n);
   }
#else
   while (methodlist_head)
   {
      Method *w = methodlist_head;
      methodlist_head = methodlist_head->next;
      printd(5, "QoreClass::~QoreClass() this=%08p normal list: deleting %s.%s()\n", this, name, w->name);
      delete w;
   }
   // delete private member list
   delete privateMemberList;
   delete pending_privateMemberList;
#endif
   // delete any pending methods
   delete_pending_methods();
   free(name);
   if (scl)
      scl->deref();
   if (system_constructor)
      delete system_constructor;
}

inline void QoreClass::addBaseClassesToSubclass(class QoreClass *sc)
{      
   if (scl)
   {
      // initialize list, just in case
      scl->parseInit(this, bcal);
      scl->sml.addBaseClassesToSubclass(this, sc);
   }
   sc->scl->sml.add(sc, this);
}

inline void QoreClass::checkSpecialIntern(class Method *m)
{
   // set quick pointers
   if (!methodGate && !strcmp(m->name, "methodGate"))
      methodGate = m;
   else if (!memberGate && !strcmp(m->name, "memberGate"))
      memberGate = m;
}

inline void QoreClass::checkSpecial(class Method *m)
{
   // set quick pointers
   if (!constructor && !strcmp(m->name, "constructor"))
      constructor = m;
   else if (!destructor && !strcmp(m->name, "destructor"))
      destructor = m;
   else if (!copyMethod && !strcmp(m->name, "copy"))
      copyMethod = m;
   else 
      checkSpecialIntern(m);
}

inline class Method *QoreClass::findLocalMethod(char *nme)
{
#ifdef HAVE_QORE_HASH_MAP
   hm_method_t::iterator i = hm.find(nme);
   if (i != hm.end())
      return i->second;

#else
   class Method *w = methodlist_head;

   while (w)
   {
      printd(5, "QoreClass::findLocalMethod(%s) == %s (class %s)\n", nme, w->name, name);
      if (!strcmp(nme, w->name))
	 return w;
      w = w->next;
   }
#endif
   return NULL;
}

inline Method *QoreClass::findMethod(char *nme)
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

inline Method *QoreClass::findMethod(char *nme, bool *priv)
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

inline Method *QoreClass::parseFindMethod(char *nme)
{
   class Method *m;
   if ((m = findLocalMethod(nme)))
      return m;

   // look in pending methods
   m = pending_head;
   while (m)
   {
      if (!strcmp(m->name, nme))
	 return m;
      m = m->next;
   }
   return NULL;
}

// called from subclasses only
inline Method *QoreClass::resolveSelfMethodIntern(char *nme)
{
   class Method *m = parseFindMethod(nme);

   // if still not found now look in superclass methods
   if (!m && scl)
      m = scl->resolveSelfMethod(nme);

   return m;
}

inline Method *QoreClass::resolveSelfMethod(char *nme)
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
	 m = pending_head;

	 while (m)
	 {
	    if (!strcmp(m->name, nme))
	    {
	       printd(5, "QoreClass::resolveSelfMethod(%s) resolved to pending method %s::%s() %08p\n", nme, name, nme, m);
	       break;
	    }

	    m = m->next;
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

inline Method *QoreClass::resolveSelfMethod(class NamedScope *nme)
{
   // first find class
   class QoreClass *qc = parseFindScopedClassWithMethod(nme);
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
	 m = qc->pending_head;

	 while (m)
	 {
	    if (!strcmp(m->name, nstr))
	       break;
	    m = m->next;
	 }
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

inline void QoreClass::addMethod(Method *m)
{
   printd(5, "QoreClass::addMethod(%08p) %s.%s() (this=%08p)\n", m, name ? name : "<pending>", m->name, this);

   // check for illegal private constructor or destructor
   if (!strcmp(m->name, "constructor") || !strcmp(m->name, "destructor"))
   {
      if (m->isPrivate())
	 parseException("ILLEGAL-PRIVATE-METHOD", "%s methods cannot be private", m->name);
   }

   if (parseFindMethod(m->name))
   {
      parse_error("method '%s::%s()' has already been defined", name, m->name);
      delete m;
   }
   else
   {
      // insert in pending list for parse init
      m->next = pending_head;
      pending_head = m;

      // if there is a base class constructor argument list, then put it at the class level
      if (m->bcal)
      {
	 // if the constructor is being defined after the class has already been initialized, then throw a parse exception
	 if (numMethods())
	    parse_error("constructors making explicit calls to base classes must be defined when the class is defined");
	 else
	    bcal = m->bcal;
      }
   }
}

// adds a builtin method to the class - no duplicate checking is made
inline void QoreClass::addMethod(char *nme, q_method_t m)
{
#ifdef DEBUG
   if (!strcmp(nme, "constructor") || !strcmp(nme, "destructor") || !strcmp(nme, "copy"))
      run_time_error("cannot call QoreClass::addMethod('%s')  use setConstructor() setDestructor() setCopy() instead", nme);
#endif

   sys = true;
   BuiltinMethod *b = new BuiltinMethod(this, nme, m);
   Method *o = new Method(b);
   insertMethod(o);
   // check for special methods (except constructor and destructor)
   checkSpecialIntern(o);
}

inline void QoreClass::setSystemConstructor(q_constructor_t m)
{
   sys = true;
   system_constructor = new Method(new BuiltinMethod(this, m));
}

// sets a builtin function as class constructor - no duplicate checking is made
inline void QoreClass::setConstructor(q_constructor_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   constructor = o;
}

// sets a builtin function as class destructor - no duplicate checking is made
inline void QoreClass::setDestructor(q_destructor_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   destructor = o;
}

// sets a builtin function as class copy constructor - no duplicate checking is made
inline void QoreClass::setCopy(q_copy_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   copyMethod = o;
}

inline class QoreNode *QoreClass::evalMemberGate(class Object *self, class QoreNode *nme, class ExceptionSink *xsink)
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

inline class QoreNode *QoreClass::execConstructor(QoreNode *args, ExceptionSink *xsink)
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

inline class QoreNode *QoreClass::execSystemConstructor(QoreNode *args, class ExceptionSink *xsink)
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
#ifdef DEBUG
   if (xsink->isEvent())
   {
      o->dereference(xsink);
      run_time_error("QoreClass::execSystemConstructor() %s::constructor() o=%08p, exception in constructor, dereferencing object and returning NULL\n", name, o);
      return NULL;
   }
#endif

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

inline void QoreClass::execDestructor(Object *self, ExceptionSink *xsink)
{
   printd(5, "QoreClass::execDestructor() %s::destructor() o=%08p\n", name, self);

   class ExceptionSink de;

   if (self->isSystemObject())
   {
      if (destructor)
	 destructor->evalSystemDestructor(self, &de);

      // execute superclass destructors
      if (scl)
	 scl->sml.execSystemDestructors(self, &de);
   }
   else
   {
      if (destructor)
	 destructor->evalDestructor(self, &de);

      // execute superclass destructors
      if (scl)
	 scl->sml.execDestructors(self, &de);
   }

   xsink->assimilate(&de);
}

inline void QoreClass::execSubclassDestructor(Object *self, ExceptionSink *xsink)
{
   class ExceptionSink de;
   if (destructor)
      destructor->evalDestructor(self, &de);

   xsink->assimilate(&de);
}

inline void QoreClass::execSubclassSystemDestructor(Object *self, ExceptionSink *xsink)
{
   class ExceptionSink de;
   if (destructor)
      destructor->evalSystemDestructor(self, &de);

   xsink->assimilate(&de);
}

inline class QoreNode *QoreClass::execCopy(Object *old, ExceptionSink *xsink)
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

inline List *QoreClass::getMethodList()
{
   List *l = new List();

#ifdef HAVE_QORE_HASH_MAP
   for (hm_method_t::iterator i = hm.begin(); i != hm.end(); i++)
      l->push(new QoreNode(i->first));
#else
   Method *w = methodlist_head;

   while (w)
   {
      if (!w->isPrivate())
	 l->push(new QoreNode(w->name));
      w = w->next;
   }   
#endif
   return l;
}

// initializes all user methods
inline void QoreClass::parseInit()
{
   setParseClass(this);
   if (!initialized)
   {
      printd(5, "QoreClass::parseInit() %s this=%08p start=%08p\n", name, this, pending_head);
      if (scl)
	 scl->parseInit(this, bcal);

      if (!sys && domain & getProgram()->getParseOptions())
	 parseException("ILLEGAL-CLASS-DEFINITION", "class '%s' inherits functionality from base classes that is restricted by current parse options", name);
      initialized = true;
   }

   class Method *w = pending_head;
   while (w)
   {
      // initialize method
      if (w->bcal)
	 w->parseInitConstructor(scl);
      else
	 w->parseInit();
      w = w->next;
   }
}

// commits all pending user methods
inline void QoreClass::parseCommit()
{
   class Method *w = pending_head;

   printd(5, "QoreClass::parseCommit() %s this=%08p start=%08p\n", name, this, w);
   while (w)
   {
      insertMethod(w);
      checkSpecial(w);
      w = w->next;
   }

   // add all pending private members
#ifdef HAVE_QORE_HASH_MAP
   hm_qn_t::iterator i;
   while ((i = pending_pmm.begin()) != pending_pmm.end())
   { 
      //printd(5, "QoreClass::parseCommit() %s committing private member %08p %s\n", name, i->first, i->first);
      pmm[i->first] = NULL;
      pending_pmm.erase(i);
   }
#else
   Member *mw = pending_privateMemberList->head;
   while (mw)
   {
      class Member *n = mw->next;
      privateMemberList->add(mw);
      mw = n;
   }
   pending_privateMemberList->head = NULL;
#endif

   pending_head = NULL;
}

// deletes all pending user methods
inline void QoreClass::parseRollback()
{
   delete_pending_methods();
}

inline void Method::userInit(UserFunction *u, int p)
{
   name = strdup(u->name);
   type = OTF_USER;
   func.userFunc = u;
   priv = p;
#ifndef HAVE_QORE_HASH_MAP
   next = NULL;
#endif
}

inline Method::Method(UserFunction *u, int p, BCAList *b)
{
   userInit(u, p);
   bcal = b;
}

inline Method::Method(BuiltinMethod *b)
{
   name = b->name;
   type = OTF_BUILTIN;
   func.builtin = b;
   priv = 0;
   bcal = NULL;
#ifndef HAVE_QORE_HASH_MAP
   next = NULL;
#endif
}

inline Method::~Method()
{
   if (name && type != OTF_BUILTIN)
      free(name);
   if (type == OTF_USER)
      func.userFunc->deref();
   else
      func.builtin->deref();
   if (bcal)
      bcal->deref();
}

inline bool Method::isSynchronized() 
{
   if (type == OTF_BUILTIN)
      return false;
   return func.userFunc->isSynchronized();
}

inline bool Method::inMethod(class Object *self)
{
   if (type == OTF_USER)
      return ::inMethod(func.userFunc->name, self);
   return ::inMethod(func.builtin->name, self);
}

inline void Method::evalSystemConstructor(Object *self, QoreNode *args, class BCList *bcl, class BCEAList *bceal, ExceptionSink *xsink)
{
   // type must be OTF_BUILTIN
   func.builtin->evalSystemConstructor(self, args, xsink);
}

inline void Method::evalSystemDestructor(class Object *self, class ExceptionSink *xsink)
{
   // get pointer to private data object from class ID of base type
   void *ptr = self->getAndClearPrivateData(func.builtin->myclass->getID());
   //printd(5, "Method::evalSystemDestructor() class=%s (%08p) id=%d ptr=%08p\n", func.builtin->myclass->getName(), func.builtin->myclass, func.builtin->myclass->getID(), ptr);
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

inline class Method *Method::copy()
{
   class Method *nof;
   if (type == OTF_USER)
   {
      func.userFunc->ROreference();
      nof = new Method;
      nof->userInit(func.userFunc, priv);
      if (bcal)
      {
	 bcal->ref();
	 nof->bcal = bcal;
      }
   }
   else
   {
      func.builtin->ROreference();
      nof = new Method(func.builtin);
   }
   return nof;
}

// NOTE: caller must unlock structure
static inline class QoreNode **getStackObjectValuePtr(char *name, class VLock *vl, ExceptionSink *xsink)
{
   // this will always return a value
   Object *o = getStackObject();

   return o->getMemberValuePtr(name, vl, xsink);
}

// NOTE: caller must unlock structure
static inline class QoreNode **getExistingStackObjectValuePtr(char *name, class VLock *vl, ExceptionSink *xsink)
{
   // this will always return a value
   Object *o = getStackObject();

   return o->getExistingValuePtr(name, vl, xsink);
}

// NOTE: caller must unlock structure
static inline class QoreNode *getStackObjectValue(char *name, class VLock *vl, ExceptionSink *xsink)
{
   // this will always return a value
   Object *o = getStackObject();

   return o->getMemberValueNoMethod(name, vl, xsink);
}

static inline class QoreNode *evalStackObjectValue(char *name, class ExceptionSink *xsink)
{
   class QoreNode *rv;

   tracein("evalStackObjectValue()");
   class Object *o = getStackObject();
#ifdef DEBUG
   if (!o)
      run_time_error("evalStackObjectalue(%s) object context is NULL", name);
#endif
   printd(5, "evalStackObjectValue() o=%08p (%s)\n", o, o->getClass()->getName());
   rv = o->evalMemberNoMethod(name, xsink);
   traceout("evalStackObjectValue()");
   return rv;
}

// assumes that there is always an object on top of the stack
static inline void deleteStackObjectKey(char *name, ExceptionSink *xsink)
{
   getStackObject()->deleteMemberValue(name, xsink);
}

static inline QoreClass *getStackClass()
{
   class Object *obj = getStackObject();
   if (obj)
      return obj->getClass();
   return NULL;
}

inline void QoreClass::addPrivateMember(char *nme)
{
#ifdef HAVE_QORE_HASH_MAP
   hm_qn_t::iterator i;
   if ((i = pmm.find(nme)) == pmm.end())
   {
      if ((i = pending_pmm.find(nme)) == pending_pmm.end())
      {
	 //printd(5, "QoreClass::addPrivateMember() this=%08p %s adding %08p %s\n", this, name, nme, nme);
	 pending_pmm[nme] = NULL;
      }
      else
#else
   if (!privateMemberList->inlist(nme))
   {
      if (pending_privateMemberList->add(nme))
#endif
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

inline void QoreClass::mergePrivateMembers(class MemberList *ml)
{
   class Member *w = ml->head;
   while (w)
   {
      addPrivateMember(w->name);
      w->name = NULL;
      w = w->next;
   }
   delete ml;
}

inline int MemberList::add(char *name)
{
   class Member *w = head;
   while (w)
   {
      if (!strcmp(name, w->name))
	 return -1;
      w = w->next;
   }
   // add new member to list
   add(new Member(name));
   return 0;
}

#endif // _QORE_QORECLASS_H
