/*
  QoreClass.cc

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/Sequence.h>
#include <qore/intern/QoreClassIntern.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

// global class ID sequence
DLLLOCAL Sequence classIDSeq;

static inline const char *pubpriv(bool priv) { return priv ? "private" : "public"; }

// private QoreClass implementation
struct qore_qc_private {
   char *name;                  // the name of the class
   BCAList *bcal;               // base constructor argument list
   BCList *scl;                 // base class list
   hm_method_t hm, hm_pending,     // regular method maps
      shm, shm_pending;            // static method maps

   member_map_t private_members, pending_private_members;   // private member lists (maps)
   member_map_t public_members, pending_public_members; // public member lists (maps)

   const QoreMethod *system_constructor, *constructor, *destructor,
      *copyMethod, *methodGate, *memberGate, *deleteBlocker,
      *memberNotification;

   qore_classid_t classID,          // class ID
      methodID;                     // for subclasses of builtin classes that will not have their own private data,
                                    // instead they will get the private data from this class
   bool sys,                        // system class?
      initialized,                  // is initialized?
      has_delete_blocker,           // has a delete_blocker function somewhere in the hierarchy?
      has_public_members            // has a public member list somewhere in the hierarchy?
#ifdef QORE_CLASS_SYNCHRONOUS
      ,
      synchronous_class,            // should all class methods be wrapped in a recursive thread lock?
      has_synchronous_in_hierarchy  // is there at least one class somewhere in the hierarchy requiring the recursive lock?
#endif
      ;
   int domain;                      // capabilities of builtin class to use in the context of parse restrictions
   QoreReferenceCounter nref;       // namespace references
   int num_methods, num_user_methods, num_static_methods, num_static_user_methods;
   // to be used in parsing
   QoreTypeInfo typeInfo;

   DLLLOCAL qore_qc_private(const QoreClass *cls, const char *nme, int dom = QDOM_DEFAULT) 
      : bcal(0), scl(0), 
	sys(false), initialized(false), has_delete_blocker(false), 
	has_public_members(false),
#ifdef QORE_CLASS_SYNCHRONOUS
	synchronous_class(false), has_synchronous_in_hierarchy(false), 
#endif
	domain(dom), 
	num_methods(0), num_user_methods(0),
	num_static_methods(0), num_static_user_methods(0),
	typeInfo(cls) {
      name = nme ? strdup(nme) : 0;

      // quick pointers
      system_constructor = constructor = destructor = copyMethod = 
	 methodGate = memberGate = deleteBlocker = memberNotification = 0;
   }

   DLLLOCAL ~qore_qc_private() {
      //printd(5, "QoreClass::~QoreClass() deleting %p %s\n", this, name);

      hm_method_t::iterator i = hm.begin();
      while (i != hm.end()) {
	 const QoreMethod *m = i->second;
	 //printd(5, "QoreClass::~QoreClass() deleting method %p %s::%s()\n", m, name, m->getName());
	 hm.erase(i);
	 i = hm.begin();
	 delete m;
      }      

      i = shm.begin();
      while (i != shm.end()) {
	 const QoreMethod *m = i->second;
	 //printd(5, "QoreClass::~QoreClass() deleting static method %p %s::%s()\n", m, name, m->getName());
	 shm.erase(i);
	 i = shm.begin();
	 delete m;
      }

      // delete private member list
      member_map_t::iterator j = private_members.begin();
      while (j != private_members.end()) {
	 char *n = j->first;
	 delete j->second;
	 private_members.erase(j);
	 j = private_members.begin();
	 //printd(5, "QoreClass::~QoreClass() freeing private member %p '%s'\n", n, n);
	 free(n);
      }

      while ((j = pending_private_members.begin()) != pending_private_members.end()) {
	 char *n = j->first;
	 delete j->second;
	 pending_private_members.erase(j);
	 //printd(5, "QoreClass::~QoreClass() freeing pending private member %p '%s'\n", n, n);
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

   DLLLOCAL void delete_pending_methods() {
      hm_method_t::iterator i = hm_pending.begin();
      while (i != hm_pending.end()) {
	 const QoreMethod *m = i->second;
	 //printd(5, "QoreClass::~QoreClass() deleting pending method %p %s::%s()\n", m, name, m->getName());
	 hm_pending.erase(i);
	 i = hm_pending.begin();
	 delete m;
      }

      i = shm_pending.begin();
      while (i != shm_pending.end()) {
	 const QoreMethod *m = i->second;
	 //printd(5, "QoreClass::~QoreClass() deleting static pending method %p %s::%s()\n", m, name, m->getName());
	 shm_pending.erase(i);
	 i = shm_pending.begin();
	 delete m;
      }
   }

   // checks for all special methods except constructor & destructor
   DLLLOCAL void checkSpecialIntern(const QoreMethod *m) {
      // set quick pointers
      if (!methodGate && !strcmp(m->getName(), "methodGate"))
	 methodGate = m;
      else if (!memberGate && !strcmp(m->getName(), "memberGate"))
	 memberGate = m;
      else if (!memberNotification && !strcmp(m->getName(), "memberNotification"))
	 memberNotification = m;
   }

   // checks for all special methods except constructor & destructor
   DLLLOCAL bool checkSpecialStaticIntern(const QoreMethod *m) {
      // set quick pointers
      if ((!methodGate && !strcmp(m->getName(), "methodGate"))
	  || (!memberGate && !strcmp(m->getName(), "memberGate"))
	  || (!memberNotification && !strcmp(m->getName(), "memberNotification")))
	 return true;
      return false;
   }

   // checks for all special methods
   DLLLOCAL void checkSpecial(const QoreMethod *m) {
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

   DLLLOCAL void initialize() {
      if (!initialized) {
	 initialized = true;
	 printd(5, "QoreClass::initialize() %s class=%p pending size=%d, scl=%p, bcal=%p\n", name, typeInfo.qc, hm_pending.size(), scl, bcal);

	 // initialize new private members
	 for (member_map_t::iterator i = pending_private_members.begin(), e = pending_private_members.end(); i != e; ++i) {
	    if (i->second)
	       i->second->parseInit(i->first, true);
	 }
   
	 // initialize new public members
	 for (member_map_t::iterator i = pending_public_members.begin(), e = pending_public_members.end(); i != e; ++i) {
	    if (i->second)
	       i->second->parseInit(i->first, false);
	 }
   
	 if (scl) {
	    QoreClass *qc = const_cast<QoreClass *>(typeInfo.qc);
#ifdef QORE_CLASS_SYNCHRONOUS
	    scl->parseInit(qc, bcal, has_delete_blocker, has_synchronous_in_hierarchy);
#else
	    scl->parseInit(qc, bcal, has_delete_blocker);
#endif
	 }

	 if (!sys && domain & getProgram()->getParseOptions())
	    parseException("ILLEGAL-CLASS-DEFINITION", "class '%s' inherits functionality from base classes that is restricted by current parse options", name);

	 // check new members for conflicts in base classes
	 for (member_map_t::iterator i = pending_private_members.begin(), e = pending_private_members.end(); i != e; ++i) {
	    parseCheckMemberInBaseClasses(i->first, i->second, true);
	 }

	 for (member_map_t::iterator i = pending_public_members.begin(), e = pending_public_members.end(); i != e; ++i) {
	    parseCheckMemberInBaseClasses(i->first, i->second, false);
	 }
      }
   }

   DLLLOCAL const int parseCheckMemberAccess(const char *mem, const QoreTypeInfo *&memberTypeInfo) const {
      const_cast<qore_qc_private *>(this)->initialize();

      bool priv;
      const QoreClass *sclass = parseFindPublicPrivateMember(mem, memberTypeInfo, priv);
      
      if (!sclass) {
	 if (parseHasPublicMembersInHierarchy()) {
	    parse_error("illegal access to unknown member '%s' in a class with a public member list (or inherited public member list)", mem);
	    return -1;
	 }
	 return 0;
      }

      if (priv) {
	 if (!parseCheckPrivateClassAccess(typeInfo.qc)) {
	    memberTypeInfo = 0;
	    if (name)
	       parse_error("illegal access to private member '%s' of class '%s'", mem, name);
	    else
	       parse_error("illegal access to private member '%s'", mem);
	    return -1;
	 }
      }
      return 0;
   }

   DLLLOCAL const int parseCheckInternalMemberAccess(const char *mem) const {
      const_cast<qore_qc_private *>(this)->initialize();

      // throws a parse exception if there are public members and the name is not valid
      bool priv;
      const QoreTypeInfo *memberTypeInfo;
      const QoreClass *sclass = parseFindPublicPrivateMember(mem, memberTypeInfo, priv);
      if (!sclass && parseHasPublicMembersInHierarchy()) {
	 parse_error("illegal access to unknown member '%s' (class has a public member list or inherited public member list)", mem);
	 return -1;
      }
      return 0;
   }

   DLLLOCAL bool parseHasPublicMembersInHierarchy() const {
      if (has_public_members || !pending_public_members.empty())
	 return true;
      
      return scl ? scl->parseHasPublicMembersInHierarchy() : false;
   }
   
   DLLLOCAL const QoreClass *parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &priv) const {
      bool found = false;
      member_map_t::const_iterator i = private_members.find(const_cast<char *>(mem));
      if (i != private_members.end())
	 found = true;
      else {
	 i = pending_private_members.find(const_cast<char *>(mem));
	 if (i != pending_private_members.end())
	    found = true;
      }
      if (found) {
	 priv = true;
	 memberTypeInfo = i->second;
	 return typeInfo.qc;
      }

      i = public_members.find(const_cast<char *>(mem));
      if (i != public_members.end())
	 found = true;
      else {
	 i = pending_public_members.find(const_cast<char *>(mem));
	 if (i != pending_public_members.end())
	    found = true;
      }

      if (found) {
	 priv = false;
	 memberTypeInfo = i->second;
	 return typeInfo.qc;
      }

      return scl ? scl->parseFindPublicPrivateMember(mem, memberTypeInfo, priv) : 0;
   }

   DLLLOCAL int checkExistingMember(char *mem, const QoreParseTypeInfo *memberTypeInfo, bool priv, const QoreClass *sclass, const QoreTypeInfo *existingMemberTypeInfo, bool is_priv) const {
      //printd(5, "checkExistingMember() mem=%s priv=%d is_priv=%d sclass=%s\n", mem, priv, is_priv, sclass->getName());

      // here we know that the member already exists, so either it will be a
      // duplicate declaration, in which case it is ignored, or it is a
      // contradictory declaration, in which case a parse exception is raised

      // if the member was previously declared public
      if (priv != is_priv) {
	 // raise an exception only if parse exceptions are enabled
	 if (getProgram()->getParseExceptionSink()) {
	    QoreStringNode *desc = new QoreStringNode;
	    if (name)
	       desc->sprintf("class '%s' ", name);
	    desc->concat("cannot declare ");
	    desc->sprintf("%s member ", pubpriv(priv));
	    desc->sprintf("'%s' when ", mem);
	    if (sclass == typeInfo.qc)
	       desc->concat("this class");
	    else
	       desc->sprintf("base class '%s'", sclass->getName());
	    desc->sprintf(" already declared this member as %s", pubpriv(is_priv));
	    getProgram()->makeParseException("PARSE-ERROR", desc);
	 }
	 return -1;
      }
      else if (memberTypeInfo || existingMemberTypeInfo) {
	 if (getProgram()->getParseExceptionSink()) {
	    QoreStringNode *desc = new QoreStringNode;
	    desc->sprintf("%s member ", pubpriv(priv));
	    desc->sprintf("'%s' was already declared in ", mem);
	    if (sclass == typeInfo.qc)
	       desc->concat("this class");
	    else
	       desc->sprintf("base class '%s'", sclass->getName());
	    if (existingMemberTypeInfo)
	       desc->sprintf(" with a type definition");
	    desc->concat(" and cannot be declared again");
	    if (name)
	       desc->sprintf(" in class '%s'", name);
	    desc->concat(" if the member has a type definition");
	    
	    getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	 }
	 return -1;
      }
      
      return 0;
   }

   DLLLOCAL int parseCheckMember(char *mem, const QoreParseTypeInfo *memberTypeInfo, bool priv) const {
      bool is_priv;
      const QoreTypeInfo *existingMemberTypeInfo;
      const QoreClass *sclass = parseFindPublicPrivateMember(mem, existingMemberTypeInfo, is_priv);
      if (!sclass)
	 return 0;

      return checkExistingMember(mem, memberTypeInfo, priv, sclass, existingMemberTypeInfo, is_priv);
   }

   DLLLOCAL int parseCheckMemberInBaseClasses(char *mem, const QoreParseTypeInfo *memberTypeInfo, bool priv) const {
      bool is_priv;
      const QoreTypeInfo *existingMemberTypeInfo;
      const QoreClass *sclass = scl ? scl->parseFindPublicPrivateMember(mem, existingMemberTypeInfo, is_priv) : 0;
      if (!sclass)
	 return 0;

      return checkExistingMember(mem, memberTypeInfo, priv, sclass, existingMemberTypeInfo, is_priv);
   }

   DLLLOCAL void parseAddPrivateMember(char *mem, QoreMemberInfo *memberInfo) {
      if (!parseCheckMember(mem, memberInfo, true)) {
	 //printd(5, "QoreClass::parseAddPrivateMember() this=%p %s adding %p %s\n", this, name, mem, mem);
	 pending_private_members[mem] = memberInfo;
	 return;
      }

      free(mem);
      delete memberInfo;
   }

   DLLLOCAL void parseAddPublicMember(char *mem, QoreMemberInfo *memberInfo) {
      if (!parseCheckMember(mem, memberInfo, false)) {
	 //printd(5, "QoreClass::parseAddPublicMember() this=%p %s adding %p %s\n", this, name, mem, mem);
	 pending_public_members[mem] = memberInfo;
	 return;
      }

      free(mem);
      delete memberInfo;
   }

   DLLLOCAL void insertStaticMethod(QoreMethod *m) {
      assert(m->isStatic());
   //printd(5, "QoreClass::insertStaticMethod() %s::%s() size=%d\n", priv->name, m->getName(), numMethods());
#ifdef DEBUG
      if (shm[m->getName()]) {
	 printd(0, "ERROR: static method '%s::%s()' inserted twice; fix your source code!\n", name, m->getName());
	 assert(false);
      }
#endif
      shm[m->getName()] = m;
      
      // maintain method counts (safely inside parse lock)
      ++num_static_methods;
      if (m->isUser())
	 ++num_static_user_methods;
   }

   DLLLOCAL void insertMethod(QoreMethod *m) {
      assert(!m->isStatic());
      //printd(5, "QoreClass::insertMethod() %s::%s() size=%d\n", name, m->getName(), numMethods());
#ifdef DEBUG
      if (hm[m->getName()]) {
	 printd(0, "ERROR: '%s::%s()' inserted twice; fix your source code!\n", name, m->getName());
	 assert(false);
      }
#endif
      hm[m->getName()] = m;
      
      // maintain method counts (safely inside parse lock)
      ++num_methods;
      if (m->isUser())
	 ++num_user_methods;
   }

   DLLLOCAL void parseCommit() {
      printd(5, "QoreClass::parseCommit() %s this=%p size=%d\n", name, this, hm_pending.size());

      hm_method_t::iterator i = hm_pending.begin();
      while (i != hm_pending.end()) {
	 QoreMethod *m = i->second;
	 hm_pending.erase(i);
	 i = hm_pending.begin();
	 insertMethod(m);
	 checkSpecial(m);
      }
      
      i = shm_pending.begin();
      while (i != shm_pending.end()) {
	 QoreMethod *m = i->second;
	 shm_pending.erase(i);
	 i = shm_pending.begin();
	 insertStaticMethod(m);
      }
      
      // add all pending private members to real member list
      member_map_t::iterator j = pending_private_members.begin();  
      while (j != pending_private_members.end()) { 
	 //printd(5, "QoreClass::parseCommit() %s committing private member %p %s\n", name, j->first, j->first);
	 private_members[j->first] = j->second;
	 pending_private_members.erase(j);
	 j = pending_private_members.begin();
      }

      // add all pending public members to real member list
      member_map_t::iterator k = pending_public_members.begin();  
      while (k != pending_public_members.end()) { 
	 //printd(5, "QoreClass::parseCommit() %s committing public member %p %s\n", name, j->first, j->first);
	 public_members[k->first] = k->second;
	 pending_public_members.erase(k);
	 k = pending_public_members.begin();
      }

      // we check base classes if they have public members if we don't have any
      // it's safe to call parseHasPublicMembersInHierarchy() because the 2nd stage
      // of parsing has completed without any errors (or we wouldn't be
      // running parseCommit())
      if (!has_public_members && (!public_members.empty() || (scl ? scl->parseHasPublicMembersInHierarchy() : false)))
	 has_public_members = true;
   }

   DLLLOCAL bool isPublicOrPrivateMember(const char *mem, bool &priv) const {
      if (private_members.find(const_cast<char *>(mem)) != private_members.end()) {
	 priv = true;
	 return true;
      }

      if (public_members.find(const_cast<char *>(mem)) != public_members.end()) {
	 priv = false;
	 return true;
      }

      return scl ? scl->isPublicOrPrivateMember(mem, priv) : false;
   }

   DLLLOCAL int initMembers(QoreObject *o, member_map_t::const_iterator i, member_map_t::const_iterator e, ExceptionSink *xsink) const {
      for (; i != e; ++i) {
	 if (i->second) {
	    AbstractQoreNode **v = o->getMemberValuePtrForInitialization(i->first);
	    // skip if already assigned by a subclass
	    if (*v)
	       continue;
	    if (i->second->exp) {
	       ReferenceHolder<AbstractQoreNode> val(i->second->exp->eval(xsink), xsink);
	       if (*xsink)
		  return -1;
	       // check types
	       if (i->second->checkMemberTypeInstantiation(i->first, *val, xsink))
		  return -1;
	       *v = val.release();
	    }
	    else {
	       *v = getDefaultValueForBuiltinValueType(i->second->getType());
	    }
	 }
      } 
      return 0;
   }

   DLLLOCAL int initMembers(QoreObject *o, ExceptionSink *xsink) const {
      if (initMembers(o, private_members.begin(), private_members.end(), xsink)
	  || initMembers(o, public_members.begin(), public_members.end(), xsink))
	 return -1;
      return 0;
   }

   DLLLOCAL QoreObject *execConstructor(const QoreListNode *args, ExceptionSink *xsink) const {
      // create new object
      QoreObject *o = new QoreObject(typeInfo.qc, getProgram());

      ReferenceHolder<BCEAList> bceal(scl ? new BCEAList : 0, xsink);

      printd(5, "QoreClass::execConstructor() %s::constructor() o=%p\n", name, o);

      // first, instantiate any members
      if (initMembers(o, xsink) || (scl && scl->initMembers(o, xsink))) {
	 o->deref(xsink);
	 return 0;
      }

      if (!constructor) {
	 if (scl) // execute superconstructors if any
	    scl->execConstructors(o, *bceal, xsink);
      }
      else { 
	 // no lock is sent with constructor, because no variable has been assigned yet
	 constructor->evalConstructor(o, args, scl, *bceal, xsink);
      }

      if (*xsink) {
	 // instead of executing the destructors for the superclasses that were already executed we call QoreObject::obliterate()
	 // which will clear out all the private data by running their dereference methods which must be OK
	 o->obliterate(xsink);
	 printd(5, "QoreClass::execConstructor() %s::constructor() o=%p, exception in constructor, dereferencing object and returning NULL\n", name, o);
	 return 0;
      }

      printd(5, "QoreClass::execConstructor() %s::constructor() returning o=%p\n", name, o);
      return o;
   }

   DLLLOCAL void addBuiltinMethod(BuiltinMethod *b, bool priv_flag, bool new_calling_convention) {
      assert(strcmp(b->getName(), "constructor"));
      assert(strcmp(b->getName(), "destructor"));
      assert(strcmp(b->getName(), "copy"));

      sys = true;
      QoreMethod *o = new QoreMethod(typeInfo.qc, b, priv_flag, false, new_calling_convention);
      insertMethod(o);
      // check for special methods (except constructor and destructor)
      checkSpecialIntern(o);
   }

   DLLLOCAL void addBuiltinStaticMethod(BuiltinMethod *b, bool priv_flag, bool new_calling_convention) {
      assert(strcmp(b->getName(), "constructor"));
      assert(strcmp(b->getName(), "destructor"));

      sys = true;
      QoreMethod *o = new QoreMethod(typeInfo.qc, b, priv_flag, true, new_calling_convention);
      insertStaticMethod(o);

      // check for special methods (except constructor and destructor) and abort if found
      assert(!checkSpecialStaticIntern(o));
   }

   DLLLOCAL void addBuiltinConstructor(BuiltinMethod *b, bool priv_flag, bool new_calling_convention) {
      sys = true;
      QoreMethod *o = new QoreMethod(typeInfo.qc, b, priv_flag, false, new_calling_convention);
      insertMethod(o);
      constructor = o;
   }
};

struct qore_method_private {
   const QoreClass *parent_class;
   int type;
   bool new_call_convention;
   union mf_u {
      UserFunction *userFunc;
      BuiltinMethod *builtin;
      
      DLLLOCAL mf_u(UserFunction *u) : userFunc(u) {}
      DLLLOCAL mf_u(BuiltinMethod *b) : builtin(b) {}
   } func;
   bool priv_flag, static_flag;

   DLLLOCAL qore_method_private(const QoreClass *p_class, UserFunction *uf, bool n_priv, bool n_static) : parent_class(p_class), type(OTF_USER), new_call_convention(false), func(uf), priv_flag(n_priv), static_flag(n_static) {
   }

   DLLLOCAL qore_method_private(UserFunction *uf, bool n_priv, bool n_static) : parent_class(0), type(OTF_USER), new_call_convention(false), func(uf), priv_flag(n_priv), static_flag(n_static) {
   }

   DLLLOCAL qore_method_private(const QoreClass *n_class, BuiltinMethod *b, bool n_priv, bool n_static, bool ncc = false) : parent_class(n_class), type(OTF_BUILTIN), new_call_convention(ncc), func(b), priv_flag(n_priv), static_flag(n_static) {
   }

   DLLLOCAL ~qore_method_private() {
      if (type == OTF_USER)
	 func.userFunc->deref();
      else
	 func.builtin->deref();
   }

   DLLLOCAL ParamList *getParams() const {
      if (type == OTF_USER) 
	 return const_cast<UserParamList *>(func.userFunc->params);
      return const_cast<ParamList *>(func.builtin->getParams()); 
   }

   DLLLOCAL const char *getName() const { return type == OTF_USER ? func.userFunc->getName() : func.builtin->getName(); }

   DLLLOCAL void parseInit() {
      assert(type == OTF_USER);
      func.userFunc->parseInitMethod(*parent_class, static_flag);
   }

   DLLLOCAL void parseInitConstructor(BCList *bcl) {
      assert(type == OTF_USER);
      func.userFunc->parseInitConstructor(*parent_class, bcl);
   }

   DLLLOCAL void parseInitDestructor() {
      assert(type == OTF_USER);
      func.userFunc->parseInitDestructor(*parent_class);
   }
   
   DLLLOCAL void parseInitCopy() {
      assert(type == OTF_USER);
      func.userFunc->parseInitCopy(*parent_class);
   }
   
   DLLLOCAL const UserFunction *getStaticUserFunction() const {
      assert(static_flag);
      assert(type == OTF_USER);
      
      return func.userFunc;
   }
   
   DLLLOCAL const BuiltinStaticMethod *getStaticBuiltinFunction() const {
      assert(static_flag);
      assert(type == OTF_BUILTIN);
      assert(!new_call_convention);

      return reinterpret_cast<BuiltinStaticMethod*>(func.builtin);
   }
   
   DLLLOCAL const BuiltinStaticMethod2 *getStaticBuiltinFunction2() const {
      assert(static_flag);
      assert(type == OTF_BUILTIN);
      assert(new_call_convention);

      return reinterpret_cast<BuiltinStaticMethod2*>(func.builtin);
   }
   
   DLLLOCAL bool existsUserParam(unsigned i) const {
      if (type != OTF_USER)
	 return true;
      
      return func.userFunc->params->numParams() > i;
   }

   DLLLOCAL bool newCallingConvention() const {
      return new_call_convention;
   }

   DLLLOCAL const QoreTypeInfo *getReturnTypeInfo() const {
      if (type == OTF_USER)
	 return func.userFunc->getReturnTypeInfo();
      return func.builtin->getReturnTypeInfo();
   }
};

class VRMutexHelper {
private:
   VRMutex *m;

public:
   DLLLOCAL VRMutexHelper(VRMutex *n_m, ExceptionSink *xsink) : m(n_m) {
      if (m && m->enter(xsink))
	 m = 0;
   }
   DLLLOCAL ~VRMutexHelper() {
      if (m)
	 m->exit();
   }
   DLLLOCAL operator bool() const { return m != 0; }
};

QoreListNode *BCEAList::findArgs(const QoreClass *qc, bool *aexeced) {
   bceamap_t::iterator i = find(qc);
   if (i != end()) {
      if (i->second->execed) {
	 *aexeced = true;
	 return 0;
      }
      *aexeced = false;
      i->second->execed = true;
      return i->second->args;
   }

   insert(std::make_pair(qc, new BCEANode()));
   *aexeced = false;
   return 0;
}

int BCEAList::add(const QoreClass *qc, QoreListNode *arg, ExceptionSink *xsink) {
   // see if class already exists in the list
   bceamap_t::iterator i = find(qc);
   if (i != end())
      return 0;

   // evaluate and save arguments
   insert(std::make_pair(qc, new BCEANode(arg ? arg->evalList(xsink) : 0)));
   if (xsink->isEvent())
      return -1;
   return 0;
}

void BCEAList::deref(ExceptionSink *xsink) {
   bceamap_t::iterator i;
   while ((i = begin()) != end()) {
      BCEANode *n = i->second;
      erase(i);
      
      if (n->args)
	 n->args->deref(xsink);
      delete n;
   }
   delete this;
}

BCANode::~BCANode() {
   if (ns)
      delete ns;
   if (name)
      free(name);
   if (argexp)
      argexp->deref(0);
}

void BCANode::resolve() {
   if (ns) {
      sclass = getRootNS()->parseFindScopedClass(ns);
      printd(5, "BCANode::resolve() this=%p resolved named scoped %s -> %p\n", this, ns->ostr, sclass);
      delete ns;
      ns = 0;
   }
   else {
      sclass = getRootNS()->parseFindClass(name);
      printd(5, "BCANode::resolve() this=%p resolved %s -> %p\n", this, name, sclass);
      free(name);
      name = 0;
   }   
}

BCNode::~BCNode() {
   delete cname;
   if (cstr)
      free(cstr);
   if (args)
      args->deref(0);
}

BCList::BCList(class BCNode *n) {
   push_back(n);
}

BCList::BCList() {
}

BCList::~BCList() {
   bclist_t::iterator i;
   while ((i = begin()) != end()) {
      delete *i;
      // erase() is constant time as long as i == begin()
      erase(i);
   }
}

void BCList::ref() const {
   ROreference();
}

void BCList::deref() {
   if (ROdereference())
      delete this;
}

void BCList::parseInit(QoreClass *cls, BCAList *bcal, bool &has_delete_blocker
#ifdef QORE_CLASS_SYNCHRONOUS
		       , bool &has_synchronous_in_hierarchy
#endif
   ) {
   printd(5, "BCList::parseInit(%s) this=%p empty=%d, bcal=%p\n", cls->getName(), this, empty(), bcal);
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if (!(*i)->sclass) {
	 if ((*i)->cname) {
	    (*i)->sclass = getRootNS()->parseFindScopedClass((*i)->cname);
	    printd(5, "BCList::parseInit() %s inheriting %s (%p)\n", cls->getName(), (*i)->cname->ostr, (*i)->sclass);
	    delete (*i)->cname;
	    (*i)->cname = 0;
	 }
	 else {
	    (*i)->sclass = getRootNS()->parseFindClass((*i)->cstr);
	    printd(5, "BCList::parseInit() %s inheriting %s (%p)\n", cls->getName(), (*i)->cstr, (*i)->sclass);
	    free((*i)->cstr);
	    (*i)->cstr = 0;
	 }
      }
      // recursively add base classes to special method list
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 if (!has_delete_blocker && (*i)->sclass->has_delete_blocker())
	    has_delete_blocker = true;
#ifdef QORE_CLASS_SYNCHRONOUS
	 if (!has_synchronous_in_hierarchy && (*i)->sclass->has_synchronous_in_hierarchy())
	    has_synchronous_in_hierarchy = true;
#endif
         (*i)->sclass->addBaseClassesToSubclass(cls, (*i)->is_virtual);
	 // include all subclass domains in this class' domain
	 cls->addDomain((*i)->sclass->getDomain());
      }
   }

   // compare each class in the list to ensure that there are no duplicates
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 bclist_t::iterator j = i;
	 while (++j != end())
	    if ((*i)->sclass == (*j)->sclass)
	       parse_error("class '%s' cannot inherit '%s' more than once", cls->getName(), (*i)->sclass->getName());
      }	 
   }

   // if there is a base constructor list, resolve all classes and 
   // ensure that all classes referenced are base classes of this class
   if (bcal) {
      for (bcalist_t::iterator i = bcal->begin(); i != bcal->end(); i++) {
	 (*i)->resolve();
	 if ((*i)->sclass && !match(*i))
	    parse_error("%s in base constructor argument list is not a base class of %s", (*i)->sclass->getName(), cls->getName());
      }
   }
}

bool BCList::isPublicOrPrivateMember(const char *mem, bool &priv) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if ((*i)->sclass && (*i)->sclass->isPublicOrPrivateMember(mem, priv))
	 return true;
   return false;
}

bool BCList::parseHasPublicMembersInHierarchy() const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if ((*i)->sclass && (*i)->sclass->parseHasPublicMembersInHierarchy())
	 return true;
   return false;
}
   
const QoreClass *BCList::parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&typeInfo, bool &priv) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 const QoreClass *qc = (*i)->sclass->parseFindPublicPrivateMember(mem, typeInfo, priv);
	 if (qc)
	    return qc;
      }
   }
   return 0;
}

// called at run time
const QoreMethod *BCList::findMethod(const char *name) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 // assert that the base class list has already been initialized if it exists
	 assert(!(*i)->sclass->priv->scl || ((*i)->sclass->priv->scl && (*i)->sclass->priv->initialized));

	 const QoreMethod *m;
	 if ((m = (*i)->sclass->findMethod(name)))
	    return m;
      }
   }
   return 0;
}

// called at parse time
const QoreMethod *BCList::findParseMethod(const char *name) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->findParseMethod(name)))
	    return m;
      }
   }
   return 0;
}

const QoreMethod *BCList::parseFindMethodTree(const char *name) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->parseFindMethodTree(name)))
	    return m;
      }
   }
   return 0;
}

// only called at run-time
const QoreMethod *BCList::findMethod(const char *name, bool &priv) const {
   for (bclist_t::const_iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->findMethod(name, priv))) {
	    if ((*i)->priv)
	       priv = true;
	    return m;
	 }
      }
   }
   return 0;
}

// called at run time
const QoreMethod *BCList::findStaticMethod(const char *name) const {
   for (bclist_t::const_iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 // assert that the base class list has already been initialized if it exists
	 assert(!(*i)->sclass->priv->scl || ((*i)->sclass->priv->scl && (*i)->sclass->priv->initialized));

	 const QoreMethod *m;
	 if ((m = (*i)->sclass->findStaticMethod(name)))
	    return m;
      }
   }
   return 0;
}

// called at parse time
const QoreMethod *BCList::findParseStaticMethod(const char *name) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->findParseStaticMethod(name)))
	    return m;
      }
   }
   return 0;
}

const QoreMethod *BCList::parseFindStaticMethodTree(const char *name) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->parseFindStaticMethodTree(name)))
	    return m;
      }
   }
   return 0;
}

// only called at run-time
const QoreMethod *BCList::findStaticMethod(const char *name, bool &priv) const {
   for (bclist_t::const_iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->findStaticMethod(name, priv))) {
	    if ((*i)->priv)
	       priv = true;
	    return m;
	 }
      }
   }
   return 0;
}

bool BCList::match(class BCANode *bca) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if (bca->sclass == (*i)->sclass) {
	 (*i)->args = bca->argexp;
	 bca->argexp = 0;
	 (*i)->hasargs = true;
	 return true;
      }
   }
   return false;
}

bool BCList::isPrivateMember(const char *str) const {
   for (bclist_t::const_iterator i = begin(); i != end(); i++)
      if ((*i)->sclass->isPrivateMember(str))
	 return true;
   return false;
}

const QoreMethod *BCList::resolveSelfMethod(const char *name) {
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->resolveSelfMethodIntern(name)))
	    return m;
      }
   }
   return 0;
}

bool BCList::execDeleteBlockers(QoreObject *o, ExceptionSink *xsink) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      //printd(5, "BCList::execDeleteBlockers() %s o=%p (for subclass %s)\n", (*i)->sclass->getName(), o, o->getClass()->getName());

      if ((*i)->sclass->execDeleteBlocker(o, xsink))
	 return true;
   }
   return false;
}

void BCList::execConstructors(QoreObject *o, class BCEAList *bceal, ExceptionSink *xsink) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      printd(5, "BCList::execConstructors() %s::constructor() o=%p (for subclass %s)\n", (*i)->sclass->getName(), o, o->getClass()->getName()); 
      
      // do not execute constructors for virtual base classes
      if ((*i)->is_virtual)
	 continue;
      (*i)->sclass->execSubclassConstructor(o, bceal, xsink);
      if (xsink->isEvent())
	 break;
   }
}

void BCList::execConstructorsWithArgs(QoreObject *o, class BCEAList *bceal, ExceptionSink *xsink) const {
   // if there are base constructor arguments that haven't already been overridden
   // by a base constructor argument specification in a subclass, evaluate them now
   for (bclist_t::const_iterator i = begin(); i != end(); ++i)
      if ((*i)->hasargs && bceal->add((*i)->sclass, (*i)->args, xsink))
	 return;
   execConstructors(o, bceal, xsink);
}

bool BCList::parseCheckHierarchy(const QoreClass *cls) const {
   for (bclist_t::const_iterator i = begin(); i != end(); ++i)
      if ((*i)->sclass->parseCheckHierarchy(cls))
	 return true;
   return false;
}

BCAList::BCAList(class BCANode *n) {
   push_back(n);
}

BCAList::~BCAList() {
   bcalist_t::iterator i;
   while ((i = begin()) != end()) {
      delete *i;
      erase(i);
   }
}

bool QoreClass::has_delete_blocker() const {
   return priv->has_delete_blocker;
}

BCSMList *QoreClass::getBCSMList() const {
   return priv->scl ? &priv->scl->sml : 0;
}

const QoreMethod *QoreClass::findLocalStaticMethod(const char *nme) const {
   hm_method_t::const_iterator i = priv->shm.find(nme);
   return (i != priv->shm.end()) ? i->second : 0;
}

const QoreMethod *QoreClass::findLocalMethod(const char *nme) const {
   hm_method_t::const_iterator i = priv->hm.find(nme);
   return (i != priv->hm.end()) ? i->second : 0;
}

const QoreMethod *QoreClass::findStaticMethod(const char *nme) const {
   const QoreMethod *w;
   if (!(w = findLocalStaticMethod(nme))) {
      // search superclasses
      if (priv->scl)
	 w = priv->scl->findStaticMethod(nme);
   }
   return w;
}

const QoreMethod *QoreClass::findMethod(const char *nme) const {
   const QoreMethod *w;
   if (!(w = findLocalMethod(nme))) {
      // search superclasses
      if (priv->scl)
	 w = priv->scl->findMethod(nme);
   }
   return w;
}

const QoreMethod *QoreClass::findStaticMethod(const char *nme, bool &priv_flag) const {
   priv_flag = false;

   const QoreMethod *w;
   if (!(w = findLocalStaticMethod(nme))) {
      // search superclasses
      if (priv->scl)
	 w = priv->scl->findStaticMethod(nme, priv_flag);
   }
   return w;
}

const QoreMethod *QoreClass::findMethod(const char *nme, bool &priv_flag) const {
   priv_flag = false;

   const QoreMethod *w;
   if (!(w = findLocalMethod(nme))) {
      // search superclasses
      if (priv->scl)
	 w = priv->scl->findMethod(nme, priv_flag);
   }
   return w;
}

const QoreMethod *QoreClass::findParseMethod(const char *nme) {
   const QoreMethod *w;
   if ((w = findLocalMethod(nme)))
      return w;

   // search superclasses
   return priv->scl ? priv->scl->findParseMethod(nme) : 0;
}

const QoreMethod *QoreClass::findParseStaticMethod(const char *nme) {
   const QoreMethod *w;
   if ((w = findLocalStaticMethod(nme)))
      return w;

   // search superclasses
   return priv->scl ? priv->scl->findParseStaticMethod(nme) : 0;
}

// only called when parsing
void QoreClass::setName(const char *n) {
   assert(!priv->name);
   priv->name = strdup(n);
}

bool QoreClass::is_unique() const {
   return priv->nref.is_unique();
}

QoreClass *QoreClass::getReference() {
   //printd(5, "QoreClass::getReference() %08x %s %d -> %d\n", this, priv->name, nref.reference_count(), nref.reference_count() + 1);
   priv->nref.ROreference();
   return this;
}

void QoreClass::nderef() {
   //printd(5, "QoreClass::nderef() %p %s %d -> %d\n", this, priv->name, nref.reference_count(), nref.reference_count() - 1);
   if (priv->nref.ROdereference())
      delete this;
}

bool QoreClass::hasCopy() const {
   return priv->copyMethod ? true : false; 
}

qore_classid_t QoreClass::getID() const { 
   return priv->classID; 
}

qore_classid_t QoreClass::getIDForMethod() const { 
   return priv->methodID;
}

bool QoreClass::isSystem() const { 
   return priv->sys;
}

bool QoreClass::hasMemberGate() const {
   return priv->memberGate != 0;
}

bool QoreClass::hasMemberNotification() const {
   return priv->memberNotification != 0;
}

int QoreClass::getDomain() const {
   return priv->domain;
}

const char *QoreClass::getName() const { 
   return priv->name; 
}

int QoreClass::numMethods() const {
   return priv->num_methods;

   //not thread-safe!
   //return priv->hm.size();
}

int QoreClass::numStaticMethods() const {
   return priv->num_static_methods;

   //not thread-safe!
   //return priv->shm.size();
}

int QoreClass::numUserMethods() const {
   return priv->num_user_methods;
}

int QoreClass::numStaticUserMethods() const {
   return priv->num_static_user_methods;
}

const QoreMethod *QoreClass::parseFindStaticMethod(const char *nme) {
   const QoreMethod *m;
   if ((m = findLocalStaticMethod(nme)))
      return m;

   // look in pending methods
   hm_method_t::iterator i = priv->shm_pending.find(nme);
   return i != priv->shm_pending.end() ? i->second : 0;
}

const QoreMethod *QoreClass::parseFindMethod(const char *nme) {
   const QoreMethod *m;
   if ((m = findLocalMethod(nme)))
      return m;

   // look in pending methods
   hm_method_t::iterator i = priv->hm_pending.find(nme);
   if (i != priv->hm_pending.end())
      return i->second;

   return 0;
}

const QoreMethod *QoreClass::parseFindMethodTree(const char *nme) {
   const QoreMethod *m = parseFindMethod(nme);
   if (!m && priv->scl)
      m = priv->scl->parseFindMethodTree(nme);
   return m;
}

const QoreMethod *QoreClass::parseFindStaticMethodTree(const char *nme) {
   const QoreMethod *m = parseFindStaticMethod(nme);
   if (!m && priv->scl)
      m = priv->scl->parseFindStaticMethodTree(nme);
   return m;
}

void QoreClass::addBuiltinBaseClass(QoreClass *qc, QoreListNode *xargs) {
   if (!priv->scl)
      priv->scl = new BCList();
   priv->scl->push_back(new BCNode(qc, xargs));
}

void QoreClass::addDefaultBuiltinBaseClass(QoreClass *qc, QoreListNode *xargs) {
   addBuiltinBaseClass(qc, xargs);
   // make sure no methodID has already been assigned
   assert(priv->methodID == priv->classID);
   priv->methodID = qc->priv->classID;
}

void QoreClass::addBuiltinVirtualBaseClass(QoreClass *qc) {
   assert(qc);

   //printd(5, "adding %s as virtual base class to %s\n", qc->priv->name, priv->name);
   if (!priv->scl)
      priv->scl = new BCList();
   priv->scl->push_back(new BCNode(qc, 0, true));   
}

void QoreClass::setSystemConstructor(q_system_constructor_t m) {
   priv->sys = true;
   priv->system_constructor = new QoreMethod(this, new BuiltinSystemConstructor(this, m));
}

void QoreClass::setSystemConstructor2(q_system_constructor2_t m) {
   priv->sys = true;
   priv->system_constructor = new QoreMethod(this, new BuiltinSystemConstructor2(this, m), false, false, true);
}

// deletes all pending user methods
void QoreClass::parseRollback() {
   priv->delete_pending_methods();
}

QoreMethod::QoreMethod(const QoreClass *p_class, UserFunction *u, bool n_priv, bool n_static) : priv(new qore_method_private(p_class, u, n_priv, n_static)) {
}

// created at parse time, parent class assigned when method attached to class
QoreMethod::QoreMethod(UserFunction *u, bool n_priv, bool n_static) : priv(new qore_method_private(u, n_priv, n_static)) {
}

QoreMethod::QoreMethod(const QoreClass *p_class, BuiltinMethod *b, bool n_priv, bool n_static) : priv(new qore_method_private(p_class, b, n_priv, n_static)) {
}

QoreMethod::QoreMethod(const QoreClass *p_class, BuiltinMethod *b, bool n_priv, bool n_static, bool new_calling_convention) : priv(new qore_method_private(p_class, b, n_priv, n_static, new_calling_convention)) {
}

QoreMethod::~QoreMethod() {
   delete priv;
}

ParamList *QoreMethod::getParams() const {
   return priv->getParams();
}

bool QoreMethod::newCallingConvention() const {
   return priv->newCallingConvention();
}

int QoreMethod::getType() const {
   return priv->type;
}

bool QoreMethod::isUser() const {
   return priv->type == CT_USER;
}

bool QoreMethod::isBuiltin() const {
   return priv->type == CT_BUILTIN;
}

bool QoreMethod::isPrivate() const { 
   return priv->priv_flag; 
}

bool QoreMethod::isStatic() const {
   return priv->static_flag;
}

const char *QoreMethod::getName() const {
   return priv->getName();
}

const QoreClass *QoreMethod::getClass() const {
   return priv->parent_class;
}

void QoreMethod::assign_class(const QoreClass *p_class) {
   assert(!priv->parent_class);
   priv->parent_class = p_class;
}

bool QoreMethod::isSynchronized() const {
   if (priv->type == OTF_BUILTIN)
      return false;
   return priv->func.userFunc->isSynchronized();
}

bool QoreMethod::inMethod(const QoreObject *self) const {
   if (priv->type == OTF_USER)
      return ::inMethod(priv->func.userFunc->getName(), self);
   return ::inMethod(priv->func.builtin->getName(), self);
}

void QoreMethod::evalSystemConstructor(QoreObject *self, int code, va_list args) const {
   // type must be OTF_BUILTIN
   if (priv->new_call_convention)
      reinterpret_cast<BuiltinSystemConstructor2 *>(priv->func.builtin)->eval(*priv->parent_class, self, code, args);
   else
      reinterpret_cast<BuiltinSystemConstructor*>(priv->func.builtin)->eval(self, code, args);
}

void QoreMethod::evalSystemDestructor(QoreObject *self, ExceptionSink *xsink) const {
   // get pointer to private data object from class ID of base type
   AbstractPrivateData *ptr = self->getAndClearPrivateData(priv->func.builtin->myclass->getID(), xsink);
   //printd(5, "QoreMethod::evalSystemDestructor() class=%s (%p) id=%d ptr=%p\n", priv->func.builtin->myclass->getName(), priv->func.builtin->myclass, priv->func.builtin->myclass->getID(), ptr);
   // NOTE: ptr may be null for builtin subclasses without private data
   if (ptr) {
#ifdef QORE_CLASS_SYNCHRONOUS
      // grab class synchronous lock if appropriate
      bool lck = priv->parent_class->is_synchronous_class();
      VRMutexHelper vh(lck ? self->getClassSyncLock() : 0, xsink);
      assert(!(lck && !vh));
#endif
      if (priv->new_call_convention)
	 reinterpret_cast<BuiltinDestructor2*>(priv->func.builtin)->evalSystem(*priv->parent_class, self, ptr, xsink);
      else
	 reinterpret_cast<BuiltinDestructor*>(priv->func.builtin)->evalSystem(self, ptr, xsink);
   }
}

void QoreMethod::parseInit() {
   priv->parseInit();
}

void QoreMethod::parseInitConstructor(BCList *bcl) {
   priv->parseInitConstructor(bcl);
}

void QoreMethod::parseInitDestructor() {
   priv->parseInitDestructor();
}

void QoreMethod::parseInitCopy() {
   priv->parseInitCopy();
}

QoreMethod *QoreMethod::copy(const QoreClass *p_class) const {
   if (priv->type == OTF_USER) {
      priv->func.userFunc->ROreference();
      return new QoreMethod(p_class, priv->func.userFunc, priv->priv_flag, priv->static_flag);
   }

   priv->func.builtin->ROreference();
   return new QoreMethod(p_class, priv->func.builtin, priv->priv_flag, priv->static_flag);
}

const QoreTypeInfo *QoreMethod::getReturnTypeInfo() const {
   return priv->getReturnTypeInfo();
}

static const QoreClass *getStackClass() {
   QoreObject *obj = getStackObject();
   if (obj)
      return obj->getClass();
   return 0;
}

void QoreClass::parseAddPrivateMember(char *nme, QoreMemberInfo *mInfo) {
   priv->parseAddPrivateMember(nme, mInfo);
}

void QoreClass::parseAddPublicMember(char *nme, QoreMemberInfo *mInfo) {
   priv->parseAddPublicMember(nme, mInfo);
}

bool BCSMList::isBaseClass(QoreClass *qc) const {
   class_list_t::const_iterator i = begin();
   while (i != end()) {
      //printd(5, "BCSMList::isBaseClass() %s (%d) == %s (%d)\n", qc->getName(), qc->getID(), (*i).first->getName(), (*i).first->getID());
      if (qc == (*i).first)
	 return true;
      i++;
   }
   return false;
}

void BCSMList::addBaseClassesToSubclass(QoreClass *thisclass, QoreClass *sc, bool is_virtual) {
   //printd(5, "BCSMList::addBaseClassesToSubclass(this=%s, sc=%s) size=%d\n", thisclass->getName(), sc->getName());
   for (class_list_t::const_iterator i = begin(), e = end(); i != e; ++i)
      sc->priv->scl->sml.add(thisclass, (*i).first, is_virtual || (*i).second);
}

void BCSMList::add(QoreClass *thisclass, QoreClass *qc, bool is_virtual) {
   if (thisclass == qc) {
      parse_error("class '%s' cannot inherit itself", qc->getName());
      return;
   }

   // see if class already exists in list
   class_list_t::const_iterator i = begin();
   while (i != end()) {
      if ((*i).first == qc)
         return;
      if ((*i).first == thisclass) {
      	 parse_error("circular reference in class hierarchy, '%s' is an ancestor of itself", thisclass->getName());
      	 return;
      }
      i++;
   }

   // append to the end of the doubly-linked list
   push_back(std::make_pair(qc, is_virtual));
}

void BCSMList::execDestructors(QoreObject *o, ExceptionSink *xsink) const {
   class_list_t::const_reverse_iterator i = rbegin();
   // cast below required by g++ 3.2 at least
   while (i != rend()) {
      printd(5, "BCSMList::execDestructors() %s::destructor() o=%p virt=%s (subclass %s)\n", (*i).first->getName(), o, (*i).second ? "true" : "false", o->getClass()->getName());
      if (!(*i).second)
	 (*i).first->execSubclassDestructor(o, xsink);
      i++;
   }
}

void BCSMList::execSystemDestructors(QoreObject *o, ExceptionSink *xsink) const {
   class_list_t::const_reverse_iterator i = rbegin();
   while (i != rend()) {
      printd(5, "BCSMList::execSystemDestructors() %s::destructor() o=%p virt=%s (subclass %s)\n", (*i).first->getName(), o, (*i).second ? "true" : "false", o->getClass()->getName());
      if (!(*i).second)
	 (*i).first->execSubclassSystemDestructor(o, xsink);
      i++;
   }
}

void BCSMList::execCopyMethods(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const {
   class_list_t::const_iterator i = begin();
   while (i != end()) {
      if (!(*i).second) {
	 (*i).first->execSubclassCopy(self, old, xsink);
	 if (xsink->isEvent())
	    break;
      }
      i++;
   }
}

QoreClass *BCSMList::getClass(qore_classid_t cid) const {
   class_list_t::const_iterator i = begin();
   while (i != end()) {
      if ((*i).first->getID() == cid)
	 return (*i).first;
      i++;
   }
   return 0;
}

QoreClass::QoreClass(const char *nme, int dom) {
   priv = new qore_qc_private(this, nme, dom);

   priv->classID = priv->methodID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating '%s' ID:%d (this=%p)\n", priv->name, priv->classID, this);
}

QoreClass::QoreClass() {
   priv = new qore_qc_private(this, 0);

   priv->classID = priv->methodID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating unnamed class ID:%d (this=%p)\n", priv->classID, this);
}

QoreClass::QoreClass(qore_classid_t id, const char *nme) {
   priv = new qore_qc_private(this, nme);

   priv->classID = id;
   printd(5, "QoreClass::QoreClass() creating copy of '%s' ID:%d (this=%p)\n", priv->name, priv->classID, this);
}

QoreClass::~QoreClass() {
   delete priv;
}

BCAList *QoreClass::getBaseClassConstructorArgumentList() const {
   return priv->bcal;
}

QoreClass *QoreClass::getClass(qore_classid_t cid) const {
   if (cid == priv->classID)
      return (QoreClass *)this;
   return priv->scl ? priv->scl->sml.getClass(cid) : 0;
}

const QoreClass *QoreClass::parseGetClass(qore_classid_t cid, bool &cpriv) const {
   cpriv = false;
   const_cast<QoreClass *>(this)->priv->initialize();
   if (cid == priv->classID)
      return (QoreClass *)this;
   return priv->scl ? priv->scl->getClass(cid, cpriv) : 0;
}

const QoreClass *QoreClass::getClassIntern(qore_classid_t cid, bool &cpriv) const {
   if (cid == priv->classID)
      return (QoreClass *)this;
   return priv->scl ? priv->scl->getClass(cid, cpriv) : 0;
}

const QoreClass *QoreClass::getClass(qore_classid_t cid, bool &cpriv) const {
   cpriv = false;
   return getClassIntern(cid, cpriv);
}

AbstractQoreNode *QoreMethod::eval(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const {
   QORE_TRACE("QoreMethod::eval()");
#ifdef DEBUG
   const char *oname = self ? self->getClass()->getName() : "<n/a: static>";
   printd(5, "QoreMethod::eval() %s::%s() (object=%p, pgm=%p, static=%s)\n", oname, getName(), self, self ? self->getProgram() : 0, isStatic() ? "true" : "false");
#endif

   if (isStatic()) {
      if (priv->type == OTF_USER)
	 return priv->func.userFunc->eval(args, 0, xsink, priv->parent_class->getName());
      if (priv->new_call_convention)
	 return reinterpret_cast<BuiltinStaticMethod2*>(priv->func.builtin)->eval(*this, args, xsink);
      return reinterpret_cast<BuiltinStaticMethod*>(priv->func.builtin)->eval(*this, args, xsink);
   }

   AbstractQoreNode *rv = 0;
   {
      // switch to new program for imported objects
      ProgramContextHelper pch(self->getProgram(), xsink);

      if (priv->type == OTF_USER) {
	 // ignore class synchronous flags for user methods - currently only possible
	 // with builtin classes and therefore only affects builtin code
	 // allowing it to be used in user methods could open up the possibility for deadlocks
	 // on the destructor, which we would like to avoid
	 rv = priv->func.userFunc->eval(args, self, xsink, priv->parent_class->getName());
      }
      else {
	 // save current program location in case there's an exception
	 const char *o_fn = get_pgm_file();
	 int o_ln, o_eln;
	 get_pgm_counter(o_ln, o_eln);

	 // evalute arguments before calling builtin method
	 QoreListNodeEvalOptionalRefHolder new_args(args, xsink);
	 if (*xsink)
	    return 0;

	 // reset program position after arguments are evaluted
	 update_pgm_counter_pgm_file(o_ln, o_eln, o_fn);   

	 {
#ifdef QORE_CLASS_SYNCHRONOUS
	    // grab class synchronous lock if appropriate
	    bool lck = priv->parent_class->is_synchronous_class();
	    VRMutexHelper vh(lck ? self->getClassSyncLock() : 0, xsink);
	    assert(!(lck && !vh));
#endif
	    if (priv->new_call_convention)
	       rv = self->evalBuiltinMethodWithPrivateData(*this, priv->func.builtin, *new_args, xsink);
	    else
	       rv = self->evalBuiltinMethodWithPrivateData(priv->func.builtin, *new_args, xsink);
	 }
	 if (xsink->isException())
	    xsink->addStackInfo(CT_BUILTIN, self->getClass()->getName(), getName(), o_fn, o_ln, o_eln);
      }
   }
   
#ifdef DEBUG
   printd(5, "QoreMethod::eval() %s::%s() returning %p (type=%s, refs=%d)\n",
	  oname, getName(), rv, rv ? rv->getTypeName() : "(null)", rv ? rv->reference_count() : 0);
#endif

   return rv;
}

void QoreMethod::evalConstructor(QoreObject *self, const QoreListNode *args, class BCList *bcl, class BCEAList *bceal, ExceptionSink *xsink) const {
   QORE_TRACE("QoreMethod::evalConstructor()");
#ifdef DEBUG
   const char *oname = self->getClass()->getName();
   printd(5, "QoreMethod::evalConstructor() %s::%s() (object=%p, pgm=%p)\n", oname, getName(), self, self->getProgram());
#endif

   if (priv->type == OTF_USER)
      discard(priv->func.userFunc->evalConstructor(args, self, bcl, bceal, priv->parent_class->getName(), xsink), xsink);
   else {
      // evalute arguments before calling builtin method
      QoreListNodeEvalOptionalRefHolder new_args(args, xsink);
      if (*xsink)
	 return;

      // switch to new program for imported objects
      ProgramContextHelper pch(self->getProgram(), xsink);
      if (priv->new_call_convention)
	 reinterpret_cast<BuiltinConstructor2*>(priv->func.builtin)->eval(*priv->parent_class, self, *new_args, bcl, bceal, priv->parent_class->getName(), xsink);
      else
	 reinterpret_cast<BuiltinConstructor*>(priv->func.builtin)->eval(self, *new_args, bcl, bceal, priv->parent_class->getName(), xsink);
   }

#ifdef DEBUG
   printd(5, "QoreMethod::evalConstructor() %s::%s() done\n", oname, getName());
#endif
}

void QoreMethod::evalCopy(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const {
   // switch to new program for imported objects
   ProgramContextHelper pch(self->getProgram(), xsink);

   if (priv->type == OTF_USER)
      priv->func.userFunc->evalCopy(old, self, priv->parent_class->getName(), xsink);
   else { 
      // builtin function
#ifdef QORE_CLASS_SYNCHRONOUS
      // grab class synchronous lock if appropriate
      bool lck = priv->parent_class->is_synchronous_class();
      VRMutexHelper vh(lck ? self->getClassSyncLock() : 0, xsink);
      assert(!(lck && !vh));
#endif
      old->evalCopyMethodWithPrivateData(*priv->parent_class, priv->func.builtin, self, priv->new_call_convention, xsink);
   }
}

bool QoreMethod::evalDeleteBlocker(QoreObject *self) const {
   // can only be builtin
   return self->evalDeleteBlocker(priv->func.builtin);
}

void QoreMethod::evalDestructor(QoreObject *self, ExceptionSink *xsink) const {
   // switch to new program for imported objects
   ProgramContextHelper pch(self->getProgram(), xsink);

   if (priv->type == OTF_USER)
      priv->func.userFunc->eval(0, self, xsink, priv->parent_class->getName());
   else { // builtin function
      AbstractPrivateData *ptr = self->getAndClearPrivateData(priv->parent_class->getID(), xsink);
      if (ptr) {
#ifdef QORE_CLASS_SYNCHRONOUS
	 // grab class synchronous lock if appropriate
	 bool lck = priv->parent_class->is_synchronous_class();
	 VRMutexHelper vh(lck ? self->getClassSyncLock() : 0, xsink);
	 assert(!(lck && !vh));
#endif
	 if (priv->new_call_convention)
	    reinterpret_cast<BuiltinDestructor2*>(priv->func.builtin)->eval(*priv->parent_class, self, ptr, priv->parent_class->getName(), xsink);
	 else
	    reinterpret_cast<BuiltinDestructor*>(priv->func.builtin)->eval(self, ptr, priv->parent_class->getName(), xsink);
      }
      // in case there is no private data, ignore: we cannot execute the destructor
      // this might happen in the case that the data was deleted externally, for example
      // with QoreObject::externalDelete()
   }
}

const UserFunction *QoreMethod::getStaticUserFunction() const {
   return priv->getStaticUserFunction();
}

const BuiltinStaticMethod *QoreMethod::getStaticBuiltinFunction() const {
   return priv->getStaticBuiltinFunction();
}

const BuiltinStaticMethod2 *QoreMethod::getStaticBuiltinFunction2() const {
   return priv->getStaticBuiltinFunction2();
}

bool QoreMethod::existsUserParam(unsigned i) const {
   return priv->existsUserParam(i);
}

QoreClass *QoreClass::copyAndDeref() {
   QORE_TRACE("QoreClass::copyAndDeref");
   QoreClass *noc = new QoreClass(priv->classID, priv->name);
   noc->priv->methodID = priv->methodID;

   printd(5, "QoreClass::copyAndDeref() name=%s (%p) new name=%s (%p)\n", priv->name, priv->name, noc->priv->name, noc->priv->name);

   // set up method list
   for (hm_method_t::iterator i = priv->hm.begin(); i != priv->hm.end(); i++) {
      QoreMethod *nf = i->second->copy(noc);

      noc->priv->hm[nf->getName()] = nf;
      if (i->second == priv->constructor)
	 noc->priv->constructor  = nf;
      else if (i->second == priv->destructor)
	 noc->priv->destructor   = nf;
      else if (i->second == priv->copyMethod)
	 noc->priv->copyMethod   = nf;
      else if (i->second == priv->methodGate)
	 noc->priv->methodGate   = nf;
      else if (i->second == priv->memberGate)
	 noc->priv->memberGate   = nf;
      else if (i->second == priv->memberNotification)
	 noc->priv->memberNotification = nf;
   }

   // set up static method list
   for (hm_method_t::iterator i = priv->shm.begin(); i != priv->shm.end(); i++) {
      QoreMethod *nf = i->second->copy(noc);

      noc->priv->shm[nf->getName()] = nf;
   }

   // copy private member list
   for (member_map_t::iterator i = priv->private_members.begin(); i != priv->private_members.end(); i++)
      noc->priv->private_members[strdup(i->first)] = i->second->copy();

   if (priv->scl) {
      priv->scl->ref();
      noc->priv->scl = priv->scl;
   }

   nderef();
   return noc;
}

void QoreClass::insertMethod(QoreMethod *m) {
   priv->insertMethod(m);
}      

void QoreClass::insertStaticMethod(QoreMethod *m) {
   priv->insertStaticMethod(m);
}      

void QoreClass::addDomain(int dom) {
   priv->domain |= dom;
}

AbstractQoreNode *QoreClass::evalMethod(QoreObject *self, const char *nme, const QoreListNode *args, ExceptionSink *xsink) const {
   QORE_TRACE("QoreClass::evalMethod()");
   const QoreMethod *w;
   int external = (this != getStackClass());
   printd(5, "QoreClass::evalMethod() %s::%s() %s call attempted\n", priv->name, nme, external ? "external" : "internal" );

   if (!strcmp(nme, "copy"))
      return execCopy(self, xsink);

   bool priv_flag;
   if (!(w = findMethod(nme, priv_flag)) && !(w = findStaticMethod(nme, priv_flag))) {
      if (priv->methodGate && !priv->methodGate->inMethod(self)) // call methodGate with unknown method name and arguments
	 return evalMethodGate(self, nme, args, xsink);
      // otherwise return an exception
      xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() has been defined", priv->name, nme);
      return 0;
   }

   // check for illegal explicit call
   if (w == priv->constructor || w == priv->destructor || w == priv->deleteBlocker) {
      xsink->raiseException("ILLEGAL-EXPLICIT-METHOD-CALL", "explicit calls to ::%s() methods are not allowed", nme);
      return 0;      
   }

   if (external) {
      if (w->isPrivate()) {
	 xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s() is private and cannot be accessed externally", priv->name, nme);
	 return 0;
      }
      else if (priv_flag) {
	 xsink->raiseException("BASE-CLASS-IS-PRIVATE", "%s() is a method of a privately-inherited class of %s", nme, priv->name);
	 return 0;
      }
   }

   return self->evalMethod(*w, args, xsink);
}

AbstractQoreNode *QoreClass::evalMethodGate(QoreObject *self, const char *nme, const QoreListNode *args, ExceptionSink *xsink) const {
   printd(5, "QoreClass::evalMethodGate() method=%s args=%p\n", nme, args);

   ReferenceHolder<QoreListNode> args_holder(xsink);

   // build new argument list
   if (args) {
      if (args->needs_eval())
	 args_holder = args->evalList(xsink);
      else
	 args_holder = args->copy();
      if (*xsink)
	 return 0;
   }
   else
      args_holder = new QoreListNode();

   args_holder->insert(new QoreStringNode(nme));

   return self->evalMethod(*priv->methodGate, *args_holder, xsink);
}

bool QoreClass::isPrivateMember(const char *str) const {
   member_map_t::const_iterator i = priv->private_members.find((char *)str);
   if (i != priv->private_members.end())
      return true;

   if (priv->scl)
      return priv->scl->isPrivateMember(str);
   return false;
}

AbstractQoreNode *QoreClass::evalMemberGate(QoreObject *self, const QoreString *nme, ExceptionSink *xsink) const {
   assert(nme && nme->getEncoding() == QCS_DEFAULT);

   printd(5, "QoreClass::evalMemberGate() member=%s\n", nme->getBuffer());
   // do not run memberGate method if we are already in it...
   if (!priv->memberGate || priv->memberGate->inMethod(self))
      return 0;

   ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
   args->push(new QoreStringNode(*nme));

   return self->evalMethod(*priv->memberGate, *args, xsink);
}

void QoreClass::execMemberNotification(QoreObject *self, const char *mem, ExceptionSink *xsink) const {
   // cannot run this method when executing from within the class
   assert((this != getStackClass()));

   //printd(5, "QoreClass::execMemberNotification() member=%s\n", mem);

   ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
   args->push(new QoreStringNode(mem));
   discard(self->evalMethod(*priv->memberNotification, *args, xsink), xsink);
}

QoreObject *QoreClass::execConstructor(const QoreListNode *args, ExceptionSink *xsink) const {
   return priv->execConstructor(args, xsink);
}

QoreObject *QoreClass::execSystemConstructor(int code, ...) const {
   assert(priv->system_constructor);

   va_list args;

   // create new object
   QoreObject *o = new QoreObject(this, 0);

   va_start(args, code);
   // no lock is sent with constructor, because no variable has been assigned yet
   priv->system_constructor->evalSystemConstructor(o, code, args);
   va_end(args);

   printd(5, "QoreClass::execSystemConstructor() %s::execSystemConstructor() returning %p\n", priv->name, o);
   return o;
}

void QoreClass::execSubclassConstructor(QoreObject *self, class BCEAList *bceal, ExceptionSink *xsink) const {
   // if there is no constructor, execute the superclass constructors directly
   if (!priv->constructor){
      if (priv->scl) // execute superconstructors if any
	 priv->scl->execConstructors(self, bceal, xsink);
   }
   else { // no lock is sent with constructor, because no variable has been assigned yet
      bool already_executed;
      QoreListNode *args = bceal->findArgs(this, &already_executed);
      if (!already_executed) {
	 priv->constructor->evalConstructor(self, args, priv->scl, bceal, xsink);
      }
   }
}

bool QoreClass::execDeleteBlocker(QoreObject *self, ExceptionSink *xsink) const {
   printd(5, "QoreClass::execDeleteBlocker(self=%p) this=%p '%s' has_delete_blocker=%s deleteBlocker=%p\n", self, this, priv->name, priv->has_delete_blocker ? "true" : "false", priv->deleteBlocker);
   if (priv->has_delete_blocker) {
      if (priv->scl) // execute superclass delete blockers if any
	 if (priv->scl->execDeleteBlockers(self, xsink))
	    return true;
      if (priv->deleteBlocker) {
	 return priv->deleteBlocker->evalDeleteBlocker(self);
      }
   }
   return false;
}

void QoreClass::execDestructor(QoreObject *self, ExceptionSink *xsink) const {
   printd(5, "QoreClass::execDestructor() %s::destructor() o=%p\n", priv->name, self);

   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;

   if (self->isSystemObject()) {
      if (priv->destructor)
	 priv->destructor->evalSystemDestructor(self, &de);
      else
	 self->defaultSystemDestructor(priv->classID, &de);

      // execute superclass destructors
      if (priv->scl)
	 priv->scl->sml.execSystemDestructors(self, &de);
   }
   else {
      if (priv->destructor)
	 priv->destructor->evalDestructor(self, &de);
      else if (priv->sys)
	 self->defaultSystemDestructor(priv->classID, &de);

      // execute superclass destructors
      if (priv->scl)
	 priv->scl->sml.execDestructors(self, &de);
   }

   xsink->assimilate(&de);
}

void QoreClass::execSubclassDestructor(QoreObject *self, ExceptionSink *xsink) const {
   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;
   if (priv->destructor)
      priv->destructor->evalDestructor(self, &de);
   else if (priv->sys)
      self->defaultSystemDestructor(priv->classID, &de);

   xsink->assimilate(&de);
}

void QoreClass::execSubclassSystemDestructor(QoreObject *self, ExceptionSink *xsink) const {
   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;
   if (priv->destructor)
      priv->destructor->evalSystemDestructor(self, &de);
   else if (priv->sys)
      self->defaultSystemDestructor(priv->classID, &de);

   xsink->assimilate(&de);
}

QoreObject *QoreClass::execCopy(QoreObject *old, ExceptionSink *xsink) const {
   QoreHashNode *h = old->copyData(xsink);
   if (*xsink)
      return 0;

   // save current program location in case there's an exception
   const char *o_fn = 0;
   int o_ln = 0, o_eln = 0;

   ReferenceHolder<QoreObject> self(new QoreObject(this, getProgram(), h), xsink);

   // execute superclass copy methods
   if (priv->scl) {
      o_fn = get_pgm_file();
      get_pgm_counter(o_ln, o_eln);

      priv->scl->sml.execCopyMethods(*self, old, xsink);
   }
   
   if (priv->copyMethod && !xsink->isEvent()) {
      // reload the old position for the copy method
      if (o_fn)
	 update_pgm_counter_pgm_file(o_ln, o_eln, o_fn);
      
      priv->copyMethod->evalCopy(*self, old, xsink);
      if (xsink->isException())
	 xsink->addStackInfo(priv->copyMethod->getType(), old->getClass()->getName(), "copy", o_fn, o_ln, o_eln);
   }

   return *xsink ? 0 : self.release();
}

void QoreClass::execSubclassCopy(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const {
   if (priv->copyMethod)
      priv->copyMethod->evalCopy(self, old, xsink);
}

void QoreClass::addBaseClassesToSubclass(QoreClass *sc, bool is_virtual) {
   if (priv->scl)
      priv->scl->sml.addBaseClassesToSubclass(this, sc, is_virtual);
   sc->priv->scl->sml.add(sc, this, is_virtual);
}

// private, called from subclasses only
const QoreMethod *QoreClass::resolveSelfMethodIntern(const char *nme) {
   const QoreMethod *m = parseFindMethod(nme);
   if (!m)
      m = parseFindStaticMethod(nme);

   // if still not found now look in superclass methods
   if (!m && priv->scl)
      m = priv->scl->resolveSelfMethod(nme);

   return m;
}

const QoreMethod *QoreClass::resolveSelfMethod(const char *nme) {
   const QoreMethod *m = findLocalMethod(nme);
#ifdef DEBUG
   if (m)
      printd(5, "QoreClass::resolveSelfMethod(%s) resolved to %s::%s() %p\n", nme, getName(), nme, m);
#endif
   bool err = false;
   if (m && (m == priv->constructor || m == priv->destructor))
      err = true;

   // look in pending methods
   if (!m) {
      // pending methods are not set to the quick pointers, so we have to compare the strings...
      if (!strcmp(nme, "constructor") || !strcmp(nme, "destructor"))
	 err = true;
      else {
	 hm_method_t::iterator i = priv->hm_pending.find(nme);
	 if (i != priv->hm_pending.end()) {
	    m = i->second;
	    printd(5, "QoreClass::resolveSelfMethod(%s) resolved to pending method %s::%s() %p\n", nme, getName(), nme, m);
	 }
      }
   }

   // if not found check static methods
   if (!err && !m) {
      m = findLocalStaticMethod(nme);
      // check pending methods
      if (!m) {
	 hm_method_t::iterator i = priv->shm_pending.find(nme);
	 if (i != priv->shm_pending.end()) {
	    m = i->second;
	    printd(5, "QoreClass::resolveSelfMethod(%s) resolved to pending static method %s::%s() %p\n", nme, getName(), nme, m);
	 }
      }
#ifdef DEBUG
      else
	 printd(5, "QoreClass::resolveSelfMethod(%s) resolved to static %s::%s() %p\n", nme, getName(), nme, m);
#endif
   }

   // if still not found now look in superclass methods
   if (!err && !m && priv->scl) {
      m = priv->scl->resolveSelfMethod(nme);
#ifdef DEBUG
      if (m)
	 printd(5, "QoreClass::resolveSelfMethod(%s) resolved to <base class>::%s() %p\n", nme, nme, m);
#endif
   }

   if (err) {
      parse_error("explicit calls to ::%s() methods are not allowed", nme);
      m = 0;
   }
   else if (!m)
      parse_error("no method %s::%s() has been defined", priv->name ? priv->name : "<pending>", nme);

   return m;
}

const QoreMethod *QoreClass::resolveSelfMethod(NamedScope *nme) {
   // first find class
   QoreClass *qc = getRootNS()->parseFindScopedClassWithMethod(nme);
   if (!qc)
      return 0;

   // see if class is base class of this class
   if (qc != this && (!priv->scl || !priv->scl->sml.isBaseClass(qc))) {
      parse_error("'%s' is not a base class of '%s'", qc->getName(), priv->name ? priv->name : "<pending>");
      return 0;
   }

   const char *nstr = nme->getIdentifier();
   const QoreMethod *m = qc->findParseMethod(nstr);
   bool err = false;
   if (m && (m == priv->constructor || m == priv->destructor))
      err = true;

   // look in pending methods
   if (!m) {
      // pending methods are not set to the quick pointers, so we have to compare the strings...
      if (!strcmp(nstr, "constructor") || !strcmp(nstr, "destructor"))
	 err = true;
      else {
	 hm_method_t::iterator i = qc->priv->hm_pending.find(nstr);
	 if (i != qc->priv->hm_pending.end())
	    m = i->second;
      }
   }

   if (err) {
      parse_error("explicit calls to ::%s() methods are not allowed", nstr);
      m = 0;
   }
   else if (!m)
      parse_error("no method %s::%s() has been defined", qc->getName(), nstr);

   return m;
}

// for adding user-defined (qore language) methods to a class
void QoreClass::addMethod(QoreMethod *m) {
   printd(5, "QoreClass::addMethod(%p) %s.%s() (this=%p)\n", m, priv->name ? priv->name : "<pending>", m->getName(), this);

   m->assign_class(this);
   bool dst = !strcmp(m->getName(), "destructor");
   // check for illegal private constructor or destructor
   if (!strcmp(m->getName(), "constructor") || dst) {
      if (m->isPrivate())
	 parseException("ILLEGAL-PRIVATE-METHOD", "%s methods cannot be private", m->getName());
      if (m->isStatic())
	 parseException("ILLEGAL-STATIC-METHOD", "%s methods cannot be static", m->getName());
   }
   else if (m->isStatic() && priv->checkSpecialStaticIntern(m))
      parseException("ILLEGAL-STATIC-METHOD", "%s methods cannot be static", m->getName());

   // if the method already exists or the user is trying to define a user destructor on a system object
   // (system objects without explicit destructors have an implicit default system destructor that cannot be overridden)
   if ((!m->isStatic() && parseFindMethod(m->getName())) || (m->isStatic() && parseFindStaticMethod(m->getName())) || (priv->sys && dst)) {
      parse_error("%smethod '%s::%s()' has already been defined", m->isStatic() ? "static " : "", priv->name ? priv->name : "<pending>", m->getName());
      delete m;
   }
   else {
      // insert in appropriate pending list for parse init
      if (m->isStatic())
	 priv->shm_pending[m->getName()] = m;
      else
	 priv->hm_pending[m->getName()] = m;
   }
}

int QoreClass::parseAddBaseClassArgumentList(class BCAList *new_bcal) {
   // if the constructor is being defined after the class has already been initialized, then throw a parse exception
   if (numMethods()) {
      parse_error("constructors giving explicit arguments to base constructors must be defined when the class is defined");
      return -1;
   }
   else if (priv->bcal) {
      parse_error("a constructor with a base class argument list has already been defined");
      return -1;
   }
   priv->bcal = new_bcal;
   return 0;
}

// adds a builtin method to the class (duplicate checking is made in debug mode and causes an abort)
void QoreClass::addMethod(const char *nme, q_method_t m, bool priv_flag) {
   priv->addBuiltinMethod(new BuiltinNormalMethod(this, nme, m, QDOM_DEFAULT, 0), priv_flag, false);
}

void QoreClass::addMethodExtended(const char *nme, q_method_t m, bool priv_flag, int domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   const QoreTypeInfo **typeList = 0;
   const AbstractQoreNode **defaultArgList = 0;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   BuiltinMethod *b = new BuiltinNormalMethod(this, nme, m, domain, returnTypeInfo, num_params, typeList, defaultArgList);
   priv->addBuiltinMethod(b, priv_flag, false);
}

void QoreClass::addMethodExtendedList(const char *nme, q_method_t m, bool priv_flag, int domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, const QoreTypeInfo **typeList, const AbstractQoreNode **defaultArgList) {
   BuiltinMethod *b = new BuiltinNormalMethod(this, nme, m, domain, returnTypeInfo, num_params, typeList, defaultArgList);
   priv->addBuiltinMethod(b, priv_flag, false);
}

// adds a builtin method with the new generic calling convention to the class (duplicate checking is made in debug mode and causes an abort)
void QoreClass::addMethod2(const char *nme, q_method2_t m, bool priv_flag) {
   priv->addBuiltinMethod(new BuiltinNormalMethod2(this, nme, m, QDOM_DEFAULT, 0), priv_flag, true);
}

void QoreClass::addMethodExtended2(const char *nme, q_method2_t m, bool priv_flag, int domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   const QoreTypeInfo **typeList = 0;
   const AbstractQoreNode **defaultArgList = 0;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   BuiltinMethod *b = new BuiltinNormalMethod2(this, nme, m, domain, returnTypeInfo, num_params, typeList, defaultArgList);
   priv->addBuiltinMethod(b, priv_flag, true);
}

void QoreClass::addMethodExtendedList2(const char *nme, q_method2_t m, bool priv_flag, int domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, const QoreTypeInfo **typeList, const AbstractQoreNode **defaultArgList) {
   BuiltinMethod *b = new BuiltinNormalMethod2(this, nme, m, domain, returnTypeInfo, num_params, typeList, defaultArgList);
   priv->addBuiltinMethod(b, priv_flag, true);
}

// adds a builtin static method to the class
void QoreClass::addStaticMethod2(const char *nme, q_static_method2_t m, bool priv_flag) {
   BuiltinMethod *b = new BuiltinStaticMethod2(this, nme, m, QDOM_DEFAULT, 0);
   priv->addBuiltinStaticMethod(b, priv_flag, true);
}

void QoreClass::addStaticMethodExtended2(const char *nme, q_static_method2_t m, bool priv_flag, int domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   const QoreTypeInfo **typeList = 0;
   const AbstractQoreNode **defaultArgList = 0;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   BuiltinMethod *b = new BuiltinStaticMethod2(this, nme, m, domain, returnTypeInfo, num_params, typeList, defaultArgList);
   priv->addBuiltinStaticMethod(b, priv_flag, true);
}

void QoreClass::addStaticMethodExtendedList2(const char *nme, q_static_method2_t m, bool priv_flag, int domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, const QoreTypeInfo **typeList, const AbstractQoreNode **defaultArgList) {
   BuiltinMethod *b = new BuiltinStaticMethod2(this, nme, m, domain, returnTypeInfo, num_params, typeList, defaultArgList);
   priv->addBuiltinStaticMethod(b, priv_flag, true);
}

// adds a builtin static method to the class
void QoreClass::addStaticMethod(const char *nme, q_func_t m, bool priv_flag) {
   BuiltinMethod *b = new BuiltinStaticMethod(this, nme, m, QDOM_DEFAULT, 0);
   priv->addBuiltinStaticMethod(b, priv_flag, false);
}

void QoreClass::addStaticMethodExtended(const char *nme, q_func_t m, bool priv_flag, int domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   const QoreTypeInfo **typeList = 0;
   const AbstractQoreNode **defaultArgList = 0;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   BuiltinMethod *b = new BuiltinStaticMethod(this, nme, m, domain, returnTypeInfo, num_params, typeList, defaultArgList);
   priv->addBuiltinStaticMethod(b, priv_flag, false);
}

void QoreClass::addStaticMethodExtendedList(const char *nme, q_func_t m, bool priv_flag, int domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, const QoreTypeInfo **typeList, const AbstractQoreNode **defaultArgList) {
   BuiltinMethod *b = new BuiltinStaticMethod(this, nme, m, domain, returnTypeInfo, num_params, typeList, defaultArgList);
   priv->addBuiltinStaticMethod(b, priv_flag, false);
}

// sets a builtin function as constructor - no duplicate checking is made
void QoreClass::setConstructor(q_constructor_t m) {
   priv->addBuiltinConstructor(new BuiltinConstructor(this, m), false, false);
}

void QoreClass::setConstructorExtended(q_constructor_t m, bool priv_flag, int n_domain, unsigned num_params, ...) {
   const QoreTypeInfo **typeList = 0;
   const AbstractQoreNode **defaultArgList = 0;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }
   BuiltinMethod *b = new BuiltinConstructor(this, m, n_domain, num_params, typeList, defaultArgList);
   priv->addBuiltinConstructor(b, priv_flag, false);
}

void QoreClass::setConstructorExtendedList(q_constructor_t m, bool priv_flag, int n_domain, unsigned num_params, const QoreTypeInfo **typeList, const AbstractQoreNode **defaultArgList) {
   BuiltinMethod *b = new BuiltinConstructor(this, m, n_domain, num_params, typeList, defaultArgList);
   priv->addBuiltinConstructor(b, priv_flag, false);
}

// sets a builtin function as constructor - no duplicate checking is made
void QoreClass::setConstructor2(q_constructor2_t m) {
   priv->addBuiltinConstructor(new BuiltinConstructor2(this, m), false, true);
}

void QoreClass::setConstructorExtended2(q_constructor2_t m, bool priv_flag, int n_domain, unsigned num_params, ...) {
   const QoreTypeInfo **typeList = 0;
   const AbstractQoreNode **defaultArgList = 0;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }
   BuiltinMethod *b = new BuiltinConstructor2(this, m, n_domain, num_params, typeList, defaultArgList);
   priv->addBuiltinConstructor(b, priv_flag, false);
}

void QoreClass::setConstructorExtendedList2(q_constructor2_t m, bool priv_flag, int n_domain, unsigned num_params, const QoreTypeInfo **typeList, const AbstractQoreNode **defaultArgList) {
   BuiltinMethod *b = new BuiltinConstructor2(this, m, n_domain, num_params, typeList, defaultArgList);
   priv->addBuiltinConstructor(b, priv_flag, false);
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor(q_destructor_t m) {
   priv->sys = true;
   QoreMethod *o = new QoreMethod(this, new BuiltinDestructor(this, m));
   insertMethod(o);
   priv->destructor = o;
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor2(q_destructor2_t m) {
   priv->sys = true;
   QoreMethod *o = new QoreMethod(this, new BuiltinDestructor2(this, m), false, false, true);
   insertMethod(o);
   priv->destructor = o;
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy(q_copy_t m) {
   priv->sys = true;
   QoreMethod *o = new QoreMethod(this, new BuiltinCopy(this, m));
   insertMethod(o);
   priv->copyMethod = o;
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy2(q_copy2_t m) {
   priv->sys = true;
   QoreMethod *o = new QoreMethod(this, new BuiltinCopy2(this, m), false, false, true);
   insertMethod(o);
   priv->copyMethod = o;
}

// sets the delete_blocker function
void QoreClass::setDeleteBlocker(q_delete_blocker_t m) {
   priv->sys = true;
   QoreMethod *o = new QoreMethod(this, new BuiltinDeleteBlocker(this, m));
   insertMethod(o);
   priv->deleteBlocker = o;
   priv->has_delete_blocker = true;
}

QoreListNode *QoreClass::getMethodList() const {
   QoreListNode *l = new QoreListNode();

   for (hm_method_t::const_iterator i = priv->hm.begin(); i != priv->hm.end(); i++)
      l->push(new QoreStringNode(i->first));
   return l;
}

QoreListNode *QoreClass::getStaticMethodList() const {
   QoreListNode *l = new QoreListNode();

   for (hm_method_t::const_iterator i = priv->shm.begin(); i != priv->shm.end(); i++)
      l->push(new QoreStringNode(i->first));
   return l;
}

// one-time initialization
void QoreClass::initialize() {
   priv->initialize();
}

// initializes all user methods
void QoreClass::parseInit() {
   setParseClass(this);
   initialize();

   // initialize methods
   for (hm_method_t::iterator i = priv->hm_pending.begin(); i != priv->hm_pending.end(); i++) {
      if (!strcmp(i->second->getName(), "constructor"))
	 i->second->parseInitConstructor(priv->scl);
      else if (!strcmp(i->second->getName(), "destructor"))
	 i->second->parseInitDestructor();
      else if (!strcmp(i->second->getName(), "copy"))
	 i->second->parseInitCopy();
      else
	 i->second->parseInit();
   }

   // initialize static methods
   for (hm_method_t::iterator i = priv->shm_pending.begin(); i != priv->shm_pending.end(); i++) {
      i->second->parseInit();
   }

   if (priv->bcal) {
      if (!priv->scl) {
	 parse_error("base constructor arguments given for a class that has no parent classes");
      }
      delete priv->bcal;
      priv->bcal = 0;
   }
}

// commits all pending user methods and pending private members
void QoreClass::parseCommit() {
   priv->parseCommit();
}

void QoreClass::parseSetBaseClassList(class BCList *bcl) {
   assert(!priv->scl);
   priv->scl = bcl;
}

bool QoreClass::parseCheckHierarchy(const QoreClass *cls) const {
   if (cls == this)
      return true;

   return priv->scl ? priv->scl->parseCheckHierarchy(cls) : false;
}

#ifdef QORE_CLASS_SYNCHRONOUS
void QoreClass::setSynchronousClass() {
   assert(!priv->synchronous_class);

   priv->synchronous_class = true;
   priv->has_synchronous_in_hierarchy = true;
}

bool QoreClass::has_synchronous_in_hierarchy() const {
   return priv->has_synchronous_in_hierarchy;
}

bool QoreClass::is_synchronous_class() const {
   return priv->synchronous_class;
}
#endif

const QoreMethod *QoreClass::getConstructor() const {
   return priv->constructor;
}

const QoreMethod *QoreClass::getSystemConstructor() const {
   return priv->system_constructor;
}

const QoreMethod *QoreClass::getDestructor() const {
   return priv->destructor;
}

const QoreMethod *QoreClass::getCopyMethod() const {
   return priv->copyMethod;
}

const QoreMethod *QoreClass::getMemberGateMethod() const {
   return priv->memberGate;
}

const QoreMethod *QoreClass::getMethodGate() const {
   return priv->methodGate;
}

const QoreMethod *QoreClass::getMemberNotificationMethod() const {
   return priv->memberNotification;
}

const QoreTypeInfo *QoreClass::getTypeInfo() const {
   return &priv->typeInfo;
}

int QoreClass::parseCheckMemberAccess(const char *mem, const QoreTypeInfo *&memberTypeInfo) const {
   return priv->parseCheckMemberAccess(mem, memberTypeInfo);
}

int QoreClass::parseCheckInternalMemberAccess(const char *mem) const {
   return priv->parseCheckInternalMemberAccess(mem);
}

const QoreClass *QoreClass::parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&typeInfo, bool &priv_member) const {
   return priv->parseFindPublicPrivateMember(mem, typeInfo, priv_member);
}

bool QoreClass::parseHasPublicMembersInHierarchy() const {
   return priv->parseHasPublicMembersInHierarchy();
}

bool QoreClass::runtimeHasPublicMembersInHierarchy() const {
   return priv->has_public_members;
}

bool QoreClass::isPublicOrPrivateMember(const char *str, bool &priv_member) const {
   return priv->isPublicOrPrivateMember(str, priv_member);
}

int QoreClass::initMembers(QoreObject *o, ExceptionSink *xsink) const {
   return priv->initMembers(o, xsink);
}
