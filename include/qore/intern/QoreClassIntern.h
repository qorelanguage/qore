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
#include <map>

#define OTF_USER    CT_USER
#define OTF_BUILTIN CT_BUILTIN

struct QoreMemberInfo : public QoreParseTypeInfo {
   AbstractQoreNode *exp;

   DLLLOCAL QoreMemberInfo(qore_type_t t, AbstractQoreNode *e = 0) : QoreParseTypeInfo(t), exp(e) {
   }
   DLLLOCAL QoreMemberInfo(char *n, AbstractQoreNode *e = 0) : QoreParseTypeInfo(n), exp(e) {
   }
   DLLLOCAL QoreMemberInfo(const QoreClass *qc, AbstractQoreNode *e) : QoreParseTypeInfo(qc), exp(e) {
   }
   DLLLOCAL ~QoreMemberInfo() {
      if (exp)
	 exp->deref(0);
   }
   DLLLOCAL QoreMemberInfo *copy() const {
      if (!this)
         return 0;

      assert(has_type);
      if (qc)
         return new QoreMemberInfo(qc, exp ? exp->refSelf() : 0);
      return new QoreMemberInfo(qt, exp ? exp->refSelf() : 0);
   }

   DLLLOCAL void parseInit(const char *name, bool priv) {
      resolve();

      if (exp) {
	 const QoreTypeInfo *argTypeInfo = 0;
	 int lvids = 0;
	 exp = exp->parseInit(0, 0, lvids, argTypeInfo);
	 if (lvids) {
	    parse_error("illegal local variable declaration in member initialization expression");
	    while (lvids)
	       pop_local_var();
	 }
	 // throw a type exception only if parse exceptions are enabled
	 if (!parseEqual(argTypeInfo) && getProgram()->getParseExceptionSink()) {
            QoreStringNode *desc = new QoreStringNode("initialization expression for ");
	    desc->sprintf("%s member '$.%s' returns ", priv ? "private" : "public", name);
            argTypeInfo->getThisType(*desc);
            desc->concat(", but the member was declared as ");
            getThisType(*desc);
            getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
         }
      }
      else if (hasType() && qt == NT_OBJECT) {
	 parseException("PARSE-TYPE-ERROR", "%s member '$.%s' has been defined with a complex type and must be assigned when instantiated", priv ? "private" : "public", name);
      }
   }
};

typedef std::map<char *, QoreMemberInfo *, ltstr> member_map_t;

/*
  BCANode
  base class constructor argument node
*/
class BCANode {
  public:
   QoreClass *sclass;
   NamedScope *ns;
   char *name;
   QoreListNode *argexp;

   DLLLOCAL BCANode(NamedScope *n, QoreListNode *arg) {
      ns = n;
      name = NULL;
      argexp = arg;
   }
   // this method takes ownership of *n
   DLLLOCAL BCANode(char *n, QoreListNode *arg) {
      ns = NULL;
      name = n;
      argexp = arg;
   }
   DLLLOCAL ~BCANode();
   DLLLOCAL void resolve();
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

// BCSMList: Base Class Special Method List
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
      DLLLOCAL bool isPrivate() const { return priv; }
      DLLLOCAL const QoreClass *getClass(qore_classid_t cid, bool &n_priv) const {
	 assert(sclass);
	 const QoreClass *qc = (sclass->getID() == cid) ? sclass : sclass->getClassIntern(cid, n_priv);
	 if (qc && !n_priv && priv)
	    n_priv = true;
	 return qc;
      }
};

typedef safe_dslist<BCNode *> bclist_t;

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
      BCSMList sml;

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
      DLLLOCAL const QoreClass *parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&typeInfo, bool &priv) const;
      DLLLOCAL bool parseHasPublicMembersInHierarchy() const;
      DLLLOCAL bool isPublicOrPrivateMember(const char *mem, bool &priv) const;
      DLLLOCAL void ref() const;
      DLLLOCAL void deref();
      DLLLOCAL const QoreClass *getClass(qore_classid_t cid, bool &priv) const {
	 for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
	    const QoreClass *qc = (*i)->getClass(cid, priv);
	    if (qc)
	       return qc;
	 }
	 
	 return 0;
      }
      DLLLOCAL int initMembers(QoreObject *o, ExceptionSink *xsink) const {
	 for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
	    if ((*i)->sclass->initMembers(o, xsink))
	       return -1;
	 }
	 
	 return 0;
      }
};

// BCEANode
// base constructor evaluated argument node; created locally at run time
class BCEANode {
public:
   QoreListNode *args;
   bool execed;
      
   DLLLOCAL BCEANode(QoreListNode *arg) : args(arg), execed(false) {}
   DLLLOCAL BCEANode() : args(0), execed(true) {}
};

struct ltqc {
   bool operator()(const class QoreClass *qc1, const class QoreClass *qc2) const {
      return qc1 < qc2;
   }
};

typedef std::map<const QoreClass *, class BCEANode *, ltqc> bceamap_t;

/*
  BCEAList
  base constructor evaluated argument list
*/
class BCEAList : public bceamap_t {
protected:
   DLLLOCAL ~BCEAList() { }
   
public:
   DLLLOCAL void deref(ExceptionSink *xsink);
   // evaluates arguments, returns -1 if an exception was thrown
   DLLLOCAL int add(const QoreClass *qc, QoreListNode *arg, ExceptionSink *xsink);
   DLLLOCAL QoreListNode *findArgs(const QoreClass *qc, bool *aexeced);
};

#endif
