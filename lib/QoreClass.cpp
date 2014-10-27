/*
  QoreClass.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/Sequence.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/ConstantList.h>
#include <qore/intern/qore_program_private.h>
#include <qore/intern/ql_crypto.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

// global class ID sequence
DLLLOCAL Sequence classIDSeq(1);

void qore_method_private::parseInit() {
   assert(!static_flag);

   //printd(5, "qore_method_private::parseInit() this: %p %s::%s() func: %p\n", this, parent_class->getName(), func->getName(), func);
   func->parseInit();

   const char* name = func->getName();
   if (strcmp(name, "constructor")
       && strcmp(name, "destructor")
       && strcmp(name, "copy")) {

      if ((!strcmp(name, "methodGate")
	   || !strcmp(name, "memberGate")
	   || !strcmp(name, "memberNotification"))) {

	 if (!func->pendingEmpty()) {
	    // ensure that there is no more than one parameter declared, and if it
	    // has a type, it must be a string
	    UserSignature *sig = UMV(func->pending_first())->getUserSignature();
	    const QoreTypeInfo *t = sig->getParamTypeInfo(0);
	    if (!stringTypeInfo->parseAccepts(t)) {
	       QoreStringNode* desc = new QoreStringNode;
	       desc->sprintf("%s::%s(%s) has an invalid signature; the first argument declared as ", parent_class->getName(), func->getName(), sig->getSignatureText());
	       t->getThisType(*desc);
	       desc->concat(" is not compatible with 'string'");
	       qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
	    }
	 }
      }
      else {
	 // make sure the method doesn't override a "final" method in a base class
	 func->checkFinal();
      }
   }
}

void SignatureHash::set(const QoreString& str) {
   DigestHelper dh(str.getBuffer(), str.size());
   dh.doDigest(0, EVP_sha1());
   assert(dh.size() == SH_SIZE);
   memcpy(buf, dh.getBuffer(), dh.size());
}

void SignatureHash::update(const QoreString& str) {
   if (!is_set) {
      set(str);
      is_set = true;
   }
   else {
      // make copy of old buffer
      unsigned char cbuf[SH_SIZE];
      memcpy(cbuf, buf, SH_SIZE);
      // set hash for new addition
      set(str);
      // xor old hash with new hash
      for (unsigned i = 0; i < SH_SIZE; ++i)
	 buf[i] ^= cbuf[i];
   }

#ifdef DEBUG
   //QoreString dbg;
   //toString(dbg);
   //printd(5, "class hash %p set to: %s\n", this, dbg.getBuffer());
#endif
}

AbstractMethod::AbstractMethod(const AbstractMethod& old) {
   assert(!old.vlist.empty());
   for (vmap_t::const_iterator i = old.vlist.begin(), e = old.vlist.end(); i != e; ++i) {
      assert(vlist.find(i->first) == vlist.end());
      i->second->ref();
      vlist.insert(vmap_t::value_type(i->first, i->second));
   }
}

AbstractMethod::~AbstractMethod() {
   for (vmap_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i)
      i->second->deref();
   for (vmap_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i)
      i->second->deref();
   for (vmap_t::iterator i = pending_save.begin(), e = pending_save.end(); i != e; ++i)
      i->second->deref();
}

int AbstractMethod::parseCommit() {
   for (vmap_t::iterator i = pending_save.begin(), e = pending_save.end(); i != e; ++i)
      i->second->deref();
   pending_save.clear();
   for (vmap_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      assert(vlist.find(i->first) == vlist.end());
      vlist.insert(vmap_t::value_type(i->first, i->second));
   }
   pending_vlist.clear();
   return vlist.empty() ? -1 : 0;
}

// merge changes from parent class method of the same name during parse initialization
void AbstractMethod::parseMergeBase(AbstractMethod& m, bool committed) {
   //printd(5, "AbstractMethod::parseMergeBase(m: %p) this: %p m.pending_save: %d m.pending_vlist: %d\n", &m, this, !m.pending_save.empty(), !m.pending_vlist.empty());
   // move pending committed variants from our vlist that are in parent's pending_save list to our pending_save
   for (vmap_t::iterator i = m.pending_save.begin(), e = m.pending_save.end(); i != e; ++i) {
      const char* sig = i->second->getAbstractSignature();
      vmap_t::iterator vi = vlist.find(sig);
      if (vi != vlist.end()) {
         pending_save.insert(vmap_t::value_type(sig, i->second));
         vlist.erase(vi);
      }
   }

   // add new pending abstract methods from parent to our list - if they are not already in our pending_vlist or in our pending_save list
   for (vmap_t::iterator i = m.pending_vlist.begin(), e = m.pending_vlist.end(); i != e; ++i) {
      const char* sig = i->second->getAbstractSignature();
      //printd(5, "AbstractMethod::parseMergeBase(m: %p) this: %p checking parent: '%s'\n", &m, this, sig);
      if (pending_save.find(sig) != pending_save.end()) {
         continue;
      }
      if (pending_vlist.find(sig) != pending_vlist.end()) {
         continue;
      }
      //printd(5, "AbstractMethod::parseMergeBase(m: %p) this: %p adding to pending_vlist from parent: '%s'\n", &m, this, sig);
      i->second->ref();
      pending_vlist.insert(vmap_t::value_type(sig, i->second));
   }

   if (!committed)
      return;

   // add committed variants to our committed list
   for (vmap_t::iterator i = m.vlist.begin(), e = m.vlist.end(); i != e; ++i) {
      const char* sig = i->second->getAbstractSignature();
      // see if this method already exists in this class
      if (vlist.find(sig) != vlist.end())
         return;
      // add to vlist
      i->second->ref();
      vlist.insert(vmap_t::value_type(sig, i->second));
      // remove from pending_vlist if present because we've already added it to the committed list
      vmap_t::iterator vi = pending_vlist.find(sig);
      if (vi != pending_vlist.end()) {
	 vi->second->deref();
         pending_vlist.erase(vi);
      }
   }
}

// merge changes from parent class method of the same name during parse initialization
void AbstractMethod::parseMergeBase(AbstractMethod& m, MethodFunctionBase* f, bool committed) {
   //printd(5, "AbstractMethod::parseMergeBase(m: %p, f: %p %s::%s) this: %p m.pending_save: %d m.pending_vlist: %d\n", &m, f, f ? f->getClassName() : "n/a", f ? f->getName() : "n/a", this, !m.pending_save.empty(), !m.pending_vlist.empty());
   // move pending committed variants from our vlist that are in parent's pending_save list to our pending_save
   for (vmap_t::iterator i = m.pending_save.begin(), e = m.pending_save.end(); i != e; ++i) {
      const char* sig = i->second->getAbstractSignature();
      vmap_t::iterator vi = vlist.find(sig);
      if (vi != vlist.end()) {
         pending_save.insert(vmap_t::value_type(sig, i->second));
         vlist.erase(vi);
      }
   }

   // add new pending abstract methods from parent to our list - if they are not already in our pending_vlist or in our pending_save list
   for (vmap_t::iterator i = m.pending_vlist.begin(), e = m.pending_vlist.end(); i != e; ++i) {
      const char* sig = i->second->getAbstractSignature();
      //printd(5, "AbstractMethod::parseMergeBase(m: %p, f: %p %s::%s) this: %p checking parent: '%s' (f: %p: %d) '%s'\n", &m, f, f ? f->getClassName() : "n/a", f ? f->getName() : "n/a", this, sig, f, f && f->parseHasVariantWithSignature(i->second), sig);

      if (f && f->parseHasVariantWithSignature(i->second)) {
         // add to our pending_save
	 i->second->ref();
         pending_save.insert(vmap_t::value_type(sig, i->second));
         continue;
      }

      if (pending_save.find(sig) != pending_save.end()) {
         continue;
      }
      if (pending_vlist.find(sig) != pending_vlist.end()) {
         continue;
      }
      //printd(5, "AbstractMethod::parseMergeBase(m: %p, f: %p %s::%s) this: %p adding to pending_vlist from parent: '%s'\n", &m, f, f ? f->getClassName() : "n/a", f ? f->getName() : "n/a", this, sig);
      i->second->ref();
      pending_vlist.insert(vmap_t::value_type(sig, i->second));
   }

   if (!committed)
      return;

   // add committed variants to our committed list
   for (vmap_t::iterator i = m.vlist.begin(), e = m.vlist.end(); i != e; ++i) {
      const char* sig = i->second->getAbstractSignature();
      if (f && f->parseHasVariantWithSignature(i->second)) {
         // we already have a pending variant with this signature, so we can ignore the parent's abstract variant
         // if there is a parse commit - the pending variant is committed and we don't need the parent's abstract record
         // if there is a parse rollback - the current class is rolled back entirely (this function is only executed
         // in one time class initialization)
         continue;
      }
      else {
         //printd(5, "AbstractMethod::parseMergeCommitted() inheriting abstract method variant %s::%s asig: %s\n", f ? f->getClassName() : "xxx", f ? f->getName() : "xxx", sig);
         // insert in the committed list for this class
         assert(vlist.find(sig) == vlist.end());
	 i->second->ref();
         vlist.insert(vmap_t::value_type(sig, i->second));
         // cannot be in pending_vlist
         assert(pending_vlist.find(sig) == pending_vlist.end());
      }
   }
}

void AbstractMethod::parseAdd(MethodVariantBase* v) {
   // see if there is already an committed variant matching this signature
   // in this case it must be inherited
   const char* sig = v->getAbstractSignature();
   if (vlist.find(sig) != vlist.end())
      return;
   //printd(5, "AbstractMethod::parseAdd(v: %p) this: %p (%s) new\n", v, this, sig);

   // already referenced for "normal" insertion, ref again for abstract method insertion
   v->ref();
   pending_vlist.insert(vmap_t::value_type(sig, v));
}

void AbstractMethod::parseOverride(MethodVariantBase* v) {
   // see if there is already an committed variant matching this signature
   // in this case it must be inherited
   const char* sig = v->getAbstractSignature();
   vmap_t::iterator vi = vlist.find(sig);
   if (vi != vlist.end()) {
      pending_save.insert(vmap_t::value_type(sig, vi->second));
      // move from vlist to pending_save
      vlist.erase(vi);
      // if override is true, then we know we have a variant in a base class, so we can do nothing here
      return;
   }
}

void AbstractMethod::add(MethodVariantBase* v) {
   // see if there is already an committed variant matching this signature
   // in this case it must be inherited
   const char* sig = v->getAbstractSignature();
   if (vlist.find(sig) != vlist.end())
      return;
   // already referenced for "normal" insertion, ref again for abstract method insertion
   v->ref();
   vlist.insert(vmap_t::value_type(sig, v));
   //printd(5, "AbstractMethod::add() adding xxx::xxx(%s)\n", sig);
}

void AbstractMethod::override(MethodVariantBase* v) {
   // see if there is already an committed variant matching this signature
   // in this case it must be inherited
   const char* sig = v->getAbstractSignature();
   vmap_t::iterator vi = vlist.find(sig);
   if (vi != vlist.end()) {
      vi->second->deref();
      vlist.erase(vi);
   }
}

void AbstractMethod::parseCheckAbstract(const char* cname, const char* mname, vmap_t& vlist, QoreStringNode*& desc) {
   //printd(5, "AbstractMethod::parseCheckAbstract() checking %s::%s() vlist: %d\n", cname, mname, !vlist.empty());
   if (!vlist.empty()) {
      if (!desc)
         desc = new QoreStringNodeMaker("class '%s' cannot be instantiated because it has the following unimplemented abstract variants:", cname);
      for (vmap_t::const_iterator vi = vlist.begin(), ve = vlist.end(); vi != ve; ++vi) {
         MethodVariantBase* v = vi->second;
         desc->sprintf("\n * abstract %s %s::%s(%s);", v->getReturnTypeInfo()->getName(), cname, mname, v->getSignature()->getSignatureText());
      }
   }
}

// try to find match non-abstract variants in base classes (allows concrete variants to be inherited from another parent class)
void AbstractMethodMap::parseInit(qore_class_private& qc, BCList *scl) {
   if (!scl)
      return;
   //printd(5, "AbstractMethodMap::parseInit() this: %p cname: %s scl: %p ae: %d\n", this, qc.name.c_str(), scl, empty());
   for (amap_t::iterator i = begin(), e = end(); i != e; ++i) {
      for (vmap_t::iterator vi = i->second->vlist.begin(), ve = i->second->vlist.end(); vi != ve;) {
	 // if there is a matching non-abstract variant in any parent class, then move the variant from vlist to pending_save
	 MethodVariantBase* v = scl->matchNonAbstractVariant(i->first, vi->second);
	 if (v) {
	    const char* sig = vi->second->getAbstractSignature();
	    i->second->pending_save.insert(vmap_t::value_type(sig, vi->second));
	    vmap_t::iterator ti = vi++;
	    i->second->vlist.erase(ti);
	    // replace abstract variant
	    QoreMethod* m = qc.parseFindLocalMethod(i->first);
	    if (!m) {
	       m = new QoreMethod(qc.cls, new NormalUserMethod(qc.cls, i->first.c_str()), false);
	       qc.hm[m->getName()] = m;
	    }
	    m->getFunction()->replaceAbstractVariant(v);
	    continue;
	 }
	 ++vi;
      }
      //printd(5, "AbstractMethodMap::parseInit() this: %p %s::%s() vle: %d\n", this, qc.name.c_str(), i->first.c_str(), i->second->vlist.empty());
      for (vmap_t::iterator vi = i->second->pending_vlist.begin(), ve = i->second->pending_vlist.end(); vi != ve;) {
	 // if there is a matching non-abstract variant in any parent class, then remove the variant from pending_vlist
	 //printd(5, "AbstractMethodMap::parseInit() this: %p checking abstract %s::%s(%s): %p\n", this, qc.name.c_str(), i->first.c_str(), vi->second->getAbstractSignature(), vi->second);
	 MethodVariantBase* v = scl->matchNonAbstractVariant(i->first, vi->second);
	 if (v) {
	    //printd(5, "AbstractMethodMap::parseInit() this: %p %s::%s() FOUND v: %p (%s)\n", this, qc.name.c_str(), i->first.c_str(), v, v->getAbstractSignature());
	    vmap_t::iterator ti = vi++;
	    ti->second->deref();
	    i->second->pending_vlist.erase(ti);
	    // replace abstract variant
	    QoreMethod* m = qc.parseFindLocalMethod(i->first);
	    //printd(5, "AbstractMethodMap::parseInit() this: %p %s::%s() FOUND v: %p m: %p am: %p\n", this, qc.name.c_str(), i->first.c_str(), v, m, i->second);
	    if (!m) {
	       m = new QoreMethod(qc.cls, new NormalUserMethod(qc.cls, i->first.c_str()), false);
	       qc.hm[m->getName()] = m;
	    }
	    m->getFunction()->replaceAbstractVariant(v);
	    continue;
	 }
	 ++vi;
      }
   }
}

void AbstractMethodMap::parseAddAbstractVariant(const char* name, MethodVariantBase* f) {
   amap_t::iterator i = amap_t::find(name);
   if (i == end()) {
      AbstractMethod* m = new AbstractMethod;
      // already referenced for "normal" insertion, ref again for abstract method insertion
      f->ref();
      const char* sig = f->getAbstractSignature();
      m->pending_vlist.insert(vmap_t::value_type(sig, f));
      //printd(5, "AbstractMethodMap::parseAddAbstractVariant(name: '%s', v: %p) this: %p first (%s)\n", name, f, this, sig);
      insert(amap_t::value_type(name, m));
      return;
   }
   //printd(5, "AbstractMethodMap::parseAddAbstractVariant(name: '%s', v: %p) this: %p additional\n", name, f, this);
   i->second->parseAdd(f);
}

void AbstractMethodMap::parseOverrideAbstractVariant(const char* name, MethodVariantBase* f) {
   amap_t::iterator i = amap_t::find(name);
   if (i == end())
      return;
   i->second->parseOverride(f);
}

void AbstractMethodMap::addAbstractVariant(const char* name, MethodVariantBase* f) {
   amap_t::iterator i = amap_t::find(name);
   if (i == end()) {
      AbstractMethod* m = new AbstractMethod;
      // already referenced for "normal" insertion, ref again for abstract method insertion
      f->ref();
      m->vlist.insert(vmap_t::value_type(f->getAbstractSignature(), f));
      //printd(5, "AbstractMethodMap::addAbstractVariant(name: xxx::%s asig: %s, v: %p) this: %p (new)\n", name, f->getAbstractSignature(), f, this);
      insert(amap_t::value_type(name, m));
      return;
   }
   //printd(5, "AbstractMethodMap::addAbstractVariant(name: xxx::%s asig: %s, v: %p) this: %p\n", name, f->getAbstractSignature(), f, this);
   i->second->add(f);
}

void AbstractMethodMap::overrideAbstractVariant(const char* name, MethodVariantBase* f) {
   amap_t::iterator i = amap_t::find(name);
   if (i == end())
      return;
   i->second->override(f);
   if (i->second->empty()) {
      delete i->second;
      erase(i);
   }
}

// we check if there are any abstract method variants still in the committed lists
void AbstractMethodMap::parseCheckAbstractNew(const char* name) const {
   if (empty())
      return;

   QoreStringNode* desc = 0;
   for (amap_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      AbstractMethod::parseCheckAbstract(name, i->first.c_str(), i->second->vlist, desc);
      AbstractMethod::parseCheckAbstract(name, i->first.c_str(), i->second->pending_vlist, desc);
   }

   //printd(5, "AbstractMethodMap::parseCheckAbstractNew() class: %s desc: %p (%s)\n", name, desc, desc ? desc->getBuffer() : "n/a");

   if (desc)
      parseException("ABSTRACT-CLASS-ERROR", desc);
}

// FIXME: check private method variant access at runtime

struct SelfLocalVarParseHelper {
   QoreProgramLocation loc;
   DLLLOCAL SelfLocalVarParseHelper(LocalVar* selfid) { push_local_var(selfid, loc); }
   DLLLOCAL ~SelfLocalVarParseHelper() { pop_local_var(); }
};

void raiseNonExistentMethodCallWarning(const QoreClass* qc, const char* method) {
   qore_program_private::makeParseWarning(getProgram(), QP_WARN_NONEXISTENT_METHOD_CALL, "NON-EXISTENT-METHOD-CALL", "call to non-existent method '%s::%s()'; this call will be evaluated at run-time, so if the method is called on an object of a subclass that implements this method, then it could be a valid call, however in any other case it will result in a run-time exception.  To avoid seeing this warning, use the cast<> operator (note that if the cast is invalid at run-time, a run-time exception will be raised) or turn off the warning by using '%%disable-warning non-existent-method-call' in your code", qc->getName(), method);
}

class VRMutexHelper {
private:
   VRMutex *m;

public:
   DLLLOCAL VRMutexHelper(VRMutex *n_m, ExceptionSink* xsink) : m(n_m) {
      if (m && m->enter(xsink))
	 m = 0;
   }
   DLLLOCAL ~VRMutexHelper() {
      if (m)
	 m->exit();
   }
   DLLLOCAL operator bool() const { return m != 0; }
};

qore_class_private::qore_class_private(QoreClass* n_cls, const char* nme, int64 dom, QoreTypeInfo *n_typeInfo)
   : cls(n_cls),
     ns(0),
     scl(0),
     pend_pub_const(this),   // pending public constants
     pend_priv_const(this),  // pending private constants
     pub_const(this),        // committed public constants
     priv_const(this),       // committed private constants
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
     pub(false),
     final(false),
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

   if (nme)
      name = nme;
   else {
      name = parse_pop_name();
   }
   printd(5, "qore_class_private::qore_class_private() this=%p creating '%s' ID:%d cls: %p pub: %d\n", this, name.c_str(), classID, cls, pub);
}

// only called while the parse lock for the QoreProgram owning "old" is held
qore_class_private::qore_class_private(const qore_class_private& old, QoreClass* n_cls)
   : name(old.name), 
     cls(n_cls),
     ns(0),
     scl(0), // parent class list must be copied after new_copy set in old
     ahm(old.ahm),
     pend_pub_const(this),              // pending public constants
     pend_priv_const(this),             // pending private constants
     pub_const(old.pub_const, this),    // committed public constants
     priv_const(old.priv_const, this),  // committed private constants
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
     pub(false), // set the public flag to false; only code directly declared public in a module can be exported
     final(old.final),
     domain(old.domain), 
     num_methods(old.num_methods), 
     num_user_methods(old.num_user_methods),
     num_static_methods(old.num_static_methods), 
     num_static_user_methods(old.num_static_user_methods),
     typeInfo(old.typeInfo), 
     orNothingTypeInfo(old.orNothingTypeInfo),
     selfid(old.selfid), 
     hash(old.hash),
     ptr(old.ptr),
     new_copy(0) {
   QORE_TRACE("qore_class_private::qore_class_private(const qore_class_private &old)");
   printd(5, "qore_class_private::qore_class_private() this=%p creating copy of '%s' ID:%d cls=%p old=%p\n", this, name.c_str(), classID, cls, old.cls);

   if (!old.initialized)
      const_cast<qore_class_private &>(old).initialize();
      
   // must set after old class has been initialized
   has_delete_blocker = old.has_delete_blocker; 

   // set pointer to new copy
   old.new_copy = cls;

   // copy parent class list, if any, after new_copy is set in old
   scl = old.scl ? new BCList(*old.scl) : 0;

   printd(5, "qore_class_private::qore_class_private() old name=%s (%p) new name=%s (%p)\n", old.name.c_str(), old.name.c_str(), name.c_str(), name.c_str());

   // copy methods and maintain method pointers
   for (hm_method_t::const_iterator i = old.hm.begin(), e = old.hm.end(); i != e; ++i) {
      QoreMethod* nf = i->second->copy(cls);

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
      QoreMethod* nf = i->second->copy(cls);
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
   printd(5, "qore_class_private::~qore_class_private() this=%p %s\n", this, name.c_str());

   assert(public_vars.empty());
   assert(private_vars.empty());

   if (!pending_public_vars.empty())
      pending_public_vars.del();
   if (!pending_private_vars.empty())
      pending_private_vars.del();

   // delete normal methods
   for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      //printd(5, "QoreClass::~QoreClass() deleting method %p %s::%s()\n", m, name, m->getName());
      delete i->second;
   }      

   // delete static methods
   for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i) {
      //printd(5, "QoreClass::~QoreClass() deleting static method %p %s::%s()\n", m, name, m->getName());
      delete i->second;
   }

   delete scl;
   delete system_constructor;

   if (owns_typeinfo)
      delete typeInfo;

   if (owns_ornothingtypeinfo)
      delete orNothingTypeInfo;
}

static void breaky() {
}

void qore_class_private::initialize() {
   //printd(5, "qore_class_private::initialize() this: %p '%s' initialized: %d scl: %p\n", this, name.c_str(), initialized, scl);
   if (initialized)
      return;

   if (!sys)
      breaky();

   qcp_set_t qcp_set;
   initializeIntern(qcp_set);
}

int qore_class_private::initializeIntern(qcp_set_t& qcp_set) {
   //printd(5, "QoreClass::initializeIntern() this: %p %s class: %p scl: %p initialized: %d\n", this, name.c_str(), cls, scl, initialized);

   if (initialized)
      return 0;

   initialized = true;

   assert(!name.empty());
   //printd(5, "QoreClass::initialize() %s class: %p scl: %p\n", name.c_str(), cls, scl);

   if (scl && scl->initialize(cls, has_delete_blocker, qcp_set))
      return -1;

   QoreParseClassHelper qpch(cls);

   // first resolve types in pending variants in all method signatures (incl. return types)
   // since abstract method functions are copied by reference from the normal list; this resolves all pending
   // method function signatures as well
   for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      i->second->priv->func->resolvePendingSignatures();
   for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i)
      i->second->priv->func->resolvePendingSignatures();

   QoreProgram* pgm = getProgram();
   if (pgm && !sys && (qore_program_private::parseAddDomain(pgm, domain)))
      parseException("ILLEGAL-CLASS-DEFINITION", "class '%s' inherits functionality from base classes that is restricted by current parse options", name.c_str());

   if (!qcp_set.insert(this).second) {
      parse_error("circular reference in class hierarchy, '%s' is an ancestor of itself", name.c_str());
      scl->valid = false;
      return -1;
   }

   // initialize parent classes
   if (scl) {
      // merge direct base class abstract method lists to ourselves
      for (BCList::iterator i = scl->begin(), e = scl->end(); i != e; ++i) {
         if ((*i)->sclass) {
            // called during class initialization to copy committed abstract variants to our variant lists
            AbstractMethodMap& mm = (*i)->sclass->priv->ahm;
            //printd(5, "qore_class_private::initializeIntern() this: %p '%s' parent: %p '%s' mm empty: %d\n", this, name.c_str(), (*i)->sclass, (*i)->sclass->getName(), (int)mm.empty());
            for (amap_t::iterator j = mm.begin(), e = mm.end(); j != e; ++j) {
               // skip if vlist is empty
               if (j->second->vlist.empty() && j->second->pending_vlist.empty()) {
                  //printd(5, "qore_class_private::initializeIntern() this: %p '%s' skipping %s::%s(): vlist empty (pending_vlist empty: %d)\n", this, name.c_str(), (*i)->sclass->getName(), j->first.c_str(), (int)j->second->pending_vlist.empty());
                  continue;
               }
               amap_t::iterator vi = ahm.find(j->first);
               if (vi != ahm.end()) {
                  vi->second->parseMergeBase(*(j->second), true);
                  continue;
               }
               // now we import the abstract method to our class
               AbstractMethod* m = new AbstractMethod;
               // see if there are pending normal variants...
               hm_method_t::iterator mi = hm.find(j->first);
               // merge committed parent abstract variants with any pending local variants
               m->parseMergeBase((*j->second), mi == hm.end() ? 0 : mi->second->getFunction(), true);
               //if (m->vlist.empty())
               //if (m->vlist.empty() && m->pending_vlist.empty())
	       if (m->empty())
                  delete m;
               else {
                  ahm.insert(amap_t::value_type(j->first, m));
		  //printd(5, "qore_class_private::initializeIntern() this: %p '%s' insert abstract method variant %s::%s()\n", this, name.c_str(), (*i)->sclass->getName(), j->first.c_str());
	       }
	    }
	 }
      }
   }

   return 0;
}

// returns a non-static method if it exists in the local class and has been committed to the class
QoreMethod* qore_class_private::findLocalCommittedMethod(const char* nme) {
   QoreMethod* m = parseFindLocalMethod(nme);
   return m && !m->priv->func->committedEmpty() ? m : 0;
}

// returns a non-static method if it exists in the local class and has been committed to the class
const QoreMethod* qore_class_private::findLocalCommittedMethod(const char* nme) const {
   const QoreMethod* m = parseFindLocalMethod(nme);
   return m && !m->priv->func->committedEmpty() ? m : 0;
}

// returns a static method if it exists in the local class and has been committed to the class
QoreMethod* qore_class_private::findLocalCommittedStaticMethod(const char* nme) {
   QoreMethod* m = parseFindLocalStaticMethod(nme);
   return m && !m->priv->func->committedEmpty() ? m : 0;
}

// returns a static method if it exists in the local class and has been committed to the class
const QoreMethod* qore_class_private::findLocalCommittedStaticMethod(const char* nme) const {
   const QoreMethod* m = parseFindLocalStaticMethod(nme);
   return m && !m->priv->func->committedEmpty() ? m : 0;
}

int qore_class_private::initMembers(QoreObject& o, member_map_t::const_iterator i, member_map_t::const_iterator e, ExceptionSink* xsink) const {
   for (; i != e; ++i) {
      if (i->second) {
	 AbstractQoreNode** v = o.getMemberValuePtrForInitialization(i->first);
	 assert(!*v);
	 if (i->second->exp) {
	    ReferenceHolder<AbstractQoreNode> val(i->second->exp->eval(xsink), xsink);
	    if (*xsink)
	       return -1;
	    // check types
	    AbstractQoreNode* nv = i->second->getTypeInfo()->acceptInputMember(i->first, *val, xsink);
	    if (*xsink)
	       return -1;
	    *v = nv;
	    val.release();
	 }
	 else {
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
	    *v = i->second->getTypeInfo()->getDefaultValue();
#else
	    *v = 0;
#endif
	 }
      }
   } 
   return 0;
}

void qore_class_private::execBaseClassConstructor(QoreObject* self, BCEAList *bceal, ExceptionSink* xsink) const {
   // if there is no constructor, execute the superclass constructors directly
   if (!constructor){
      if (scl) // execute base class constructors if any
	 scl->execConstructors(self, bceal, xsink);

      return;
   }
   // no lock is sent with constructor, because no variable has been assigned yet
   bool already_executed;
   const AbstractQoreFunctionVariant *variant;
   QoreListNode* args = bceal->findArgs(cls->getID(), &already_executed, variant);
   if (!already_executed) {
      constructor->priv->evalConstructor(variant, self, args, bceal, xsink);
   }
}

QoreObject* qore_class_private::execConstructor(const AbstractQoreFunctionVariant *variant, const QoreListNode* args, ExceptionSink* xsink) const {
#ifdef DEBUG
   // instantiation checks have to be made at parse time
   for (amap_t::const_iterator i = ahm.begin(), e = ahm.end(); i != e; ++i) {
      printd(0, "qore_class_private::execConstructor() %s::constructor() abstract error '%s':\n", name.c_str(), i->first.c_str());
      vmap_t& v = i->second->vlist;
      for (vmap_t::const_iterator vi = v.begin(), ve = v.end(); vi != ve; ++vi) {
	 printd(0, " + vlist: %s\n", vi->first);
      }
      v = i->second->pending_vlist;
      for (vmap_t::const_iterator vi = v.begin(), ve = v.end(); vi != ve; ++vi) {
	 printd(0, " + pending_vlist: %s\n", vi->first);
      }
      v = i->second->pending_save;
      for (vmap_t::const_iterator vi = v.begin(), ve = v.end(); vi != ve; ++vi) {
	 printd(0, " + pending_save: %s\n", vi->first);
      }
   }
   assert(ahm.empty());
#endif

   // create new object
   QoreObject* self = new QoreObject(cls, getProgram());

   ReferenceHolder<BCEAList> bceal(scl ? new BCEAList : 0, xsink);

   printd(5, "qore_class_private::execConstructor() class=%p %s::constructor() o=%p variant=%p\n", cls, name.c_str(), self, variant);

   // instantiate members first
   initMembers(*self, *bceal, xsink);

   if (!*xsink) {
      // it's possible for constructor = 0 and variant != 0, when a class is instantiated to initialize a constant
      // and the matched variant is pending
      if (!constructor && !variant) {
	 if (scl) { // execute superconstructors if any
	    CODE_CONTEXT_HELPER(CT_BUILTIN, "constructor", self, xsink);

	    scl->execConstructors(self, *bceal, xsink);
	 }
      }
      else {
	 if (!constructor) {
	    hm_method_t::const_iterator i = hm.find("constructor");
	    assert(i != hm.end());
	    i->second->priv->evalConstructor(variant, self, args, *bceal, xsink);
	 }
	 else
	    constructor->priv->evalConstructor(variant, self, args, *bceal, xsink);
	 printd(5, "qore_class_private::execConstructor() class=%p %s done\n", cls, name.c_str());
      }
   }

   if (*xsink) {
      // instead of executing the destructors for the superclasses that were already executed we call QoreObject::obliterate()
      // which will clear out all the private data by running their dereference methods which must be OK
      self->obliterate(xsink);
      printd(5, "qore_class_private::execConstructor() this=%p %s::constructor() o=%p, exception in constructor, obliterating QoreObject and returning 0\n", this, name.c_str(), self);
      return 0;
   }

   printd(5, "qore_class_private::execConstructor() this=%p %s::constructor() returning o=%p\n", this, name.c_str(), self);
   return self;
}

void qore_class_private::parseCommit() {
   //printd(5, "qore_class_private::parseCommit() %s this=%p cls=%p hm.size=%d\n", name.c_str(), this, cls, hm.size());
   if (parse_init_called)
      parse_init_called = false;

   if (parse_init_partial_called)
      parse_init_partial_called = false;
   
   if (has_new_user_changes) {
      // signature string
      QoreString csig;

      // add parent classes to signature if creating for the first time
      if (!hash && scl) {
         for (bclist_t::const_iterator i = scl->begin(), e = scl->end(); i != e; ++i) {
            assert((*i)->sclass);
            qore_class_private* qc = (*i)->sclass->priv;
            csig.sprintf("inherits %s %s ", (*i)->isPrivate() ? "priv" : "pub", qc->name.c_str());
            if (qc->hash) {
               csig.concat('[');
               qc->hash.toString(csig);
               csig.concat("]\n");
            }
            else
               csig.sprintf("{%d}\n", qc->classID);
         }
      }

      // commit pending "normal" (non-static) method variants
      for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
	 bool is_new = i->second->priv->func->committedEmpty();
	 i->second->priv->func->parseCommitMethod(csig, 0);
	 if (is_new) {
	    checkAssignSpecial(i->second);
	    ++num_methods;
	    ++num_user_methods;
	 }
      }

      // commit pending static method variants
      for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i) {
	 bool is_new = i->second->priv->func->committedEmpty();
	 i->second->priv->func->parseCommitMethod(csig, "static");
	 if (is_new) {
	    ++num_static_methods;
	    ++num_static_user_methods;
	 }
      }

      // commit abstract method variant list changes
      ahm.parseCommit();

      {
	 // add all pending private members to real member list
	 member_map_t::iterator i = pending_private_members.begin();  
	 while (i != pending_private_members.end()) { 
	    //printd(5, "QoreClass::parseCommit() %s committing private member %p %s\n", name.c_str(), j->first, j->first);
	    if (i->second)
	       csig.sprintf("priv mem %s %s %s\n", i->second->getTypeInfo()->getName(), i->first, get_type_name(i->second->exp));
	    else
	       csig.sprintf("priv mem %s\n", i->first);
	    private_members[i->first] = i->second;
	    pending_private_members.erase(i);
	    i = pending_private_members.begin();
	 }
      }
   
      {
	 // add all pending public members to real member list
	 member_map_t::iterator i = pending_public_members.begin();  
	 while (i != pending_public_members.end()) { 
	    //printd(5, "QoreClass::parseCommit() %s committing public member %p %s\n", name.c_str(), j->first, j->first);
	    if (i->second)
	       csig.sprintf("pub mem %s %s %s\n", i->second->getTypeInfo()->getName(), i->first, get_type_name(i->second->exp));
	    else
	       csig.sprintf("pub mem %s\n", i->first);
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
	    //printd(5, "QoreClass::parseCommit() %s committing private var %p %s\n", name.c_str(), l->first, l->first);
	    if (i->second)
	       csig.sprintf("priv var %s %s %s\n", i->second->getTypeInfo()->getName(), i->first, get_type_name(i->second->exp));
	    else
	       csig.sprintf("priv var %s\n", i->first);
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
	    //printd(5, "QoreClass::parseCommit() %s committing public var %p %s\n", name.c_str(), j->first, j->first);
	    if (i->second)
	       csig.sprintf("pub var %s %s %s\n", i->second->getTypeInfo()->getName(), i->first, get_type_name(i->second->exp));
	    else
	       csig.sprintf("pub var %s\n", i->first);
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

      // process pending constants for signature
      {
	 ConstantListIterator cli(pend_priv_const);
	 while (cli.next())
	    csig.sprintf("priv const %s %s\n", cli.getName().c_str(), get_type_name(cli.getValue()));
      }
      {
	 ConstantListIterator cli(pend_pub_const);
	 while (cli.next())
	    csig.sprintf("pub const %s %s\n", cli.getName().c_str(), get_type_name(cli.getValue()));
      }

      // commit pending constants
      priv_const.assimilate(pend_priv_const);
      pub_const.assimilate(pend_pub_const);

      // if there are any signature changes, then change the class' signature
      if (!csig.empty()) {
         printd(5, "qore_class_private::parseCommit() this:%p '%s' sig:\n%s", this, name.c_str(), csig.getBuffer());
	 hash.update(csig);
      }

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

void qore_class_private::addBuiltinMethod(const char* mname, MethodVariantBase* variant) {
   assert(strcmp(mname, "constructor"));
   assert(strcmp(mname, "destructor"));
   assert(strcmp(mname, "copy"));

   hm_method_t::iterator i = hm.find(mname);
   QoreMethod* nm;
   if (i == hm.end()) {
      MethodFunctionBase *m = new BuiltinNormalMethod(cls, mname);
      nm = new QoreMethod(cls, m, false);
      insertBuiltinMethod(nm);
   }
   else {
      nm = i->second;
   }

   // set the pointer from the variant back to the owning method
   variant->setMethod(nm);

   nm->priv->addBuiltinVariant(variant);

   if (variant->isAbstract())
      ahm.addAbstractVariant(mname, variant);
   else
      ahm.overrideAbstractVariant(mname, variant);
}

void qore_class_private::addBuiltinStaticMethod(const char* mname, MethodVariantBase* variant) {
   assert(strcmp(mname, "constructor"));
   assert(strcmp(mname, "destructor"));

   hm_method_t::iterator i = shm.find(mname);
   QoreMethod* nm;
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
   QoreMethod* nm;
   if (!constructor) {
      MethodFunctionBase *m = new ConstructorMethodFunction(cls);
      nm = new QoreMethod(cls, m, false);
      constructor = nm;
      insertBuiltinMethod(nm, true);
   }
   else {
      nm = const_cast<QoreMethod* >(constructor);
   }

   // set the pointer from the variant back to the owning method
   variant->setMethod(nm);

   nm->priv->addBuiltinVariant(variant);
}

void qore_class_private::addBuiltinDestructor(BuiltinDestructorVariantBase *variant) {
   assert(!destructor);
   DestructorMethodFunction *m = new DestructorMethodFunction(cls);
   QoreMethod* qm = new QoreMethod(cls, m, false);
   destructor = qm;
   insertBuiltinMethod(qm, true);
   // set the pointer from the variant back to the owning method
   variant->setMethod(qm);

   qm->priv->addBuiltinVariant(variant);
}

void qore_class_private::addBuiltinCopyMethod(BuiltinCopyVariantBase *variant) {
   assert(!copyMethod);
   CopyMethodFunction *m = new CopyMethodFunction(cls);
   QoreMethod* qm = new QoreMethod(cls, m, false);
   copyMethod = qm;
   insertBuiltinMethod(qm, true);
   // set the pointer from the variant back to the owning method
   variant->setMethod(qm);

   qm->priv->addBuiltinVariant(variant);
}

void qore_class_private::setDeleteBlocker(q_delete_blocker_t func) {
   assert(!deleteBlocker);
   BuiltinDeleteBlocker *m = new BuiltinDeleteBlocker(func);
   QoreMethod* qm = new QoreMethod(cls, m, false);
   qm->priv->setBuiltin();
   deleteBlocker = qm;
   insertBuiltinMethod(qm, true);
   has_delete_blocker = true;
}

void qore_class_private::setBuiltinSystemConstructor(BuiltinSystemConstructorBase *m) {
   assert(!system_constructor);
   QoreMethod* qm = new QoreMethod(cls, m, false);
   qm->priv->setBuiltin();
   system_constructor = qm;
}

void qore_class_private::setPublic() {
   assert(!pub);
   pub = true;
}

QoreListNode* BCEAList::findArgs(qore_classid_t classid, bool *aexeced, const AbstractQoreFunctionVariant *&variant) {
   bceamap_t::iterator i = lower_bound(classid);
   if (i == end() || i->first != classid) {
#ifdef HAVE_STL_INSERT_WITH_HINT
      insert(i, std::make_pair(classid, new BCEANode));
#else
      insert(std::make_pair(classid, new BCEANode));
#endif
      *aexeced = false;
      variant = 0;
      return 0;
   }

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

int BCEAList::add(qore_classid_t classid, const QoreListNode* arg, const AbstractQoreFunctionVariant *variant, ExceptionSink* xsink) {
   // see if class already exists in the list
   bceamap_t::iterator i = lower_bound(classid);
   bool n = ((i == end() || i->first != classid));
   if (!n && i->second->execed)
      return 0;

   // evaluate arguments
   ReferenceHolder<QoreListNode> nargs(arg ? arg->evalList(xsink) : 0, xsink);
   if (*xsink)
      return -1;

   // save arguments
   if (n)
#ifdef HAVE_STL_INSERT_WITH_HINT
      insert(i, std::make_pair(classid, new BCEANode(nargs.release(), variant)));
#else
      insert(std::make_pair(classid, new BCEANode(nargs.release(), variant)));
#endif
   else {
      assert(!i->second->args);
      assert(!i->second->variant);
      assert(!i->second->execed);
      i->second->args = nargs.release();
      i->second->variant = variant;
   }
   return 0;
}

void BCEAList::deref(ExceptionSink* xsink) {
   bceamap_t::iterator i;
   while ((i = begin()) != end()) {
      BCEANode* n = i->second;
      erase(i);
      
      if (n->args)
	 n->args->deref(xsink);
      delete n;
   }
   delete this;
}

// resolves classes, parses arguments, and attempts to find constructor variant
void BCANode::parseInit(BCList *bcl, const char* classname) {
   QoreClass* sclass = 0;
   if (ns) {
      sclass = qore_root_ns_private::parseFindScopedClass(loc, *ns);
      printd(5, "BCANode::parseInit() this=%p resolved named scoped %s -> %p\n", this, ns->ostr, sclass);
      delete ns;
      ns = 0;
   }
   else {
      sclass = qore_root_ns_private::parseFindClass(loc, name);
      printd(5, "BCANode::parseInit() this=%p resolved %s -> %p\n", this, name, sclass);
      free(name);
      name = 0;
   }

   if (sclass) {
      if (!bcl->match(sclass))
	 parse_error(loc, "%s in base constructor argument list is not a base class of %s", sclass->getName(), classname);
      else {
	 classid = sclass->getID();

	 // find constructor variant
	 const QoreMethod* m = sclass->getConstructor();
	 int lvids = 0;
	 const QoreTypeInfo *argTypeInfo;	 
	 if (m) {
	    lvids = parseArgsVariant(loc, 0, 0, m->getFunction(), argTypeInfo);
	 }
	 else {
	    if (args)
	       args = args->parseInitList(0, 0, lvids, argTypeInfo);
	 }
	 if (lvids) {
	    parse_error(loc, "illegal local variable declaration in base class constructor argument");
	    while (lvids--)
	       pop_local_var();
	 }
      }
   }
}

int BCNode::initialize(QoreClass* cls, bool& has_delete_blocker, qcp_set_t& qcp_set) {
   if (!sclass) {
      if (cname) {
	 // if the class cannot be found, RootQoreNamespace::parseFindScopedClass() will throw the appropriate exception
	 sclass = qore_root_ns_private::parseFindScopedClass(loc, *cname);
	 printd(5, "BCNode::parseInit() %s inheriting %s (%p)\n", cls->getName(), cname->ostr, sclass);
	 delete cname;
	 cname = 0;
      }
      else {
	 // if the class cannot be found, qore_root_ns_private::parseFindClass() will throw the appropriate exception
	 sclass = qore_root_ns_private::parseFindClass(loc, cstr);
	 printd(5, "BCNode::parseInit() %s inheriting %s (%p)\n", cls->getName(), cstr, sclass);
	 free(cstr);
	 cstr = 0;
      }
      //printd(5, "BCNode::parseInit() cls: %p '%s' inherits %p '%s' final: %d\n", cls, cls->getName(), sclass, sclass ? sclass->getName() : "n/a", sclass ? sclass->priv->final : 0);
   }
   int rc = 0;
   // recursively add base classes to special method list
   if (sclass) {
      rc = sclass->priv->initializeIntern(qcp_set);
      if (!has_delete_blocker && sclass->has_delete_blocker())
	 has_delete_blocker = true;
      // include all subclass domains in this class' domain
      if (!sclass->priv->addBaseClassesToSubclass(cls, is_virtual))
         cls->priv->domain |= sclass->priv->domain;
      if (sclass->priv->final)
         parse_error("class '%s' cannot inherit 'final' class '%s'", cls->getName(), sclass->getName());
   }

   return rc;
}

const QoreClass* BCNode::getClass(const qore_class_private& qc, bool &n_priv) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return 0;
   
   const QoreClass* rv = sclass->priv->getClassIntern(qc, n_priv);
   
   if (rv && !n_priv && priv)
      n_priv = true;
   return rv;
}

int BCList::initialize(QoreClass* cls, bool &has_delete_blocker, qcp_set_t& qcp_set) {
   printd(5, "BCList::parseInit(%s) this=%p empty=%d\n", cls->getName(), this, empty());
   for (bclist_t::iterator i = begin(), e = end(); i != e;) {
      if ((*i)->initialize(cls, has_delete_blocker, qcp_set)) {
         valid = false;
         delete *i;
         erase(i++);
         continue;
      }
      ++i;
   }

   // compare each class in the list to ensure that there are no duplicates
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 bclist_t::iterator j = i;
	 while (++j != e) {
	    if (!(*j)->sclass)
	       continue;
	    if ((*i)->sclass->getID() == (*j)->sclass->getID())
	       parse_error("class '%s' cannot inherit '%s' more than once", cls->getName(), (*i)->sclass->getName());
	 }
      }
   }

   return valid ? 0 : -1;
}

const qore_class_private* BCList::isPublicOrPrivateMember(const char* mem, bool &priv) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if ((*i)->sclass && (*i)->sclass->isPublicOrPrivateMember(mem, priv)) {
         if (!priv && (*i)->priv)
            priv = true;
	 return (*i)->sclass->priv;
      }
   return 0;
}

bool BCList::parseHasPublicMembersInHierarchy() const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if ((*i)->sclass && (*i)->sclass->parseHasPublicMembersInHierarchy())
	 return true;
   return false;
}
   
bool BCList::runtimeGetMemberInfo(const char* mem, const QoreTypeInfo *&memberTypeInfo, bool &priv) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if ((*i)->sclass && (*i)->sclass->priv->runtimeGetMemberInfo(mem, memberTypeInfo, priv)) {
         if (!priv && (*i)->priv)
               priv = true;
	 return true;
      }
   return false;
}

const QoreClass* BCList::parseFindPublicPrivateMember(const QoreProgramLocation*& loc, const char* mem, const QoreTypeInfo *&memberTypeInfo, bool &member_has_type_info, bool &priv) const {
   //printd(5, "BCList::parseFindPublicPrivateMember() this: %p '%s' valid: %d\n", this, mem, valid);

   if (!valid)
      return 0;

   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 const QoreClass* qc = (*i)->sclass->priv->parseFindPublicPrivateMember(loc, mem, memberTypeInfo, member_has_type_info, priv);
	 if (qc) {
	    if (!priv && (*i)->priv)
	       priv = true;

	    return qc;
	 }
      }
   }
   return 0;
}

const QoreClass* BCList::parseFindPublicPrivateVar(const QoreProgramLocation*& loc, const char* name, const QoreTypeInfo *&varTypeInfo, bool &has_type_info, bool &priv) const {
   if (!valid)
      return 0;

   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 const QoreClass* qc = (*i)->sclass->priv->parseFindPublicPrivateVar(loc, name, varTypeInfo, has_type_info, priv);
         if (qc) {
            if (!priv && (*i)->priv)
               priv = true;

            return qc;
         }
      }
   }
   return 0;
}

// called at run time
const QoreMethod* BCList::findCommittedMethod(const char* name, bool &priv_flag) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 // this can be called before the class has been initialized if called by
	 // external code when adding builtin methods to the class
	 // assert that the base class list has already been initialized if it exists
	 //assert(!(*i)->sclass->priv->scl || ((*i)->sclass->priv->scl && (*i)->sclass->priv->initialized));

	 const QoreMethod* m;
	 if ((m = (*i)->sclass->priv->findCommittedMethod(name, priv_flag))) {
	    if (!priv_flag && (*i)->priv)
	       priv_flag = true;
	    return m;
	 }
      }
   }
   return 0;
}

// called at parse time
const QoreMethod* BCList::parseFindCommittedMethod(const char* name) {
   if (!valid)
      return 0;

   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 (*i)->sclass->priv->initialize();
	 const QoreMethod* m;
	 if ((m = (*i)->sclass->priv->parseFindCommittedMethod(name)))
	    return m;
      }
   }
   return 0;
}

const QoreMethod* BCList::parseFindMethodTree(const char* name) {
   if (!valid)
      return 0;

   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 const QoreMethod* m;
	 if ((m = (*i)->sclass->parseFindMethodTree(name)))
	    return m;
      }
   }
   return 0;
}

const QoreMethod* BCList::parseFindAnyMethodTree(const char* name) {
   if (!valid)
      return 0;

   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 const QoreMethod* m;
	 if ((m = (*i)->sclass->priv->parseFindAnyMethodIntern(name)))
	    return m;
      }
   }
   return 0;
}

// called at run time
const QoreMethod* BCList::findCommittedStaticMethod(const char* name, bool &priv_flag) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 // this can be called before the class has been initialized if called by
	 // external code when adding builtin methods to the class
	 // assert that the base class list has already been initialized if it exists
	 //assert(!(*i)->sclass->priv->scl || ((*i)->sclass->priv->scl && (*i)->sclass->priv->initialized));

	 const QoreMethod* m;
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
const QoreMethod* BCList::parseFindCommittedStaticMethod(const char* name) {
   if (!valid)
      return 0;

   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 (*i)->sclass->priv->initialize();
	 const QoreMethod* m;
	 if ((m = (*i)->sclass->priv->parseFindCommittedStaticMethod(name)))
	    return m;
      }
   }
   return 0;
}
*/

const QoreMethod* BCList::parseFindStaticMethodTree(const char* name) {
   if (!valid)
      return 0;

   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if ((*i)->sclass) {
	 const QoreMethod* m;
	 if ((m = (*i)->sclass->priv->parseFindStaticMethod(name)))
	    return m;
      }
   }
   return 0;
}

bool BCList::match(const QoreClass* cls) {
   for (bclist_t::iterator i = begin(); i != end(); i++) {
      if (cls == (*i)->sclass) {
	 return true;
      }
   }
   return false;
}

bool BCList::isPrivateMember(const char* str) const {
   for (bclist_t::const_iterator i = begin(); i != end(); i++)
      if ((*i)->sclass && (*i)->sclass->isPrivateMember(str))
	 return true;
   return false;
}

const QoreMethod* BCList::parseResolveSelfMethod(const char* name) {
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      if ((*i)->sclass) {
	 (*i)->sclass->priv->initialize();
	 const QoreMethod* m;
	 if ((m = (*i)->sclass->priv->parseResolveSelfMethodIntern(name)))
	    return m;
      }
   }
   return 0;
}

bool BCList::execDeleteBlockers(QoreObject* o, ExceptionSink* xsink) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      //printd(5, "BCList::execDeleteBlockers() %s o=%p (for subclass %s)\n", (*i)->sclass->getName(), o, o->getClass()->getName());
      if ((*i)->sclass->execDeleteBlocker(o, xsink))
	 return true;
   }
   return false;
}

int BCList::initMembers(QoreObject& o, BCEAList* bceal, ExceptionSink* xsink) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      printd(5, "BCList::initMembers() %s::constructor() o=%p (for subclass %s) virtual=%d\n", (*i)->sclass->getName(), &o, o.getClass()->getName(), (*i)->is_virtual); 

      if ((*i)->sclass->priv->initMembers(o, bceal, xsink))
	 return -1;
   }
   return 0;
}

void BCList::execConstructors(QoreObject* o, BCEAList *bceal, ExceptionSink* xsink) const {
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

bool BCList::parseCheckHierarchy(const QoreClass* cls) const {
   for (bclist_t::const_iterator i = begin(); i != end(); ++i)
      if ((*i)->sclass && (*i)->sclass->parseCheckHierarchy(cls))
	 return true;
   return false;
}

void BCList::addNewAncestors(QoreMethod* m) {
   QoreFunction *f = m->getFunction();
   const char* name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass* qc = (*i)->sclass;
      // should be only called from builtin classes, therefore qc != NULL
      assert(qc);
      const QoreMethod* w = qc->priv->findLocalCommittedMethod(name);
      if (w)
	 f->addNewAncestor(w->getFunction());
      qc->priv->addNewAncestors(m);
   }
}

void BCList::addNewStaticAncestors(QoreMethod* m) {
   QoreFunction *f = m->getFunction();
   const char* name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass* qc = (*i)->sclass;
      // should be only called from builtin classes, therefore qc != NULL
      assert(qc);
      const QoreMethod* w = qc->priv->findLocalCommittedStaticMethod(name);
      if (w)
	 f->addNewAncestor(w->getFunction());
      qc->priv->addNewStaticAncestors(m);
   }
}

void BCList::addAncestors(QoreMethod* m) {
   const char* name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass* qc = (*i)->sclass;
      assert(qc);

      const QoreMethod* w = qc->priv->findLocalCommittedMethod(name);
      if (w)
	 m->getFunction()->addAncestor(w->getFunction());

      qc->priv->addAncestors(m);
   }
}

void BCList::addStaticAncestors(QoreMethod* m) {
   const char* name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass* qc = (*i)->sclass;
      assert(qc);

      const QoreMethod* w = qc->priv->findLocalCommittedStaticMethod(name);
      if (w)
	 m->getFunction()->addAncestor(w->getFunction());
      qc->priv->addStaticAncestors(m);
   }
}

void BCList::parseAddAncestors(QoreMethod* m) {
   const char* name = m->getName();

   //printd(5, "BCList::parseAddAncestors(%p %s) this: %p size: %d\n", m, name, this, size());
   
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      // if there was a parse error finding the base class, then skip
      QoreClass* qc = (*i)->sclass;
      if (!qc) {
	 //printd(5, "BCList::parseAddAncestors(%p %s) this: %p qc: %p NOT FOUND\n", m, name, this, qc);
	 continue;
      }

      const QoreMethod* w = qc->priv->parseFindLocalMethod(name);
      //printd(5, "BCList::parseAddAncestors(%p %s) this: %p qc: %p w: %p\n", m, name, this, qc, w);

      if (w)
         m->getFunction()->addAncestor(w->getFunction());

      qc->priv->parseAddAncestors(m);
   }
}

void BCList::parseAddStaticAncestors(QoreMethod* m) {
   const char* name = m->getName();
   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass* qc = (*i)->sclass;
      // qc may be 0 if there were a parse error with an unknown class earlier
      if (!qc)
	 continue;
      const QoreMethod* w = qc->priv->parseFindLocalStaticMethod(name);
      if (w)
	 m->getFunction()->addAncestor(w->getFunction());

      qc->priv->parseAddStaticAncestors(m);
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

AbstractQoreNode* BCList::parseFindConstantValue(const char* cname, const QoreTypeInfo *&typeInfo, bool check) {
   if (!valid)
      return 0;

   for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
      QoreClass* qc = (*i)->sclass;
      // qc may be 0 if there were a parse error with an unknown class earlier
      if (!qc)
	 continue;

      AbstractQoreNode* rv = qore_class_private::parseFindConstantValue(qc, cname, typeInfo, check);
      if (rv)
	 return rv;
   }
   return 0;
}

QoreVarInfo *BCList::parseFindStaticVar(const char* vname, const QoreClass*& qc, bool check) const {
   if (!valid)
      return 0;

   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      const QoreClass* nqc = (*i)->sclass;
      // qc may be 0 if there were a parse error with an unknown class earlier
      if (!nqc)
	 continue;

      QoreVarInfo *vi = nqc->priv->parseFindStaticVar(vname, qc, check);
      if (vi)
	 return vi;
   }
   return 0;
}

const QoreClass* BCList::getClass(const qore_class_private& qc, bool &priv) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      const QoreClass* rv = (*i)->getClass(qc, priv);
      if (rv)
	 return rv;
   }
	 
   return 0;
}

MethodVariantBase* BCList::matchNonAbstractVariant(const std::string& name, MethodVariantBase* v) const {
   for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      const QoreClass* nqc = (*i)->sclass;
      //printd(5, "BCList::matchNonAbstractVariant() this: %p %s::%s %p (%s) ncq: %p\n", this, nqc ? nqc->getName() : "n/a", name.c_str(), v, v->getAbstractSignature(), nqc);

      // nqc may be 0 if there were a parse error with an unknown class earlier
      if (!nqc)
	 continue;

      QoreMethod* m = nqc->priv->parseFindLocalMethod(name);
      if (m) {
	 MethodFunctionBase* f = m->getFunction();
	 MethodVariantBase* ov = f->parseHasVariantWithSignature(v);
	 //printd(5, "BCList::matchNonAbstractVariant() this: %p %s::%s %p (%s) m: %p (%s) ov: %p (%s) abstract: %d\n", this, nqc ? nqc->getName() : "n/a", name.c_str(), v, v->getAbstractSignature(), m, m->getName(), ov, ov ? ov->getAbstractSignature() : "n/a", ov ? ov->isAbstract() : 0);
	 if (ov && !ov->isAbstract())
	    return ov;
      }
      if (nqc->priv->scl) {
	 MethodVariantBase* ov = nqc->priv->scl->matchNonAbstractVariant(name, v);
	 if (ov)
	    return ov;
      }
   }

   return 0;
}

int BCAList::execBaseClassConstructorArgs(BCEAList *bceal, ExceptionSink* xsink) const {
   for (bcalist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if (bceal->add((*i)->classid, (*i)->getArgs(), (*i)->getVariant(), xsink))
	 return -1;
   }
   return 0;
}

bool QoreClass::runtimeGetMemberInfo(const char* mem, const QoreTypeInfo *&memberTypeInfo, bool &priv_member) const {
   memberTypeInfo = 0;
   return priv->runtimeGetMemberInfo(mem, memberTypeInfo, priv_member);
}

bool QoreClass::hasAbstract() const {
   return priv->hasAbstract();
}

const QoreMethod* QoreClass::parseGetConstructor() const {
   const_cast<QoreClass* >(this)->priv->initialize();
   if (priv->constructor)
      return priv->constructor;
   return priv->parseFindLocalMethod("constructor");
}

const QoreMethod* QoreClass::parseFindLocalMethod(const char* mname) const {
   return priv->parseFindLocalMethod(mname);
}

bool QoreClass::has_delete_blocker() const {
   return priv->has_delete_blocker;
}

BCSMList *QoreClass::getBCSMList() const {
   return priv->scl ? &priv->scl->sml : 0;
}

const QoreMethod* QoreClass::findLocalStaticMethod(const char* nme) const {
   return priv->findLocalCommittedStaticMethod(nme);
}

const QoreMethod* QoreClass::findLocalMethod(const char* nme) const {
   return priv->findLocalCommittedMethod(nme);
}

// FIXME: make entry point only, called at run-time; grab program parse lock before accessing method maps
const QoreMethod* QoreClass::findStaticMethod(const char* nme) const {
   bool p = false;
   return priv->findStaticMethod(nme, p);
}

// FIXME: make entry point only, called at run-time; grab program parse lock before accessing method maps
const QoreMethod* QoreClass::findStaticMethod(const char* nme, bool &priv_flag) const {
   return priv->findStaticMethod(nme, priv_flag);
}

// FIXME: make entry point only, called at run-time; grab program parse lock before accessing method maps
const QoreMethod* QoreClass::findMethod(const char* nme) const {
   bool p = false;
   return priv->findMethod(nme, p);
}

// FIXME: make entry point only, called at run-time; grab program parse lock before accessing method maps
const QoreMethod* QoreClass::findMethod(const char* nme, bool &priv_flag) const {
   return priv->findMethod(nme, priv_flag);
}

const QoreExternalMethodVariant *QoreClass::findUserMethodVariant(const char* mname, const QoreMethod*& method, const type_vec_t &argTypeList) const {
   return priv->findUserMethodVariant(mname, method, argTypeList);
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

const char* QoreClass::getName() const {
   return priv->name.c_str(); 
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

const QoreMethod* QoreClass::parseFindMethodTree(const char* nme) {
   priv->initialize();
   return priv->parseFindMethod(nme);
}

const QoreMethod* QoreClass::parseFindStaticMethodTree(const char* nme) {
   priv->initialize();
   return priv->parseFindStaticMethod(nme);
}

void QoreClass::addBuiltinBaseClass(QoreClass* qc, QoreListNode* xargs) {
   assert(!xargs);
   if (!priv->scl)
      priv->scl = new BCList;
   priv->scl->push_back(new BCNode(qc));
}

void QoreClass::addDefaultBuiltinBaseClass(QoreClass* qc, QoreListNode* xargs) {
   addBuiltinBaseClass(qc, xargs);
   // make sure no methodID has already been assigned
   assert(priv->methodID == priv->classID);
   priv->methodID = qc->priv->classID;
}

void QoreClass::addBuiltinVirtualBaseClass(QoreClass* qc) {
   assert(qc);

   //printd(5, "adding %s as virtual base class to %s\n", qc->priv->name.c_str(), priv->name.c_str());
   if (!priv->scl)
      priv->scl = new BCList;
   priv->scl->push_back(new BCNode(qc, true));
}

void qore_class_private::parseRollback() {
   if (parse_init_called)
      parse_init_called = false;
   
   if (parse_init_partial_called)
      parse_init_partial_called = false;

   if (!has_new_user_changes) {
      // verify status
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
	 shm.erase(i++);
	 continue;
      }

      i->second->priv->func->parseRollbackMethod();
      ++i;
   }

   // rollback pending abstract method variant changes
   ahm.parseRollback();

   // rollback pending constants
   pend_priv_const.parseDeleteAll();
   pend_pub_const.parseDeleteAll();

   assert(pending_private_vars.empty());
   assert(pending_public_vars.empty());

   // set flags
   if (pending_has_public_memdecl)
      pending_has_public_memdecl = false;

   has_new_user_changes = false;
}

QoreMethod::QoreMethod(const QoreClass* n_parent_class, MethodFunctionBase *n_func, bool n_static) : priv(new qore_method_private(n_parent_class, n_func, n_static)) {
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

const char* QoreMethod::getName() const {
   return priv->getName();
}

const std::string& QoreMethod::getNameStr() const {
   return priv->getNameStr();
}

const QoreClass* QoreMethod::getClass() const {
   return priv->parent_class;
}

const char* QoreMethod::getClassName() const {
   return priv->parent_class->getName();
}

void QoreMethod::assign_class(const QoreClass* p_class) {
   assert(!priv->parent_class);
   priv->parent_class = p_class;
}

// FIXME: DEPRECATED API non functional
bool QoreMethod::isSynchronized() const {
   return false;
}

// only called for ::methodGate() and ::memberGate() which cannot be overloaded
bool QoreMethod::inMethod(const QoreObject* self) const {
   return ::runtime_in_object_method(priv->func->getName(), self);
}

QoreMethod* QoreMethod::copy(const QoreClass* p_class) const {
   return new QoreMethod(p_class, priv->func->copy(p_class), priv->static_flag);
}

const QoreTypeInfo *QoreMethod::getUniqueReturnTypeInfo() const {
   return priv->getUniqueReturnTypeInfo();
}

static const QoreClass* getStackClass() {
   const qore_class_private* qc = runtime_get_class();
   return qc ? qc->cls : 0;
}

void QoreClass::addPublicMember(const char* mname, const QoreTypeInfo *n_typeInfo, AbstractQoreNode* initial_value) {
   priv->addPublicMember(mname, n_typeInfo, initial_value);
}

void QoreClass::addPrivateMember(const char* mname, const QoreTypeInfo *n_typeInfo, AbstractQoreNode* initial_value) {
   priv->addPrivateMember(mname, n_typeInfo, initial_value);
}

bool BCSMList::isBaseClass(QoreClass* qc) const {
   class_list_t::const_iterator i = begin();
   while (i != end()) {
      QoreClass* sc = (*i).first;
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

int BCSMList::addBaseClassesToSubclass(QoreClass* thisclass, QoreClass* sc, bool is_virtual) {
   //printd(5, "BCSMList::addBaseClassesToSubclass(this=%s, sc=%s) size=%d\n", thisclass->getName(), sc->getName());
   for (class_list_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      //printd(5, "BCSMList::addBaseClassesToSubclass() %s sc: %s is_virt: %d\n", thisclass->getName(), sc->getName(), is_virtual);
      if (sc->priv->scl->sml.add(thisclass, (*i).first, is_virtual || (*i).second)) {
         //printd(5, "BCSMList::addBaseClassesToSubclass() %s sc: %s is_virt: %d FAILED\n", thisclass->getName(), sc->getName(), is_virtual);
         return -1;
      }
   }
   return 0;
}

int BCSMList::add(QoreClass* thisclass, QoreClass* qc, bool is_virtual) {
   if (thisclass->getID() == qc->getID()) {
      thisclass->priv->scl->valid = false;
      parse_error("class '%s' cannot inherit itself", thisclass->getName());
      return -1;
   }

   // see if class already exists in list
   class_list_t::const_iterator i = begin();
   while (i != end()) {
      if (i->first->getID() == qc->getID())
         return 0;
      if (i->first->getID() == thisclass->getID()) {
         thisclass->priv->scl->valid = false;
      	 parse_error("circular reference in class hierarchy, '%s' is an ancestor of itself", thisclass->getName());
      	 return -1;
      }
      i++;
   }

   // append to the end of the list
   push_back(std::make_pair(qc, is_virtual));
   return 0;
}

void BCSMList::execDestructors(QoreObject* o, ExceptionSink* xsink) const {
   for (class_list_t::const_reverse_iterator i = rbegin(), e = rend(); i != e; ++i) {
      //printd(5, "BCSMList::execDestructors() %s::destructor() o: %p virt: %s (subclass %s)\n", i->first->getName(), o, i->second ? "true" : "false", o->getClass()->getName());
      if (!i->second)
	 i->first->priv->execBaseClassDestructor(o, xsink);
   }
}

void BCSMList::execSystemDestructors(QoreObject* o, ExceptionSink* xsink) const {
   for (class_list_t::const_reverse_iterator i = rbegin(), e = rend(); i != e; ++i) {
      printd(5, "BCSMList::execSystemDestructors() %s::destructor() o=%p virt=%s (subclass %s)\n", i->first->getName(), o, i->second ? "true" : "false", o->getClass()->getName());
      if (!i->second)
	 i->first->priv->execBaseClassSystemDestructor(o, xsink);
   }
}

void BCSMList::execCopyMethods(QoreObject* self, QoreObject* old, ExceptionSink* xsink) const {
   for (class_list_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if (!i->second) {
	 i->first->priv->execBaseClassCopy(self, old, xsink);
	 if (xsink->isEvent())
	    break;
      }
   }
}

QoreClass* BCSMList::getClass(qore_classid_t cid) const {
   for (class_list_t::const_iterator i = begin(), e = end(); i != e; ++i) {
      if (i->first->getID() == cid)
	 return i->first;
   }
   return 0;
}

void BCSMList::resolveCopy() {
   for (class_list_t::iterator i = begin(), e = end(); i != e; ++i) {
      assert(i->first->priv->new_copy);
      i->first = i->first->priv->new_copy;
   }
}

QoreClass::QoreClass(const char* nme, int dom) : priv(new qore_class_private(this, nme, dom)) {
   priv->orNothingTypeInfo = new OrNothingTypeInfo(*(priv->typeInfo), nme);
   priv->owns_ornothingtypeinfo = true;
}

QoreClass::QoreClass(const char* nme, int64 dom, const QoreTypeInfo *typeInfo) {
   assert(typeInfo);
   priv = new qore_class_private(this, nme, dom, const_cast<QoreTypeInfo *>(typeInfo));

   printd(5, "QoreClass::QoreClass() this=%p creating '%s' with custom typeinfo\n", this, priv->name.c_str());

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
   priv->orNothingTypeInfo = new OrNothingTypeInfo(*(priv->typeInfo), priv->name.c_str());
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

QoreClass* QoreClass::getClass(qore_classid_t cid) const {
   if (cid == priv->classID)
      return (QoreClass* )this;
   return priv->scl ? priv->scl->sml.getClass(cid) : 0;
}

const QoreClass* QoreClass::getClassIntern(qore_classid_t cid, bool &cpriv) const {
   if (cid == priv->classID)
      return (QoreClass* )this;
   return priv->scl ? priv->scl->getClass(cid, cpriv) : 0;
}

const QoreClass* QoreClass::getClass(qore_classid_t cid, bool &cpriv) const {
   cpriv = false;
   return getClassIntern(cid, cpriv);
}

const QoreClass* QoreClass::getClass(const QoreClass& qc, bool &cpriv) const {
   cpriv = false;
   return priv->getClassIntern(*(qc.priv), cpriv);
}

AbstractQoreNode* QoreMethod::evalNormalVariant(QoreObject* self, const QoreExternalMethodVariant *ev, const QoreListNode* args, ExceptionSink* xsink) const {
   const AbstractQoreFunctionVariant *variant = reinterpret_cast<const AbstractQoreFunctionVariant *>(ev);

   CodeEvaluationHelper ceh(xsink, getFunction(), variant, getName(), args, variant->className());
   if (*xsink) return 0;

   return METHV_const(variant)->evalMethod(self, ceh, xsink);      
}

int64 QoreMethod::bigIntEvalNormalVariant(QoreObject* self, const QoreExternalMethodVariant *ev, const QoreListNode* args, ExceptionSink* xsink) const {
   const AbstractQoreFunctionVariant *variant = reinterpret_cast<const AbstractQoreFunctionVariant *>(ev);

   CodeEvaluationHelper ceh(xsink, getFunction(), variant, getName(), args, variant->className());
   if (*xsink) return 0;

   return METHV_const(variant)->bigIntEvalMethod(self, ceh, xsink);      
}

int QoreMethod::intEvalNormalVariant(QoreObject* self, const QoreExternalMethodVariant *ev, const QoreListNode* args, ExceptionSink* xsink) const {
   const AbstractQoreFunctionVariant *variant = reinterpret_cast<const AbstractQoreFunctionVariant *>(ev);

   CodeEvaluationHelper ceh(xsink, getFunction(), variant, getName(), args, variant->className());
   if (*xsink) return 0;

   return METHV_const(variant)->intEvalMethod(self, ceh, xsink);      
}

bool QoreMethod::boolEvalNormalVariant(QoreObject* self, const QoreExternalMethodVariant *ev, const QoreListNode* args, ExceptionSink* xsink) const {
   const AbstractQoreFunctionVariant *variant = reinterpret_cast<const AbstractQoreFunctionVariant *>(ev);

   CodeEvaluationHelper ceh(xsink, getFunction(), variant, getName(), args, variant->className());
   if (*xsink) return 0;

   return METHV_const(variant)->boolEvalMethod(self, ceh, xsink);      
}

double QoreMethod::floatEvalNormalVariant(QoreObject* self, const QoreExternalMethodVariant *ev, const QoreListNode* args, ExceptionSink* xsink) const {
   const AbstractQoreFunctionVariant *variant = reinterpret_cast<const AbstractQoreFunctionVariant *>(ev);

   CodeEvaluationHelper ceh(xsink, getFunction(), variant, getName(), args, variant->className());
   if (*xsink) return 0;

   return METHV_const(variant)->floatEvalMethod(self, ceh, xsink);      
}

bool QoreMethod::existsVariant(const type_vec_t &paramTypeInfo) const {
   return priv->func->existsVariant(paramTypeInfo);
}

QoreClass::QoreClass(const QoreClass &old) : priv(new qore_class_private(*old.priv, this)) {
}

void QoreClass::insertMethod(QoreMethod* m) {
   priv->insertBuiltinMethod(m);
}      

void QoreClass::insertStaticMethod(QoreMethod* m) {
   priv->insertBuiltinStaticMethod(m);
}      

const QoreClass* qore_class_private::parseGetClass(const qore_class_private& qc, bool &cpriv) const {
   cpriv = false;
   const_cast<qore_class_private*>(this)->initialize();
   if (qc.classID == classID || (qc.name == name && qc.hash == hash))
      return (QoreClass*)cls;
   return scl ? scl->getClass(qc, cpriv) : 0;
}

bool qore_class_private::hasCallableMethod(const char* m, int mask) const {
   bool external = (cls != getStackClass());
   const QoreMethod* w = 0;
   bool priv_flag = false;

   if (mask & QCCM_NORMAL)
      w = findMethod(m, priv_flag);

   if (!w && (mask & QCCM_STATIC))
      w = findStaticMethod(m, priv_flag);

   return !w || (external && priv_flag) ? false : true;   
}

const QoreMethod* qore_class_private::getMethodForEval(const char* nme, ExceptionSink* xsink) const {
   bool external = (cls != getStackClass());
   printd(5, "qore_class_private::getMethodForEval() %s::%s() %s call attempted\n", name.c_str(), nme, external ? "external" : "internal" );

   const QoreMethod* w;

   bool priv_flag = false;
   // FIXME: check locking when accessing method maps at runtime
   if (!(w = findMethod(nme, priv_flag)) && !(w = findStaticMethod(nme, priv_flag)))
      return 0;

   //printd(5, "QoreClass::getMethodForEval() %s::%s() found method %p class %s\n", name.c_str(), nme, w, w->getClassName());

   // check for illegal explicit call
   if (w == constructor || w == destructor || w == deleteBlocker) {
      xsink->raiseException("ILLEGAL-EXPLICIT-METHOD-CALL", "explicit calls to ::%s() methods are not allowed", nme);
      return 0;
   }

   if (external) {
      if (w->isPrivate()) {
	 xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s() is private and cannot be accessed externally", name.c_str(), nme);
	 return 0;
      }
      else if (priv_flag) {
	 xsink->raiseException("BASE-CLASS-IS-PRIVATE", "%s() is a method of a privately-inherited class of %s", nme, name.c_str());
	 return 0;
      }
   }

   return w;
}

AbstractQoreNode* QoreClass::evalMethod(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("QoreClass::evalMethod()");

   if (!strcmp(nme, "copy")) {
      if (args) {
         xsink->raiseException("COPY-ERROR", "while calling %s::copy(): it is illegal to pass arguments to copy methods", self->getClassName());
         return 0;
      }
      return execCopy(self, xsink);
   }

   const QoreMethod* w = priv->getMethodForEval(nme, xsink);
   if (*xsink)
      return 0;

   if (w)
      return qore_method_private::eval(*w, self, args, xsink);

   // first see if there is a pseudo-method for this
   QoreClass* qc = 0;
   w = pseudo_classes_find_method(NT_OBJECT, nme, qc);
   if (w)
      return qore_method_private::evalPseudoMethod(w, 0, self, args, xsink);
   else if (priv->methodGate && !priv->methodGate->inMethod(self)) // call methodGate with unknown method name and arguments
      return evalMethodGate(self, nme, args, xsink);
   else {
      xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() has been defined and no pseudo-method <object>::%s() is available", self->getClassName(), nme, nme);
      return 0;
   }

   /*
     if (priv->methodGate && !priv->methodGate->inMethod(self)) // call methodGate with unknown method name and arguments
     return evalMethodGate(self, nme, args, xsink);

     return pseudo_classes_eval(self, nme, args, xsink);
   */
}

int64 QoreClass::bigIntEvalMethod(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("QoreClass::bigIntEvalMethod()");

   if (!strcmp(nme, "copy")) {
      discard(execCopy(self, xsink), xsink);
      return 0;
   }

   const QoreMethod* w = priv->getMethodForEval(nme, xsink);
   if (*xsink)
      return 0;

   if (!w) {
      if (priv->methodGate && !priv->methodGate->inMethod(self)) { // call methodGate with unknown method name and arguments
	 ReferenceHolder<AbstractQoreNode> rv(evalMethodGate(self, nme, args, xsink), xsink);
	 return *xsink ? 0 : rv->getAsBigInt();
      }

      return pseudo_classes_int64_eval(self, nme, args, xsink);
   }
   
   return qore_method_private::bigIntEval(*w, self, args, xsink);
}

int QoreClass::intEvalMethod(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("QoreClass::intEvalMethod()");

   if (!strcmp(nme, "copy")) {
      discard(execCopy(self, xsink), xsink);
      return 0;
   }

   const QoreMethod* w = priv->getMethodForEval(nme, xsink);
   if (*xsink)
      return 0;

   if (!w) {
      if (priv->methodGate && !priv->methodGate->inMethod(self)) { // call methodGate with unknown method name and arguments
	 ReferenceHolder<AbstractQoreNode> rv(evalMethodGate(self, nme, args, xsink), xsink);
	 return *xsink ? 0 : rv->getAsInt();
      }

      return pseudo_classes_int_eval(self, nme, args, xsink);
   }
   
   return qore_method_private::intEval(*w, self, args, xsink);
}

bool QoreClass::boolEvalMethod(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("QoreClass::boolEvalMethod()");

   if (!strcmp(nme, "copy")) {
      discard(execCopy(self, xsink), xsink);
      return false;
   }

   const QoreMethod* w = priv->getMethodForEval(nme, xsink);
   if (*xsink)
      return false;

   if (!w) {
      if (priv->methodGate && !priv->methodGate->inMethod(self)) { // call methodGate with unknown method name and arguments
	 ReferenceHolder<AbstractQoreNode> rv(evalMethodGate(self, nme, args, xsink), xsink);
	 return *xsink ? false : rv->getAsBool();
      }

      return pseudo_classes_bool_eval(self, nme, args, xsink);
   }
   
   return qore_method_private::boolEval(*w, self, args, xsink);
}

double QoreClass::floatEvalMethod(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("QoreClass::floatEvalMethod()");

   if (!strcmp(nme, "copy")) {
      discard(execCopy(self, xsink), xsink);
      return 0.0;
   }

   const QoreMethod* w = priv->getMethodForEval(nme, xsink);
   if (*xsink)
      return 0.0;

   if (!w) {
      if (priv->methodGate && !priv->methodGate->inMethod(self)) { // call methodGate with unknown method name and arguments
	 ReferenceHolder<AbstractQoreNode> rv(evalMethodGate(self, nme, args, xsink), xsink);
	 return *xsink ? 0.0 : rv->getAsFloat();
      }

      return pseudo_classes_double_eval(self, nme, args, xsink);
   }
   
   return qore_method_private::floatEval(*w, self, args, xsink);
}

AbstractQoreNode* QoreClass::evalMethodGate(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
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

bool QoreClass::isPrivateMember(const char* str) const {
   member_map_t::const_iterator i = priv->private_members.find((char *)str);
   if (i != priv->private_members.end())
      return true;

   if (priv->scl)
      return priv->scl->isPrivateMember(str);
   return false;
}

AbstractQoreNode* QoreClass::evalMemberGate(QoreObject* self, const QoreString *nme, ExceptionSink* xsink) const {
   assert(nme && nme->getEncoding() == QCS_DEFAULT);

   printd(5, "QoreClass::evalMemberGate() member=%s\n", nme->getBuffer());
   // do not run memberGate method if we are already in it...
   if (!priv->memberGate || priv->memberGate->inMethod(self))
      return 0;

   ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
   args->push(new QoreStringNode(*nme));

   return self->evalMethod(*priv->memberGate, *args, xsink);
}

void QoreClass::execMemberNotification(QoreObject* self, const char* mem, ExceptionSink* xsink) const {
   // cannot run this method when executing from within the class
   assert((this != getStackClass()));

   //printd(5, "QoreClass::execMemberNotification() member=%s\n", mem);

   ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
   args->push(new QoreStringNode(mem));
   discard(self->evalMethod(*priv->memberNotification, *args, xsink), xsink);
}

QoreObject* QoreClass::execConstructor(const AbstractQoreFunctionVariant *variant, const QoreListNode* args, ExceptionSink* xsink) const {
#ifdef QORE_MANAGE_STACK
   if (check_stack(xsink))
      return 0;
#endif
   return priv->execConstructor(variant, args, xsink);
}

QoreObject* QoreClass::execConstructor(const QoreListNode* args, ExceptionSink* xsink) const {
   return priv->execConstructor(0, args, xsink);
}

QoreObject* qore_class_private::execSystemConstructor(QoreObject* self, int code, va_list args) const {
   assert(system_constructor);
   const_cast<qore_class_private*>(this)->initialize();
   // no lock is sent with constructor, because no variable has been assigned yet
   system_constructor->priv->evalSystemConstructor(self, code, args);
   return self;
}

QoreObject* QoreClass::execSystemConstructor(int code, ...) const {
   va_list args;

   // create new object
   QoreObject* o = new QoreObject(this, 0);

   va_start(args, code);
   priv->execSystemConstructor(o, code, args);
   va_end(args);

   printd(5, "QoreClass::execSystemConstructor() %s::execSystemConstructor() returning %p\n", priv->name.c_str(), o);
   return o;
}

bool QoreClass::execDeleteBlocker(QoreObject* self, ExceptionSink* xsink) const {
   return priv->execDeleteBlocker(self, xsink);
}

bool qore_class_private::execDeleteBlocker(QoreObject* self, ExceptionSink* xsink) const {
   printd(5, "qore_class_private::execDeleteBlocker(self=%p) this=%p '%s' has_delete_blocker=%s deleteBlocker=%p\n", self, this, name.c_str(), has_delete_blocker ? "true" : "false", deleteBlocker);
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

void QoreClass::execDestructor(QoreObject* self, ExceptionSink* xsink) const {
   priv->execDestructor(self, xsink);
}

void qore_class_private::execDestructor(QoreObject* self, ExceptionSink* xsink) const {
   //printd(5, "qore_class_private::execDestructor() %s::destructor() o: %p scl: %p sml: %p\n", name.c_str(), self, scl, scl ? &scl->sml : 0);

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

   xsink->assimilate(de);
}

void qore_class_private::execBaseClassDestructor(QoreObject* self, ExceptionSink* xsink) const {
   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;
   if (destructor)
      destructor->priv->evalDestructor(self, &de);
   else if (sys)
      self->defaultSystemDestructor(classID, &de);

   xsink->assimilate(de);
}

void qore_class_private::execBaseClassSystemDestructor(QoreObject* self, ExceptionSink* xsink) const {
   // we use a new, blank exception sink to ensure all destructor code gets executed 
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;
   if (destructor)
      destructor->priv->evalSystemDestructor(self, &de);
   else if (sys)
      self->defaultSystemDestructor(classID, &de);

   xsink->assimilate(de);
}

void qore_class_private::execBaseClassCopy(QoreObject* self, QoreObject* old, ExceptionSink* xsink) const {
   if (copyMethod)
      copyMethod->priv->evalCopy(self, old, xsink);
}

QoreObject* QoreClass::execCopy(QoreObject* old, ExceptionSink* xsink) const {
   return priv->execCopy(old, xsink);
}

QoreObject* qore_class_private::execCopy(QoreObject* old, ExceptionSink* xsink) const {
   // check for illegal private calls
   if (copyMethod && copyMethod->isPrivate() && cls != getStackClass()) {
      xsink->raiseException("METHOD-IS-PRIVATE", "%s::copy() is private and cannot be accessed externally", name.c_str());
      return 0;
   }

   QoreHashNode* h = old->copyData(xsink);
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

int qore_class_private::addBaseClassesToSubclass(QoreClass* sc, bool is_virtual) {
   //printd(5, "qore_class_private::addBaseClassesToSubclass() this: %p '%s' sc: %p '%s' is_virtual: %d scl: %p\n", this, name.c_str(), sc, sc->getName(), is_virtual, scl);
   if (scl && scl->sml.addBaseClassesToSubclass(cls, sc, is_virtual))
      return -1;
   return sc->priv->scl->sml.add(sc, cls, is_virtual);
}

// searches all methods, both pending and comitted
const QoreMethod* qore_class_private::parseResolveSelfMethod(const char* nme) {
   initialize();
   const QoreMethod* m = parseResolveSelfMethodIntern(nme);

   if (!m) {
      parse_error("no method %s::%s() has been defined; if you want to make a call to a method that will be defined in an inherited class, then use 'self.%s()' instead", name.c_str(), nme, nme);
      return 0;
   }
   printd(5, "qore_class_private::parseResolveSelfMethod(%s) resolved to %s::%s() %p (static=%d)\n", nme, name.c_str(), nme, m, m->isStatic());

   const char* mname = m->getName();
   // make sure we're not calling a method that cannot be called directly
   if (!m->isStatic() && (!strcmp(mname, "constructor") || !strcmp(mname, "destructor") || !strcmp(mname, "copy"))) {
      parse_error("explicit calls to %s() methods are not allowed", nme);
      return 0;
   }

   return m;
}

const QoreMethod* qore_class_private::parseResolveSelfMethod(NamedScope* nme) {
   // first find class
   QoreClass* qc = qore_root_ns_private::parseFindScopedClassWithMethod(*nme, true);
   if (!qc)
      return 0;

   // see if class is base class of this class
   if (qc != cls && (!scl || !scl->sml.isBaseClass(qc))) {
      parse_error("'%s' is not a base class of '%s'", qc->getName(), name.c_str());
      return 0;
   }

   return qc->priv->parseResolveSelfMethod(nme->getIdentifier());
}

int qore_class_private::addUserMethod(const char* mname, MethodVariantBase *f, bool n_static) {
   // FIXME: set class name at parse time
   const char* tname = name.c_str();
   printd(5, "QoreClass::addUserMethod(%s, umv=%p, priv=%d, static=%d) this=%p %s\n", mname, f, f->isPrivate(), n_static, this, tname);

   std::auto_ptr<MethodVariantBase> func(f);

   if (f->isAbstract() && initialized) {
      parseException("ILLEGAL-ABSTRACT-METHOD", "abstract %s::%s(): abstract methods cannot be added to a class once the class has been committed", name.c_str(), mname);
      return -1;
   }

   bool dst = !strcmp(mname, "destructor");
   bool con = dst ? false : !strcmp(mname, "constructor");

   // check for illegal static method
   if (n_static) {
      if ((con || dst || checkSpecialStaticIntern(mname))) {
         parseException("ILLEGAL-STATIC-METHOD", "%s methods cannot be static", mname);
         return -1;
      }
   }

   bool cpy = dst || con ? false : !strcmp(mname, "copy");
   // check for illegal method overloads
   if (sys && (con || cpy)) {
      parseException("ILLEGAL-METHOD-OVERLOAD", "class %s is builtin; %s methods in builtin classes cannot be overloaded; create a subclass instead", name.c_str(), mname);
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

   QoreMethod* m = const_cast<QoreMethod* >(!n_static ? parseFindMethod(mname) : parseFindStaticMethod(mname));
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
      MethodFunctionBase* mfb;
      if (con) {
	 mfb = new ConstructorMethodFunction(cls);
	 // set selfid immediately if adding a constructor variant
	 reinterpret_cast<UserConstructorVariant*>(f)->getUserSignature()->setSelfId(&selfid);
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

   //printd(5, "qore_class_private::addUserMethod() %s::%s() f: %p (%d)\n", tname, mname, f, ((QoreReferenceCounter*)f)->reference_count());

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

   // add this variant to the abstract map if it's an abstract variant
   if (f->isAbstract()) {
      assert(!n_static);
      if (!f->hasBody()) {
         //printd(5, "qore_class_private::addUserMethod() this: %p adding abstract method variant %s::%s()\n", this, name.c_str(), mname);
         ahm.parseAddAbstractVariant(mname, f);
      }
      else {
         // this is a concrete method variant that must have an abstract implementation in a parent class
         f->clearAbstract();
         ahm.parseOverrideAbstractVariant(mname, f);
      }
   }
   else if (!n_static)
      ahm.parseOverrideAbstractVariant(mname, f);

   return 0;
}

void QoreClass::addAbstractMethodVariantExtended3(const char *n_name, bool n_priv, int64 n_flags, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }
   //printd(5, "QoreClass::addAbstractMethodVariantExtended3() %s::%s() num_params: %d\n", getName(), n_name, num_params);

   priv->addBuiltinMethod(n_name, new BuiltinAbstractMethodVariant(n_priv, n_flags, returnTypeInfo, typeList, defaultArgList, nameList));
}

// adds a builtin method to the class (duplicate checking is made in debug mode and causes an abort)
void QoreClass::addMethod(const char* nme, q_method_t m, bool priv_flag) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethodVariant(m, priv_flag));
}

void QoreClass::addMethodExtended(const char* nme, q_method_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinNormalMethodVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList));
}

void QoreClass::addMethodExtended3(const char* nme, q_method_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinNormalMethodVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addMethodExtended3(const char* nme, q_method_int64_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinNormalMethodBigIntVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addMethodExtended3(const char* nme, q_method_bool_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinNormalMethodBoolVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addMethodExtended3(const char* nme, q_method_double_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinNormalMethodFloatVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addMethodExtendedList(const char* nme, q_method_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethodVariant(m, priv_flag, false, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

// adds a builtin method with the new generic calling convention to the class (duplicate checking is made in debug mode and causes an abort)
void QoreClass::addMethod2(const char* nme, q_method2_t m, bool priv_flag) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethod2Variant(m, priv_flag));
}

void QoreClass::addMethodExtended2(const char* nme, q_method2_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinNormalMethod2Variant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList));
}

void QoreClass::addMethodExtendedList2(const char* nme, q_method2_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethod2Variant(m, priv_flag, false, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

void QoreClass::addMethodExtendedList3(const void *ptr, const char* nme, q_method3_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinMethod(nme, new BuiltinNormalMethod3Variant(ptr, m, priv_flag, false, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

// adds a builtin static method to the class
void QoreClass::addStaticMethod2(const char* nme, q_static_method2_t m, bool priv_flag) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethod2Variant(m, priv_flag));
}

void QoreClass::addStaticMethodExtended2(const char* nme, q_static_method2_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethod2Variant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList));
}

void QoreClass::addStaticMethodExtendedList2(const char* nme, q_static_method2_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethod2Variant(m, priv_flag, false, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

void QoreClass::addStaticMethodExtendedList3(const void *ptr, const char* nme, q_static_method3_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethod3Variant(ptr, m, priv_flag, false, flags, domain, returnTypeInfo, n_typeList, n_defaultArgList));
}

// adds a builtin static method to the class
void QoreClass::addStaticMethod(const char* nme, q_func_t m, bool priv_flag) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethodVariant(m, priv_flag));
}

void QoreClass::addStaticMethodExtended(const char* nme, q_func_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, args);
      va_end(args);
   }

   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethodVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList));
}

void QoreClass::addStaticMethodExtended3(const char* nme, q_func_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }

   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethodVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addStaticMethodExtended3(const char* nme, q_func_int64_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinStaticMethodBigIntVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addStaticMethodExtended3(const char* nme, q_func_bool_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinStaticMethodBoolVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addStaticMethodExtended3(const char* nme, q_func_double_t m, bool priv_flag, int64 flags, int64 domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }

   priv->addBuiltinMethod(nme, new BuiltinStaticMethodFloatVariant(m, priv_flag, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addStaticMethodExtendedList(const char* nme, q_func_t m, bool priv_flag, int64 n_flags, int64 n_domain, const QoreTypeInfo *n_returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) {
   priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethodVariant(m, priv_flag, false, n_flags, n_domain, n_returnTypeInfo, n_typeList, n_defaultArgList));
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

void QoreClass::setConstructorExtended3(q_constructor_t m, bool priv_flag, int64 n_flags, int64 n_domain, unsigned num_params, ...) {
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t nameList;
   if (num_params) {
      va_list args;
      va_start(args, num_params);
      qore_process_params(num_params, typeList, defaultArgList, nameList, args);
      va_end(args);
   }
   priv->addBuiltinConstructor(new BuiltinConstructorVariant(m, priv_flag, n_flags, n_domain, typeList, defaultArgList, nameList));
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

QoreListNode* QoreClass::getMethodList() const {
   QoreListNode* l = new QoreListNode;

   for (hm_method_t::const_iterator i = priv->hm.begin(), e = priv->hm.end(); i != e; ++i)
      l->push(new QoreStringNode(i->first));
   return l;
}

QoreListNode* QoreClass::getStaticMethodList() const {
   QoreListNode* l = new QoreListNode;

   for (hm_method_t::const_iterator i = priv->shm.begin(), e = priv->shm.end(); i != e; ++i)
      l->push(new QoreStringNode(i->first));
   return l;
}

void qore_class_private::parseInitPartial() {
   if (parse_init_partial_called)
      return;

   initialize();

   // the class could be initialized out of line during initialize9)
   if (parse_init_partial_called)
      return;

   NamespaceParseContextHelper nspch(ns);   
   QoreParseClassHelper qpch(cls);
   parseInitPartialIntern();
}

void qore_class_private::parseInitPartialIntern() {
   assert(!parse_init_partial_called);
   parse_init_partial_called = true;

   //printd(5, "class_private::parseInitPartialIntern() this: %p '%s' scl: %p user_changes: %d\n", this, name.c_str(), scl, has_new_user_changes);

   // initialize parents first for abstract method handling
   if (scl) {
      for (bclist_t::iterator i = scl->begin(), e = scl->end(); i != e; ++i) {
         if ((*i)->sclass) {
            (*i)->sclass->priv->parseInit();
            //printd(5, "qore_class_private::parseInitPartialIntern() this: %p '%s' merging base class abstract methods from %p '%s'\n", this, name.c_str(), (*i)->sclass, (*i)->sclass->getName());

            // copy pending abstract changes from parent classes to the local class

            // get parent's abstract method map
            AbstractMethodMap& mm = (*i)->sclass->priv->ahm;

            // iterate parent's method map and merge parent changes to our method map
            for (amap_t::iterator ai = mm.begin(), ae = mm.end(); ai != ae; ++ai) {
               amap_t::iterator vi = ahm.find(ai->first);
               //printd(5, "qore_class_private::parseInitPartialIntern() this: %p '%s' checking '%s::%s()' found: %d\n", this, name.c_str(), (*i)->sclass->getName(), ai->first.c_str(), vi != ahm.end());
               if (vi != ahm.end()) {
                  vi->second->parseMergeBase(*(ai->second));
                  continue;
               }
               AbstractMethod* m = new AbstractMethod;
               // see if there are pending normal variants...
               hm_method_t::iterator mi = hm.find(ai->first);
               //printd(5, "qore_class_private::parseInitPartialIntern() this: %p '%s' looking for local '%s': %d\n", this, name.c_str(), ai->first.c_str(), mi != hm.end());
               m->parseMergeBase(*(ai->second), mi == hm.end() ? 0 : mi->second->getFunction());
	       if (m->empty())
		  delete m;
	       else
		  ahm.insert(amap_t::value_type(ai->first, m));
               //printd(5, "qore_class_private::parseInitPartialIntern() this: %p '%s' insert abstract method variant %s::%s()\n", this, name.c_str(), name.c_str(), ai->first.c_str());
            }
         }
      }
   }

   if (!has_new_user_changes)
      return;

   // do processing related to parent classes
   if (scl) {
      // setup inheritance list for new methods
      for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
         bool is_new = i->second->priv->func->committedEmpty();

	 //printd(5, "class_private::parseInitPartialIntern() this: %p %s::%s is_new: %d cs: %d (%s)\n", this, name.c_str(), i->first.c_str(), is_new, checkSpecial(i->second->getName()), i->second->getName());

         if (is_new && !checkSpecial(i->second->getName()))
	    parseAddAncestors(i->second);
      }

      // setup inheritance list for new static methods
      for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i) {
         bool is_new = i->second->priv->func->committedEmpty();
         if (is_new)
            parseAddStaticAncestors(i->second);
      }
   }

   {
      VariableBlockHelper vbh;
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
   // make sure initialize() is called first
   initialize();

   //printd(5, "qore_class_private::parseInit() this: %p '%s' parse_init_called: %d parse_init_partial_called: %d\n", this, name.c_str(), parse_init_called, parse_init_partial_called);
   if (parse_init_called)
      return;

   parse_init_called = true;

   if (has_new_user_changes) {
      NamespaceParseContextHelper nspch(ns);
      QoreParseClassHelper qpch(cls);

      if (!parse_init_partial_called)
	 parseInitPartialIntern();

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

   //printd(5, "qore_class_private::parseInit() this: %p cls: %p %s scl: %p\n", this, cls, name.c_str(), scl);
   // search for new concrete variants of abstract variants last
   ahm.parseInit(*this, scl);
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

const QoreExternalMethodVariant *qore_class_private::findUserMethodVariant(const char* mname, const QoreMethod*& method, const type_vec_t &argTypeList) const {
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

int qore_class_private::checkExistingVarMember(const QoreProgramLocation& loc, char *dname, bool decl_has_type_info, bool priv, const QoreClass* sclass, bool has_type_info, bool is_priv, bool var) const {
   //printd(5, "qore_class_private::checkExistingVarMember() name=%s priv=%d is_priv=%d sclass=%s\n", name.c_str(), priv, is_priv, sclass->getName());

   // here we know that the member or var already exists, so either it will be a
   // duplicate declaration, in which case it is ignored, or it is a
   // contradictory declaration, in which case a parse exception is raised

   // if the var was previously declared public
   if (priv != is_priv) {
      // raise an exception only if parse exceptions are enabled
      if (getProgram()->getParseExceptionSink()) {
	 QoreStringNode* desc = new QoreStringNode;
	 desc->sprintf("class '%s' ", name.c_str());
	 desc->concat("cannot declare ");
	 desc->sprintf("%s %s ", privpub(priv), var ? "static variable" : "member");
	 desc->sprintf("'%s' when ", dname);
	 if (sclass == cls)
	    desc->concat("this class");
	 else
	    desc->sprintf("base class '%s'", sclass->getName());
	 desc->sprintf(" already declared this %s as %s", var ? "variable" : "member", privpub(is_priv));
	 qore_program_private::makeParseException(getProgram(), loc, "PARSE-ERROR", desc);
      }
      return -1;
   }
   else if (decl_has_type_info || has_type_info) {
      if (getProgram()->getParseExceptionSink()) {
	 QoreStringNode* desc = new QoreStringNode;
	 desc->sprintf("%s %s ", privpub(priv), var ? "static variable" : "member");
	 desc->sprintf("'%s' was already declared in ", dname);
	 if (sclass == cls)
	    desc->concat("this class");
	 else
	    desc->sprintf("base class '%s'", sclass->getName());
	 if (has_type_info)
	    desc->sprintf(" with a type definition");
	 desc->concat(" and cannot be declared again");
	 desc->sprintf(" in class '%s'", name.c_str());
	 desc->concat(" if the declaration has a type definition");
	    
	 qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", desc);
      }
      return -1;
   }
      
   return 0;
}

AbstractQoreNode* qore_class_private::evalPseudoMethod(const AbstractQoreNode* n, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("qore_class_private::evalPseudoMethod()");

   const QoreMethod* m = findPseudoMethod(n, nme, xsink);
   if (!m)
      return 0;

   //printd(5, "qore_class_private::evalPseudoMethod() %s::%s() found method %p class %s\n", priv->name, nme, w, w->getClassName());

   return qore_method_private::evalPseudoMethod(m, 0, n, args, xsink);
}

int64 qore_class_private::bigIntEvalPseudoMethod(const AbstractQoreNode* n, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("qore_class_private::bigIntEvalPseudoMethod()");

   const QoreMethod* m = findPseudoMethod(n, nme, xsink);
   if (!m)
      return 0;

   //printd(5, "qore_class_private::bigIntEvalPseudoMethod() %s::%s() found method %p class %s\n", priv->name, nme, m, m->getClassName());

   return qore_method_private::bigIntEvalPseudoMethod(m, 0, n, args, xsink);
}

int qore_class_private::intEvalPseudoMethod(const AbstractQoreNode* n, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("qore_class_private::intEvalPseudoMethod()");

   const QoreMethod* m = findPseudoMethod(n, nme, xsink);
   if (!m)
      return 0;

   //printd(5, "qore_class_private::intEvalPseudoMethod() %s::%s() found method %p class %s\n", priv->name, nme, m, m->getClassName());

   return qore_method_private::intEvalPseudoMethod(m, 0, n, args, xsink);
}

bool qore_class_private::boolEvalPseudoMethod(const AbstractQoreNode* n, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("qore_class_private::boolEvalPseudoMethod()");

   const QoreMethod* m = findPseudoMethod(n, nme, xsink);
   if (!m)
      return 0;

   //printd(5, "qore_class_private::boolEvalPseudoMethod() %s::%s() found method %p class %s\n", priv->name, nme, m, m->getClassName());

   return qore_method_private::boolEvalPseudoMethod(m, 0, n, args, xsink);
}

double qore_class_private::floatEvalPseudoMethod(const AbstractQoreNode* n, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("qore_class_private::intEvalPseudoMethod()");

   const QoreMethod* m = findPseudoMethod(n, nme, xsink);
   if (!m)
      return 0;

   //printd(5, "qore_class_private::floatEvalPseudoMethod() %s::%s() found method %p class %s\n", priv->name, nme, m, m->getClassName());

   return qore_method_private::floatEvalPseudoMethod(m, 0, n, args, xsink);
}

AbstractQoreNode* qore_class_private::evalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::evalPseudoMethod(m, variant, n, args, xsink);
}

int64 qore_class_private::bigIntEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::bigIntEvalPseudoMethod(m, variant, n, args, xsink);
}

int qore_class_private::intEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::intEvalPseudoMethod(m, variant, n, args, xsink);
}

bool qore_class_private::boolEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::boolEvalPseudoMethod(m, variant, n, args, xsink);
}

double qore_class_private::floatEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::floatEvalPseudoMethod(m, variant, n, args, xsink);
}

bool qore_class_private::parseCheckPrivateClassAccess() const {
   // see if shouldBeClass is a parent class of the class currently being parsed
   QoreClass* pc = getParseClass();

   //printd(5, "qore_class_private::parseCheckPrivateClassAccess(%p '%s') pc: %p '%s' found: %p\n", this, name.c_str(), pc, pc ? pc->getName() : "n/a", pc ? pc->getClass(classID) : 0);

   if (!pc)
      return false;

   if (pc->priv->classID == classID || (pc->priv->name == name && pc->priv->hash == hash))
      return true;

   bool pv;
   return pc->priv->parseGetClass(*this, pv) || (scl && scl->getClass(*(pc->priv), pv));
}

bool qore_class_private::runtimeCheckPrivateClassAccess() const {
   const qore_class_private* qc = runtime_get_class();
   if (!qc) {
      //printd(5, "runtimeCheckPrivateClassAccess() this: %p '%s' no runtime class context: failed\n", this, name.c_str());
      return QTI_NOT_EQUAL;
   }
   //bool np = false; printd(5, "runtimeCheckPrivateClassAccess() qc: %p '%s' test: %p '%s' okl: %d okr: %d\n", qc, qc->name.c_str(), this, name.c_str(), qc->getClassIntern(*this, np), (scl && scl->getClass(*qc, np)));
   bool priv = false;
   return qc->getClassIntern(*this, priv) || (scl && scl->getClass(*qc, priv)) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
}

qore_type_result_e qore_class_private::parseCheckCompatibleClass(const qore_class_private& oc) const {
   // make sure both classes are initialized
   const_cast<qore_class_private*>(this)->initialize();
   const_cast<qore_class_private&>(oc).initialize();

   //printd(5, "qore_class_private::parseCheckCompatibleClass(%p '%s') this: %p '%s'\n", &oc, oc.name.c_str(), this, name.c_str());

   if (classID == oc.classID || (oc.name == name && oc.hash == hash))
      return QTI_IDENT;

   bool priv = false;
   if (!parseGetClass(oc, priv) && !oc.parseGetClass(*this, priv))
      return QTI_NOT_EQUAL;

   if (!priv)
      return QTI_AMBIGUOUS;

   return parseCheckPrivateClassAccess() ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
}

qore_type_result_e qore_class_private::runtimeCheckCompatibleClass(const qore_class_private& oc) const {
   if (classID == oc.classID || (oc.name == name && oc.hash == hash))
      return QTI_IDENT;

   bool priv = false;
   if (!oc.scl || !oc.scl->getClass(*this, priv))
      return QTI_NOT_EQUAL;

   if (!priv)
      return QTI_AMBIGUOUS;

   return runtimeCheckPrivateClassAccess() ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
}

bool QoreClass::hasParentClass() const {
   return (bool)priv->scl;
}

bool QoreClass::parseCheckHierarchy(const QoreClass* cls) const {
   if (cls == this)
      return true;

   return priv->scl ? priv->scl->parseCheckHierarchy(cls) : false;
}

const QoreMethod* QoreClass::getConstructor() const {
   return priv->constructor;
}

const QoreMethod* QoreClass::getSystemConstructor() const {
   return priv->system_constructor;
}

const QoreMethod* QoreClass::getDestructor() const {
   return priv->destructor;
}

const QoreMethod* QoreClass::getCopyMethod() const {
   return priv->copyMethod;
}

const QoreMethod* QoreClass::getMemberGateMethod() const {
   return priv->memberGate;
}

const QoreMethod* QoreClass::getMethodGate() const {
   return priv->methodGate;
}

const QoreMethod* QoreClass::getMemberNotificationMethod() const {
   return priv->memberNotification;
}

const QoreTypeInfo *QoreClass::getTypeInfo() const {
   return priv->getTypeInfo();
}

const QoreTypeInfo *QoreClass::getOrNothingTypeInfo() const {
   return priv->getOrNothingTypeInfo();
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

bool QoreClass::isPublicOrPrivateMember(const char* str, bool &priv_member) const {
   return (bool)priv->isPublicOrPrivateMember(str, priv_member);
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

void QoreClass::parseAssimilatePublicConstants(ConstantList &cmap) {
   priv->parseAssimilatePublicConstants(cmap);
}

void QoreClass::parseAssimilatePrivateConstants(ConstantList &cmap) {
   priv->parseAssimilatePrivateConstants(cmap);
}

/*
AbstractQoreNode* QoreClass::getConstantValue(const char* cname, const QoreTypeInfo *&typeInfo) {
   return priv->getConstantValue(cname, typeInfo);
}
*/

void QoreClass::parseAddPublicConstant(const std::string &cname, AbstractQoreNode* val) {
   priv->parseAddPublicConstant(cname, val);
}

void QoreClass::addBuiltinConstant(const char* name, AbstractQoreNode* value, bool is_priv, const QoreTypeInfo *typeInfo) {
   priv->addBuiltinConstant(name, value, is_priv, typeInfo);
}

void QoreClass::addBuiltinStaticVar(const char* name, AbstractQoreNode* value, bool is_priv, const QoreTypeInfo *typeInfo) {
   priv->addBuiltinStaticVar(name, value, is_priv, typeInfo);
}

void MethodFunctionBase::parseInit() {
   QoreFunction::parseInit();
}

void MethodFunctionBase::parseCommit() {
   QoreFunction::parseCommit();
   if (!pending_save.empty()) {
      // purge abstract variants from pending_save
      for (vlist_t::iterator i = pending_save.begin(), e = pending_save.end(); i != e; ++i)
	 (*i)->deref();
      pending_save.clear();
   }
}

void MethodFunctionBase::parseRollback() {
   QoreFunction::parseRollback();
   if (!pending_save.empty()) {
      // move abstract variants from pending_save back to vlist
      for (vlist_t::iterator i = pending_save.begin(), e = pending_save.end(); i != e; ++i)
	 vlist.push_back(*i);
      pending_save.clear();
   }
}

int MethodFunctionBase::checkFinalVariant(const MethodFunctionBase* m, const MethodVariantBase* v) const {
   if (!v->isFinal())
      return 0;

   const AbstractFunctionSignature* sig = v->getSignature(), * vs = 0;
   int rc = parseCompareResolvedSignature(pending_vlist, sig, vs);
   if (rc == QTI_NOT_EQUAL)
      return 0;

   const char* stat = isStatic() ? "static " : "";
   parse_error("'final' method %s%s::%s(%s) cannot be overridden in a child class with %s%s::%s(%s)", stat, m->qc->getName(), getName(), sig->getSignatureText(), stat, qc->getName(), getName(), vs->getSignatureText());
   return -1;
}

void MethodFunctionBase::checkFinal() const {
   // only check if we have new pending variants in this method
   if (pending_vlist.empty()) {
      //printd(5, "MethodFunctionBase::checkFinal() %s::%s() pending list is empty\n", qc->getName(), getName());
      return;
   }

   ilist_t::const_iterator i = ilist.begin(), e = ilist.end();
   ++i;
   for (; i != e; ++i) {
      const MethodFunctionBase* m = METHFB_const(*i);
      //printd(5, "MethodFunctionBase::checkFinal() base method %s::%s() pend_has_final: %d has_final: %d against child %s::%s()\n", m->qc->getName(), getName(), m->pending_has_final, m->has_final, qc->getName(), getName());
      if (m->pending_has_final) {
         for (vlist_t::const_iterator i = m->pending_vlist.begin(), e = m->pending_vlist.end(); i != e; ++i) {
            if (checkFinalVariant(m, METHVB_const(*i)))
               return;
         }
      }
      if (m->has_final) {
         for (vlist_t::const_iterator i = m->vlist.begin(), e = m->vlist.end(); i != e; ++i) {
            if (checkFinalVariant(m, METHVB_const(*i)))
               return;
         }
      }
   }
}

void MethodFunctionBase::addBuiltinMethodVariant(MethodVariantBase *variant) {
   if (all_private && !variant->isPrivate())
      all_private = false;
   if (!has_final && variant->isFinal())
      has_final = true;
   addBuiltinVariant(variant);
}

int MethodFunctionBase::parseAddUserMethodVariant(MethodVariantBase *variant) {
   int rc = addPendingVariant(variant);
   if (!rc) {
      if (pending_all_private && !variant->isPrivate())
         pending_all_private = false;
      if (!pending_has_final && variant->isFinal())
         pending_has_final = true;
   }
   return rc;
}

void MethodFunctionBase::parseCommitMethod(QoreString& csig, const char* mod) {
   // add to signature
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      MethodVariantBase* v = METHVB(*i);
      csig.concat("abstract ");
      csig.concat(v->isPrivate() ? "priv " : "pub ");
      if (mod) {
         csig.concat(mod);
         csig.concat(' ');
      }
      csig.concat(name);
      csig.concat('(');
      csig.concat(v->getSignature()->getSignatureText());
      csig.concat(')');
      csig.concat('\n');
   }

   parseCommit();
   if (!pending_all_private) {
      if (all_private)
         all_private = false;
      pending_all_private = true;
   }
   if (pending_has_final) {
      if (!has_final)
         has_final = true;
      pending_has_final = false;
   }
}

void MethodFunctionBase::parseRollbackMethod() {
   parseRollback();
   pending_all_private = true;
}

void MethodFunctionBase::replaceAbstractVariantIntern(MethodVariantBase* variant) {
   variant->ref();
   AbstractFunctionSignature& sig = *(variant->getSignature());
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      (*i)->parseResolveUserSignature();
      if ((*i)->isSignatureIdentical(sig)) {
	 (*i)->deref();
	 pending_vlist.erase(i);
	 pending_vlist.push_back(variant);
	 //printd(5, "MethodFunctionBase::replaceAbstractVariantIntern() this: %p replacing %p ::%s%s in pending_vlist\n", this, variant, getName(), variant->getAbstractSignature());
	 return;
      }
   }
   for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      if ((*i)->isSignatureIdentical(sig)) {
	 pending_save.push_back(*i);
	 vlist.erase(i);
	 vlist.push_back(variant);
	 //printd(5, "MethodFunctionBase::replaceAbstractVariantIntern() this: %p replacing %p ::%s%s in vlist\n", this, variant, getName(), variant->getAbstractSignature());
	 return;
      }
   }
   //printd(5, "MethodFunctionBase::replaceAbstractVariantIntern() this: %p adding %p ::%s%s to pending_vlist\n", this, variant, getName(), variant->getAbstractSignature());
   pending_vlist.push_back(variant);
}

void MethodFunctionBase::replaceAbstractVariant(MethodVariantBase* variant) {
   replaceAbstractVariantIntern(variant);
   if (pending_all_private && !variant->isPrivate())
      pending_all_private = false;
   if (!pending_has_final && variant->isFinal())
      pending_has_final = true;
}

// if an identical signature is found to the passed variant, then it is removed from the abstract list
MethodVariantBase* MethodFunctionBase::parseHasVariantWithSignature(MethodVariantBase* v) const {
   v->parseResolveUserSignature();
   AbstractFunctionSignature& sig = *(v->getSignature());
   for (vlist_t::const_iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      (*i)->parseResolveUserSignature();
      if ((*i)->isSignatureIdentical(sig))
	 return reinterpret_cast<MethodVariantBase*>(*i);
   }
   for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      if ((*i)->isSignatureIdentical(sig))
	 return reinterpret_cast<MethodVariantBase*>(*i);
   }
   return 0;
}

int ConstructorMethodVariant::constructorPrelude(const QoreClass &thisclass, CodeEvaluationHelper &ceh, QoreObject* self, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const {
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

   ceh.restorePosition();
   return 0;
}

UserConstructorVariant::~UserConstructorVariant() {
   delete bcal;
}

void UserConstructorVariant::parseInit(QoreFunction* f) {
   MethodFunctionBase* mf = static_cast<MethodFunctionBase*>(f);
   const QoreClass& parent_class = *(mf->MethodFunctionBase::getClass());

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
   statements->parseInitConstructor(parent_class.getTypeInfo(), this, bcal, qore_class_private::getBaseClassList(parent_class));

   // recheck types against committed types if necessary
   if (recheck)
      f->parseCheckDuplicateSignatureCommitted(&signature);
}

void UserCopyVariant::evalCopy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink* xsink) const {
   // there can only be max 1 param
   assert(signature.numParams() <= 1);

   QoreListNode* args = new QoreListNode;
   args->push(self->refSelf());
   ceh.setArgs(args);

   UserVariantExecHelper uveh(this, &ceh, false, xsink);
   if (!uveh)
      return;

   CODE_CONTEXT_HELPER(CT_USER, "copy", self, xsink);

   if (scl) {
      scl->sml.execCopyMethods(self, old, xsink);
      if (*xsink)
	 return;
      ceh.restorePosition();
   }

   ProgramThreadCountContextHelper tch(xsink, pgm, true);
   if (*xsink) return;
   discard(evalIntern(uveh.getArgv(), self, xsink), xsink);
}

void UserCopyVariant::parseInit(QoreFunction* f) {
   MethodFunctionBase* mf = static_cast<MethodFunctionBase*>(f);
   const QoreClass& parent_class = *(mf->MethodFunctionBase::getClass());

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
	       QoreStringNode* desc = new QoreStringNode("copy constructor will be passed ");
	       parent_class.getTypeInfo()->getThisType(*desc);
	       desc->concat(", but the object's parameter was defined expecting ");
	       typeInfo->getThisType(*desc);
	       desc->concat(" instead");
	       qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
	    }
	 }
      }
      else { // set to class' type
	 signature.setFirstParamType(parent_class.getTypeInfo());
      }
   }

   // only 1 variant is possible, no need to recheck types
}

void BuiltinCopyVariantBase::evalCopy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink* xsink) const {
   CODE_CONTEXT_HELPER(CT_BUILTIN, "copy", self, xsink);
   
   if (scl) {
      scl->sml.execCopyMethods(self, old, xsink);
      if (*xsink)
	 return;
      ceh.restorePosition();
   }
   
   old->evalCopyMethodWithPrivateData(thisclass, this, self, xsink);
}

void ConstructorMethodFunction::evalConstructor(const AbstractQoreFunctionVariant *variant, const QoreClass &thisclass, QoreObject* self, const QoreListNode* args, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const {
   // setup call, save runtime position, and evaluate arguments
   CodeEvaluationHelper ceh(xsink, this, variant, "constructor", args, thisclass.getName());
   if (*xsink)
      return;

   if (CONMV_const(variant)->isPrivate() && !qore_class_private::runtimeCheckPrivateClassAccess(thisclass)) {
      xsink->raiseException("CONSTRUCTOR-IS-PRIVATE", "%s::constructor(%s) is private and therefore this class cannot be directly instantiated with the new operator by external code", thisclass.getName(), variant->getSignature()->getSignatureText());
      return;
   }

   CONMV_const(variant)->evalConstructor(thisclass, self, ceh, bcl, bceal, xsink);      
}

void CopyMethodFunction::evalCopy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, BCList *scl, ExceptionSink* xsink) const {
   assert(vlist.singular());

   const AbstractQoreFunctionVariant *variant = first();
   qore_call_t ct = variant->getCallType();

   // setup call, save runtime position
   CodeEvaluationHelper ceh(xsink, this, variant, "copy", 0, thisclass.getName(), ct, true);
   if (*xsink) return;

   COPYMV_const(variant)->evalCopy(thisclass, self, old, ceh, scl, xsink);
}

void DestructorMethodFunction::evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const {
   assert(vlist.singular());

   const AbstractQoreFunctionVariant* variant = first();
   qore_call_t ct = variant->getCallType();

   // setup call, save runtime position
   CodeEvaluationHelper ceh(xsink, this, variant, "destructor", 0, thisclass.getName(), ct);
   if (*xsink) return;

   DESMV_const(variant)->evalDestructor(thisclass, self, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
AbstractQoreNode* NormalMethodFunction::evalMethod(const AbstractQoreFunctionVariant *variant, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   const MethodVariant* mv = METHV_const(variant);
   if (variant && mv->isAbstract()) {
      xsink->raiseException("ABSTRACT-VARIANT-ERROR", "cannot call abstract variant %s::%s(%s) directly", getClassName(), mname, mv->getSignature()->getSignatureText());
      return 0;
   }

   return mv->evalMethod(self, ceh, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
int64 NormalMethodFunction::bigIntEvalMethod(const AbstractQoreFunctionVariant *variant, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->bigIntEvalMethod(self, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
int NormalMethodFunction::intEvalMethod(const AbstractQoreFunctionVariant *variant, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->intEvalMethod(self, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
bool NormalMethodFunction::boolEvalMethod(const AbstractQoreFunctionVariant *variant, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->boolEvalMethod(self, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
double NormalMethodFunction::floatEvalMethod(const AbstractQoreFunctionVariant *variant, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->floatEvalMethod(self, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
AbstractQoreNode* NormalMethodFunction::evalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->evalPseudoMethod(n, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
int64 NormalMethodFunction::bigIntEvalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->bigIntEvalPseudoMethod(n, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
int NormalMethodFunction::intEvalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->intEvalPseudoMethod(n, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
bool NormalMethodFunction::boolEvalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return false;

   return METHV_const(variant)->boolEvalPseudoMethod(n, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
double NormalMethodFunction::floatEvalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0.0;

   return METHV_const(variant)->floatEvalPseudoMethod(n, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
AbstractQoreNode* StaticMethodFunction::evalMethod(const AbstractQoreFunctionVariant *variant, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->evalMethod(0, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
int64 StaticMethodFunction::bigIntEvalMethod(const AbstractQoreFunctionVariant *variant, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->bigIntEvalMethod(0, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
int StaticMethodFunction::intEvalMethod(const AbstractQoreFunctionVariant *variant, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->intEvalMethod(0, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
bool StaticMethodFunction::boolEvalMethod(const AbstractQoreFunctionVariant *variant, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->boolEvalMethod(0, ceh, xsink);      
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
double StaticMethodFunction::floatEvalMethod(const AbstractQoreFunctionVariant *variant, const QoreListNode* args, ExceptionSink* xsink) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, getClassName());
   if (*xsink) return 0;

   return METHV_const(variant)->floatEvalMethod(0, ceh, xsink);      
}

const qore_class_private* MethodVariantBase::getClassPriv() const {
   return qore_class_private::get(*(qmethod->getClass()));
}

const char* MethodVariantBase::getAbstractSignature() {
   if (asig.empty())
      getSignature()->addAbstractParameterSignature(asig);
   return asig.c_str();
}

AbstractQoreNode* BuiltinNormalMethodVariantBase::evalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
   CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), 0, xsink);
   return evalImpl(NULL, (AbstractPrivateData*)n, ceh.getArgs(), xsink);
}

int64 BuiltinNormalMethodVariantBase::bigIntEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
   CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), 0, xsink);
   return bigIntEvalImpl(NULL, (AbstractPrivateData*)n, ceh.getArgs(), xsink);
}

int BuiltinNormalMethodVariantBase::intEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
   CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), 0, xsink);
   return intEvalImpl(NULL, (AbstractPrivateData*)n, ceh.getArgs(), xsink);
}

bool BuiltinNormalMethodVariantBase::boolEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
   CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), 0, xsink);
   return boolEvalImpl(NULL, (AbstractPrivateData*)n, ceh.getArgs(), xsink);
}

double BuiltinNormalMethodVariantBase::floatEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
   CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), 0, xsink);
   return floatEvalImpl(NULL, (AbstractPrivateData*)n, ceh.getArgs(), xsink);
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
   DLLLOCAL const QoreMethod* getMethod() const {
      assert(i != m.end());
      return i->second;
   }
};
#define HMI_CAST(p) (reinterpret_cast<qmi_priv *>(p))

QoreMethodIterator::QoreMethodIterator(const QoreClass* qc) : priv(new qmi_priv(qc->priv->hm)) {
}

QoreMethodIterator::~QoreMethodIterator() {
   delete HMI_CAST(priv);
}

bool QoreMethodIterator::next() {
   return HMI_CAST(priv)->next();
}

const QoreMethod* QoreMethodIterator::getMethod() const {
   return HMI_CAST(priv)->getMethod();
}

QoreStaticMethodIterator::QoreStaticMethodIterator(const QoreClass* qc) : priv(new qmi_priv(qc->priv->shm)) {
}

QoreStaticMethodIterator::~QoreStaticMethodIterator() {
   delete HMI_CAST(priv);
}

bool QoreStaticMethodIterator::next() {
   return HMI_CAST(priv)->next();
}

const QoreMethod* QoreStaticMethodIterator::getMethod() const {
   return HMI_CAST(priv)->getMethod();
}

void QoreMemberInfo::parseInit(const char* name, bool priv) {
   if (!typeInfo) {
      typeInfo = parseTypeInfo->resolveAndDelete(loc);
      parseTypeInfo = 0;
   }
#ifdef DEBUG
   else assert(!parseTypeInfo);
#endif
   
   if (exp) {
      const QoreTypeInfo *argTypeInfo = 0;
      int lvids = 0;
      exp = exp->parseInit(0, 0, lvids, argTypeInfo);
      if (lvids) {
	 parse_error(loc, "illegal local variable declaration in member initialization expression");
	 while (lvids)
	    pop_local_var();
      }
      // throw a type exception only if parse exceptions are enabled
      if (!typeInfo->parseAccepts(argTypeInfo) && getProgram()->getParseExceptionSink()) {
	 QoreStringNode* desc = new QoreStringNode("initialization expression for ");
	 desc->sprintf("%s member '$.%s' returns ", priv ? "private" : "public", name);
	 argTypeInfo->getThisType(*desc);
	 desc->concat(", but the member was declared as ");
	 typeInfo->getThisType(*desc);
	 qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", desc);
      }
   }
}

void QoreVarInfo::parseInit(const char* name, bool priv) {
   if (!typeInfo) {
      typeInfo = parseTypeInfo->resolveAndDelete(loc);
      parseTypeInfo = 0;
   }
#ifdef DEBUG
   else assert(!parseTypeInfo);
#endif

   val.set(typeInfo);

   if (exp) {
      const QoreTypeInfo *argTypeInfo = 0;
      int lvids = 0;
      exp = exp->parseInit(0, 0, lvids, argTypeInfo);
      if (lvids) {
	 parse_error(loc, "illegal local variable declaration in class static variable initialization expression");
	 while (lvids)
	    pop_local_var();
      }
      // throw a type exception only if parse exceptions are enabled
      if (!typeInfo->parseAccepts(argTypeInfo) && getProgram()->getParseExceptionSink()) {
	 QoreStringNode* desc = new QoreStringNode("initialization expression for ");
	 desc->sprintf("%s class static variable '%s' returns ", priv ? "private" : "public", name);
	 argTypeInfo->getThisType(*desc);
	 desc->concat(", but the variable was declared as ");
	 typeInfo->getThisType(*desc);
	 qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", desc);
      }
   }
}

QoreParseClassHelper::QoreParseClassHelper(QoreClass* cls) : old(getParseClass()), oldns(cls ? parse_get_ns() : 0), rn(cls) {
   setParseClass(cls);
   if (cls)
      parse_set_ns(qore_class_private::get(*cls)->ns);
}

QoreParseClassHelper::~QoreParseClassHelper() {
   if (rn)
      parse_set_ns(oldns);
   setParseClass(old);
}
