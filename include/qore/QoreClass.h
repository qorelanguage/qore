/*
  QoreClass.h

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

#ifndef _QORE_QORECLASS_H

#define _QORE_QORECLASS_H

#include <qore/ReferenceObject.h>
#include <qore/hash_map.h>
#include <qore/common.h>
#include <qore/BuiltinMethod.h>
#include <qore/qore_thread.h>

#include <qore/safe_dslist>
#include <list>

#define OTF_USER    CT_USER
#define OTF_BUILTIN CT_BUILTIN

class Method {
   private:
      int type;
      union {
	    class UserFunction *userFunc;
	    class BuiltinMethod *builtin;
      } func;
      bool priv;
      char *name;
      class QoreClass *parent_class;

      DLLLOCAL Method(class QoreClass *p_class);
      DLLLOCAL void userInit(UserFunction *u, int p);

   public:
      DLLLOCAL Method(class UserFunction *u, int p);
      DLLLOCAL Method(class QoreClass *p_class, class BuiltinMethod *b);
      DLLLOCAL ~Method();
      DLLLOCAL bool inMethod(class Object *self) const;
      DLLLOCAL class QoreNode *eval(class Object *self, class QoreNode *args, class ExceptionSink *xsink);
      DLLLOCAL void evalConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL void evalDestructor(class Object *self, class ExceptionSink *xsink);
      DLLLOCAL void evalSystemConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL void evalSystemDestructor(class Object *self, class ExceptionSink *xsink);
      DLLLOCAL void evalCopy(class Object *self, class Object *old, class ExceptionSink *xsink);
      DLLLOCAL class Method *copy(class QoreClass *p_class) const;
      DLLLOCAL void parseInit();
      DLLLOCAL void parseInitConstructor(class BCList *bcl);
      DLLLOCAL int getType() const
      {
	 return type;
      }
      DLLLOCAL bool isPrivate() const
      { 
	 return priv; 
      }
      DLLLOCAL const char *getName() const
      {
	 return name;
      }
      // only called when method is user
      DLLLOCAL bool isSynchronized() const;
      DLLLOCAL class QoreClass *get_class()
      {
	 return parent_class;
      }
      DLLLOCAL void assign_class(class QoreClass *p_class);
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
      // this method takes ownership of *n
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

//  BCAList
//  base class constructor argument list
//  this data structure will not be modified even if the class is copied
//  to a subprogram object
class BCAList : public bcalist_t
{
   public:
      DLLLOCAL BCAList(class BCANode *n);
      DLLLOCAL ~BCAList();
};

typedef std::list<class QoreClass *> class_list_t;

// BCSMList: Base Class Special Method List
// unique list of base classes for a class hierarchy to ensure that "special" methods, constructor(), destructor(), copy() - are executed only once
class BCSMList : public class_list_t
{
   public:
      DLLLOCAL inline void add(class QoreClass *thisclass, class QoreClass *qc);
      DLLLOCAL inline void addBaseClassesToSubclass(class QoreClass *thisclass, class QoreClass *sc);
      DLLLOCAL inline bool isBaseClass(class QoreClass *qc) const;
      DLLLOCAL inline class QoreClass *getClass(int cid) const;
      //inline void execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline void execDestructors(class Object *o, class ExceptionSink *xsink);
      DLLLOCAL inline void execSystemDestructors(class Object *o, class ExceptionSink *xsink);
      DLLLOCAL inline void execCopyMethods(class Object *self, class Object *old, class ExceptionSink *xsink);
};

// BCNode 
// base class pointer, also stores arguments for base class constructors
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
      // this method takes ownership of *str
      DLLLOCAL inline BCNode(char *str, bool p)
      {
	 cname = NULL;
	 cstr = str;
	 sclass = NULL;
	 args = NULL;
	 hasargs = false;
	 priv = p;
      }
      DLLLOCAL inline BCNode(class QoreClass *qc, class QoreNode *xargs = NULL)
      {
	 cname = NULL;
	 cstr = NULL;
	 sclass = qc;
	 args = xargs;
	 hasargs = xargs ? true : false;
	 priv = false;
      }
      DLLLOCAL ~BCNode();
};

typedef safe_dslist<class BCNode *> bclist_t;

//  BCList
//  linked list of base classes, constructors called head->tail, 
//  destructors called in reverse order (tail->head) (stored in BCSMList)
//  note that this data structure cannot be modified even if the class is
//  copied to a subprogram object and extended
//  this class is a ReferenceObject so it won't be copied when the class is copied
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
      DLLLOCAL inline BCList();
      DLLLOCAL inline void parseInit(class QoreClass *thisclass, class BCAList *bcal);
      DLLLOCAL inline class Method *resolveSelfMethod(const char *name);
      DLLLOCAL inline class Method *findMethod(const char *name);
      DLLLOCAL inline class Method *findMethod(const char *name, bool *p);
      DLLLOCAL inline bool match(class BCANode *bca);
      DLLLOCAL inline void execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL void execConstructorsWithArgs(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline void execSystemConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline bool isPrivateMember(const char *str) const;
      DLLLOCAL inline void ref();
      DLLLOCAL void deref();
};

/*
  QoreClass
  defines a qore-language class
*/
class QoreClass{
      friend class BCList;
      friend class BCSMList;

   private:
      char *name;                  // the name of the class
      class BCAList *bcal;         // base class constructor argument list
      class BCList *scl;           // base class list
      hm_method_t hm, hm_pending;  // method maps
      strset_t pmm, pending_pmm;   // private member lists (sets)
      class Method *system_constructor, *constructor, *destructor, *copyMethod, *methodGate, *memberGate;
      int classID,                 // class ID
	 methodID;                 // for subclasses of builtin classes that will not have their own private data, 
                                   //   instead they will get the private data from this class
      bool sys, initialized;       // system class?, is initialized?
      int domain;                  // capabilities of builtin class to use in the context of parse restrictions
      class ReferenceObject nref;  // namespace references

      // called by each constructor
      DLLLOCAL inline void init(const char *nme, int dom = 0);
      // private constructor only called when the class is copied
      DLLLOCAL QoreClass(int id, const char *nme);
      DLLLOCAL inline class Method *parseFindMethod(const char *name);
      DLLLOCAL inline void insertMethod(class Method *o);
      // checks for all special methods except constructor & destructor
      DLLLOCAL inline void checkSpecialIntern(class Method *m);
      // checks for all special methods
      DLLLOCAL inline void checkSpecial(class Method *m);
      DLLLOCAL class QoreNode *evalMethodGate(class Object *self, const char *nme, class QoreNode *args, class ExceptionSink *xsink);
      DLLLOCAL inline class Method *resolveSelfMethodIntern(const char *nme);
      DLLLOCAL inline void delete_pending_methods();

   public:
      DLLEXPORT QoreClass(const char *nme, int dom = 0);
      DLLEXPORT ~QoreClass();
      
      DLLEXPORT void addMethod(const char *nme, q_method_t m);
      DLLEXPORT void setDestructor(q_destructor_t m);
      DLLEXPORT void setConstructor(q_constructor_t m);
      DLLEXPORT void setSystemConstructor(q_constructor_t m);
      DLLEXPORT void setCopy(q_copy_t m);
      // this method takes ownership of *name
      DLLEXPORT void addPrivateMember(char *name);
      DLLEXPORT bool isPrivateMember(const char *str) const;
      DLLEXPORT class QoreNode *evalMethod(class Object *self, const char *nme, class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *execConstructor(class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *execSystemConstructor(class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *execCopy(class Object *old, class ExceptionSink *xsink);
      DLLEXPORT class Method *findMethod(const char *nme);
      DLLEXPORT class Method *findMethod(const char *nme, bool *priv);
      DLLEXPORT class Method *findLocalMethod(const char *name);
      DLLEXPORT class List *getMethodList() const;
      DLLEXPORT class QoreClass *getClass(int cid) const;
      DLLEXPORT int numMethods() const;
      DLLEXPORT bool hasCopy() const;
      DLLEXPORT int getID() const;
      DLLEXPORT bool isSystem() const;
      DLLEXPORT bool hasMemberGate() const;
      DLLEXPORT int getDomain() const;
      DLLEXPORT const char *getName() const;
      // make a builtin class a child of a another builtin class, private inheritance makes no sense
      // (there would be too much overhead to use user-level qore interfaces to call private methods)
      // but base class constructor arguments can be given
      DLLEXPORT void addBuiltinBaseClass(class QoreClass *qc, class QoreNode *xargs = NULL);
      // this method will do the same as above but will also ensure that the given class' private data
      // will be used in all object methods - in this case the class cannot have any private data
      DLLEXPORT void addDefaultBuiltinBaseClass(class QoreClass *qc, class QoreNode *xargs = NULL);

      DLLLOCAL QoreClass();
      DLLLOCAL void addMethod(class Method *f);
      DLLLOCAL class QoreNode *evalMemberGate(class Object *self, class QoreNode *name, class ExceptionSink *xsink);
      DLLLOCAL inline void execSubclassConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL inline void execSubclassSystemConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink);      
      DLLLOCAL void execDestructor(class Object *self, class ExceptionSink *xsink);
      DLLLOCAL inline void execSubclassDestructor(class Object *self, class ExceptionSink *xsink);
      DLLLOCAL inline void execSubclassSystemDestructor(class Object *self, class ExceptionSink *xsink);
      DLLLOCAL inline void execSubclassCopy(class Object *self, class Object *old, class ExceptionSink *xsink);
      DLLLOCAL class Method *resolveSelfMethod(const char *nme);
      DLLLOCAL class Method *resolveSelfMethod(class NamedScope *nme);
      DLLLOCAL inline void addDomain(int dom);
      DLLLOCAL class QoreClass *copyAndDeref();
      DLLLOCAL void addBaseClassesToSubclass(class QoreClass *sc);
      // returns 0 for success, -1 for error
      DLLLOCAL int parseAddBaseClassArgumentList(class BCAList *bcal);
      // only called when parsing, sets the name of the class
      DLLLOCAL void setName(const char *n);
      // returns true if reference count is 1
      DLLLOCAL bool is_unique() const;
      // references and returns itself
      DLLLOCAL class QoreClass *getReference();
      // dereferences the class, deletes if reference count is 0
      DLLLOCAL void nderef();
      DLLLOCAL void parseInit();
      DLLLOCAL void parseCommit();
      DLLLOCAL void parseRollback();
      DLLLOCAL int getIDForMethod() const;
      DLLLOCAL void parseSetBaseClassList(class BCList *bcl);
};

#endif // _QORE_QORECLASS_H
