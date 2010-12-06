/*
  QoreClass.cpp

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
#include <qore/intern/ConstantList.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

// global class ID sequence
DLLLOCAL Sequence classIDSeq(1);

// FIXME: check private method variant access at runtime

struct SelfLocalVarParseHelper {
   DLLLOCAL SelfLocalVarParseHelper(LocalVar *selfid) { push_local_var(selfid); }
   DLLLOCAL ~SelfLocalVarParseHelper() { pop_local_var(); }
};

void raiseNonExistentMethodCallWarning(const QoreClass *qc, const char *method) {
   getProgram()->makeParseWarning(QP_WARN_NONEXISTENT_METHOD_CALL, "NON-EXISTENT-METHOD-CALL", "call to non-existant method '%s::%s()'; this call will be evaluated at run-time, so if the method is called on an object of a subclass that implements this method, then it could be a valid call, however in any other case it will result in a run-time exception.  To avoid seeing this warning, use the cast<> operator (note that if the cast is invalid at run-time, a run-time exception will be raised) or turn off the warning by using '%%disable-warning non-existent-method-call' in your code", qc->getName(), method);
}

struct qore_method_private {
   const QoreClass *parent_class;
   MethodFunctionBase *func;
   bool static_flag, all_user;

   DLLLOCAL qore_method_private(const QoreClass *n_parent_class, MethodFunctionBase *n_func, bool n_static) : parent_class(n_parent_class), func(n_func), static_flag(n_static), all_user(true) {
   }

   DLLLOCAL ~qore_method_private() {
      func->deref();
   }
   
   DLLLOCAL void setBuiltin() {
      all_user = false;
   }

   DLLLOCAL bool isUniquelyUser() const {
      return all_user;
   }

   DLLLOCAL int addUserVariant(MethodVariantBase *variant) {
      return func->parseAddUserMethodVariant(variant);
   }

   DLLLOCAL void addBuiltinVariant(MethodVariantBase *variant) {
      all_user = false;
      func->addBuiltinMethodVariant(variant);
   }

   DLLLOCAL MethodFunctionBase *getFunction() const {
      return const_cast<MethodFunctionBase *>(func);
   }

   DLLLOCAL const char *getName() const {
      return func->getName();
   }

   DLLLOCAL void parseInit() {
      assert(!static_flag);

      //printd(0, "qore_method_private::parseInit() this=%p %s::%s() func=%p\n", this, parent_class->getName(), func->getName(), func);

      const char *name = func->getName();

      func->parseInit();
      if (!strcmp(name, "constructor") 
	  && !strcmp(name, "destructor")
	  && !strcmp(name, "copy")) {

	 if ((!strcmp(name, "methodGate")
	      || !strcmp(name, "memberGate")
	      || !strcmp(name, "memberNotification"))
	     && !func->pendingEmpty()) {
	    // ensure that there is no more than one parameter declared, and if it
	    // has a type, it must be a string
	    UserSignature *sig = UMV(func->pending_first())->getUserSignature();
	    const QoreTypeInfo *t = sig->getParamTypeInfo(0);
	    if (!stringTypeInfo->parseAccepts(t)) {
	       QoreStringNode *desc = new QoreStringNode;
	       desc->sprintf("%s::%s(%s) has an invalid signature; the first argument declared as ", parent_class->getName(), func->getName(), sig->getSignatureText());
	       t->getThisType(*desc);
	       desc->concat(" is not compatible with 'string'");
	       getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	    }
	 }
      }
   }

   DLLLOCAL void parseInitStatic() {
      assert(static_flag);
      func->parseInit();
   }

   DLLLOCAL const QoreTypeInfo *getUniqueReturnTypeInfo() const {
      return func->getUniqueReturnTypeInfo();
   }

   DLLLOCAL void evalConstructor(const AbstractQoreFunctionVariant *variant, QoreObject *self, const QoreListNode *args, BCEAList *bceal, ExceptionSink *xsink) {
      CONMF(func)->evalConstructor(variant, *parent_class, self, args, parent_class->priv->scl, bceal, xsink);
   }

   DLLLOCAL void evalCopy(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const {
      // switch to new program for imported objects
      ProgramContextHelper pch(self->getProgram(), xsink);

      COPYMF(func)->evalCopy(*parent_class, self, old, parent_class->priv->scl, xsink);
   }

   DLLLOCAL bool evalDeleteBlocker(QoreObject *self) const {
      // can only be builtin
      return self->evalDeleteBlocker(parent_class->priv->methodID, reinterpret_cast<BuiltinDeleteBlocker *>(func));
   }

   DLLLOCAL void evalDestructor(QoreObject *self, ExceptionSink *xsink) const {
      // switch to new program for imported objects
      ProgramContextHelper pch(self->getProgram(), xsink);

      DESMF(func)->evalDestructor(*parent_class, self, xsink);
   }

   DLLLOCAL void evalSystemDestructor(QoreObject *self, ExceptionSink *xsink) const {
      // execute function directly
      DESMF(func)->evalDestructor(*parent_class, self, xsink);
   }

   DLLLOCAL void evalSystemConstructor(QoreObject *self, int code, va_list args) const {
      BSYSCONB(func)->eval(*parent_class, self, code, args);
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

qore_class_private::qore_class_private(QoreClass *n_cls, const char *nme, int64 dom, QoreTypeInfo *n_typeInfo) 
   : name(nme ? strdup(nme) : 0), 
     cls(n_cls),
     scl(0), 
     system_constructor(0),
     constructor(0),
     destructor(0),
     copyMethod(0),
     methodGate(0),
     memberGate(0),
     deleteBlocker(0),
     memberNotification(0),
     classID(classIDSeq.next()),
     methodID(classID),
     sys(false), 
     initialized(false), 
     parse_init_called(false),
     parse_init_partial_called(false),
     has_delete_blocker(false), 
     has_public_memdecl(false),
     pending_has_public_memdecl(false),
     owns_typeinfo(n_typeInfo ? false : true),
     resolve_copy_done(false),
     has_new_user_changes(false),
     owns_ornothingtypeinfo(false),
     domain(dom), 
     num_methods(0), 
     num_user_methods(0),
     num_static_methods(0), 
     num_static_user_methods(0),
     typeInfo(n_typeInfo ? n_typeInfo : new QoreTypeInfo(cls)), 
     orNothingTypeInfo(0),
     selfid("self", typeInfo), 
     ptr(0),
     new_copy(0) {
   assert(methodID == classID);
   printd(5, "qore_class_private::qore_class_private() creating '%s' ID:%d this=%p cls=%p\n", name ? name : "(null)", classID, this, cls);
}

// only called while the parse lock for the QoreProgram owning "old" is held
qore_class_private::qore_class_private(const qore_class_private &old, QoreClass *n_cls) 
   : name(strdup(old.name)), 
     cls(n_cls),
     scl(0), // parent class list must be copied after new_copy set in old
     pub_const(old.pub_const),
     priv_const(old.priv_const),
     system_constructor(old.system_constructor ? old.system_constructor->copy(cls) : 0),
     constructor(0), // method pointers set below when methods are copied
     destructor(0),
     copyMethod(0),
     methodGate(0),
     memberGate(0),
     deleteBlocker(old.deleteBlocker ? old.deleteBlocker->copy(cls) : 0),
     memberNotification(0),
     classID(old.classID),
     methodID(old.methodID),
     sys(old.sys), 
     initialized(true), 
     parse_init_called(false),
     parse_init_partial_called(false),
     has_public_memdecl(old.has_public_memdecl),
     pending_has_public_memdecl(false),
     owns_typeinfo(false),
     resolve_copy_done(false),
     has_new_user_changes(false),
     owns_ornothingtypeinfo(false),
     domain(old.domain), 
     num_methods(old.num_methods), 
     num_user_methods(old.num_user_methods),
     num_static_methods(old.num_static_methods), 
     num_static_user_methods(old.num_static_user_methods),
     typeInfo(old.typeInfo), 
     orNothingTypeInfo(old.orNothingTypeInfo),
     selfid(old.selfid), 
     ptr(old.ptr),
     new_copy(0) {
   QORE_TRACE("qore_class_private::qore_class_private(const qore_class_private &old)");
   printd(5, "qore_class_private::qore_class_private() creating copy of '%s' ID:%d this=%p cls=%p old=%p\n", name, classID, this, cls, old.cls);

   if (!old.initialized)
      const_cast<qore_class_private &>(old).initialize();
      
   // must set after old class has been initialized
   has_delete_blocker = old.has_delete_blocker; 

   // set pointer to new copy
   old.new_copy = cls;

   // copy parent class list, if any, after new_copy is set in old
   scl = old.scl ? new BCList(*old.scl) : 0;

   printd(5, "qore_class_private::qore_class_private() old name=%s (%p) new name=%s (%p)\n", old.name, old.name, name, name);

   // copy methods and maintain method pointers
   for (hm_method_t::const_iterator i = old.hm.begin(), e = old.hm.end(); i != e; ++i) {
      QoreMethod *nf = i->second->copy(cls);

      hm[nf->getName()] = nf;
      if (i->second == old.constructor)
	 constructor  = nf;
      else if (i->second == old.destructor)
	 destructor   = nf;
      else if (i->second == old.copyMethod)
	 copyMethod   = nf;
      else if (i->second == old.methodGate)
	 methodGate   = nf;
      else if (i->second == old.memberGate)
	 memberGate   = nf;
      else if (i->second == old.memberNotification)
	 memberNotification = nf;
   }
      
   // copy static methods
   for (hm_method_t::const_iterator i = old.shm.begin(), e = old.shm.end(); i != e; ++i) {
      QoreMethod *nf = i->second->copy(cls);
      shm[nf->getName()] = nf;
   }

   // copy private member list
   for (member_map_t::const_iterator i = old.private_members.begin(), e = old.private_members.end(); i != e; ++i)
      private_members[strdup(i->first)] = i->second->copy();

   // copy public member list
   for (member_map_t::const_iterator i = old.public_members.begin(), e = old.public_members.end(); i != e; ++i)
      public_members[strdup(i->first)] = i->second->copy();

   // copy private static var list
   for (var_map_t::const_iterator i = old.private_vars.begin(), e = old.private_vars.end(); i != e; ++i)
      private_vars[strdup(i->first)] = i->second->copy();

   // copy public static var list
   for (var_map_t::const_iterator i = old.public_vars.begin(), e = old.public_vars.end(); i != e; ++i)
      public_vars[strdup(i->first)] = i->second->copy();
}

qore_class_private::~qore_class_private() {
   printd(5, "qore_class_private::~qore_class_private() deleting %p %s\n", this, name ? name : "(null)");

   if (!private_vars.empty() || !public_vars.empty()) {
      ExceptionSink xsink;
   
      private_vars.del(&xsink);
      public_vars.del(&xsink);
   }

   for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      //printd(5, "QoreClass::~QoreClass() deleting method %p %s::%s()\n", m, name, m->getName());
      delete i->second;
   }      

   for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i) {
      //printd(5, "QoreClass::~QoreClass() deleting static method %p %s::%s()\n", m, name, m->getName());
      delete i->second;
   }

   free(name);
   delete scl;
   delete system_constructor;

   if (owns_typeinfo)
      delete typeInfo;

   if (owns_ornothingtypeinfo)
      delete orNothingTypeInfo;
}

void qore_class_private::initialize() {
   if (!initialized) {
      initialized = true;

      assert(name);
      printd(5, "QoreClass::initialize() %s class=%p scl=%p\n", name, cls, scl);

      // first resolve types in pending variants in all method signatures (incl. return types)
      for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
	 i->second->priv->func->resolvePendingSignatures();
      for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i)
	 i->second->priv->func->resolvePendingSignatures();

      if (scl)
	 scl->parseInit(cls, has_delete_blocker);

      const QoreProgram *pgm = getProgram();
      if (pgm && !sys && domain & pgm->getParseOptions())
	 parseException("ILLEGAL-CLASS-DEFINITION", "class '%s' inherits functionality from base classes that is restricted by current parse options", name);
   }
}

// returns a non-static method if it exists in the local class and has been committed to the class
QoreMethod *qore_class_private::findLocalCommittedMethod(const char *nme) {
   QoreMethod *m = parseFindLocalMethod(nme);
   return m && !m->priv->func->committedEmpty() ? m : 0;
}

// returns a non-static method if it exists in the local class and has been committed to the class
const QoreMethod *qore_class_private::findLocalCommittedMethod(const char *nme) const {
   const QoreMethod *m = parseFindLocalMethod(nme);
   return m && !m->priv->func->committedEmpty() ? m : 0;
}

// returns a static method if it exists in the local class and has been committed to the class
QoreMethod *qore_class_private::findLocalCommittedStaticMethod(const char *nme) {
   QoreMethod *m = parseFindLocalStaticMethod(nme);
   return m && !m->priv->func->committedEmpty() ? m : 0;
}

// returns a static method if it exists in the local class and has been committed to the class
const QoreMethod *qore_class_private::findLocalCommittedStaticMethod(const char *nme) const {
   const QoreMethod *m = parseFindLocalStaticMethod(nme);
   return m && !m->priv->func->committedEmpty() ? m : 0;
}

void qore_class_private::execBaseClassConstructor(QoreObject *self, BCEAList *bceal, ExceptionSink *xsink) const {
   // if there is no constructor, execute the superclass constructors directly
   if (!constructor){
      if (scl) // execute base class constructors if any
	 scl->execConstructors(self, bceal, xsink);

      // instantiate members after base constructors have been executed
      initMembers(self, xsink);
      return;
   }
   // no lock is sent with constructor, because no variable has been assigned yet
   bool already_executed;
   const AbstractQoreFunctionVariant *variant;
   QoreListNode *args = bceal->findArgs(cls->getID(), &already_executed, variant);
   if (!already_executed) {
      constructor->priv->evalConstructor(variant, self, args, bceal, xsink);
   }
}

QoreObject *qore_class_private::execConstructor(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const {
   // create new object
   QoreObject *self = new QoreObject(cls, getProgram());

   ReferenceHolder<BCEAList> bceal(scl ? new BCEAList : 0, xsink);

   printd(5, "qore_class_private::execConstructor() class=%p %s::constructor() o=%p variant=%p\n", cls, name, self, variant);

   // it's possible for constructor = 0 and variant != 0, when a class is instantiated to initialize a constant
   // and the matched variant is pending
   if (!constructor && !variant) {
      if (scl) { // execute superconstructors if any
	 CODE_CONTEXT_HELPER(CT_BUILTIN, "constructor", self, xsink);

	 scl->execConstructors(self, *bceal, xsink);
      }

      // instantiate members after base constructors have been executed
      if (!*xsink)
	 initMembers(self, xsink);
   }
   else {
      if (!constructor) {
	 hm_method_t::const_iterator i = hm.find("constructor");
	 assert(i != hm.end());
	 i->second->priv->evalConstructor(variant, self, args, *bceal, xsink);
      }
      else
	 constructor->priv->evalConstructor(variant, self, args, *bceal, xsink);
      printd(5, "qore_class_private::execConstructor() class=%p %s done\n", cls, name);
   }

   if (*xsink) {
      // instead of executing the destructors for the superclasses that were already executed we call QoreObject::obliterate()
      // which will clear out all the private data by running their dereference methods which must be OK
      self->obliterate(xsink);
      printd(5, "qore_class_private::execConstructor() this=%p %s::constructor() o=%p, exception in constructor, obliterating QoreObject and returning 0\n", this, name, self);
      return 0;
   }

   printd(5, "qore_class_private::execConstructor() this=%p %s::constructor() returning o=%p\n", this, name, self);
   return self;
}

void qore_class_private::parseCommit() {
   //printd(5, "qore_class_private::parseCommit() %s this=%p cls=%p hm.size=%d\n", name, this, cls, hm.size());
   if (parse_init_called)
      parse_init_called = false;

   if (parse_init_partial_called)
      parse_init_partial_called = false;
   
   if (has_new_user_changes) {
      // commit pending "normal" (non-static) method variants
      for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
	 bool is_new = i->second->priv->func->committedEmpty();
	 i->second->priv->func->parseCommitMethod();
	 if (is_new) {
	    checkAssignSpecial(i->second);
	    ++num_methods;
	    ++num_user_methods;
	 }
      }

      // commit pending static method variants
      for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i) {
	 bool is_new = i->second->priv->func->committedEmpty();
	 i->second->priv->func->parseCommitMethod();
	 if (is_new) {
	    ++num_static_methods;
	    ++num_static_user_methods;
	 }
      }

      {
	 // add all pending private members to real member list
	 member_map_t::iterator i = pending_private_members.begin();  
	 while (i != pending_private_members.end()) { 
	    //printd(5, "QoreClass::parseCommit() %s committing private member %p %s\n", name, j->first, j->first);
	    private_members[i->first] = i->second;
	    pending_private_members.erase(i);
	    i = pending_private_members.begin();
	 }
      }
   
      {
	 // add all pending public members to real member list
	 member_map_t::iterator i = pending_public_members.begin();  
	 while (i != pending_public_members.end()) { 
	    //printd(5, "QoreClass::parseCommit() %s committing public member %p %s\n", name, j->first, j->first);
	    public_members[i->first] = i->second;
	    pending_public_members.erase(i);
	    i = pending_public_members.begin();
	 }
      }

      // exceptions thrown when initializing static class variables
      // will be stored here and cannot be caught by user code
      ExceptionSink xsink;
      {
	 // add all pending private static vars to real list and initialize them
	 var_map_t::iterator i = pending_private_vars.begin();  
	 while (i != pending_private_vars.end()) { 
	    //printd(5, "QoreClass::parseCommit() %s committing private var %p %s\n", name, l->first, l->first);
	    private_vars[i->first] = i->second;
	    // initialize variable
	    initVar(i->first, *(i->second), &xsink);

	    pending_private_vars.erase(i);
	    i = pending_private_vars.begin();
	 }
      }
   
      {
	 // add all pending public static vars to real list and initialize them
	 var_map_t::iterator i = pending_public_vars.begin();  
	 while (i != pending_public_vars.end()) { 
	    //printd(5, "QoreClass::parseCommit() %s committing public var %p %s\n", name, j->first, j->first);
	    public_vars[i->first] = i->second;
	    // initialize variable
	    initVar(i->first, *(i->second), &xsink);

	    pending_public_vars.erase(i);
	    i = pending_public_vars.begin();
	 }
      }

      // set flags
      if (pending_has_public_memdecl) {
	 if (!has_public_memdecl)
	    has_public_memdecl = true;
	 pending_has_public_memdecl = false;
      }

      // commit pending constants
      priv_const.assimilate(&pend_priv_const);
      pub_const.assimilate(&pend_pub_const);

      has_new_user_changes = false;
   }
#ifdef DEBUG
   else {
      for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
	 assert(i->second->priv->func->pendingEmpty());
      for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i)
	 assert(i->second->priv->func->pendingEmpty());
      assert(pending_private_members.empty());
      assert(pending_public_members.empty());
      assert(pending_private_vars.empty());
      assert(pending_public_vars.empty());
      assert(!pending_has_public_memdecl);
   }
#endif

   // we check base classes if they have public members if we don't have any
   // it's safe to call parseHasPublicMembersInHierarchy() because the 2nd stage
   // of parsing has completed without any errors (or we wouldn't be
   // running parseCommit())
   if (!has_public_memdecl && (scl ? scl->parseHasPublicMembersInHierarchy() : false))
      has_public_memdecl = true;
}

void qore_class_private::addBuiltinMethod(const char *mname, MethodVariantBase *variant) {
   assert(strcmp(mname, "constructor"));
   assert(strcmp(mname, "destructor"));
   assert(strcmp(mname, "copy"));

   hm_method_t::iterator i = hm.find(mname);
   QoreMethod *nm;
   if (i == hm.end()) {
      MethodFunctionBase *m = new BuiltinNormalMethod(cls, mname);
      nm = new QoreMethod(cls, m, false);
      insertBuiltinMethod(nm, true);
   }
   else {
      nm = i->second;
   }

   // set the pointer from the variant back to the owning method
   variant->setMethod(nm);

   nm->priv->addBuiltinVariant(variant);
}

void qore_class_private::addBuiltinStaticMethod(const char *mname, MethodVariantBase *variant) {
   assert(strcmp(mname, "constructor"));
   assert(strcmp(mname, "destructor"));

   hm_method_t::iterator i = shm.find(mname);
   QoreMethod *nm;
   if (i == shm.end()) {
      MethodFunctionBase *m = new BuiltinStaticMethod(cls, mname);
      nm = new QoreMethod(cls, m, true);
      insertBuiltinStaticMethod(nm);
   }
   else {
      nm = i->second;
   }

   // set the pointer from the variant back to the owning method
   variant->setMethod(nm);

   nm->priv->addBuiltinVariant(variant);
}

void qore_class_private::addBuiltinConstructor(BuiltinConstructorVariantBase *variant) {
   QoreMethod *nm;
   if (!constructor) {
      MethodFunctionBase *m = new ConstructorMethodFunction(cls);
      nm = new QoreMethod(cls, m, false);
      constructor = nm;
      insertBuiltinMethod(nm, true);
   }
   else {
      nm = const_cast<QoreMethod *>(constructor);
   }

   // set the pointer from the variant back to the owning method
   variant->setMethod(nm);

   nm->priv->addBuiltinVariant(variant);
}

void qore_class_private::addBuiltinDestructor(BuiltinDestructorVariantBase *variant) {
   assert(!destructor);
   DestructorMethodFunction *m = new DestructorMethodFunction(cls);
   QoreMethod *qm = new QoreMethod(cls, m, false);
   destructor = qm;
   insertBuiltinMethod(qm, true);
   // set the pointer from the variant back to the owning method
   variant->setMethod(qm);

   qm->priv->addBuiltinVariant(variant);
}

void qore_class_private::addBuiltinCopyMethod(BuiltinCopyVariantBase *variant) {
   assert(!copyMethod);
   CopyMethodFunction *m = new CopyMethodFunction(cls);
   QoreMethod *qm = new QoreMethod(cls, m, false);
   copyMethod = qm;
   insertBuiltinMethod(qm, true);
   // set the pointer from the variant back to the owning method
   variant->setMethod(qm);

   qm->priv->addBuiltinVariant(variant);
}

void qore_class_private::setDeleteBlocker(q_delete_blocker_t func) {
   assert(!deleteBlocker);
   BuiltinDeleteBlocker *m = new BuiltinDeleteBlocker(func);
   QoreMethod *qm = new QoreMethod(cls, m, false);
   qm->priv->setBuiltin();
   deleteBlocker = qm;
   insertBuiltinMethod(qm, true);
   has_delete_blocker = true;
}

void qore_class_private::setBuiltinSystemConstructor(BuiltinSystemConstructorBase *m) {
   assert(!system_constructor);
   QoreMethod *qm = new QoreMethod(cls, m, false);
   qm->priv->setBuiltin();
   system_constructor = qm;
}

QoreListNode *BCEAList::findArgs(qore_classid_t classid, bool *aexeced, const AbstractQoreFunctionVariant *&variant) {
   bceamap_t::iterator i = find(classid);
   if (i != end()) {
      if (i->second->execed) {
	 *aexeced = true;
	 variant = 0;
	 return 0;
      }
      *aexeced = false;
      i->second->execed = true;
      variant = i->second->variant;
      return i->second->args;
   }

   insert(std::make_pair(classid, new BCEANode));
   *aexeced = false;
   variant = 0;
   return 0;
}

int BCEAList::add(qore_classid_t classid, const QoreListNode *arg, const AbstractQoreFunctionVariant *variant, ExceptionSink *xsink) {
   // see if class already exists in the list
   bceamap_t::iterator i = find(classid);
   if (i != end())
      return 0;

   // evaluate arguments
   ReferenceHolder<QoreListNode> nargs(arg ? arg->evalList(xsink) : 0, xsink);
   if (*xsink)
      return -1;

   // save arguments
   insert(std::make_pair(classid, new BCEANode(nargs.release(), variant)));
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

// resolves classes, parses arguments, and attempts to find constructor variant
void BCANode::parseInit(BCList *bcl, const char *classname) {
   QoreClass *sclass = 0;
   if (ns) {
      sclass = getRootNS()->parseFindScopedClass(ns);
      printd(5, "BCANode::parseInit() this=%p resolved named scoped %s -> %p\n", this, ns->ostr, sclass);
      delete ns;
      ns = 0;
   }
   else {
      sclass = getRootNS()->parseFindClass(name);
      printd(5, "BCANode::parseInit() this=%p resolved %s -> %p\n", this, name, sclass);
      free(name);
      name = 0;
   }

   if (sclass) {
      if (!bcl->match(sclass))
	 parse_error("%s in base constructor argument list is not a base class of %s", sclass->getName(), classname);
      else {
	 classid = sclass->getID();

	 // find constructor variant
	 const QoreMethod *m = sclass->getConstructor();
	 int lvids = 0;
	 const QoreTypeInfo *argTypeInfo;	 
	 if (m) {
	    lvids = parseArgsVariant(0, 0, m->getFunction(), argTypeInfo);
	 }
	 else {
	    if (args)
	       args = args->parseInitList(0, PF_REFERENCE_OK, lvids, argTypeInfo);
	 }
	 if (lvids) {
	    parse_error("illegal local variable declaration in base class constructor argument");
	    while (lvids--)
	       pop_local_var();
	 }
      }
   }
}

void BCNode::parseInit(QoreClass *cls, bool &has_delete_blocker) {
   if (!sclass) {
      if (cname) {
	 // if the class cannot be found, RootQoreNamespace::parseFindScopedClass() will throw the appropriate exception
	 sclass = getRootNS()->parseFindScopedClass(cname);
	 printd(5, "BCList::parseInit() %s inheriting %s (%p)\n", cls->getName(), cname->ostr, sclass);
	 delete cname;
	 cname = 0;
      }
      else {
	 // if the class cannot be found, RootQoreNamespace::parseFindClass() will throw the appropriate exception
	 sclass = getRootNS()->parseFindClass(cstr);
	 printd(5, "BCList::parseInit() %s inheriting %s (%p)\n", cls->getName(), cstr, sclass);
	 free(cstr);
	 cstr = 0;
      }
   }
   // recursively add base classes to special method list
   if (sclass) {
      sclass->initialize();
      if (!has_delete_blocker && sclass->has_delete_blocker())
	 has_delete_blocker = true;
      sclass->addBaseClassesToSubclass(cls, is_virtual);
      // include all subclass domains in this class' domain
      cls->addDomain(sclass->getDomain64());
   }
}

void BCList::parseInit(QoreClass *cls, bool &has_delete_blocker) {
   printd(5, "BCList::parseInit(%s) this=%p empty=%d\n", cls->getName(), this, empty());
   for (bclist_t::iterator i = begin(), e = end(); i != e; i++) {
      (*i)->parseInit(cls, has_delete_blocker);
   }

   // compare each class in the list to ensure that there are no duplicates
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 bclist_t::iterator j = i;
	 while (++j != end())
	    if ((*i)->sclass->getID() == (*j)->sclass->getID())
	       parse_error("class '%s' cannot inherit '%s' more than once", cls->getName(), (*i)->sclass->getName());
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
   
bool BCList::runtimeGetMemberInfo(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &priv) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if ((*i)->sclass && (*i)->sclass->priv->runtimeGetMemberInfo(mem, memberTypeInfo, priv))
	 return true;
   return false;
}

const QoreClass *BCList::parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &member_has_type_info, bool &priv) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 const QoreClass *qc = (*i)->sclass->parseFindPublicPrivateMember(mem, memberTypeInfo, member_has_type_info, priv);
	 if (qc)
	    return qc;
      }
   }
   return 0;
}

const QoreClass *BCList::parseFindPublicPrivateVar(const char *name, const QoreTypeInfo *&varTypeInfo, bool &has_type_info, bool &priv) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 const QoreClass *qc = qore_class_private::parseFindPublicPrivateVar((*i)->sclass, name, varTypeInfo, has_type_info, priv);
	 if (qc)
	    return qc;
      }
   }
   return 0;
}

// called at run time
const QoreMethod *BCList::findCommittedMethod(const char *name, bool &priv_flag) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 // this can be called before the class has been initialized if called by
	 // external code when adding builtin methods to the class
	 // assert that the base class list has already been initialized if it exists
	 //assert(!(*i)->sclass->priv->scl || ((*i)->sclass->priv->scl && (*i)->sclass->priv->initialized));

	 const QoreMethod *m;
	 if ((m = (*i)->sclass->priv->findCommittedMethod(name, priv_flag))) {
	    if ((*i)->priv)
	       priv_flag = true;
	    return m;
	 }
      }
   }
   return 0;
}

// called at parse time
const QoreMethod *BCList::parseFindCommittedMethod(const char *name) {
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->priv->parseFindCommittedMethod(name)))
	    return m;
      }
   }
   return 0;
}

const QoreMethod *BCList::parseFindMethodTree(const char *name) {
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->parseFindMethodTree(name)))
	    return m;
      }
   }
   return 0;
}

// called at run time
const QoreMethod *BCList::findCommittedStaticMethod(const char *name, bool &priv_flag) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 // this can be called before the class has been initialized if called by
	 // external code when adding builtin methods to the class
	 // assert that the base class list has already been initialized if it exists
	 //assert(!(*i)->sclass->priv->scl || ((*i)->sclass->priv->scl && (*i)->sclass->priv->initialized));

	 const QoreMethod *m;
	 if ((m = (*i)->sclass->priv->findCommittedStaticMethod(name, priv_flag))) {
	    if ((*i)->priv)
	       priv_flag = true;
	    return m;
	 }
      }
   }
   return 0;
}

/*
// called at parse time
const QoreMethod *BCList::parseFindCommittedStaticMethod(const char *name) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->priv->parseFindCommittedStaticMethod(name)))
	    return m;
      }
   }
   return 0;
}
*/

const QoreMethod *BCList::parseFindStaticMethodTree(const char *name) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->priv->parseFindStaticMethod(name)))
	    return m;
      }
   }
   return 0;
}

bool BCList::match(const QoreClass *cls) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if (cls == (*i)->sclass) {
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

const QoreMethod *BCList::parseResolveSelfMethod(const char *name) {
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 (*i)->sclass->initialize();
	 const QoreMethod *m;
	 if ((m = (*i)->sclass->parseResolveSelfMethodIntern(name)))
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

void BCList::execConstructors(QoreObject *o, BCEAList *bceal, ExceptionSink *xsink) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      printd(5, "BCList::execConstructors() %s::constructor() o=%p (for subclass %s) virtual=%d\n", (*i)->sclass->getName(), o, o->getClass()->getName(), (*i)->is_virtual); 

      // do not execute constructors for virtual base classes
      if ((*i)->is_virtual)
	 continue;
      (*i)->sclass->priv->execBaseClassConstructor(o, bceal, xsink);
      if (*xsink)
	 break;
   }
}

bool BCList::parseCheckHierarchy(const QoreClass *cls) const {
   for (bclist_t::const_iterator i = begin(); i != end(); ++i)
      if ((*i)->sclass->parseCheckHierarchy(cls))
	 return true;
   return false;
}

void BCList::addNewAncestors(QoreMethod *m) {
   AbstractQoreFunction *f = m->getFunction();
   const char *name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass *qc = (*i)->sclass;
      assert(qc);
      const QoreMethod *w = qc->priv->findLocalCommittedMethod(name);
      if (w)
	 f->addNewAncestor(w->getFunction());
      qc->priv->addNewAncestors(m);
   }
}

void BCList::addNewStaticAncestors(QoreMethod *m) {
   AbstractQoreFunction *f = m->getFunction();
   const char *name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass *qc = (*i)->sclass;
      assert(qc);
      const QoreMethod *w = qc->priv->findLocalCommittedStaticMethod(name);
      if (w)
	 f->addNewAncestor(w->getFunction());
      qc->priv->addNewStaticAncestors(m);
   }
}

void BCList::addAncestors(QoreMethod *m) {
   const char *name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass *qc = (*i)->sclass;
      assert(qc);
      const QoreMethod *w = qc->priv->findLocalCommittedMethod(name);
      if (w)
	 m->getFunction()->addAncestor(w->getFunction());

      qc->priv->addAncestors(m);
   }
}

void BCList::addStaticAncestors(QoreMethod *m) {
   const char *name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass *qc = (*i)->sclass;
      assert(qc);
      const QoreMethod *w = qc->priv->findLocalCommittedStaticMethod(name);
      if (w)
	 m->getFunction()->addAncestor(w->getFunction());
      qc->priv->addStaticAncestors(m);
   }
}

void BCList::parseAddAncestors(QoreMethod *m) {
   const char *name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      // if there was a parse error finding the base class, then skip
      QoreClass *qc = (*i)->sclass;
      if (!qc)
	 continue;

      const QoreMethod *w = qc->priv->parseFindLocalMethod(name);
      //printd(5, "BCList::parseAddAncestors(%p %s) this=%p qc=%p w=%p\n", m, m->getName(), this, qc, w);

      if (w)
	 m->getFunction()->addAncestor(w->getFunction());

      qc->priv->addAncestors(m);
   }
}

void BCList::parseAddStaticAncestors(QoreMethod *m) {
   const char *name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass *qc = (*i)->sclass;
      // qc may be 0 if there were a parse error with an unknown class earlier
      if (!qc)
	 continue;
      const QoreMethod *w = qc->priv->parseFindLocalStaticMethod(name);
      if (w)
	 m->getFunction()->addAncestor(w->getFunction());
      qc->priv->addStaticAncestors(m);
   }
}

void BCList::resolveCopy() {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      assert((*i)->sclass->priv->new_copy);
      (*i)->sclass = (*i)->sclass->priv->new_copy;
      (*i)->sclass->priv->resolveCopy();
   }

   sml.resolveCopy();
}

AbstractQoreNode *BCList::parseFindConstantValueIntern(const char *cname, const QoreTypeInfo *&typeInfo) {
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass *qc = (*i)->sclass;
      // qc may be 0 if there were a parse error with an unknown class earlier
      if (!qc)
	 continue;

      AbstractQoreNode *rv = qore_class_private::parseFindConstantValueIntern(qc, cname, typeInfo);
      if (rv)
	 return rv;
   }
   return 0;
}

QoreVarInfo *BCList::parseFindStaticVarIntern(const char *vname, const QoreClass *&qc) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      const QoreClass *nqc = (*i)->sclass;
      // qc may be 0 if there were a parse error with an unknown class earlier
      if (!nqc)
	 continue;

      QoreVarInfo *vi = nqc->priv->parseFindStaticVarIntern(vname, qc);
      if (vi)
	 return vi;
   }
   return 0;
}

int BCAList::execBaseClassConstructorArgs(BCEAList *bceal, ExceptionSink *xsink) const {
   for (bcalist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if (bceal->add((*i)->classid, (*i)->getArgs(), (*i)->getVariant(), xsink))
	 return -1;
   }
   return 0;
}

bool QoreClass::runtimeGetMemberInfo(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &priv_member) const {
   memberTypeInfo = 0;
   return priv->runtimeGetMemberInfo(mem, memberTypeInfo, priv_member);
}

const QoreMethod *QoreClass::parseGetConstructor() const {
   const_cast<QoreClass *>(this)->initialize();
   if (priv->constructor)
      return priv->constructor;
   return priv->parseFindLocalMethod("constructor");
}

const QoreMethod *QoreClass::parseFindLocalMethod(const char *name) const {
   return priv->parseFindLocalMethod(name);
}

bool QoreClass::has_delete_blocker() const {
   return priv->has_delete_blocker;
}

BCSMList *QoreClass::getBCSMList() const {
   return priv->scl ? &priv->scl->sml : 0;
}

const QoreMethod *QoreClass::findLocalStaticMethod(const char *nme) const {
   return priv->findLocalCommittedStaticMethod(nme);
}

const QoreMethod *QoreClass::findLocalMethod(const char *nme) const {
   return priv->findLocalCommittedMethod(nme);
}

// FIXME: make entry point only, called at run-time; grab program parse lock before accessing method maps
const QoreMethod *QoreClass::findStaticMethod(const char *nme) const {
   bool p = false;
   return priv->findStaticMethod(nme, p);
}

// FIXME: make entry point only, called at run-time; grab program parse lock before accessing method maps
const QoreMethod *QoreClass::findStaticMethod(const char *nme, bool &priv_flag) const {
   return priv->findStaticMethod(nme, priv_flag);
}

// FIXME: make entry point only, called at run-time; grab program parse lock before accessing method maps
const QoreMethod *QoreClass::findMethod(const char *nme) const {
   bool p = false;
   return priv->findMethod(nme, p);
}

// FIXME: make entry point only, called at run-time; grab program parse lock before accessing method maps
const QoreMethod *QoreClass::findMethod(const char *nme, bool &priv_flag) const {
   return priv->findMethod(nme, priv_flag);
}

const QoreExternalMethodVariant *QoreClass::findUserMethodVariant(const char *name, const QoreMethod *&method, const type_vec_t &argTypeList) const {
   return priv->findUserMethodVariant(name, method, argTypeList);
}

// only called when parsing
void QoreClass::setName(const char *n) {
   assert(!priv->name);
   assert(!priv->orNothingTypeInfo);

   priv->name = strdup(n);
   priv->orNothingTypeInfo = new OrNothingTypeInfo(*(priv->typeInfo), priv->name);
   //printd(5, "QoreClass::setName() this=%p %s\n", this, n);
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

bool QoreClass::hasMethodGate() const {
   return priv->methodGate != 0;
}

bool QoreClass::hasMemberNotification() const {
   return priv->memberNotification != 0;
}

int QoreClass::getDomain() const {
   return (int)priv->domain;
}

int64 QoreClass::getDomain64() const {
   return priv->domain;
}

const char *QoreClass::getName() const { 
   return priv->name; 
}

int QoreClass::numMethods() const {
   return priv->num_methods;
}

int QoreClass::numStaticMethods() const {
   return priv->num_static_methods;
}

int QoreClass::numUserMethods() const {
   return priv->num_user_methods;
}

int QoreClass::numStaticUserMethods() const {
   return priv->num_static_user_methods;
}

const QoreMethod *QoreClass::parseFindMethodTree(const char *nme) {
   initialize();
   return priv->parseFindMethod(nme);
}

const QoreMethod *QoreClass::parseFindStaticMethodTree(const char *nme) {
   initialize();
   return priv->parseFindStaticMethod(nme);
}

void QoreClass::addBuiltinBaseClass(QoreClass *qc, QoreListNode *xargs) {
   assert(!xargs);
   if (!priv->scl)
      priv->scl = new BCList;
   priv->scl->push_back(new BCNode(qc));
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
      priv->scl = new BCList;
   priv->scl->push_back(new BCNode(qc, true));   
}

// deletes all pending user methods
void QoreClass::parseRollback() {
   priv->parseRollback();
}

void qore_class_private::parseRollback() {
   if (parse_init_called)
      parse_init_called = false;
   
   if (parse_init_partial_called)
      parse_init_partial_called = false;
   
   if (!has_new_user_changes) {
      for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
	 assert(i->second->priv->func->pendingEmpty());
      for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i)
	 assert(i->second->priv->func->pendingEmpty());
      assert(!pending_has_public_memdecl);
      return;
   }

   // rollback pending "normal" (non-static) method variants
   for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e;) {
      // if there are no committed variants, then the method must be deleted
      if (i->second->priv->func->committedEmpty()) {
	 delete i->second;
	 hm.erase(i++);
	 continue;
      }

      i->second->priv->func->parseRollbackMethod();
      ++i;
   }

   // rollback pending static method variants
   for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e;) {
      // if there are no committed variants, then the method must be deleted
      if (i->second->priv->func->committedEmpty()) {
	 delete i->second;
	 hm.erase(i++);
	 continue;
      }

      i->second->priv->func->parseRollbackMethod();
      ++i;
   }

   // rollback pending constants
   pend_priv_const.parseDeleteAll();
   pend_pub_const.parseDeleteAll();

   // set flags
   if (pending_has_public_memdecl)
      pending_has_public_memdecl = false;

   has_new_user_changes = false;
}

QoreMethod::QoreMethod(const QoreClass *n_parent_class, MethodFunctionBase *n_func, bool n_static) : priv(new qore_method_private(n_parent_class, n_func, n_static)) {
}

QoreMethod::~QoreMethod() {
   delete priv;
}

MethodFunctionBase *QoreMethod::getFunction() const {
   return priv->getFunction();
}

// DEPRECATED
bool QoreMethod::newCallingConvention() const {
   return false;
}

bool QoreMethod::isUser() const {
   return priv->isUniquelyUser();
}

bool QoreMethod::isBuiltin() const {
   return !priv->isUniquelyUser();
}

bool QoreMethod::isPrivate() const { 
   return priv->func->isUniquelyPrivate();
}

bool QoreMethod::parseIsPrivate() const { 
   return priv->func->parseIsUniquelyPrivate();
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

const char *QoreMethod::getClassName() const {
   return priv->parent_class->getName();
}

void QoreMethod::assign_class(const QoreClass *p_class) {
   assert(!priv->parent_class);
   priv->parent_class = p_class;
}

// FIXME: DEPRECATED API non functional
bool QoreMethod::isSynchronized() const {
   return false;
}

// only called for ::methodGate() and ::memberGate() which cannot be overloaded
bool QoreMethod::inMethod(const QoreObject *self) const {
   return ::inMethod(priv->func->getName(), self);
}

QoreMethod *QoreMethod::copy(const QoreClass *p_class) const {
   return new QoreMethod(p_class, priv->func->copy(p_class), priv->static_flag);
}

const QoreTypeInfo *QoreMethod::getUniqueReturnTypeInfo() const {
   return priv->getUniqueReturnTypeInfo();
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

void QoreClass::addPublicMember(const char *name, const QoreTypeInfo *n_typeInfo, AbstractQoreNode *initial_value) {
   priv->addPublicMember(name, n_typeInfo, initial_value);
}

void QoreClass::addPrivateMember(const char *name, const QoreTypeInfo *n_typeInfo, AbstractQoreNode *initial_value) {
   priv->addPrivateMember(name, n_typeInfo, initial_value);
}

bool BCSMList::isBaseClass(QoreClass *qc) const {
   class_list_t::const_iterator i = begin();
   while (i != end()) {
      QoreClass *sc = (*i).first;
      printd(5, "BCSMList::isBaseClass() %p %s (%d) == %s (%d)\n", this, qc->getName(), qc->getID(), sc->getName(), sc->getID());
      if (qc->getID() == sc->getID() || (sc->priv->scl && sc->priv->scl->sml.isBaseClass(qc))) {
	 //printd(5, "BCSMList::isBaseClass() %p %s (%d) TRUE\n", this, qc->getName(), qc->getID());
	 return true;
      }
      ++i;
   }
   //printd(5, "BCSMList::isBaseClass() %p %s (%d) FALSE\n", this, qc->getName(), qc->getID());
   return false;
}

void BCSMList::addBaseClassesToSubclass(QoreClass *thisclass, QoreClass *sc, bool is_virtual) {
   //printd(5, "BCSMList::addBaseClassesToSubclass(this=%s, sc=%s) size=%d\n", thisclass->getName(), sc->getName());
   for (class_list_t::const_iterator i = begin(), e = end(); i != e; ++i)
      sc->priv->scl->sml.add(thisclass, (*i).first, is_virtual || (*i).second);
}

void BCSMList::add(QoreClass *thisclass, QoreClass *qc, bool is_virtual) {
   if (thisclass->getID() == qc->getID()) {
      parse_error("class '%s' cannot inherit itself", thisclass->getName());
      return;
   }

   // see if class already exists in list
   class_list_t::const_iterator i = begin();
   while (i != end()) {
      if ((*i).first->getID() == qc->getID())
         return;
      if ((*i).first->getID() == thisclass->getID()) {
      	 parse_error("circular reference in class hierarchy, '%s' is an ancestor of itself", thisclass->getName());
      	 return;
      }
      i++;
   }

   // append to the end of the list
   push_back(std::make_pair(qc, is_virtual));
}

void BCSMList::execDestructors(QoreObject *o, ExceptionSink *xsink) const {
   for (class_list_t::const_reverse_iterator i = rbegin(), e = rend(); i != e; ++i) {
      printd(5, "BCSMList::execDestructors() %s::destructor() o=%p virt=%s (subclass %s)\n", (*i).first->getName(), o, (*i).second ? "true" : "false", o->getClass()->getName());
      if (!(*i).second)
	 (*i).first->priv->execBaseClassDestructor(o, xsink);
   }
}

void BCSMList::execSystemDestructors(QoreObject *o, ExceptionSink *xsink) const {
   for (class_list_t::const_reverse_iterator i = rbegin(), e = rend(); i != e; ++i) {
      printd(5, "BCSMList::execSystemDestructors() %s::destructor() o=%p virt=%s (subclass %s)\n", (*i).first->getName(), o, (*i).second ? "true" : "false", o->getClass()->getName());
      if (!(*i).second)
	 (*i).first->priv->execBaseClassSystemDestructor(o, xsink);
   }
}

void BCSMList::execCopyMethods(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const {
   for (class_list_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if (!(*i).second) {
	 (*i).first->priv->execBaseClassCopy(self, old, xsink);
	 if (xsink->isEvent())
	    break;
      }
   }
}

QoreClass *BCSMList::getClass(qore_classid_t cid) const {
   for (class_list_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i).first->getID() == cid)
	 return (*i).first;
   }
   return 0;
}

void BCSMList::resolveCopy() {
   for (class_list_t::iterator i = begin(), e = end(); i != e; ++i) {
      assert((*i).first->priv->new_copy);
      (*i).first = (*i).first->priv->new_copy;
   }
}

QoreClass::QoreClass(const char *nme, int dom) : priv(new qore_class_private(this, nme, dom)) {
   priv->orNothingTypeInfo = new OrNothingTypeInfo(*(priv->typeInfo), nme);
   priv->owns_ornothingtypeinfo = true;
}

QoreClass::QoreClass(const char *nme, int64 dom, const QoreTypeInfo *typeInfo) {
   assert(typeInfo);
   priv = new qore_class_private(this, nme, dom, const_cast<QoreTypeInfo *>(typeInfo));

   printd(5, "QoreClass::QoreClass() this=%p creating '%s' with custom typeinfo\n", this, priv->name);

   // see if typeinfo already accepts NOTHING
   if (typeInfo->parseAcceptsReturns(NT_NOTHING))
      priv->orNothingTypeInfo = const_cast<QoreTypeInfo *>(typeInfo);
   else {
      if (!typeInfo->hasInputFilter()) {
	 priv->orNothingTypeInfo = new OrNothingTypeInfo(*typeInfo, nme);
	 priv->owns_ornothingtypeinfo = true;
      }
   }
}

QoreClass::QoreClass() : priv(new qore_class_private(this, 0)) {
   //priv->orNothingTypeInfo = new OrNothingTypeInfo(*(priv->typeInfo), );
   priv->owns_ornothingtypeinfo = true;
}

QoreClass::~QoreClass() {
   delete priv;
}

void QoreClass::setUserData(const void *n_ptr) {
   priv->setUserData(n_ptr);
}

const void *QoreClass::getUserData() const {
   return priv->getUserData();
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

AbstractQoreNode *QoreMethod::evalNormalVariant(QoreObject *self, const QoreExternalMethodVariant *ev, const QoreListNode *args, ExceptionSink *xsink) const {   
   const MethodVariantBase *variant = METHVB_const(ev);
   
   CodeEvaluationHelper ceh(xsink, getName(), args, variant->className());
   if (*xsink) return 0;

   if (ceh.processDefaultArgs(priv->func, variant, true))
      return 0;

   ceh.setCallType(variant->getCallType());
   ceh.setReturnTypeInfo(variant->getReturnTypeInfo());

   return METHV_const(variant)->evalMethod(self, ceh.getArgs(), xsink);      
}

AbstractQoreNode *QoreMethod::eval(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const {
   QORE_TRACE("QoreMethod::eval()");
#ifdef DEBUG
   const char *oname = self ? self->getClass()->getName() : "<n/a: static>";
   printd(5, "QoreMethod::eval() %s::%s() (object=%p, pgm=%p, static=%s)\n", oname, getName(), self, self ? self->getProgram() : 0, isStatic() ? "true" : "false");
#endif

   if (isStatic())
      return SMETHF(priv->func)->evalMethod(0, args, xsink);

   // switch to new program for imported objects
   ProgramContextHelper pch(self->getProgram(), xsink);

   AbstractQoreNode *rv = NMETHF(priv->func)->evalMethod(0, self, args, xsink);
   printd(5, "QoreMethod::eval() %s::%s() returning %p (type=%s, refs=%d)\n", oname, getName(), rv, rv ? rv->getTypeName() : "(null)", rv ? rv->reference_count() : 0);
   return rv;
}

bool QoreMethod::existsVariant(const type_vec_t &paramTypeInfo) const {
   return priv->func->existsVariant(paramTypeInfo);
}

QoreClass::QoreClass(const QoreClass &old) : priv(new qore_class_private(*old.priv, this)) {
}

void QoreClass::insertMethod(QoreMethod *m) {
   priv->insertBuiltinMethod(m);
}      

void QoreClass::insertStaticMethod(QoreMethod *m) {
   priv->insertBuiltinStaticMethod(m);
}      

void QoreClass::addDomain(int64 dom) {
   priv->domain |= dom;
}

AbstractQoreNode *QoreClass::evalMethod(QoreObject *self, const char *nme, const QoreListNode *args, ExceptionSink *xsink) const {
   QORE_TRACE("QoreClass::evalMethod()");

   if (!strcmp(nme, "copy"))
      return execCopy(self, xsink);

   bool external = (this != getStackClass());
   printd(5, "QoreClass::evalMethod() %s::%s() %s call attempted\n", priv->name, nme, external ? "external" : "internal" );

   const QoreMethod *w;

   bool priv_flag = false;
   // FIXME: check locking when accessing method maps at runtime
   if (!(w = findMethod(nme, priv_flag)) && !(w = findStaticMethod(nme, priv_flag))) {
      if (priv->methodGate && !priv->methodGate->inMethod(self)) // call methodGate with unknown method name and arguments
	 return evalMethodGate(self, nme, args, xsink);
      // otherwise return an exception
      xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() has been defined", priv->name, nme);
      return 0;
   }

   //printd(5, "QoreClass::evalMethod() %s::%s() found method %p class %s\n", priv->name, nme, w, w->getClassName());

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

   return w->eval(self, args, xsink);
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

QoreObject *QoreClass::execConstructor(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const {
   return priv->execConstructor(variant, args, xsink);
}

QoreObject *QoreClass::execConstructor(const QoreListNode *args, ExceptionSink *xsink) const {
   return priv->execConstructor(0, args, xsink);
}

QoreObject *qore_class_private::execSystemConstructor(QoreObject *self, int code, va_list args) const {
   assert(system_constructor);
   // no lock is sent with constructor, because no variable has been assigned yet
   system_constructor->priv->evalSystemConstructor(self, code, args);
   return self;
}

QoreObject *QoreClass::execSystemConstructor(int code, ...) const {
   va_list args;

   // create new object
   QoreObject *o = new QoreObject(this, 0);

   va_start(args, code);
   priv->execSystemConstructor(o, code, args);
   va_end(args);

   printd(5, "QoreClass::execSystemConstructor() %s::execSystemConstructor() returning %p\n", priv->name, o);
   return o;
}

bool QoreClass::execDeleteBlocker(QoreObject *self, ExceptionSink *xsink) const {
   return priv->execDeleteBlocker(self, xsink);
}

bool qore_class_private::execDeleteBlocker(QoreObject *self, ExceptionSink *xsink) const {
   printd(5, "qore_class_private::execDeleteBlocker(self=%p) this=%p '%s' has_delete_blocker=%s deleteBlocker=%p\n", self, this, name, has_delete_blocker ? "true" : "false", deleteBlocker);
   if (has_delete_blocker) {
      if (scl) // execute superclass delete blockers if any
	 if (scl->execDeleteBlockers(self, xsink))
	    return true;
      if (deleteBlocker) {
	 return deleteBlocker->priv->evalDeleteBlocker(self);
      }
   }
   return false;
}

void QoreClass::execDestructor(QoreObject *self, ExceptionSink *xsink) const {
   priv->execDestructor(self, xsink);
}

void qore_class_private::execDestructor(QoreObject *self, ExceptionSink *xsink) const {
   printd(5, "qore_class_private::execDestructor() %s::destructor() o=%p\n", name, self);

   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;

   if (self->isSystemObject()) {
      if (destructor) 
	 destructor->priv->evalSystemDestructor(self, &de);
      else
	 self->defaultSystemDestructor(classID, &de);

      // execute superclass destructors
      if (scl)
	 scl->sml.execSystemDestructors(self, &de);
   }
   else {
      if (destructor)
	 destructor->priv->evalDestructor(self, &de);
      else if (sys)
	 self->defaultSystemDestructor(classID, &de);

      // execute superclass destructors
      if (scl)
	 scl->sml.execDestructors(self, &de);
   }

   xsink->assimilate(&de);
}

void qore_class_private::execBaseClassDestructor(QoreObject *self, ExceptionSink *xsink) const {
   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;
   if (destructor)
      destructor->priv->evalDestructor(self, &de);
   else if (sys)
      self->defaultSystemDestructor(classID, &de);

   xsink->assimilate(&de);
}

void qore_class_private::execBaseClassSystemDestructor(QoreObject *self, ExceptionSink *xsink) const {
   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;
   if (destructor)
      destructor->priv->evalSystemDestructor(self, &de);
   else if (sys)
      self->defaultSystemDestructor(classID, &de);

   xsink->assimilate(&de);
}

void qore_class_private::execBaseClassCopy(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const {
   if (copyMethod)
      copyMethod->priv->evalCopy(self, old, xsink);
}

QoreObject *QoreClass::execCopy(QoreObject *old, ExceptionSink *xsink) const {
   return priv->execCopy(old, xsink);
}

QoreObject *qore_class_private::execCopy(QoreObject *old, ExceptionSink *xsink) const {
   // check for illegal private calls
   if (copyMethod && copyMethod->isPrivate() && cls != getStackClass()) {
      xsink->raiseException("METHOD-IS-PRIVATE", "%s::copy() is private and cannot be accessed externally", name);
      return 0;
   }

   QoreHashNode *h = old->copyData(xsink);
   if (*xsink) {
      assert(!h);
      return 0;
   }

   ReferenceHolder<QoreObject> self(new QoreObject(cls, getProgram(), h), xsink);

   if (copyMethod)
      copyMethod->priv->evalCopy(*self, old, xsink);
   else if (scl) // execute superclass copy methods
      scl->sml.execCopyMethods(*self, old, xsink);

   return *xsink ? 0 : self.release();
}

void QoreClass::addBaseClassesToSubclass(QoreClass *sc, bool is_virtual) {
   if (priv->scl)
      priv->scl->sml.addBaseClassesToSubclass(this, sc, is_virtual);
   sc->priv->scl->sml.add(sc, this, is_virtual);
}

// private, called from subclasses only
const QoreMethod *QoreClass::parseResolveSelfMethodIntern(const char *nme) {
   const QoreMethod *m = priv->parseFindLocalMethod(nme);
   if (!m)
      m = priv->parseFindLocalStaticMethod(nme);

   // if still not found now look in superclass methods
   if (!m && priv->scl)
      m = priv->scl->parseResolveSelfMethod(nme);

   return m;
}

// searches all methods, both pending and comitted
const QoreMethod *QoreClass::parseResolveSelfMethod(const char *nme) {
   initialize();
   const QoreMethod *m = parseResolveSelfMethodIntern(nme);

   if (!m) {
      parse_error("no method %s::%s() has been defined; if you want to make a call to a method that will be defined in an inherited class, then use $self.%s() instead", priv->name ? priv->name : "<pending>", nme, nme);
      return 0;
   }
   printd(5, "QoreClass::parseResolveSelfMethod(%s) resolved to %s::%s() %p (static=%d)\n", nme, getName(), nme, m, m->isStatic());

   const char *mname = m->getName();
   // make sure we're not calling a method that cannot be called directly
   if (!m->isStatic() && (!strcmp(mname, "constructor") || !strcmp(mname, "destructor") || !strcmp(mname, "copy"))) {
      parse_error("explicit calls to ::%s() methods are not allowed", nme);
      return 0;
   }

   return m;
}

const QoreMethod *QoreClass::parseResolveSelfMethod(NamedScope *nme) {
   // first find class
   QoreClass *qc = getRootNS()->parseFindScopedClassWithMethod(nme);
   if (!qc)
      return 0;

   // see if class is base class of this class
   if (qc != this && (!priv->scl || !priv->scl->sml.isBaseClass(qc))) {
      parse_error("'%s' is not a base class of '%s'", qc->getName(), priv->name ? priv->name : "<pending>");
      return 0;
   }

   return qc->parseResolveSelfMethod(nme->getIdentifier());
}

// for adding user-defined (qore language) methods to a class
int QoreClass::addUserMethod(const char *mname, MethodVariantBase *f, bool n_static) {
   return priv->addUserMethod(mname, f, n_static);
}

int qore_class_private::addUserMethod(const char *mname, MethodVariantBase *f, bool n_static) {
   // FIXME: set class name at parse time
   const char *tname = name ? name : "<pending>";
   printd(5, "QoreClass::addUserMethod(%s, umv=%p, priv=%d, static=%d) this=%p %s\n", mname, f, f->isPrivate(), n_static, this, tname);

   std::auto_ptr<MethodVariantBase> func(f);

   bool dst = !strcmp(mname, "destructor");
   bool con = dst ? false : !strcmp(mname, "constructor");

   // check for illegal static method
   if (n_static && (con || dst || checkSpecialStaticIntern(mname))) {
      parseException("ILLEGAL-STATIC-METHOD", "%s methods cannot be static", mname);
      return -1;
   }

   bool cpy = dst || con ? false : !strcmp(mname, "copy");
   // check for illegal method overloads
   if (sys && (con || cpy)) {
      parseException("ILLEGAL-METHOD-OVERLOAD", "class %s is builtin; %s methods in builtin classes cannot be overloaded; create a subclass instead", name, mname);
      return -1;
   }

   // set flags for other special methods
   bool methGate, memGate, hasMemberNotification;
   if (dst || con || cpy)
      methGate = memGate = hasMemberNotification = false;
   else {
      methGate = !strcmp(mname, "methodGate");
      memGate = methGate ? false : !strcmp(mname, "memberGate");
      hasMemberNotification = methGate || memGate ? false : !strcmp(mname, "memberNotification");
   }

   QoreMethod *m = const_cast<QoreMethod *>(!n_static ? parseFindMethod(mname) : parseFindStaticMethod(mname));
   if (!n_static && m && (dst || cpy || methGate || memGate || hasMemberNotification)) {
      parseException("ILLEGAL-METHOD-OVERLOAD", "a %s::%s() method has already been defined; cannot overload %s methods", tname, mname, mname);
      return -1;
   }

   // now we add the new variant to a method, creating the method if necessary

   if (!has_new_user_changes)
      has_new_user_changes = true;

   bool is_new = false;
   // if the method does not exist, then create it
   if (!m) {
      is_new = true;
      MethodFunctionBase *mfb;
      if (con) {
	 mfb = new ConstructorMethodFunction(cls);
	 // set selfid immediately if adding a cosntructor variant
	 reinterpret_cast<UserConstructorVariant *>(f)->getUserSignature()->setSelfId(&selfid);
      }
      else if (dst)
	 mfb = new DestructorMethodFunction(cls);
      else if (cpy)
	 mfb = new CopyMethodFunction(cls);
      else if (n_static)
	 mfb = new StaticUserMethod(cls, mname);
      else
	 mfb = new NormalUserMethod(cls, mname);

      m = new QoreMethod(cls, mfb, n_static);
   }

   // add this variant to the method
   if (m->priv->addUserVariant(func.release())) {      
      if (is_new)
	 delete m;
      return -1;
   }

   // set the pointer from the variant back to the owning method
   f->setMethod(m);

   // add the new method to the class if it's a new method
   if (is_new) {
      if (n_static) {
	 shm[m->getName()] = m;
      }
      else {
	 hm[m->getName()] = m;
      }
   }
   return 0;
}

// adds a builtin method to the class (duplicate checking is made in debug mode and causes an abort)
void QoreClass::addMethod(const char *nme, q_method_t m, bool priv_flag) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethodVariant(m, priv_flag));
}

void QoreClass::addMethodExtended(const char *nme, q_method_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinNormalMethodVariant(m, priv_flag, flags, domain, returnTypeInfo, typeList, defaultArgList));
}

void QoreClass::addMethodExtendedList(const char *nme, q_method_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethodVariant(m, priv_flag, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

// adds a builtin method with the new generic calling convention to the class (duplicate checking is made in debug mode and causes an abort)
void QoreClass::addMethod2(const char *nme, q_method2_t m, bool priv_flag) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethod2Variant(m, priv_flag));
}

void QoreClass::addMethodExtended2(const char *nme, q_method2_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinNormalMethod2Variant(m, priv_flag, flags, domain, returnTypeInfo, typeList, defaultArgList));
}

void QoreClass::addMethodExtendedList2(const char *nme, q_method2_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethod2Variant(m, priv_flag, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

void QoreClass::addMethodExtendedList3(const void *ptr, const char *nme, q_method3_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethod3Variant(ptr, m, priv_flag, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

// adds a builtin static method to the class
void QoreClass::addStaticMethod2(const char *nme, q_static_method2_t m, bool priv_flag) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethod2Variant(m, priv_flag));
}

void QoreClass::addStaticMethodExtended2(const char *nme, q_static_method2_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethod2Variant(m, priv_flag, flags, domain, returnTypeInfo, typeList, defaultArgList));
}

void QoreClass::addStaticMethodExtendedList2(const char *nme, q_static_method2_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethod2Variant(m, priv_flag, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

void QoreClass::addStaticMethodExtendedList3(const void *ptr, const char *nme, q_static_method3_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethod3Variant(ptr, m, priv_flag, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

// adds a builtin static method to the class
void QoreClass::addStaticMethod(const char *nme, q_func_t m, bool priv_flag) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethodVariant(m, priv_flag));
}

void QoreClass::addStaticMethodExtended(const char *nme, q_func_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethodVariant(m, priv_flag, flags, domain, returnTypeInfo, typeList, defaultArgList));
}

void QoreClass::addStaticMethodExtendedList(const char *nme, q_func_t m, bool priv_flag, int64 n_flags, int64 n_domain, const QoreTypeInfo *n_returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethodVariant(m, priv_flag, n_flags, n_domain, n_returnTypeInfo, n_typeList, n_defaultArgList));
}

// sets a builtin function as constructor - no duplicate checking is made
void QoreClass::setConstructor(q_constructor_t m) {
   priv->addBuiltinConstructor(new BuiltinConstructorVariant(m, false));
}

void QoreClass::setConstructorExtended(q_constructor_t m, bool priv_flag, int64 n_flags, int64 n_domain, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }
   priv->addBuiltinConstructor(new BuiltinConstructorVariant(m, priv_flag, n_flags, n_domain, typeList, defaultArgList));
}

void QoreClass::setConstructorExtendedList(q_constructor_t m, bool priv_flag, int64 n_flags, int64 n_domain, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinConstructor(new BuiltinConstructorVariant(m, priv_flag, n_flags, n_domain, n_typeList, n_defaultArgList));
}

// sets a builtin function as constructor - no duplicate checking is made
void QoreClass::setConstructor2(q_constructor2_t m) {
   priv->addBuiltinConstructor(new BuiltinConstructor2Variant(m, false));
}

void QoreClass::setConstructorExtended2(q_constructor2_t m, bool priv_flag, int64 n_flags, int64 n_domain, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }
   priv->addBuiltinConstructor(new BuiltinConstructor2Variant(m, priv_flag, n_flags, n_domain, typeList, defaultArgList));
}

void QoreClass::setConstructorExtendedList2(q_constructor2_t m, bool priv_flag, int64 n_flags, int64 n_domain, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinConstructor(new BuiltinConstructor2Variant(m, priv_flag, n_flags, n_domain, n_typeList, n_defaultArgList));
}

void QoreClass::setConstructorExtendedList3(const void *ptr, q_constructor3_t m, bool priv_flag, int64 n_flags, int64 n_domain, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinConstructor(new BuiltinConstructor3Variant(ptr, m, priv_flag, n_flags, n_domain, n_typeList, n_defaultArgList));
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor(q_destructor_t m) {
   priv->addBuiltinDestructor(new BuiltinDestructorVariant(m));
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor2(q_destructor2_t m) {
   priv->addBuiltinDestructor(new BuiltinDestructor2Variant(m));
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor3(const void *ptr, q_destructor3_t m) {
   priv->addBuiltinDestructor(new BuiltinDestructor3Variant(ptr, m));
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy(q_copy_t m) {
   priv->addBuiltinCopyMethod(new BuiltinCopyVariant(this, m));
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy2(q_copy2_t m) {
   priv->addBuiltinCopyMethod(new BuiltinCopy2Variant(this, m));
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy3(const void *ptr, q_copy3_t m) {
   priv->addBuiltinCopyMethod(new BuiltinCopy3Variant(ptr, this, m));
}

// sets the delete_blocker function
void QoreClass::setDeleteBlocker(q_delete_blocker_t m) {
   priv->setDeleteBlocker(m);
}

void QoreClass::setSystemConstructor(q_system_constructor_t m) {
   priv->setBuiltinSystemConstructor(new BuiltinSystemConstructor(this, m));
}

void QoreClass::setSystemConstructor2(q_system_constructor2_t m) {
   priv->setBuiltinSystemConstructor(new BuiltinSystemConstructor2(this, m));
}

QoreListNode *QoreClass::getMethodList() const {
   QoreListNode *l = new QoreListNode();

   for (hm_method_t::const_iterator i = priv->hm.begin(), e = priv->hm.end(); i != e; ++i)
      l->push(new QoreStringNode(i->first));
   return l;
}

QoreListNode *QoreClass::getStaticMethodList() const {
   QoreListNode *l = new QoreListNode();

   for (hm_method_t::const_iterator i = priv->shm.begin(), e = priv->shm.end(); i != e; ++i)
      l->push(new QoreStringNode(i->first));
   return l;
}

// one-time initialization
void QoreClass::initialize() {
   priv->initialize();
}

// initializes all user methods
void QoreClass::parseInit() {
   priv->parseInit();
}

void QoreClass::parseInitPartial() {
   priv->parseInitPartial();
}

void qore_class_private::parseInitPartial() {
   if (parse_init_partial_called)
      return;

   parse_init_partial_called = true;

   QoreParseClassHelper qpch(cls);
   initialize();

   if (!has_new_user_changes)
      return;

   // setup inheritance list for new methods
   for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      bool is_new = i->second->priv->func->committedEmpty();
      if (is_new) {
	 if (!checkSpecial(i->second->getName()))
	    parseAddAncestors(i->second);
      }
   }

   // setup inheritance list for new static methods
   for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i) {
      bool is_new = i->second->priv->func->committedEmpty();
      if (is_new)
	 parseAddStaticAncestors(i->second);
   }

   {
      SelfLocalVarParseHelper slvph(&selfid);
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

      // initialize new private static vars
      for (var_map_t::iterator i = pending_private_vars.begin(), e = pending_private_vars.end(); i != e; ++i) {
	 if (i->second)
	    i->second->parseInit(i->first, true);
      }
      
      // initialize new public static vars
      for (var_map_t::iterator i = pending_public_vars.begin(), e = pending_public_vars.end(); i != e; ++i) {
	 if (i->second)
	    i->second->parseInit(i->first, false);
      }
   }
   
   // check new members for conflicts in base classes
   for (member_map_t::iterator i = pending_private_members.begin(), e = pending_private_members.end(); i != e; ++i) {
      parseCheckMemberInBaseClasses(i->first, i->second->parseHasTypeInfo(), true);
   }
   
   for (member_map_t::iterator i = pending_public_members.begin(), e = pending_public_members.end(); i != e; ++i) {
      parseCheckMemberInBaseClasses(i->first, i->second->parseHasTypeInfo(), false);
   }
}

void qore_class_private::parseInit() {
   if (parse_init_called)
      return;

   parse_init_called = true;

   if (!has_new_user_changes)
      return;

   parseInitPartial();

   QoreParseClassHelper qpch(cls);

   // initialize constants
   pend_priv_const.parseInit();
   pend_pub_const.parseInit();

   // initialize methods
   for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      i->second->priv->parseInit();
   }

   // initialize static methods
   for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i) {
      i->second->priv->parseInitStatic();
   }
}

void qore_class_private::recheckBuiltinMethodHierarchy() {
   initialize();

   if (!scl)
      return;
      
   for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      scl->addNewAncestors(i->second);

   for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i)
      scl->addNewStaticAncestors(i->second);
}

const QoreExternalMethodVariant *qore_class_private::findUserMethodVariant(const char *mname, const QoreMethod *&method, const type_vec_t &argTypeList) const {
   bool p = false;
   method = findMethod(mname, p);
   if (!method)
      return 0;
   // make sure it's not a special method
   if (method == constructor
       || method == destructor
       || method == copyMethod) {
#ifdef DEBUG
      printd(0, "cannot call QoreClass::findUserMethodVariant() with special methods like '%s'\n", mname);
      assert(false);
#endif
      return 0;
   }
   return reinterpret_cast<const QoreExternalMethodVariant *>(method ? method->priv->func->runtimeFindVariant(argTypeList, true) : 0);
}

void qore_class_private::resolveCopy() {
   if (resolve_copy_done)
      return;

   resolve_copy_done = true;

   // resolve inheritance lists in methods
   for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      i->second->priv->func->resolveCopy();

   for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i)
      i->second->priv->func->resolveCopy();

   if (scl)
      scl->resolveCopy();
}

bool QoreClass::hasParentClass() const {
   return (bool)priv->scl;
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
   return priv->getTypeInfo();
}

const QoreTypeInfo *QoreClass::getOrNothingTypeInfo() const {
   return priv->getOrNothingTypeInfo();
}

int QoreClass::parseCheckMemberAccess(const char *mem, const QoreTypeInfo *&memberTypeInfo, int pflag) const {
   return priv->parseCheckMemberAccess(mem, memberTypeInfo, pflag);
}

int QoreClass::parseCheckInternalMemberAccess(const char *mem, const QoreTypeInfo *&memberTypeInfo) const {
   return priv->parseCheckInternalMemberAccess(mem, memberTypeInfo);
}

const QoreClass *QoreClass::parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &member_has_type_info, bool &priv_member) const {
   return priv->parseFindPublicPrivateMember(mem, memberTypeInfo, member_has_type_info, priv_member);
}

bool QoreClass::parseHasPublicMembersInHierarchy() const {
   return priv->parseHasPublicMembersInHierarchy();
}

bool QoreClass::runtimeHasPublicMembersInHierarchy() const {
   return priv->has_public_memdecl;
}

void QoreClass::parseSetEmptyPublicMemberDeclaration() {   
   priv->pending_has_public_memdecl = true;
   priv->has_new_user_changes = true;
}

bool QoreClass::isPublicOrPrivateMember(const char *str, bool &priv_member) const {
   return priv->isPublicOrPrivateMember(str, priv_member);
}

int QoreClass::initMembers(QoreObject *o, ExceptionSink *xsink) const {
   return priv->initMembers(o, xsink);
}

bool QoreClass::hasPrivateCopyMethod() const {
   return priv->copyMethod && priv->copyMethod->isPrivate() ? true : false;
}

bool QoreClass::parseHasPrivateCopyMethod() const {
   return priv->copyMethod && priv->copyMethod->parseIsPrivate() ? true : false;
}

bool QoreClass::parseHasMethodGate() const {
   return priv->parseHasMethodGate();
}

void QoreClass::recheckBuiltinMethodHierarchy() {
   priv->recheckBuiltinMethodHierarchy();
}

void QoreClass::unsetPublicMemberFlag() {
   assert(priv->has_public_memdecl);
   priv->has_public_memdecl = false;
}

void QoreClass::resolveCopy() {
   priv->resolveCopy();
}

void QoreClass::parseAssimilatePublicConstants(ConstantList &cmap) {
   priv->parseAssimilatePublicConstants(cmap);
}

void QoreClass::parseAssimilatePrivateConstants(ConstantList &cmap) {
   priv->parseAssimilatePrivateConstants(cmap);
}

/*
AbstractQoreNode *QoreClass::getConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) {
   return priv->getConstantValue(cname, typeInfo);
}
*/

void QoreClass::parseAddPublicConstant(const std::string &cname, AbstractQoreNode *val) {
   priv->parseAddPublicConstant(cname, val);
}

void QoreClass::addBuiltinConstant(const char *name, AbstractQoreNode *value, bool is_priv, const QoreTypeInfo *typeInfo) {
   priv->addBuiltinConstant(name, value, is_priv, typeInfo);
}

void QoreClass::addBuiltinStaticVar(const char *name, AbstractQoreNode *value, bool is_priv, const QoreTypeInfo *typeInfo) {
   priv->addBuiltinStaticVar(name, value, is_priv, typeInfo);
}

void MethodFunctionBase::addBuiltinMethodVariant(MethodVariantBase *variant) {
   if (all_private && !variant->isPrivate())
      all_private = false;
   addBuiltinVariant(variant);
}

int MethodFunctionBase::parseAddUserMethodVariant(MethodVariantBase *variant) {
   int rc = parseAddVariant(variant);
   if (!rc && pending_all_private && !variant->isPrivate())
      pending_all_private = false;
   return rc;
}

void MethodFunctionBase::parseCommitMethod() {
   parseCommit();
   if (all_private && !pending_all_private)
      all_private = false;
   pending_all_private = true;
}

void MethodFunctionBase::parseRollbackMethod() {
   parseRollback();
   pending_all_private = true;
}

void NormalMethodFunction::parseInit() {
   if (parse_init_done)
      return;
   parse_init_done = true;

   //printd(5, "NormalUserMethod::parseInitMethod() this=%p %s::%s() static_flag=%d\n", this, getClassName(), getName(), static_flag);
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      UserMethodVariant *v = UMV(*i);
      v->parseInitMethod(*qc, false);

      // recheck types against committed types if necessary
      if (v->getRecheck())
	 parseCheckDuplicateSignatureCommitted(v);
   }
}

void StaticMethodFunction::parseInit() {
   if (parse_init_done)
      return;
   parse_init_done = true;

   //printd(5, "StaticUserMethod::parseInitMethod() this=%p %s::%s() static_flag=%d\n", this, getClassName(), getName(), static_flag);
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      UserMethodVariant *v = UMV(*i);
      v->parseInitMethod(*qc, true);

      // recheck types against committed types if necessary
      if (v->getRecheck())
	 parseCheckDuplicateSignatureCommitted(v);
   }
}

void ConstructorMethodFunction::parseInit() {
   if (parse_init_done)
      return;
   parse_init_done = true;

   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      UserConstructorVariant *v = UCONV(*i);
      v->parseInitConstructor(*qc, qc->priv->scl);

      // recheck types against committed types if necessary
      if (v->getRecheck())
	 parseCheckDuplicateSignatureCommitted(v);
   }
}

void DestructorMethodFunction::parseInit() {
   if (parse_init_done)
      return;
   parse_init_done = true;

   // there can be only one destructor variant
   assert(!pending_vlist.plural());

   if (pending_vlist.empty())
      return;

   UserDestructorVariant *v = UDESV(pending_first());
   assert(!v->getRecheck());
   v->parseInitDestructor(*qc);
}

void CopyMethodFunction::parseInit() {
   if (parse_init_done)
      return;
   parse_init_done = true;

   // there can be only one copy method variant
   assert(!pending_vlist.plural());

   if (pending_vlist.empty())
      return;

   UserCopyVariant *v = UCOPYV(pending_first());
   assert(!v->getRecheck());
   v->parseInitCopy(*qc);
}

int ConstructorMethodVariant::constructorPrelude(const QoreClass &thisclass, CodeEvaluationHelper &ceh, QoreObject *self, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const {
   if (bcl) {
      const BCAList *bcal = getBaseClassArgumentList();
      if (bcal) {
	 bcal->execBaseClassConstructorArgs(bceal, xsink);
	 if (*xsink)
	    return -1;
      }
      bcl->execConstructors(self, bceal, xsink);
      if (*xsink)
	 return -1;
   }

   // initialize members
   if (thisclass.initMembers(self, xsink))
      return -1;

   ceh.restorePosition();
   return 0;
}

UserConstructorVariant::~UserConstructorVariant() {
   delete bcal;
}

void UserConstructorVariant::parseInitConstructor(const QoreClass &parent_class, BCList *bcl) {
   signature.resolve();
   assert(!signature.getReturnTypeInfo() || signature.getReturnTypeInfo() == nothingTypeInfo);

   // push return type on stack (no return value can be used)
   ParseCodeInfoHelper rtih("constructor", nothingTypeInfo);

   if (bcal && !parent_class.hasParentClass()) {
      parse_error("base constructor arguments given for class '%s' that has no parent classes", parent_class.getName());
      delete bcal;
      bcal = 0;
   }

   //printd(5, "UserConstructorVariant::parseInitConstructor() this=%p %s::constructor() params=%d\n", this, parent_class.getName(), signature.numParams());
   // must be called even if statements is NULL
   statements->parseInitConstructor(parent_class.getTypeInfo(), this, bcal, bcl);
}

void UserCopyVariant::evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink *xsink) const {
   // there can only be max 1 param
   assert(signature.numParams() <= 1);

   ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
   args->push(self->refSelf());

   UserVariantExecHelper uveh(this, *args, xsink);
   if (!uveh)
      return;

   CODE_CONTEXT_HELPER(CT_USER, "copy", self, xsink);

   if (scl) {
      scl->sml.execCopyMethods(self, old, xsink);
      if (*xsink)
	 return;
      ceh.restorePosition();
   }
   
   discard(evalIntern(uveh.getArgv(), self, xsink, thisclass.getName()), xsink);
}

void UserCopyVariant::parseInitCopy(const QoreClass &parent_class) {
   signature.resolve();

   // make sure there is max one parameter in the copy method      
   if (signature.numParams() > 1)
      parse_error("maximum of one parameter may be defined in class copy methods (%d defined); this parameter will be assigned to the old object when the method is executed", signature.numParams());

   // push return type on stack (no return value can be used)
   ParseCodeInfoHelper rtih("copy", nothingTypeInfo);
   
   // must be called even if statements is NULL
   statements->parseInitMethod(parent_class.getTypeInfo(), this);
   
   // see if there is a type specification for the sole parameter and make sure it matches the class if there is
   if (signature.numParams()) {
      const QoreTypeInfo *typeInfo = signature.getParamTypeInfo(0);
      if (typeInfo) {
	 if (!typeInfo->isClass(&parent_class)) {
	    // raise parse exception if parse exceptions have not been suppressed
	    if (getProgram()->getParseExceptionSink()) {
	       QoreStringNode *desc = new QoreStringNode("copy constructor will be passed ");
	       parent_class.getTypeInfo()->getThisType(*desc);
	       desc->concat(", but the object's parameter was defined expecting ");
	       typeInfo->getThisType(*desc);
	       desc->concat(" instead");
	       getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	    }
	 }
      }
      else { // set to class' type
	 signature.setFirstParamType(parent_class.getTypeInfo());
      }
   }
}

void BuiltinCopyVariantBase::evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink *xsink) const {
   CODE_CONTEXT_HELPER(CT_BUILTIN, "copy", self, xsink);
   
   if (scl) {
      scl->sml.execCopyMethods(self, old, xsink);
      if (*xsink)
	 return;
      ceh.restorePosition();
   }
   
   old->evalCopyMethodWithPrivateData(thisclass, this, self, xsink);
}

void ConstructorMethodFunction::evalConstructor(const AbstractQoreFunctionVariant *variant, const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const {
   // setup call, save runtime position, and evaluate arguments
   CodeEvaluationHelper ceh(xsink, "constructor", args, thisclass.getName());
   if (*xsink)
      return;

   bool check_args = variant;
   // find variant with evaluated args
   if (!variant) {
      variant = findVariant(ceh.getArgs(), false, xsink);
      if (!variant) {
	 assert(*xsink);
	 return;
      }
   }

   if (CONMV_const(variant)->isPrivate() && !runtimeCheckPrivateClassAccess(&thisclass)) {
      xsink->raiseException("CONSTRUCTOR-IS-PRIVATE", "%s::constructor(%s) is private and therefore this class cannot be directly instantiated with the new operator by external code", thisclass.getName(), variant->getSignature()->getSignatureText());
      return;
   }
   if (ceh.processDefaultArgs(this, variant, check_args))
      return;

   qore_call_t ct = variant->getCallType();
   ceh.setCallType(ct);
   ceh.setReturnTypeInfo(variant->getReturnTypeInfo());

   CONMV_const(variant)->evalConstructor(thisclass, self, ceh, bcl, bceal, xsink);      
}

void CopyMethodFunction::evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, BCList *scl, ExceptionSink *xsink) const {
   assert(vlist.singular());

   const AbstractQoreFunctionVariant *variant = first();
   qore_call_t ct = variant->getCallType();

   // setup call, save runtime position
   CodeEvaluationHelper ceh(xsink, "copy", 0, thisclass.getName(), ct);

   COPYMV_const(variant)->evalCopy(thisclass, self, old, ceh, scl, xsink);
}

void DestructorMethodFunction::evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const {
   assert(vlist.singular());

   const AbstractQoreFunctionVariant *variant = first();
   qore_call_t ct = variant->getCallType();

   // setup call, save runtime position
   CodeEvaluationHelper ceh(xsink, "destructor", 0, thisclass.getName(), ct);

   DESMV_const(variant)->evalDestructor(thisclass, self, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
AbstractQoreNode *NormalMethodFunction::evalMethod(const AbstractQoreFunctionVariant *variant, QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const {
   const char *mname = getName();
   CodeEvaluationHelper ceh(xsink, mname, args, getClassName());
   if (*xsink) return 0;

   bool check_args = variant;
   if (!variant) {
      variant = findVariant(ceh.getArgs(), false, xsink);
      if (!variant) {
	 assert(*xsink);
	 return 0;
      }
   }
   ceh.setClassName(METHVB_const(variant)->className());

   if (ceh.processDefaultArgs(this, variant, check_args))
      return 0;

   ceh.setCallType(variant->getCallType());
   ceh.setReturnTypeInfo(variant->getReturnTypeInfo());

   return METHV_const(variant)->evalMethod(self, ceh.getArgs(), xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
AbstractQoreNode *StaticMethodFunction::evalMethod(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const {
   const char *mname = getName();
   CodeEvaluationHelper ceh(xsink, mname, args, getClassName());
   if (*xsink) return 0;

   bool check_args = variant;
   if (!variant) {
      variant = findVariant(ceh.getArgs(), false, xsink);
      if (!variant) {
	 assert(*xsink);
	 return 0;
      }
   }
   ceh.setClassName(METHVB_const(variant)->className());

   if (ceh.processDefaultArgs(this, variant, check_args))
      return 0;

   ceh.setCallType(variant->getCallType());
   ceh.setReturnTypeInfo(variant->getReturnTypeInfo());

   return METHV_const(variant)->evalMethod(0, ceh.getArgs(), xsink);      
}

class qmi_priv {
public:
   hm_method_t &m;
   hm_method_t::iterator i;

   DLLLOCAL qmi_priv(hm_method_t &n_m) : m(n_m) {
      i = m.end();
   }
   DLLLOCAL bool next() {
      if (i == m.end())
	 i = m.begin();
      else
	 ++i;
      return i != m.end();
   }
   DLLLOCAL const QoreMethod *getMethod() const {
      assert(i != m.end());
      return i->second;
   }
};
#define HMI_CAST(p) (reinterpret_cast<qmi_priv *>(p))

QoreMethodIterator::QoreMethodIterator(const QoreClass *qc) : priv(new qmi_priv(qc->priv->hm)) {
}

QoreMethodIterator::~QoreMethodIterator() {
   delete HMI_CAST(priv);
}

bool QoreMethodIterator::next() {
   return HMI_CAST(priv)->next();
}

const QoreMethod *QoreMethodIterator::getMethod() const {
   return HMI_CAST(priv)->getMethod();
}

QoreStaticMethodIterator::QoreStaticMethodIterator(const QoreClass *qc) : priv(new qmi_priv(qc->priv->shm)) {
}

QoreStaticMethodIterator::~QoreStaticMethodIterator() {
   delete HMI_CAST(priv);
}

bool QoreStaticMethodIterator::next() {
   return HMI_CAST(priv)->next();
}

const QoreMethod *QoreStaticMethodIterator::getMethod() const {
   return HMI_CAST(priv)->getMethod();
}

// this is the "noop" function for class methods that do nothing if the
// incorrect argument types are passed - for backwards compatibility
AbstractQoreNode *class_noop(QoreObject *self, AbstractPrivateData *ptr, const QoreListNode *args, ExceptionSink *xsink) {
   return 0;
}

AbstractQoreNode *class_string_noop(QoreObject *self, AbstractPrivateData *ptr, const QoreListNode *args, ExceptionSink *xsink) {
   return null_string();
}

AbstractQoreNode *class_int_noop(QoreObject *self, AbstractPrivateData *ptr, const QoreListNode *args, ExceptionSink *xsink) {
   return zero();
}
