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

DLLLOCAL class QoreNode *internalObjectVarRef(class QoreNode *n, class ExceptionSink *xsink);
DLLLOCAL void initObjects();
DLLLOCAL void deleteObjects();

class Method {
   private:
      int type;
      union {
	    class UserFunction *userFunc;
	    class BuiltinMethod *builtin;
      } func;
      bool priv;

      DLLLOCAL inline Method();
      DLLLOCAL inline void userInit(UserFunction *u, int p);

   protected:

   public:
      char *name;
      class Method *next;
      class BCAList *bcal; // for subclass constructors only

      DLLLOCAL Method(class UserFunction *u, int p, class BCAList *b);
      DLLLOCAL inline Method(class BuiltinMethod *b);
      DLLLOCAL ~Method();
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
      DLLLOCAL inline int getType() const;
      DLLLOCAL inline bool isPrivate() const;
      DLLLOCAL inline char *getName() const;
      // only called when method is user
      DLLLOCAL inline bool isSynchronized() const;
};

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
      DLLLOCAL inline BCEAList();
      DLLLOCAL inline void deref(class ExceptionSink *xsink);
      DLLLOCAL inline void add(class QoreClass *qc, class QoreNode *arg, class ExceptionSink *xsink);
      DLLLOCAL inline class QoreNode *findArgs(class QoreClass *qc, bool *aexeced);
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
      DLLLOCAL ~BCANode();
      DLLLOCAL inline void resolve();
};

typedef safe_dslist<class BCANode *> bcalist_t;

/*
  BCAList
  base class constructor argument list
  this data structure will not be modified even if the class is copied
  to a subprogram object
*/
class BCAList : public ReferenceObject, public bcalist_t
{
   protected:
      DLLLOCAL inline ~BCAList();

   public:
      DLLLOCAL BCAList(class BCANode *n);
      DLLLOCAL inline void ref();
      DLLLOCAL void deref();
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

// BCNode 
// base class pointer
class BCNode
{
   public:
      class NamedScope *cname;
      char *cstr;
      class QoreClass *sclass;
      class QoreNode *args;
      bool hasargs;
      bool priv;
      
      DLLLOCAL inline BCNode(class NamedScope *c, bool p)
      {
	 cname = c;
	 cstr = NULL;
	 sclass = NULL;
	 args = NULL;
	 hasargs = false;
	 priv = p;
      }
      DLLLOCAL inline BCNode(char *str, bool p)
      {
	 cname = NULL;
	 cstr = str;
	 sclass = NULL;
	 args = NULL;
	 hasargs = false;
	 priv = p;
      }
      DLLLOCAL ~BCNode();
};

typedef safe_dslist<class BCNode *> bclist_t;

//  BCList
//  linked list of base classes, constructors called head->tail, 
//  destructors called in reverse order (tail->head)
//  note that this data structure cannot be modified even if the class is
//  copied to a subprogram object and extended
class BCList : public ReferenceObject, public bclist_t
{
   private:
      bool init;

   protected:
      DLLLOCAL inline ~BCList();

   public:
      // special method (constructor, destructor, copy) list for superclasses 
      class BCSMList sml;

      DLLLOCAL BCList(class BCNode *n);
      DLLLOCAL inline void parseInit(class QoreClass *thisclass, class BCAList *bcal);
      DLLLOCAL inline class Method *resolveSelfMethod(char *name);
      DLLLOCAL inline class Method *findMethod(char *name);
      DLLLOCAL inline class Method *findMethod(char *name, bool *p);
      DLLLOCAL inline bool match(class BCANode *bca);
      DLLLOCAL inline void execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline void execConstructorsWithArgs(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline void execSystemConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline bool isPrivateMember(char *str) const;
      DLLLOCAL inline void ref();
      DLLLOCAL void deref();
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
      DLLLOCAL class MemberList *copy() const;
      DLLLOCAL int add(char *name);
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
      DLLLOCAL inline void delete_pending_methods();

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
      DLLEXPORT void parseMergePrivateMembers(class MemberList *n);
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

DLLLOCAL void deleteStackObjectKey(char *name, ExceptionSink *xsink);
DLLLOCAL class QoreNode **getExistingStackObjectValuePtr(char *name, class VLock *vl, ExceptionSink *xsink);
DLLLOCAL class QoreNode **getStackObjectValuePtr(char *name, class VLock *vl, ExceptionSink *xsink);
DLLLOCAL class QoreNode *getStackObjectValue(char *name, class VLock *vl, ExceptionSink *xsink);

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

#endif // _QORE_QORECLASS_H
