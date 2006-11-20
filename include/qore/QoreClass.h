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

#include <qore/ReferenceObject.h>
#include <qore/StringList.h>
#include <qore/support.h>

#include <stdlib.h>
#include <string.h>

#include <qore/hash_map.h>
#include <list>

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

      DLLLOCAL inline Method() { bcal = NULL; }
      DLLLOCAL inline void userInit(UserFunction *u, int p);

   protected:

   public:
      char *name;
      class Method *next;
      class BCAList *bcal; // for subclass constructors only

      DLLLOCAL inline Method(class UserFunction *u, int p, class BCAList *b);
      DLLLOCAL inline Method(class BuiltinMethod *b);
      DLLLOCAL inline ~Method();
      DLLLOCAL inline bool inMethod(class Object *self);
      DLLLOCAL class QoreNode *eval(class Object *self, class QoreNode *args, class ExceptionSink *xsink);
      DLLLOCAL void evalConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL void evalDestructor(class Object *self, class ExceptionSink *xsink);
      DLLLOCAL inline void evalSystemConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline void evalSystemDestructor(class Object *self, class ExceptionSink *xsink);
      DLLLOCAL void evalCopy(class Object *self, class Object *old, class ExceptionSink *xsink);
      DLLLOCAL inline class Method *copy();
      DLLLOCAL inline void parseInit();
      DLLLOCAL inline void parseInitConstructor(class BCList *bcl);
      DLLLOCAL inline int getType() const
      { 
	 return type; 
      }
      DLLLOCAL inline bool isPrivate() const 
      { 
	 return priv; 
      }
      DLLLOCAL inline char *getName() const
      { 
	 return name; 
      }
      // only called when method is user
      DLLLOCAL inline bool isSynchronized() const;
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

      DLLLOCAL inline BCANode(class NamedScope *n, class QoreNode *arg)
      {
	 ns = n;
	 name = NULL;
	 argexp = arg;
      }
      DLLLOCAL inline BCANode(char *n, class QoreNode *arg)
      {
	 ns = NULL;
	 name = n;
	 argexp = arg;
      }
      DLLLOCAL inline ~BCANode();
      DLLLOCAL inline void resolve();
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
      DLLLOCAL inline ~BCAList()
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

      DLLLOCAL inline BCAList(class BCANode *n)
      {
	 head = n;
	 n->next = NULL;
      }
      DLLLOCAL inline void add(class BCANode *n)
      {
	 n->next = head;
	 head = n;
      }
      DLLLOCAL inline void ref()
      {
	 ROreference();
      }
      DLLLOCAL inline void deref()
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

      DLLLOCAL inline BCEANode(class QoreClass *qc, class QoreNode *arg)
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
      DLLLOCAL inline ~BCEAList() { }

   public:
      class BCEANode *head;

      DLLLOCAL inline BCEAList()
      {
	 head = NULL;
      }
      DLLLOCAL inline void deref(class ExceptionSink *xsink);
      DLLLOCAL inline void add(class QoreClass *qc, class QoreNode *arg, class ExceptionSink *xsink);
      DLLLOCAL inline void addDefault(class QoreClass *qc)
      {
	 class BCEANode *n = new BCEANode(qc, NULL);
	 n->next = head;
	 head = n;
	 n->execed = true;
      }
      DLLLOCAL inline class QoreNode *findArgs(class QoreClass *qc, bool *aexeced)
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
      
      DLLLOCAL inline BCNode(class NamedScope *c, bool p)
      {
	 cname = c;
	 cstr = NULL;
	 sclass = NULL;
	 next = NULL;
	 args = NULL;
	 hasargs = false;
	 priv = p;
      }
      DLLLOCAL inline BCNode(char *str, bool p)
      {
	 cname = NULL;
	 cstr = str;
	 sclass = NULL;
	 next = NULL;
	 args = NULL;
	 hasargs = false;
	 priv = p;
      }
      DLLLOCAL inline ~BCNode();
};

typedef std::list<class QoreClass *> class_list_t;

// BCSMList: Base Class System Method List
// unique list of base classes for a class hierarchy to ensure that "special" methods, constructor(), destructor(), copy() - are executed only once
class BCSMList : public class_list_t
{
   public:
      DLLLOCAL inline void add(class QoreClass *thisclass, class QoreClass *qc);
      DLLLOCAL inline void addBaseClassesToSubclass(class QoreClass *thisclass, class QoreClass *sc);
      DLLLOCAL inline bool isBaseClass(class QoreClass *qc) const
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
      DLLLOCAL inline class QoreClass *getClass(int cid) const;

      //inline void execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline void execDestructors(class Object *o, class ExceptionSink *xsink);
      DLLLOCAL inline void execSystemDestructors(class Object *o, class ExceptionSink *xsink);
      DLLLOCAL inline void execCopyMethods(class Object *self, class Object *old, class ExceptionSink *xsink);
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
      DLLLOCAL inline ~BCList()
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

      // safe_dslist<class QoreClass *> specialMethodList;

      DLLLOCAL inline BCList(class BCNode *n)
      {
	 head = tail = n;
	 init = false;
      }
      DLLLOCAL inline void add(class BCNode *n);
      DLLLOCAL inline void parseInit(class QoreClass *thisclass, class BCAList *bcal);
      DLLLOCAL inline class Method *resolveSelfMethod(char *name);
      DLLLOCAL inline class Method *findMethod(char *name);
      DLLLOCAL inline class Method *findMethod(char *name, bool *p);
      DLLLOCAL inline bool match(class BCANode *bca);
      DLLLOCAL inline void execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline void execSystemConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline bool isPrivateMember(char *str) const;
      DLLLOCAL inline void ref()
      {
	 ROreference();
      }
      DLLLOCAL inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

class Member {
   public:
      char *name;
      class Member *next;

      DLLLOCAL inline Member(char *n)
      {
	 name = n;
      }

      DLLLOCAL inline ~Member()
      {
	 if (name)
	    free(name);
      }
};

class MemberList {
   public:
      class Member *head;

      DLLLOCAL inline MemberList()
      {
	 head = NULL;
      }
      DLLLOCAL inline MemberList(char *name)
      {
	 head = new Member(name);
	 head->next = NULL;
      }
      DLLLOCAL inline ~MemberList()
      {
	 while (head)
	 {
	    class Member *w = head->next;
	    delete head;
	    head = w;
	 }
      }
      DLLLOCAL class MemberList *copy() const
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
      DLLLOCAL inline int add(char *name);
      DLLLOCAL inline void add(class Member *w)
      {
	 w->next = head;
	 head = w;
      }
      DLLLOCAL inline bool inlist(char *name) const
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
      friend class BCList;

   private:
      char *name;
      hm_method_t hm;
      hm_qn_t pmm, pending_pmm;
      class Method *pending_head, *system_constructor;
      class Method *constructor, *destructor, *copyMethod, *methodGate, *memberGate;
      int classID;
      bool sys, initialized;
      int domain;            // capabilities of builtin class to use in the context of parse restrictions
      class ReferenceObject nref;  // namespace references

      DLLLOCAL inline void init(char *nme, int dom = 0);
      DLLLOCAL QoreClass(char *nme, int id);
      DLLLOCAL inline class Method *parseFindMethod(char *name);
      DLLLOCAL inline void insertMethod(class Method *o);
      // checks for all special methods except constructor & destructor
      DLLLOCAL inline void checkSpecialIntern(class Method *m);
      // checks for all special methods
      DLLLOCAL inline void checkSpecial(class Method *m);
      DLLLOCAL class QoreNode *evalMethodGate(class Object *self, char *nme, class QoreNode *args, class ExceptionSink *xsink);
      DLLLOCAL inline class Method *resolveSelfMethodIntern(char *nme);
      DLLLOCAL inline void delete_pending_methods()
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
      DLLLOCAL ~QoreClass();

   public:
      class BCAList *bcal;         // base class constructor argument list
      class BCList *scl;           // base class list

      DLLEXPORT QoreClass(int dom, char *nme);
      DLLEXPORT QoreClass(char *nme);
      DLLLOCAL QoreClass();

      DLLEXPORT void addMethod(class Method *f);
      DLLEXPORT void addMethod(char *nme, q_method_t m);
      DLLEXPORT void setDestructor(q_destructor_t m);
      DLLEXPORT void setConstructor(q_constructor_t m);
      DLLEXPORT void setCopy(q_copy_t m);

      DLLEXPORT inline void addPrivateMember(char *name);
      DLLEXPORT inline void mergePrivateMembers(class MemberList *n);
      DLLEXPORT bool isPrivateMember(char *str) const;

      DLLEXPORT class QoreNode *evalMethod(class Object *self, char *nme, class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT inline class QoreNode *evalMemberGate(class Object *self, class QoreNode *name, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *execConstructor(class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *execSystemConstructor(class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT inline void execSubclassConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLEXPORT inline void execSubclassSystemConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLEXPORT void execDestructor(class Object *self, class ExceptionSink *xsink);
      DLLEXPORT inline void execSubclassDestructor(class Object *self, class ExceptionSink *xsink);
      DLLEXPORT inline void execSubclassSystemDestructor(class Object *self, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *execCopy(class Object *old, class ExceptionSink *xsink);
      DLLEXPORT inline void execSubclassCopy(class Object *self, class Object *old, class ExceptionSink *xsink);

      DLLEXPORT inline class Method *findMethod(char *nme);
      DLLEXPORT inline class Method *findMethod(char *nme, bool *priv);
      DLLEXPORT inline class Method *findLocalMethod(char *name);
      DLLEXPORT class Method *resolveSelfMethod(char *nme);
      DLLEXPORT class Method *resolveSelfMethod(class NamedScope *nme);
      DLLEXPORT inline void setSystemConstructor(q_constructor_t m);
      DLLEXPORT class List *getMethodList() const;
      DLLEXPORT inline int numMethods() const
      {
	 return hm.size();
      }
      DLLEXPORT inline bool hasCopy() const
      { 
	 return copyMethod ? true : false; 
      }
      DLLEXPORT class QoreClass *copyAndDeref();
      DLLEXPORT inline int getID() const
      { 
	 return classID; 
      }
      DLLEXPORT void parseInit();
      DLLEXPORT void parseCommit();
      DLLEXPORT inline void parseRollback();
      DLLEXPORT inline bool isSystem() const
      { 
	 return sys; 
      }
      DLLEXPORT void addBaseClassesToSubclass(class QoreClass *sc);
      DLLEXPORT class QoreClass *getClass(int cid) const;
      DLLEXPORT inline void deref();
      DLLEXPORT inline bool hasMemberGate() const
      {
	 return memberGate != NULL;
      }
      DLLEXPORT inline void ref()
      {
	 //printd(5, "QoreClass::ref() %08x %s %d -> %d\n", this, name, reference_count(), reference_count() + 1);
	 ROreference();
      }
      DLLEXPORT inline class QoreClass *getReference()
      {
	 //printd(5, "QoreClass::getReference() %08x %s %d -> %d\n", this, name, nref.reference_count(), nref.reference_count() + 1);
	 nref.ROreference();
	 return this;
      }
      DLLEXPORT inline void nderef();
      inline bool is_unique() const
      {
	 return nref.is_unique();
      }
      DLLEXPORT inline void addDomain(int dom)
      {
	 domain |= dom;
      }
      DLLEXPORT inline int getDomain() const
      {
	 return domain;
      }
      DLLEXPORT inline char *getName() const 
      { 
	 return name; 
      }
      DLLEXPORT inline void setName(char *n)
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
#include <qore/qore_thread.h>
#include <qore/Function.h>
#include <qore/List.h>
#include <qore/Statement.h>
#include <qore/Object.h>
#include <qore/QoreType.h>
#include <qore/Variable.h>
#include <qore/Sequence.h>
#include <qore/Namespace.h>
#include <qore/NamedScope.h>

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

inline BCNode::~BCNode()
{
   if (cname)
      delete cname;
   if (cstr)
      free(cstr);
   if (args)
      args->deref(NULL);
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
	 w->sclass = getRootNS()->parseFindScopedClass(w->cname);
	 printd(5, "BCList::parseInit() %s inheriting %s (%08p)\n", cls->getName(), w->cname->ostr, w->sclass);
      }
      else
      {
	 w->sclass = getRootNS()->parseFindClass(w->cstr);
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

inline bool BCList::isPrivateMember(char *str) const
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
   hm_method_t::iterator i = hm.find(nme);
   if (i != hm.end())
      return i->second;

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

inline void QoreClass::setSystemConstructor(q_constructor_t m)
{
   sys = true;
   system_constructor = new Method(new BuiltinMethod(this, m));
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

inline bool Method::isSynchronized() const
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
   hm_qn_t::iterator i;
   if ((i = pmm.find(nme)) == pmm.end())
   {
      if ((i = pending_pmm.find(nme)) == pending_pmm.end())
      {
	 //printd(5, "QoreClass::addPrivateMember() this=%08p %s adding %08p %s\n", this, name, nme, nme);
	 pending_pmm[nme] = NULL;
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
