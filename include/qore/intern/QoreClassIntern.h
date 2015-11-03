/*
  QoreClassIntern.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#include <qore/safe_dslist>

#include <list>

#define OTF_USER    CT_USER
#define OTF_BUILTIN CT_BUILTIN

/*
  BCANode
  base class constructor argument node
*/
class BCANode {
   public:
      class QoreClass *sclass;
      class NamedScope *ns;
      char *name;
      QoreListNode *argexp;

      DLLLOCAL inline BCANode(class NamedScope *n, QoreListNode *arg) {
	 ns = n;
	 name = NULL;
	 argexp = arg;
      }
      // this method takes ownership of *n
      DLLLOCAL inline BCANode(char *n, QoreListNode *arg) {
	 ns = NULL;
	 name = n;
	 argexp = arg;
      }
      DLLLOCAL ~BCANode();
      DLLLOCAL inline void resolve();
};

typedef safe_dslist<BCANode *> bcalist_t;

//  BCAList
//  base class constructor argument list
//  this data structure will not be modified even if the class is copied
//  to a subprogram object
class BCAList : public bcalist_t {
   public:
      DLLLOCAL BCAList(BCANode *n);
      DLLLOCAL ~BCAList();
};

typedef std::pair<QoreClass *, bool> class_virt_pair_t;
typedef std::list<class_virt_pair_t> class_list_t;

class BCEAList;

// BCSMList: Base Class Special QoreMethod List
// unique list of base classes for a class hierarchy to ensure that "special" methods, constructor(), destructor(), copy() - are executed only once
// this class also tracks virtual classes to ensure that they are not inserted into the list in a complex tree and executed here
class BCSMList : public class_list_t {
   public:
      DLLLOCAL void add(QoreClass *thisclass, QoreClass *qc, bool is_virtual);
      DLLLOCAL void addBaseClassesToSubclass(QoreClass *thisclass, QoreClass *sc, bool is_virtual);
      DLLLOCAL bool isBaseClass(QoreClass *qc) const;
      DLLLOCAL QoreClass *getClass(qore_classid_t cid) const;
      //DLLLOCAL void execConstructors(QoreObject *o, BCEAList *bceal, ExceptionSink *xsink) const;
      DLLLOCAL void execDestructors(QoreObject *o, ExceptionSink *xsink) const;
      DLLLOCAL void execSystemDestructors(QoreObject *o, ExceptionSink *xsink) const;
      DLLLOCAL void execCopyMethods(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const;
};

// BCNode 
// base class pointer, also stores arguments for base class constructors
class BCNode {
   public:
      NamedScope *cname;
      char *cstr;
      QoreClass *sclass;
      QoreListNode *args;
      bool hasargs;
      bool priv;
      bool is_virtual;
      
      DLLLOCAL BCNode(NamedScope *c, bool p) : cname(c), cstr(0), sclass(0), args(0), hasargs(false), priv(p), is_virtual(false) {
      }
      // this method takes ownership of *str
      DLLLOCAL BCNode(char *str, bool p) : cname(0), cstr(str), sclass(0), args(0), hasargs(false), priv(p), is_virtual(false) {
      }
      // for builtin base classes
      DLLLOCAL BCNode(QoreClass *qc, QoreListNode *xargs = 0, bool n_virtual  = false) 
	 : cname(0), cstr(0), sclass(qc), args(xargs), hasargs(xargs ? true : false), priv(false), is_virtual(n_virtual) {
      }
      DLLLOCAL ~BCNode();
};

typedef safe_dslist<class BCNode *> bclist_t;

//  BCList
//  linked list of base classes, constructors called head->tail, 
//  destructors called in reverse order (tail->head) (stored in BCSMList)
//  note that this data structure cannot be modified even if the class is
//  copied to a subprogram object and extended
//  this class is a QoreReferenceCounter so it won't be copied when the class is copied
class BCList : public QoreReferenceCounter, public bclist_t {
   protected:
      DLLLOCAL inline ~BCList();

   public:
      // special method (constructor, destructor, copy) list for superclasses 
      class BCSMList sml;

      DLLLOCAL BCList(BCNode *n);
      DLLLOCAL BCList();
      DLLLOCAL void parseInit(QoreClass *thisclass, BCAList *bcal, bool &has_delete_blocker
#ifdef QORE_CLASS_SYNCHRONOUS
			      , bool &synchronous_in_hierarchy
#endif
	 );
      DLLLOCAL const QoreMethod *resolveSelfMethod(const char *name);

      // only looks in committed method lists
      DLLLOCAL const QoreMethod *findParseMethod(const char *name);
      DLLLOCAL const QoreMethod *findParseStaticMethod(const char *name);

      // looks in committed and pending method lists
      DLLLOCAL const QoreMethod *parseFindMethodTree(const char *name);
      DLLLOCAL const QoreMethod *findMethod(const char *name) const;
      DLLLOCAL const QoreMethod *findMethod(const char *name, bool &p) const;
      DLLLOCAL const QoreMethod *parseFindStaticMethodTree(const char *name);
      DLLLOCAL const QoreMethod *findStaticMethod(const char *name) const;
      DLLLOCAL const QoreMethod *findStaticMethod(const char *name, bool &p) const;
      DLLLOCAL bool match(BCANode *bca);
      DLLLOCAL void execConstructors(QoreObject *o, BCEAList *bceal, ExceptionSink *xsink) const;
      DLLLOCAL void execConstructorsWithArgs(QoreObject *o, BCEAList *bceal, ExceptionSink *xsink) const;
      DLLLOCAL bool execDeleteBlockers(QoreObject *o, ExceptionSink *xsink) const;
      DLLLOCAL bool parseCheckHierarchy(const QoreClass *cls) const;
      DLLLOCAL bool isPrivateMember(const char *str) const;
      DLLLOCAL void ref() const;
      DLLLOCAL void deref();
};

#endif
