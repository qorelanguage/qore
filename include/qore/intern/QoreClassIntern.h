/*
  QoreClassIntern.h

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

#ifndef _QORE_QORECLASSINTERN_H

#define _QORE_QORECLASSINTERN_H

#include <qore/intern/BuiltinMethod.h>
#include <qore/safe_dslist>

#include <list>

#define OTF_USER    CT_USER
#define OTF_BUILTIN CT_BUILTIN

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

typedef std::pair<class QoreClass *, bool> class_virt_pair_t;
typedef std::list<class_virt_pair_t> class_list_t;

// BCSMList: Base Class Special QoreMethod List
// unique list of base classes for a class hierarchy to ensure that "special" methods, constructor(), destructor(), copy() - are executed only once
// this class also tracks virtual classes to ensure that they are not inserted into the list in a complex tree and executed here
class BCSMList : public class_list_t
{
   public:
      DLLLOCAL inline void add(class QoreClass *thisclass, class QoreClass *qc, bool is_virtual);
      DLLLOCAL inline void addBaseClassesToSubclass(class QoreClass *thisclass, class QoreClass *sc, bool is_virtual);
      DLLLOCAL inline bool isBaseClass(class QoreClass *qc) const;
      DLLLOCAL inline class QoreClass *getClass(int cid) const;
      //DLLLOCAL inline void execConstructors(class QoreObject *o, class BCEAList *bceal, class ExceptionSink *xsink) const;
      DLLLOCAL inline void execDestructors(class QoreObject *o, class ExceptionSink *xsink) const;
      DLLLOCAL inline void execSystemDestructors(class QoreObject *o, class ExceptionSink *xsink) const;
      DLLLOCAL inline void execCopyMethods(class QoreObject *self, class QoreObject *old, class ExceptionSink *xsink) const;
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
      bool is_virtual;
      
      DLLLOCAL inline BCNode(class NamedScope *c, bool p) : cname(c), cstr(0), sclass(0), args(0), hasargs(false), priv(p), is_virtual(false)
      {
      }
      // this method takes ownership of *str
      DLLLOCAL inline BCNode(char *str, bool p) : cname(0), cstr(str), sclass(0), args(0), hasargs(false), priv(p), is_virtual(false)
      {
      }
      // for builtin base classes
      DLLLOCAL inline BCNode(class QoreClass *qc, class QoreNode *xargs = 0, bool n_virtual  = false) 
	 : cname(0), cstr(0), sclass(qc), args(xargs), hasargs(xargs ? true : false), priv(false), is_virtual(n_virtual)
      {
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
      DLLLOCAL inline const QoreMethod *resolveSelfMethod(const char *name);
      DLLLOCAL inline const QoreMethod *findParseMethod(const char *name);
      DLLLOCAL inline const QoreMethod *findMethod(const char *name) const;
      DLLLOCAL inline const QoreMethod *findMethod(const char *name, bool *p) const;
      DLLLOCAL inline bool match(class BCANode *bca);
      DLLLOCAL inline void execConstructors(class QoreObject *o, class BCEAList *bceal, class ExceptionSink *xsink) const;
      DLLLOCAL void execConstructorsWithArgs(class QoreObject *o, class BCEAList *bceal, class ExceptionSink *xsink) const;
      DLLLOCAL inline void execSystemConstructors(class QoreObject *o, class BCEAList *bceal, class ExceptionSink *xsink) const;
      DLLLOCAL inline bool isPrivateMember(const char *str) const;
      DLLLOCAL inline void ref() const;
      DLLLOCAL void deref();
};

#endif
