/* -*- indent-tabs-mode: nil -*- */
/*
    QoreClass.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2018 Qore Technologies, s.r.o.

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
#include "qore/intern/Sequence.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/ConstantList.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/ql_crypto.h"
#include "qore/intern/QoreObjectIntern.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

// global class ID sequence
DLLLOCAL Sequence classIDSeq(1);

AbstractQoreClassUserData::~AbstractQoreClassUserData() {
}

QoreValue qore_method_private::evalNormalVariant(QoreObject* self, const QoreExternalMethodVariant* ev, const QoreListNode* args, ExceptionSink* xsink) const {
    const AbstractQoreFunctionVariant* variant = reinterpret_cast<const AbstractQoreFunctionVariant*>(ev);

    CodeEvaluationHelper ceh(xsink, getFunction(), variant, getName(), args, self, parent_class->priv);
    if (*xsink) return QoreValue();

    return METHV_const(variant)->evalMethod(self, ceh, xsink);
}

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
                UserSignature* sig = UMV(func->first())->getUserSignature();
                const QoreTypeInfo* t = sig->getParamTypeInfo(0);
                if (!QoreTypeInfo::parseAccepts(stringTypeInfo, t)) {
                    QoreStringNode* desc = new QoreStringNode;
                    desc->sprintf("%s::%s(%s) has an invalid signature; the first argument declared as ", parent_class->getName(), func->getName(), sig->getSignatureText());
                    QoreTypeInfo::getThisType(t, *desc);
                    desc->concat(" is not compatible with 'string'");
                    qore_program_private::makeParseException(getProgram(), *sig->getParseLocation(), "PARSE-TYPE-ERROR", desc);
                }
            }
        }
        else {
            // make sure the method doesn't override a "final" method in a base class
            func->checkFinal();
        }
    }
}

ClassAccess qore_method_private::getAccess() const {
   return func->getAccess();
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
    for (auto& i : old.vlist) {
        assert(vlist.find(i.first) == vlist.end());
        i.second->ref();
        vlist.insert(vmap_t::value_type(i.first, i.second));
    }
}

AbstractMethod::~AbstractMethod() {
    for (auto& i : vlist)
        i.second->deref();
    for (auto& i : pending_save)
        i.second->deref();
}

int AbstractMethod::parseCommit() {
    if (check_parse) {
        check_parse = false;
    }
    for (auto& i : pending_save) {
        i.second->deref();
    }
    pending_save.clear();
    return vlist.empty() ? -1 : 0;
}

// merge changes from parent class method of the same name during parse initialization
void AbstractMethod::parseMergeBase(AbstractMethod& m, bool committed) {
    //printd(5, "AbstractMethod::parseMergeBase(m: %p) this: %p m.pending_save: %d m.vlist: %d\n", &m, this, !m.pending_save.empty(), !m.vlist.empty());
    // move pending committed variants from our vlist that are in parent's pending_save list to our pending_save
    for (auto& i : m.pending_save) {
        const char* sig = i.second->getAbstractSignature();
        vmap_t::iterator vi = vlist.find(sig);
        if (vi != vlist.end()) {
            /*
            i.second->ref();
            pending_save.insert(vmap_t::value_type(sig, i.second));
            vi.second->deref();
            */
            pending_save.insert(vmap_t::value_type(sig, vi->second));
            vlist.erase(vi);
        }
    }

    // add new abstract methods from parent to our list - if they are not already in our vlist or in our pending_save list
    for (auto& i : m.vlist) {
        const char* sig = i.second->getAbstractSignature();
        //printd(5, "AbstractMethod::parseMergeBase(m: %p) this: %p checking parent: '%s'\n", &m, this, sig);
        if (pending_save.find(sig) != pending_save.end()) {
            continue;
        }
        if (vlist.find(sig) != vlist.end()) {
            continue;
        }
        //printd(5, "AbstractMethod::parseMergeBase(m: %p) this: %p adding to vlist from parent: '%s'\n", &m, this, sig);
        i.second->ref();
        vlist.insert(vmap_t::value_type(sig, i.second));
    }
}

// merge changes from parent class method of the same name during parse initialization
void AbstractMethod::parseMergeBase(AbstractMethod& m, MethodFunctionBase* f, bool committed) {
    //printd(5, "AbstractMethod::parseMergeBase(m: %p, f: %p %s::%s) this: %p m.pending_save: %d m.vlist: %d\n", &m, f, f ? f->getClassName() : "n/a", f ? f->getName() : "n/a", this, !m.pending_save.empty(), !m.vlist.empty());
    // move pending committed variants from our vlist that are in parent's pending_save list to our pending_save
    for (auto& i : m.pending_save) {
        const char* sig = i.second->getAbstractSignature();
        vmap_t::iterator vi = vlist.find(sig);
        if (vi != vlist.end()) {
            /*
            pending_save.insert(vmap_t::value_type(sig, i.second));
            */
            pending_save.insert(vmap_t::value_type(sig, vi->second));
            vlist.erase(vi);
        }
    }

    // add new abstract methods from parent to our list - if they are not already in our vlist or in our pending_save list
    for (auto& i : m.vlist) {
        const char* sig = i.second->getAbstractSignature();
        //printd(5, "AbstractMethod::parseMergeBase(m: %p, f: %p %s::%s) this: %p checking parent: '%s' (f: %p: %d) '%s'\n", &m, f, f ? f->getClassName() : "n/a", f ? f->getName() : "n/a", this, sig, f, f && f->parseHasVariantWithSignature(i.second), sig);

        if (f && f->parseHasVariantWithSignature(i.second)) {
            // add to our pending_save
            i.second->ref();
            pending_save.insert(vmap_t::value_type(sig, i.second));
            continue;
        }

        if (pending_save.find(sig) != pending_save.end()) {
            continue;
        }
        if (vlist.find(sig) != vlist.end()) {
            continue;
        }
        //printd(5, "AbstractMethod::parseMergeBase(m: %p, f: %p %s::%s) this: %p adding to vlist from parent: '%s'\n", &m, f, f ? f->getClassName() : "n/a", f ? f->getName() : "n/a", this, sig);
        i.second->ref();
        vlist.insert(vmap_t::value_type(sig, i.second));
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
    vlist.insert(vmap_t::value_type(sig, v));

    if (!check_parse) {
        check_parse = true;
    }
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

void AbstractMethod::checkAbstract(const char* cname, const char* mname, vmap_t& vlist, QoreStringNode*& desc) {
    //printd(5, "AbstractMethod::checkAbstract() checking %s::%s() vlist: %d\n", cname, mname, !vlist.empty());
    if (!vlist.empty()) {
        if (!desc) {
            desc = new QoreStringNodeMaker("class '%s' cannot be instantiated because it has the following unimplemented abstract variants:", cname);
        }
        for (auto& vi : vlist) {
            MethodVariantBase* v = vi.second;
            desc->sprintf("\n * abstract %s %s::%s(%s);", QoreTypeInfo::getName(v->getReturnTypeInfo()), cname, mname, v->getSignature()->getSignatureText());
        }
    }
}

// try to find match non-abstract variants in base classes (allows concrete variants to be inherited from another parent class)
void AbstractMethodMap::parseInit(qore_class_private& qc, BCList* scl) {
    //printd(5, "AbstractMethodMap::parseInit() this: %p cname: %s scl: %p ae: %d\n", this, qc.name.c_str(), scl, empty());
    for (amap_t::iterator i = begin(), e = end(); i != e;) {
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
        // issue #2741: remove the abstract method if there are no more abstract variants
        if (i->second->vlist.empty()) {
            delete i->second;
            erase(i++);
        }
        else {
            ++i;
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
        m->vlist.insert(vmap_t::value_type(sig, f));
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

DLLLOCAL QoreStringNode* AbstractMethodMap::checkAbstract(const char* name) const {
    if (empty())
        return nullptr;

    QoreStringNode* desc = nullptr;
    for (auto& i : *this) {
        AbstractMethod::checkAbstract(name, i.first.c_str(), i.second->vlist, desc);
    }

    //printd(5, "AbstractMethodMap::parseCheckAbstract() class: %s desc: %p (%s)\n", name, desc, desc ? desc->getBuffer() : "n/a");
    return desc;
}

int AbstractMethodMap::runtimeCheckInstantiateClass(const char* name, ExceptionSink* xsink) const {
    QoreStringNode* desc = checkAbstract(name);
    if (desc) {
        xsink->raiseException("ABSTRACT-CLASS-ERROR", desc);
        return -1;
    }
    return 0;
}

// we check if there are any abstract method variants still in the committed lists
void AbstractMethodMap::parseCheckAbstractNew(const QoreProgramLocation* loc, const char* name) const {
    QoreStringNode* desc = checkAbstract(name);
    if (desc)
        parseException(*loc, "ABSTRACT-CLASS-ERROR", desc);
}

// FIXME: check private method variant access at runtime

struct SelfLocalVarParseHelper {
    DLLLOCAL SelfLocalVarParseHelper(LocalVar* selfid) { push_local_var(selfid, &loc_builtin); }
    DLLLOCAL ~SelfLocalVarParseHelper() { pop_local_var(); }
};

void raise_nonexistent_method_call_warning(const QoreProgramLocation* loc, const QoreClass* qc, const char* method) {
   qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_NONEXISTENT_METHOD_CALL, "NON-EXISTENT-METHOD-CALL", "call to non-existent method '%s::%s()'; this call will be evaluated at run-time, so if the method is called on an object of a subclass that implements this method, then it could be a valid call, however in any other case it will result in a run-time exception.  To avoid seeing this warning, use the cast<> operator (note that if the cast is invalid at run-time, a run-time exception will be raised) or turn off the warning by using '%%disable-warning non-existent-method-call' in your code", qc->getName(), method);
}

class VRMutexHelper {
private:
   VRMutex* m;

public:
   DLLLOCAL VRMutexHelper(VRMutex* n_m, ExceptionSink* xsink) : m(n_m) {
      if (m && m->enter(xsink))
         m = 0;
   }
   DLLLOCAL ~VRMutexHelper() {
      if (m)
         m->exit();
   }
   DLLLOCAL operator bool() const { return m != 0; }
};

qore_class_private::qore_class_private(QoreClass* n_cls, std::string&& nme, int64 dom, QoreTypeInfo* n_typeInfo)
   : name(nme),
     cls(n_cls),
     constlist(this),        // constants
     classID(classIDSeq.next()),
     methodID(classID),
     sys(false),
     initialized(false),
     static_init(false),
     parse_init_called(false),
     parse_init_partial_called(false),
     has_delete_blocker(false),
     has_public_memdecl(false),
     pending_has_public_memdecl(false),
     owns_typeinfo(n_typeInfo ? false : true),
     resolve_copy_done(false),
     has_new_user_changes(false),
     has_sig_changes(false),
     owns_ornothingtypeinfo(false),
     pub(false),
     final(false),
     inject(false),
     gate_access(false),
     committed(false),
     parse_resolve_hierarchy(false),
     parse_resolve_abstract(false),
     domain(dom),
     num_methods(0),
     num_user_methods(0),
     num_static_methods(0),
     num_static_user_methods(0),
     typeInfo(n_typeInfo ? n_typeInfo : new QoreClassTypeInfo(cls, name.c_str())),
     orNothingTypeInfo(nullptr),
     selfid("self", typeInfo) {
   assert(methodID == classID);
   assert(!name.empty());

   printd(5, "qore_class_private::qore_class_private() this: %p creating '%s' ID:%d cls: %p pub: %d sys: %d\n", this, name.c_str(), classID, cls, pub, sys);
}

// only called while the parse lock for the QoreProgram owning "old" is held
qore_class_private::qore_class_private(const qore_class_private& old, QoreProgram* spgm, const char* new_name, bool inject, const qore_class_private* injectedClass)
   : name(new_name ? new_name : old.name),
     ahm(old.ahm),
     constlist(old.constlist, 0, this),    // committed constants
     classID(old.classID),
     methodID(old.methodID),
     sys(old.sys),
     initialized(true),
     static_init(true),
     parse_init_called(false),
     parse_init_partial_called(false),
     has_public_memdecl(old.has_public_memdecl),
     pending_has_public_memdecl(false),
     owns_typeinfo(false),
     resolve_copy_done(false),
     has_new_user_changes(false),
     has_sig_changes(false),
     owns_ornothingtypeinfo(false),
     pub(old.pub),
     final(old.final),
     inject(inject),
     gate_access(old.gate_access),
     committed(true),
     parse_resolve_hierarchy(true),
     parse_resolve_abstract(true),
     domain(old.domain),
     num_methods(old.num_methods),
     num_user_methods(old.num_user_methods),
     num_static_methods(old.num_static_methods),
     num_static_user_methods(old.num_static_user_methods),
     typeInfo(old.typeInfo),
     orNothingTypeInfo(old.orNothingTypeInfo),
     injectedClass(injectedClass),
     selfid(old.selfid),
     hash(old.hash),
     ptr(old.ptr),
     mud(old.mud ? old.mud->copy() : nullptr),
     spgm(spgm ? spgm->programRefSelf() : nullptr) {
    QORE_TRACE("qore_class_private::qore_class_private(const qore_class_private& old)");
    if (!old.initialized)
        const_cast<qore_class_private&>(old).initialize();

    // must set after old class has been initialized
    has_delete_blocker = old.has_delete_blocker;

    // create new class object
    cls = new QoreClass(this);

    printd(5, "qore_class_private::qore_class_private() this: %p creating copy of '%s' (old: '%s') ID:%d cls: %p old: %p sys: %d\n", this, name.c_str(), old.name.c_str(), classID, cls, old.cls, sys);

    system_constructor = old.system_constructor ? old.system_constructor->copy(cls) : nullptr;
    deleteBlocker = old.deleteBlocker ? old.deleteBlocker->copy(cls) : nullptr;

    // set pointer to new copy
    old.new_copy = cls;

    // copy parent class list, if any, after new_copy is set in old
    scl = old.scl ? new BCList(*old.scl) : nullptr;

    //printd(5, "qore_class_private::qore_class_private() old name: %s (%p) new name: %s (%p)\n", old.name.c_str(), old.name.c_str(), name.c_str(), name.c_str());

    // copy methods and maintain method pointers
    for (auto& i : old.hm) {
        QoreMethod* nf = i.second->copy(cls);

        hm[nf->getName()] = nf;
        if (i.second == old.constructor)
            constructor  = nf;
        else if (i.second == old.destructor)
            destructor   = nf;
        else if (i.second == old.copyMethod)
            copyMethod   = nf;
        else if (i.second == old.methodGate)
            methodGate   = nf;
        else if (i.second == old.memberGate)
            memberGate   = nf;
        else if (i.second == old.memberNotification)
            memberNotification = nf;
    }

    // copy static methods
    for (auto& i : old.shm) {
        QoreMethod* nf = i.second->copy(cls);
        shm[nf->getName()] = nf;
    }

    // copy member list
    for (QoreMemberMap::DeclOrderIterator i = old.members.beginDeclOrder(), e = old.members.endDeclOrder(); i != e; ++i)
        members.addNoCheck(strdup(i->first), i->second ? i->second->copy(i->first, this) : 0);

    // copy static var list
    for (QoreVarMap::DeclOrderIterator i = old.vars.beginDeclOrder(), e = old.vars.endDeclOrder(); i != e; ++i)
        vars.addNoCheck(strdup(i->first), i->second ? i->second->copy(i->first) : 0);
}

qore_class_private::~qore_class_private() {
    printd(5, "qore_class_private::~qore_class_private() this: %p %s\n", this, name.c_str());

    if (spgm) {
        spgm->deref(nullptr);
        spgm = nullptr;
    }
    //assert(!spgm);

    assert(!refs.reference_count());
    assert(!var_refs.reference_count());
    assert(!const_refs.reference_count());

    if (!vars.empty()) {
        vars.del();
    }

    // delete normal methods
    for (auto& i : hm) {
        //printd(5, "qore_class_private::~qore_class_private() deleting method %p %s::%s()\n", m, name, m->getName());
        delete i.second;
    }

    // delete static methods
    for (auto& i : shm) {
        //printd(5, "qore_class_private::~qore_class_private() deleting static method %p %s::%s()\n", m, name, m->getName());
        delete i.second;
    }

    delete scl;
    delete system_constructor;

    if (owns_typeinfo)
        delete typeInfo;

    if (owns_ornothingtypeinfo)
        delete orNothingTypeInfo;

    if (mud)
        mud->doDeref();
}

void qore_class_private::addBuiltinStaticVar(const char* vname, QoreValue value, ClassAccess access, const QoreTypeInfo* vTypeInfo) {
    assert(!vars.inList(vname));

    if (!sys) {
        sys = committed = true;
    }
    if (!has_sig_changes) {
        has_sig_changes = true;
    }

    vars.addNoCheck(strdup(vname), new QoreVarInfo(&loc_builtin, vTypeInfo, 0, value, access));
}

const QoreMethod* qore_class_private::doParseMethodAccess(const QoreMethod* m, const qore_class_private* class_ctx) {
   assert(m);
   ClassAccess ma = qore_method_private::getAccess(*m);
   return ((ma == Public) || ((ma == Private && class_ctx))) ? m : 0;
}

void qore_class_private::initialize() {
    //printd(5, "qore_class_private::initialize() this: %p '%s' initialized: %d scl: %p\n", this, name.c_str(), initialized, scl);
    if (initialized)
        return;

    initializeIntern();
}

// issue #2657: initialize class hierarchy first before initializing code and members
int qore_class_private::initializeHierarchy(qcp_set_t& qcp_set) {
    if (scl) {
        if (scl->initializeHierarchy(cls, qcp_set)) {
            initialized = true;
            return -1;
        }
    }
    return 0;
}

// process signature entries for base classes
static void do_sig(QoreString& csig, BCNode& n) {
   qore_class_private* qc = qore_class_private::get(*n.sclass);
   csig.sprintf("inherits %s %s ", privpub(n.getAccess()), qc->name.c_str());
   SignatureHash& h = qc->hash;
   if (h) {
      csig.concat('[');
      h.toString(csig);
      csig.concat("]\n");
   }
   else
      csig.sprintf("{%d}\n", qc->classID);
}

// process signature entries for class members
static void do_sig(QoreString& csig, QoreMemberMap::SigOrderIterator i) {
    if (i->second)
        csig.sprintf("%s mem %s %s %s\n", privpub(i->second->access), QoreTypeInfo::getName(i->second->getTypeInfo()), i->first, i->second->exp.getTypeName());
    else
        csig.sprintf("%s mem %s\n", privpub(i->second->access), i->first);
}

// process signature entries for class static vars
static void do_sig(QoreString& csig, QoreVarMap::SigOrderIterator i) {
    if (i->second)
        csig.sprintf("%s var %s %s %s\n", privpub(i->second->access), QoreTypeInfo::getName(i->second->getTypeInfo()), i->first, i->second->exp.getTypeName());
    else
        csig.sprintf("%s var %s\n", privpub(i->second->access), i->first);
}

// process signature entries for class constants
static void do_sig(QoreString& csig, ConstantList& clist) {
    ConstantListIterator cli(clist);
    while (cli.next())
        csig.sprintf("%s const %s %s\n", privpub(cli.getAccess()), cli.getName().c_str(), cli.getValue().getTypeName());
}

int qore_class_private::initializeIntern() {
    //printd(5, "qore_class_private::initializeIntern() this: %p %s class: %p scl: %p initialized: %d\n", this, name.c_str(), cls, scl, initialized);
    if (initialized)
        return 0;

    initialized = true;

    assert(!name.empty());
    //printd(5, "qore_class_private::initializeIntern() %s class: %p scl: %p\n", name.c_str(), cls, scl);

    // initialize static vars
    if (scl) {
        bool hdb = has_delete_blocker;
        int rc = scl->initialize(cls, hdb);
        has_delete_blocker = hdb;
        if (rc)
            return -1;
    }

    QoreParseClassHelper qpch(cls);

    // first resolve types in pending variants in all method signatures (incl. return types)
    // since abstract method functions are copied by reference from the normal list; this resolves all pending
    // method function signatures as well
    for (auto& i : hm)
        i.second->priv->func->resolvePendingSignatures();
    for (auto& i : shm)
        i.second->priv->func->resolvePendingSignatures();

    QoreProgram* pgm = getProgram();
    if (pgm && !sys && (qore_program_private::parseAddDomain(pgm, domain)))
        parseException(*loc, "ILLEGAL-CLASS-DEFINITION", "class '%s' inherits functionality from base classes that is restricted by current parse options", name.c_str());

    // signature string - also processed in parseCommit()
    QoreString csig;

    // initialize parent classes
    if (scl) {
        mergeAbstract();
        // add base classes to signature
        if (has_sig_changes) {
            for (auto& i : *scl) {
                // there could have been a parse failure and a pending rollback here
                // so BCNode::sclass could be null here
                if ((*i).sclass) {
                    assert((*i).sclass->priv->initialized);
                    do_sig(csig, *i);
                }
            }
        }
    }

    if (has_sig_changes) {
        // add methods to class signature
        // pending "normal" (non-static) method variants
        for (auto& i : hm)
            i.second->priv->func->parseSignatures(csig, nullptr);
        // pending static method variants
        for (auto& i : shm)
            i.second->priv->func->parseSignatures(csig, "static");
    }

    // add committed vars to signature first before members
    if (!vars.empty()) {
        for (QoreVarMap::SigOrderIterator i = vars.beginSigOrder(), e = vars.endSigOrder(); i != e; ++i) {
            do_sig(csig, i);
        }

        if (!sys) {
            VariableBlockHelper vbh;

            // initialize new static vars
            for (QoreVarMap::SigOrderIterator i = vars.beginSigOrder(), e = vars.endSigOrder(); i != e; ++i) {
                if (i->second)
                    i->second->parseInit(i->first);
            }
        }
    }

    if (!members.empty()) {
        // add committed members to signature
        for (QoreMemberMap::SigOrderIterator i = members.beginSigOrder(), e = members.endSigOrder(); i != e; ++i) {
            do_sig(csig, i);
        }

        if (!sys) {
            SelfLocalVarParseHelper slvph(&selfid);

            // add committed members to signature
            for (QoreMemberMap::SigOrderIterator i = members.beginSigOrder(), e = members.endSigOrder(); i != e; ++i) {
                // check new members for conflicts in base classes
                parseCheckMemberInBaseClasses(i->first, i->second);
            }

            // initialize new members
            //printd(5, "qore_class_private::initializeIntern() this: %p '%s' initialing members: %p\n", this, name.c_str(), &members);
            members.parseInit();
        }
    }

    if (has_sig_changes) {
        // process constants for class signature, private first, then public
        do_sig(csig, constlist);

        if (!csig.empty()) {
            printd(5, "qore_class_private::initializeIntern() this: %p '%s' sig:\n%s", this, name.c_str(), csig.getBuffer());
            hash.update(csig);
        }

        has_sig_changes = false;
    }
    else
        assert(csig.empty());

    return 0;
}

void qore_class_private::mergeAbstract() {
    assert(scl);
    // merge direct base class abstract method lists to ourselves
    for (auto& i : *scl) {
        if ((*i).sclass) {
            assert((*i).sclass->priv->initialized);

            // called during class initialization to copy committed abstract variants to our variant lists
            AbstractMethodMap& mm = (*i).sclass->priv->ahm;
            //printd(5, "qore_class_private::initializeIntern() this: %p '%s' parent: %p '%s' mm empty: %d\n", this, name.c_str(), (*i).sclass, (*i).sclass->getName(), (int)mm.empty());
            for (auto& j : mm) {
                // skip if vlists are empty
                if (j.second->vlist.empty()) {
                    //printd(5, "qore_class_private::initializeIntern() this: %p '%s' skipping %s::%s(): vlist empty\n", this, name.c_str(), (*i).sclass->getName(), j.first.c_str());
                    continue;
                }
                amap_t::iterator vi = ahm.find(j.first);
                if (vi != ahm.end()) {
                    vi->second->parseMergeBase(*(j.second), true);
                    continue;
                }
                // now we import the abstract method to our class
                std::unique_ptr<AbstractMethod> m(new AbstractMethod);
                // see if there are pending normal variants...
                hm_method_t::iterator mi = hm.find(j.first);
                // merge committed parent abstract variants with any pending local variants
                m->parseMergeBase((*j.second), mi == hm.end() ? 0 : mi->second->getFunction(), true);
                if (!m->empty()) {
                    ahm.insert(amap_t::value_type(j.first, m.release()));
                    //printd(5, "qore_class_private::initializeIntern() this: %p '%s' insert abstract method variant %s::%s()\n", this, name.c_str(), (*i).sclass->getName(), j.first.c_str());
                }
            }
        }
    }
}

void qore_class_private::finalizeBuiltin(const char* nspath) {
   initializeBuiltin();
   generateBuiltinSignature(nspath);
}

void qore_class_private::initializeBuiltin() {
   assert(sys);
   if (!initialized) {
      initialized = true;
      if (scl) {
         // initialize builtin parent classes first
         scl->initializeBuiltin();
         // merge abstract variants from parent classes to this class
         mergeAbstract();
      }
   }
}

void qore_class_private::generateBuiltinSignature(const char* nspath) {
   // signature string - also processed in parseCommit()
   QoreStringMaker csig("class %s::%s ", nspath, name.c_str());

   // add base classes to signature
   if (scl) {
      for (auto& i : *scl) {
         assert((*i).sclass);
         assert((*i).sclass->priv->initialized);
         do_sig(csig, *i);
      }
   }

   for (auto& i : hm)
      i.second->priv->func->parseSignatures(csig, nullptr);
   for (auto& i : shm)
      i.second->priv->func->parseSignatures(csig, "static");

   // add committed vars to signature first before members
   for (QoreVarMap::SigOrderIterator i = vars.beginSigOrder(), e = vars.endSigOrder(); i != e; ++i) {
      do_sig(csig, i);
   }

   for (QoreMemberMap::SigOrderIterator i = members.beginSigOrder(), e = members.endSigOrder(); i != e; ++i) {
      do_sig(csig, i);
   }

   do_sig(csig, constlist);
   hash.update(csig);
}

// returns a non-static method if it exists in the local class and has been committed to the class
QoreMethod* qore_class_private::findLocalCommittedMethod(const char* nme) {
    QoreMethod* m = parseFindLocalMethod(nme);
    return m && !m->priv->func->committedEmpty() ? m : nullptr;
}

// returns a non-static method if it exists in the local class and has been committed to the class
const QoreMethod* qore_class_private::findLocalCommittedMethod(const char* nme) const {
    const QoreMethod* m = parseFindLocalMethod(nme);
    return m && !m->priv->func->committedEmpty() ? m : nullptr;
}

// returns a static method if it exists in the local class and has been committed to the class
QoreMethod* qore_class_private::findLocalCommittedStaticMethod(const char* nme) {
    QoreMethod* m = parseFindLocalStaticMethod(nme);
    return m && !m->priv->func->committedEmpty() ? m : nullptr;
}

// returns a static method if it exists in the local class and has been committed to the class
const QoreMethod* qore_class_private::findLocalCommittedStaticMethod(const char* nme) const {
    const QoreMethod* m = parseFindLocalStaticMethod(nme);
    return m && !m->priv->func->committedEmpty() ? m : nullptr;
}

int qore_class_private::initMembers(QoreObject& o, bool& need_scan, ExceptionSink* xsink) const {
    assert(xsink);
    if (members.empty() && !scl)
        return 0;

#ifdef QORE_MANAGE_STACK
    if (check_stack(xsink))
        return -1;
#endif

    // make sure the object context is set before evaluating members
    CodeContextHelperBase cch("constructor", &o, this, xsink, false);
    SelfInstantiatorHelper sih(&selfid, &o);

    return runtimeInitMembers(o, need_scan, false, xsink);
}

int qore_class_private::runtimeInitMembers(QoreObject& o, bool& need_scan, bool internal_only, ExceptionSink* xsink) const {
    if (scl && scl->sml.runtimeInitInternalMembers(o, need_scan, xsink))
        return -1;

    return runtimeInitLocalMembers(o, need_scan, internal_only, xsink);
}

int qore_class_private::runtimeInitLocalMembers(QoreObject& o, bool& need_scan, bool internal_only, ExceptionSink* xsink) const {
    //printd(5, "qore_class_private::runtimeInitLocalMembers() this: %p '%s' o: %p size: %d\n", this, name.c_str(), &o, (int)members.size());

    if (!members.empty()) {
        for (QoreMemberMap::DeclOrderIterator i = members.beginDeclOrder(), e = members.endDeclOrder(); i != e; ++i) {
            if (!i->second || (i->second && internal_only && i->second->access != Internal))
                continue;

            QoreValue& v = qore_object_private::get(o)->getMemberValueRefForInitialization(i->first, i->second->access == Internal ? this : nullptr);
            assert(v.isNothing());
            if (!i->second->exp.isNothing()) {
                // set runtime location
                QoreProgramLocationHelper l(i->second->loc);
                ValueEvalRefHolder val(i->second->exp, xsink);
                if (*xsink)
                    return -1;
                if (QoreTypeInfo::mayRequireFilter(i->second->getTypeInfo(), *val)) {
                    val.ensureReferencedValue();
                    QoreTypeInfo::acceptInputMember(i->second->getTypeInfo(), i->first, *val, xsink);
                    if (*xsink)
                        return -1;
                }
                val.sanitize();
                v = val.takeReferencedValue();
                if (needs_scan(v)) {
                    qore_object_private::incScanCount(o, 1);
                    if (!need_scan)
                        need_scan = true;
                }
                //printd(5, "qore_class_private::initMembers() '%s' obj: %d v: '%s' (%p) refs: %d exp: %p refs: %d\n", i->first, needs_scan(v), v.getTypeName(), v.getInternalNode(), v.hasNode() ? v.getInternalNode()->reference_count() : 0, i->second->exp, i->second->exp->reference_count());
            }
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
            else {
                v = QoreTypeInfo::getDefaultQoreValue(i->second->getTypeInfo());
            }
#endif
        }
    }

    return 0;
}

void qore_class_private::execBaseClassConstructor(QoreObject* self, BCEAList* bceal, ExceptionSink* xsink) const {
   //printd(5, "qore_class_private::execBaseClassConstructor() '%s' constructor: %p\n", name.c_str(), constructor);
   // if there is no constructor, execute the superclass constructors directly
   if (!constructor){
      if (scl) // execute base class constructors if any
         scl->execConstructors(self, bceal, xsink);

      return;
   }
   // no lock is sent with constructor, because no variable has been assigned yet
   bool already_executed;
   const AbstractQoreFunctionVariant* variant;
   const QoreProgramLocation* aloc = nullptr;
   QoreListNode* args = bceal->findArgs(cls->getID(), &already_executed, variant, aloc);
   if (!already_executed) {
      QoreProgramOptionalLocationHelper plh(aloc);
      constructor->priv->evalConstructor(variant, self, args, bceal, xsink);
   }
}

QoreObject* qore_class_private::execConstructor(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) const {
#ifdef DEBUG
    // instantiation checks have to be made at parse time
    for (auto& i : ahm) {
        printd(0, "qore_class_private::execConstructor() %s::constructor() abstract error '%s':\n", name.c_str(), i.first.c_str());
        vmap_t& v = i.second->vlist;
        for (auto& vi : v) {
            printd(0, " + vlist: %s\n", vi.first);
        }
        v = i.second->pending_save;
        for (auto& vi : v) {
            printd(0, " + pending_save: %s\n", vi.first);
        }
    }
    assert(ahm.empty());
#endif

    // create new object
    QoreObject* self = new QoreObject(cls, getProgram());

    ReferenceHolder<BCEAList> bceal(scl ? new BCEAList : nullptr, xsink);

    printd(5, "qore_class_private::execConstructor() class: %p %s::constructor() o: %p variant: %p\n", cls, name.c_str(), self, variant);

    // if we made at least one assignment, then scan the object for recursive references after all assignments
    bool need_scan = false;

    // instantiate members first
    initMembers(*self, need_scan, xsink);

    // scan object for recursive references after all member assignments
    if (need_scan) {
        LValueHelper lvh(*self, xsink);
    }

    if (!*xsink) {
        // it's possible for constructor = 0 and variant != 0, when a class is instantiated to initialize a constant
        // and the matched variant is pending
        if (!constructor && !variant) {
            if (scl) { // execute superconstructors if any
                CodeContextHelper cch(xsink, CT_BUILTIN, "constructor", self, this);

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
            printd(5, "qore_class_private::execConstructor() class: %p %s done\n", cls, name.c_str());
        }
    }

    if (*xsink) {
        // instead of executing the destructors for the superclasses that were already executed we call QoreObject::obliterate()
        // which will clear out all the private data by running their dereference methods which must be OK
        self->obliterate(xsink);
        printd(5, "qore_class_private::execConstructor() this: %p %s::constructor() o: %p, exception in constructor, obliterating QoreObject and returning 0\n", this, name.c_str(), self);
        return 0;
    }

    printd(5, "qore_class_private::execConstructor() this: %p %s::constructor() returning o: %p\n", this, name.c_str(), self);
    return self;
}

void qore_class_private::parseCommit() {
    //printd(5, "qore_class_private::parseCommit() %s this: %p cls: %p hm.size: %d sys: %d committed: %d\n", name.c_str(), this, cls, hm.size(), sys, committed);
    if (committed) {
        return;
    }

    if (!sys) {
        committed = true;

        if (parse_init_called)
            parse_init_called = false;

        if (parse_init_partial_called)
            parse_init_partial_called = false;

        if (has_new_user_changes) {
            // signature string: note the signature is updated in two places, here and in initializeIntern()
            QoreString csig;

            // add parent classes to signature if creating for the first time
            if (has_sig_changes && scl) {
                for (auto& i : *scl) {
                    assert((*i).sclass);
                    (*i).sclass->priv->parseCommit();
                    do_sig(csig, *i);
                }
            }

            // commit pending "normal" (non-static) method variants
            for (auto& i : hm) {
                bool is_new = i.second->priv->func->committedEmpty();
                if (has_sig_changes)
                    i.second->priv->func->parseCommitMethod(csig, 0);
                else
                    i.second->priv->func->parseCommitMethod();
                if (is_new) {
                    checkAssignSpecial(i.second);
                    ++num_methods;
                    ++num_user_methods;
                }
            }

            // commit pending static method variants
            for (auto& i : shm) {
                bool is_new = i.second->priv->func->committedEmpty();
                if (has_sig_changes)
                    i.second->priv->func->parseCommitMethod(csig, "static");
                else
                    i.second->priv->func->parseCommitMethod();
                if (is_new) {
                    ++num_static_methods;
                    ++num_static_user_methods;
                }
            }

            // commit abstract method variant list changes
            ahm.parseCommit();

            if (has_sig_changes) {
                // add all static vars to signature
                for (QoreVarMap::SigOrderIterator i = vars.beginSigOrder(), e = vars.endSigOrder(); i != e; ++i) {
                    do_sig(csig, i);
                }

                for (QoreMemberMap::SigOrderIterator i = members.beginSigOrder(), e = members.endSigOrder(); i != e; ++i) {
                    do_sig(csig, i);
                }
            }

            // set flags
            if (pending_has_public_memdecl) {
                if (!has_public_memdecl)
                    has_public_memdecl = true;
                pending_has_public_memdecl = false;
            }

            // process constants for signature
            if (has_sig_changes) {
                do_sig(csig, constlist);
            }

            // if there are any signature changes, then change the class' signature
            if (has_sig_changes) {
                if (!csig.empty()) {
                    printd(5, "qore_class_private::parseCommit() this:%p '%s' sig:\n%s", this, name.c_str(), csig.getBuffer());
                    hash.update(csig);
                }
                has_sig_changes = false;
            }
            else {
                assert(csig.empty());
            }

            has_new_user_changes = false;
        }
        else {
#ifdef DEBUG
            for (auto& i : hm)
                assert(i.second->priv->func->pendingEmpty());
            for (auto& i : shm)
                assert(i.second->priv->func->pendingEmpty());
#endif
            assert(!pending_has_public_memdecl);
        }
    }
    else {
        assert(committed);
        assert(!pending_has_public_memdecl);
    }

   if (!hash)
      hash.updateEmpty();

   // we check base classes if they have public members if we don't have any
   // it's safe to call parseHasPublicMembersInHierarchy() because the 2nd stage
   // of parsing has completed without any errors (or we wouldn't be
   // running parseCommit())
   if (!has_public_memdecl && (scl ? scl->parseHasPublicMembersInHierarchy() : false))
      has_public_memdecl = true;
}

void qore_class_private::parseCommitRuntimeInit(ExceptionSink* xsink) {
    // issue #2885: ensure that static class initialization is only performed once
    if (!static_init) {
        static_init = true;
        // add all pending static vars to real list and initialize them
        vars.parseCommitRuntimeInit(xsink);
    }
}

void qore_class_private::addBuiltinMethod(const char* mname, MethodVariantBase* variant) {
    assert(strcmp(mname, "constructor"));
    assert(strcmp(mname, "destructor"));
    assert(strcmp(mname, "copy"));

    if (!sys) {
        sys = committed = true;
    }

    hm_method_t::iterator i = hm.find(mname);
    QoreMethod* nm;
    if (i == hm.end()) {
        MethodFunctionBase* m = new BuiltinNormalMethod(cls, mname);
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

    if (!sys) {
        sys = committed = true;
    }

    hm_method_t::iterator i = shm.find(mname);
    QoreMethod* nm;
    if (i == shm.end()) {
        MethodFunctionBase* m = new BuiltinStaticMethod(cls, mname);
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

void qore_class_private::addBuiltinConstructor(BuiltinConstructorVariantBase* variant) {
    if (!sys) {
        sys = committed = true;
    }

    QoreMethod* nm;
    if (!constructor) {
        MethodFunctionBase* m = new ConstructorMethodFunction(cls);
        nm = new QoreMethod(cls, m, false);
        constructor = nm;
        insertBuiltinMethod(nm, true);
    }
    else {
        nm = const_cast<QoreMethod*>(constructor);
    }

    // set the pointer from the variant back to the owning method
    variant->setMethod(nm);

    nm->priv->addBuiltinVariant(variant);
}

void qore_class_private::addBuiltinDestructor(BuiltinDestructorVariantBase* variant) {
    if (!sys) {
        sys = committed = true;
    }

    assert(!destructor);
    DestructorMethodFunction *m = new DestructorMethodFunction(cls);
    QoreMethod* qm = new QoreMethod(cls, m, false);
    destructor = qm;
    insertBuiltinMethod(qm, true);
    // set the pointer from the variant back to the owning method
    variant->setMethod(qm);

    qm->priv->addBuiltinVariant(variant);
}

void qore_class_private::addBuiltinCopyMethod(BuiltinCopyVariantBase* variant) {
    if (!sys) {
        sys = committed = true;
    }

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
   BuiltinDeleteBlocker* m = new BuiltinDeleteBlocker(func);
   QoreMethod* qm = new QoreMethod(cls, m, false);
   qm->priv->setBuiltin();
   deleteBlocker = qm;
   insertBuiltinMethod(qm, true);
   has_delete_blocker = true;
}

void qore_class_private::setBuiltinSystemConstructor(BuiltinSystemConstructorBase* m) {
    if (!sys) {
        sys = committed = true;
    }

    assert(!system_constructor);
    QoreMethod* qm = new QoreMethod(cls, m, false);
    qm->priv->setBuiltin();
    system_constructor = qm;
}

void qore_class_private::setPublic() {
   assert(!pub);
   pub = true;
}

QoreListNode* BCEAList::findArgs(qore_classid_t classid, bool* aexeced, const AbstractQoreFunctionVariant*& variant, const QoreProgramLocation*& loc) {
   bceamap_t::iterator i = lower_bound(classid);
   // not found
   if (i == end() || i->first != classid) {
      insert(i, bceamap_t::value_type(classid, new BCEANode));
      *aexeced = false;
      variant = nullptr;
      return nullptr;
   }

   // already executed
   if (i->second->execed) {
      *aexeced = true;
      variant = nullptr;
      return nullptr;
   }

   // found and not yet executed
   *aexeced = false;
   i->second->execed = true;
   variant = i->second->variant;
   loc = i->second->loc;
   return i->second->args;
}

int BCEAList::add(qore_classid_t classid, const QoreListNode* arg, const AbstractQoreFunctionVariant* variant, const QoreProgramLocation* loc, ExceptionSink* xsink) {
   // see if class already exists in the list
   bceamap_t::iterator i = lower_bound(classid);
   bool n = ((i == end() || i->first != classid));
   if (!n && i->second->execed)
      return 0;

   // save arguments for evaluation in the constructor
   if (n)
      insert(i, bceamap_t::value_type(classid, new BCEANode(loc, arg ? arg->listRefSelf() : 0, variant)));
   else {
      assert(!i->second->args);
      assert(!i->second->variant);
      assert(!i->second->execed);
      i->second->args = arg ? arg->listRefSelf() : 0;
      i->second->variant = reinterpret_cast<const MethodVariant*>(variant);
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
void BCANode::parseInit(BCList* bcl, const char* classname) {
    QoreClass* sclass = nullptr;
    if (ns) {
        sclass = qore_root_ns_private::parseFindScopedClass(loc, *ns);
        printd(5, "BCANode::parseInit() this: %p resolved named scoped %s -> %p\n", this, ns->ostr, sclass);
        delete ns;
        ns = nullptr;
    }
    else {
        sclass = qore_root_ns_private::parseFindClass(loc, name);
        printd(5, "BCANode::parseInit() this: %p resolved %s -> %p\n", this, name, sclass);
        free(name);
        name = nullptr;
    }

    if (sclass) {
        if (!bcl->match(sclass))
            parse_error(*loc, "%s in base constructor argument list is not a base class of %s", sclass->getName(), classname);
        else {
            classid = sclass->getID();

            // find constructor variant
            const QoreMethod* m = sclass->getConstructor();
            int lvids = 0;
            if (m) {
                const QoreTypeInfo* argTypeInfo = nullptr;
                lvids = parseArgsVariant(loc, qore_class_private::getSelfId(*sclass), 0, m->getFunction(), nullptr, argTypeInfo);
            }
            else {
                if (parse_args) {
                    type_vec_t argTypeInfo;
                    lvids += parse_args->initArgs(qore_class_private::getSelfId(*sclass), 0, argTypeInfo, args);
                    parse_args = nullptr;
                }
            }
            if (lvids) {
                parse_error(*loc, "illegal local variable declaration in base class constructor argument");
                while (lvids--)
                pop_local_var();
            }
        }
    }
}

int BCNode::initializeHierarchy(QoreClass* cls, qcp_set_t& qcp_set) {
    if (!sclass) {
        if (cname) {
            // if the class cannot be found, RootQoreNamespace::parseFindScopedClass() will throw the appropriate exception
            sclass = qore_root_ns_private::parseFindScopedClass(loc, *cname);
            printd(5, "BCNode::initializeHierarchy() %s inheriting %s (%p)\n", cls->getName(), cname->ostr, sclass);
            delete cname;
            cname = 0;
        }
        else {
            // if the class cannot be found, qore_root_ns_private::parseFindClass() will throw the appropriate exception
            sclass = qore_root_ns_private::parseFindClass(loc, cstr);
            printd(5, "BCNode::initializeHierarchy() %s inheriting %s (%p)\n", cls->getName(), cstr, sclass);
            free(cstr);
            cstr = 0;
        }
        if (cls == sclass) {
            parse_error(*cls->priv->loc, "class '%s' cannot inherit itself", cls->getName());
            assert(cls->priv->scl);
            cls->priv->scl->valid = false;
            sclass = nullptr;
        }
        //printd(5, "BCNode::initializeHierarchy() cls: %p '%s' inherits %p '%s' final: %d\n", cls, cls->getName(), sclass, sclass ? sclass->getName() : "n/a", sclass ? sclass->priv->final : 0);
    }
    int rc;
    // recursively add base classes to special method list
    if (sclass) {
        if (!qcp_set.insert(sclass->priv).second) {
            // issue #2317: ensure that the class is really recursive in the inheritance list before throwing an exception
            if (sclass->priv->scl && sclass->priv->scl->findInHierarchy(*sclass->priv)) {
                parse_error(*sclass->priv->loc, "circular reference in class hierarchy, '%s' is an ancestor of itself", sclass->getName());
                if (sclass->priv->scl)
                    sclass->priv->scl->valid = false;
                return -1;
            }
            return 0;
        }
        if (sclass->priv->final)
            parse_error(*cls->priv->loc, "class '%s' cannot inherit 'final' class '%s'", cls->getName(), sclass->getName());

        rc = sclass->priv->initializeHierarchy(qcp_set);
    }
    else
        rc = -1;
    return rc;
}

int BCNode::initialize(QoreClass* cls, bool& has_delete_blocker) {
    assert(sclass);
    int rc = 0;

    rc = sclass->priv->initializeIntern();
    if (!has_delete_blocker && sclass->has_delete_blocker())
        has_delete_blocker = true;
    // include all base class domains in this class's domain
    if (!sclass->priv->addBaseClassesToSubclass(cls, is_virtual)) {
        cls->priv->domain |= sclass->priv->domain;
        // import all base class member definitions into this class
        cls->priv->parseImportMembers(*sclass->priv, access);
    }
    if (sclass->priv->final)
        parse_error(*cls->priv->loc, "class '%s' cannot inherit 'final' class '%s'", cls->getName(), sclass->getName());

    return rc;
}

bool BCNode::isBaseClass(QoreClass* qc, bool toplevel) const {
    assert(sclass);

    if (!toplevel && access == Internal)
        return false;

    //printd(5, "BCNode::isBaseClass() %p %s (%d) == %s (%d)\n", this, qc->getName(), qc->getID(), sclass->getName(), sclass->getID());
    if (qc->getID() == sclass->getID() || (sclass->priv->scl && sclass->priv->scl->isBaseClass(qc, false))) {
        //printd(5, "BCNode::isBaseClass() %p %s (%d) TRUE\n", this, qc->getName(), qc->getID());
        return true;
    }
    return false;
}

const QoreMethod* BCNode::runtimeFindCommittedMethod(const char* name, ClassAccess& n_access, const qore_class_private* class_ctx, bool allow_internal) const {
   assert(sclass);

   if (access == Internal && !allow_internal)
      return 0;

   const QoreMethod* m = sclass->priv->runtimeFindCommittedMethodIntern(name, n_access, class_ctx);
   if (m && n_access < access)
      n_access = access;

   return m;
}

const QoreMethod* BCNode::runtimeFindCommittedStaticMethod(const char* name, ClassAccess& n_access, const qore_class_private* class_ctx, bool allow_internal) const {
    assert(sclass);

    if (access == Internal && !allow_internal)
        return nullptr;

    const QoreMethod* m = sclass->priv->runtimeFindCommittedStaticMethodIntern(name, n_access, class_ctx);
    if (m && n_access < access)
        n_access = access;

    return m;
}

const QoreMethod* BCNode::parseFindNormalMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal) const {
    // sclass can be 0 if the class could not be found during parse initialization
    if (!sclass)
        return nullptr;

    if (access > Public && (!class_ctx || (access == Internal && !allow_internal)))
        return nullptr;

    return sclass->priv->parseFindNormalMethodIntern(name, class_ctx);
}

const QoreMethod* BCNode::parseFindStaticMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return 0;

   if (access > Public && (!class_ctx || (access == Internal && !allow_internal)))
      return 0;

   return sclass->priv->parseFindStaticMethodIntern(name, class_ctx);
}

const QoreMethod* BCNode::parseResolveSelfMethod(const QoreProgramLocation* loc, const char* name, const qore_class_private* class_ctx, bool allow_internal) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return 0;

   if (access == Internal && !allow_internal)
      return 0;

   sclass->priv->initialize();
   return sclass->priv->parseResolveSelfMethodIntern(loc, name, class_ctx);
}

const QoreMemberInfo* BCNode::runtimeGetMemberInfo(const char* mem, ClassAccess& n_access, const qore_class_private* class_ctx, bool allow_internal) const {
   assert(sclass);

   if (access == Internal && !allow_internal)
      return 0;

   const QoreMemberInfo* rv = sclass->priv->runtimeGetMemberInfoIntern(mem, n_access, class_ctx);
   if (rv && n_access < access)
      n_access = access;
   return rv;
}

const qore_class_private* BCNode::runtimeGetMemberClass(const char* mem, ClassAccess& n_access, const qore_class_private* class_ctx, bool allow_internal) const {
   assert(sclass);

   if (access == Internal && !allow_internal)
      return 0;

   const qore_class_private* rv = sclass->priv->runtimeGetMemberClassIntern(mem, n_access, class_ctx);
   if (rv && n_access < access)
      n_access = access;
   return rv;
}

const QoreMemberInfo* BCNode::parseFindMember(const char* mem, const qore_class_private*& qc, ClassAccess& n_access, bool toplevel) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return 0;

   if (access == Internal && !toplevel)
      return 0;

   const QoreMemberInfo* rv = sclass->priv->parseFindMemberNoInit(mem, qc, n_access, false);
   if (rv && n_access < access)
      n_access = access;
   return rv;
}

const QoreVarInfo* BCNode::parseFindVar(const char* name, const qore_class_private*& qc, ClassAccess& n_access, bool toplevel) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return 0;

   if (access == Internal && !toplevel)
      return 0;

   const QoreVarInfo* vi = sclass->priv->parseFindVar(name, qc, n_access, false);
   if (vi && n_access < access)
      n_access = access;
   return vi;
}

const QoreClass* BCNode::findInHierarchy(const qore_class_private& qc) {
   // sclass can be 0 if the class could not be found during parse initialization
   return sclass ? sclass->priv->findInHierarchy(qc) : nullptr;
}

const QoreClass* BCNode::getClass(qore_classid_t cid, ClassAccess& n_access, bool toplevel) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return 0;

   if (access == Internal && !toplevel)
      return 0;

   const QoreClass* qc = (sclass->getID() == cid) ? sclass : sclass->priv->getClassIntern(cid, n_access, false);
   if (qc && n_access < access)
      n_access = access;

   return qc;
}

const QoreClass* BCNode::getClass(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return 0;

   if (access == Internal && !toplevel)
      return 0;

   const QoreClass* rv = sclass->priv->getClassIntern(qc, n_access, false);

   if (rv && n_access < access)
      n_access = access;

   return rv;
}

const QoreClass *BCNode::parseGetClass(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return nullptr;

   if (access == Internal && !toplevel)
      return nullptr;

   const QoreClass* rv = sclass->priv->parseGetClassIntern(qc, n_access, false);

   if (rv && n_access < access)
      n_access = access;

   return rv;
}

bool BCNode::runtimeIsPrivateMember(const char* str, bool toplevel) const {
   assert(sclass);

   if (access == Internal && !toplevel)
      return false;

   return sclass->priv->runtimeIsPrivateMemberIntern(str, false);
}

QoreValue BCNode::parseFindConstantValue(const char* cname, const QoreTypeInfo*& typeInfo, bool& found, const qore_class_private* class_ctx, bool allow_internal) const {
    // sclass can be 0 if the class could not be found during parse initialization
    if (!sclass)
        return QoreValue();

    if (access == Internal && !allow_internal)
        return QoreValue();

    return sclass->priv->parseFindConstantValueIntern(cname, typeInfo, found, class_ctx);
}

bool BCNode::parseCheckHierarchy(const QoreClass* cls, ClassAccess& n_access, bool toplevel) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return false;

   if (access == Internal && !toplevel)
      return false;

   if (sclass->priv->parseCheckHierarchyIntern(cls, n_access, false)) {
      //printd(5, "BCNode::parseCheckHierarchy() '%s' '%s' access: %s n_access: %s\n", sclass->getName(), cls->getName(), privpub(access), privpub(n_access));
      if (n_access < access)
         n_access = access;
      return true;
   }

   return false;
}

QoreVarInfo* BCNode::parseFindStaticVar(const char* vname, const QoreClass*& qc, ClassAccess& n_access, bool check, bool toplevel) const {
   // sclass can be 0 if the class could not be found during parse initialization
   if (!sclass)
      return nullptr;

   if (access == Internal && !toplevel)
      return nullptr;

   QoreVarInfo* vi = sclass->priv->parseFindStaticVarIntern(vname, qc, n_access, check, false);
   if (vi && n_access < access)
      n_access = access;

   return vi;
}

void BCNode::execConstructors(QoreObject* o, BCEAList* bceal, ExceptionSink* xsink) const {
   //printd(5, "BCNode::execConstructors() %s::constructor() o: %p (for subclass %s) virtual: %d\n", sclass->getName(), o, o->getClass()->getName(), is_virtual);

   // do not execute constructors for virtual base classes
   if (is_virtual)
      return;
   sclass->priv->execBaseClassConstructor(o, bceal, xsink);
}

int BCNode::addBaseClassesToSubclass(QoreClass* child, bool is_virtual) {
    // issue #2318 must check for duplicate base classes here
    // sclass may be 0 in case of a parse exception
    if (!sclass)
        return 0;

    if (sclass->priv->scl && sclass->priv->scl->findInHierarchy(*child->priv)) {
        parse_error(*loc, "OOPS %s", child->getName());
        return -1;
    }

    return sclass->priv->addBaseClassesToSubclass(child, is_virtual);
}

void BCNode::initializeBuiltin() {
    assert(sclass);
    sclass->priv->initializeBuiltin();
}

void BCList::parseResolveAbstract() {
    for (auto& i : *this) {
        if (i->sclass) {
            i->sclass->priv->parseResolveAbstract();
        }
    }
}

void BCList::rescanParents(QoreClass* cls) {
    if (rescanned)
        return;
    rescanned = true;
    // iterate sml for all virtual parent classes; must iterate with offsets;
    // the vector can be reallocated during this operation
    for (unsigned i = 0; i < sml.size(); ++i) {
        if (sml[i].second && sml[i].first->priv->scl) {
            sml[i].first->priv->scl->rescanParents(sml[i].first);
            sml[i].first->priv->scl->sml.alignBaseClassesInSubclass(sml[i].first, cls, true);
        }
    }
}

bool BCList::isBaseClass(QoreClass* qc, bool toplevel) const {
    for (auto& i : *this) {
        if ((*i).isBaseClass(qc, toplevel))
            return true;
    }
    //printd(5, "BCList::isBaseClass() %p %s (%d) FALSE\n", this, qc->getName(), qc->getID());
    return false;
}

int BCList::initializeHierarchy(QoreClass* cls, qcp_set_t& qcp_set) {
    for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
        if ((*i)->initializeHierarchy(cls, qcp_set)) {
            if (valid)
                valid = false;
        }
    }

    // compare each class in the list to ensure that there are no duplicates
    for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
        if ((*i)->sclass) {
            bclist_t::iterator j = i;
            while (++j != e) {
                if (!(*j)->sclass)
                continue;
                if ((*i)->sclass->getID() == (*j)->sclass->getID()) {
                parse_error(*cls->priv->loc, "class '%s' cannot inherit '%s' more than once", cls->getName(), (*i)->sclass->getName());
                if (valid)
                    valid = false;
                }
            }
        }
    }

    return valid ? 0 : -1;
}

int BCList::initialize(QoreClass* cls, bool& has_delete_blocker) {
    printd(5, "BCList::initialize(%s) this: %p empty: %d\n", cls->getName(), this, empty());
    for (bclist_t::iterator i = begin(), e = end(); i != e; ++i) {
        if ((*i)->initialize(cls, has_delete_blocker)) {
            if (valid)
                valid = false;
        }
    }

    return valid ? 0 : -1;
}

const qore_class_private* BCList::runtimeGetMemberClass(const char* mem, ClassAccess& access, const qore_class_private* class_ctx, bool allow_internal) const {
    for (auto& i : *this) {
        const qore_class_private* rv = (*i).runtimeGetMemberClass(mem, access, class_ctx, allow_internal);
        if (rv)
            return rv;
    }

    return nullptr;
}

bool BCList::parseHasPublicMembersInHierarchy() const {
    for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i)
        if ((*i)->sclass && (*i)->sclass->parseHasPublicMembersInHierarchy())
            return true;
    return false;
}

const QoreMemberInfo* BCList::runtimeGetMemberInfo(const char* mem, ClassAccess& access, const qore_class_private* class_ctx, bool allow_internal) const {
    for (auto& i : *this) {
        const QoreMemberInfo* rv = (*i).runtimeGetMemberInfo(mem, access, class_ctx, allow_internal);
        if (rv)
            return rv;
    }

    return nullptr;
}

const QoreMemberInfo* BCList::parseFindMember(const char* mem, const qore_class_private*& qc, ClassAccess& access, bool toplevel) const {
    if (!valid)
        return 0;

    for (auto& i : *this) {
        const QoreMemberInfo* mi = (*i).parseFindMember(mem, qc, access, toplevel);
        if (mi)
            return mi;
    }
    return nullptr;
}

const QoreVarInfo* BCList::parseFindVar(const char* name, const qore_class_private*& qc, ClassAccess& n_access, bool toplevel) const {
    if (!valid)
        return 0;

    for (auto& i : *this) {
        const QoreVarInfo* rv = (*i).parseFindVar(name, qc, n_access, toplevel);
        if (rv)
            return rv;
    }
    return nullptr;
}

// called at run time
const QoreMethod* BCList::runtimeFindCommittedMethod(const char* name, ClassAccess& access, const qore_class_private* class_ctx, bool allow_internal) const {
    for (auto& i : *this) {
        const QoreMethod* m = (*i).runtimeFindCommittedMethod(name, access, class_ctx, allow_internal);
        if (m)
            return m;
    }
    return nullptr;
}

// called at run time
const QoreMethod* BCList::runtimeFindCommittedStaticMethod(const char* name, ClassAccess& access, const qore_class_private* class_ctx, bool allow_internal) const {
    for (auto& i : *this) {
        const QoreMethod* m = (*i).runtimeFindCommittedStaticMethod(name, access, class_ctx, allow_internal);
        if (m)
            return m;
    }
    return nullptr;
}

const QoreMethod* BCList::parseFindNormalMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal) {
    if (!valid)
        return nullptr;

    for (auto& i : *this) {
        const QoreMethod* m = (*i).parseFindNormalMethod(name, class_ctx, allow_internal);
        if (m)
            return m;
    }
    return nullptr;
}

const QoreMethod* BCList::parseFindStaticMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal) {
    if (!valid)
        return 0;

    for (auto& i : *this) {
        const QoreMethod* m = (*i).parseFindStaticMethod(name, class_ctx, allow_internal);
        if (m)
            return m;
    }
    return nullptr;
}

const QoreMethod* BCList::parseResolveSelfMethod(const QoreProgramLocation* loc, const char* name, const qore_class_private* class_ctx, bool allow_internal) {
    for (auto& i : *this) {
        const QoreMethod* m = (*i).parseResolveSelfMethod(loc, name, class_ctx, allow_internal);
        if (m)
            return m;
    }
    return nullptr;
}

bool BCList::match(const QoreClass* cls) {
   for (auto& i : *this) {
      if (cls == (*i).sclass) {
         return true;
      }
   }
   return false;
}

bool BCList::runtimeIsPrivateMember(const char* str, bool toplevel) const {
   for (auto& i : *this) {
      if ((*i).runtimeIsPrivateMember(str, toplevel))
         return true;
   }
   return false;
}

bool BCList::execDeleteBlockers(QoreObject* o, ExceptionSink* xsink) const {
   for (auto& i : *this) {
      //printd(5, "BCList::execDeleteBlockers() %s o: %p (for subclass %s)\n", (*i)->sclass->getName(), o, o->getClass()->getName());
      if ((*i).sclass->execDeleteBlocker(o, xsink))
         return true;
   }
   return false;
}

/*
int BCList::initMembers(QoreObject& o, BCEAList* bceal, ExceptionSink* xsink) const {
   for (auto& i : *this) {
      printd(5, "BCList::initMembers() %s::constructor() o: %p (for subclass %s) virtual: %d\n", (*i).sclass->getName(), &o, o.getClass()->getName(), (*i).is_virtual);

      if ((*i).sclass->priv->initMembers(o, bceal, xsink))
         return -1;
   }
   return 0;
}
*/

void BCList::execConstructors(QoreObject* o, BCEAList* bceal, ExceptionSink* xsink) const {
   for (auto& i : *this) {
      (*i).execConstructors(o, bceal, xsink);
      if (*xsink)
         break;
   }
}

bool BCList::parseCheckHierarchy(const QoreClass* cls, ClassAccess& access, bool toplevel) const {
   for (auto& i : *this) {
      if ((*i).parseCheckHierarchy(cls, access, toplevel))
         return true;
   }
   return false;
}

void BCList::addNewAncestors(QoreMethod* m) {
   QoreFunction *f = m->getFunction();
   const char* name = m->getName();
   for (auto& i : *this) {
      QoreClass* qc = (*i).sclass;
      // should be only called from builtin classes, therefore qc != NULL
      assert(qc);
      assert((*i).access != Internal);
      const QoreMethod* w = qc->priv->findLocalCommittedMethod(name);
      if (w)
         f->addNewAncestor(w->getFunction(), (*i).access);
      qc->priv->addNewAncestors(m);
   }
}

void BCList::addNewStaticAncestors(QoreMethod* m) {
   QoreFunction *f = m->getFunction();
   const char* name = m->getName();
   for (auto& i : *this) {
      QoreClass* qc = (*i).sclass;
      // should be only called from builtin classes, therefore qc != NULL
      assert(qc);
      assert((*i).access != Internal);
      const QoreMethod* w = qc->priv->findLocalCommittedStaticMethod(name);
      if (w)
         f->addNewAncestor(w->getFunction(), (*i).access);
      qc->priv->addNewStaticAncestors(m);
   }
}

void BCList::addAncestors(QoreMethod* m) {
   const char* name = m->getName();
   for (auto& i : *this) {
      QoreClass* qc = (*i).sclass;
      assert(qc);

      const QoreMethod* w = qc->priv->findLocalCommittedMethod(name);
      if (w)
         m->getFunction()->addAncestor(w->getFunction(), (*i).access);

      qc->priv->addAncestors(m);
   }
}

void BCList::addStaticAncestors(QoreMethod* m) {
   const char* name = m->getName();
   for (auto& i : *this) {
      QoreClass* qc = (*i).sclass;
      assert(qc);

      const QoreMethod* w = qc->priv->findLocalCommittedStaticMethod(name);
      if (w)
         m->getFunction()->addAncestor(w->getFunction(), (*i).access);
      qc->priv->addStaticAncestors(m);
   }
}

void BCList::parseAddAncestors(QoreMethod* m) {
   const char* name = m->getName();

   //printd(5, "BCList::parseAddAncestors(%p %s) this: %p size: %d\n", m, name, this, size());

   for (auto& i : *this) {
      // if there was a parse error finding the base class, then skip
      QoreClass* qc = (*i).sclass;
      if (!qc)
         continue;

      const QoreMethod* w = qc->priv->parseFindLocalMethod(name);
      //printd(5, "BCList::parseAddAncestors(%p %s) this: %p qc: %p w: %p\n", m, name, this, qc, w);

      if (w)
         m->getFunction()->addAncestor(w->getFunction(), (*i).access);

      qc->priv->parseAddAncestors(m);
   }
}

void BCList::parseAddStaticAncestors(QoreMethod* m) {
   const char* name = m->getName();
   for (auto& i : *this) {
      QoreClass* qc = (*i).sclass;
      // qc may be 0 if there were a parse error with an unknown class earlier
      if (!qc)
         continue;

      const QoreMethod* w = qc->priv->parseFindLocalStaticMethod(name);
      if (w)
         m->getFunction()->addAncestor(w->getFunction(), (*i).access);

      qc->priv->parseAddStaticAncestors(m);
   }
}

void BCList::resolveCopy() {
   for (auto& i : *this) {
      assert((*i).sclass->priv->new_copy);
      (*i).sclass = (*i).sclass->priv->new_copy;
      (*i).sclass->priv->resolveCopy();
   }

   sml.resolveCopy();
}

QoreValue BCList::parseFindConstantValue(const char* cname, const QoreTypeInfo*& typeInfo, bool& found, const qore_class_private* class_ctx, bool allow_internal) const {
    if (!valid)
        return QoreValue();

    for (auto& i : *this) {
       QoreValue rv = (*i).parseFindConstantValue(cname, typeInfo, found, class_ctx, allow_internal);
       if (found)
           return rv;
    }
    return QoreValue();
}

QoreVarInfo* BCList::parseFindStaticVar(const char* vname, const QoreClass*& qc, ClassAccess& access, bool check, bool toplevel) const {
   if (!valid)
      return nullptr;

   for (auto& i : *this) {
      QoreVarInfo* vi = (*i).parseFindStaticVar(vname, qc, access, check, toplevel);
      if (vi)
         return vi;
   }
   return nullptr;
}

const QoreClass* BCList::findInHierarchy(const qore_class_private& qc) {
   for (auto& i : *this) {
      const QoreClass* rv = (*i).findInHierarchy(qc);
      if (rv)
         return rv;
   }

   return nullptr;
}

const QoreClass* BCList::getClass(qore_classid_t cid, ClassAccess& n_access, bool toplevel) const {
   for (auto& i : *this) {
      const QoreClass* qc = (*i).getClass(cid, n_access, toplevel);
      if (qc)
         return qc;
   }

   return nullptr;
}

const QoreClass* BCList::getClass(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const {
   for (auto& i : *this) {
      const QoreClass* rv = (*i).getClass(qc, n_access, toplevel);
      if (rv)
         return rv;
   }

   return 0;
}

const QoreClass* BCList::parseGetClass(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const {
   for (auto& i : *this) {
      const QoreClass* rv = (*i).parseGetClass(qc, n_access, toplevel);
      if (rv)
         return rv;
   }

   return nullptr;
}

MethodVariantBase* BCList::matchNonAbstractVariant(const std::string& name, MethodVariantBase* v) const {
    for (auto& i : *this) {
        const QoreClass* nqc = (*i).sclass;
        //printd(5, "BCList::matchNonAbstractVariant() this: %p %s::%s %p (%s) ncq: %p\n", this, nqc ? nqc->getName() : "n/a", name.c_str(), v, v->getAbstractSignature(), nqc);

        // nqc may be 0 if there were a parse error with an unknown class earlier
        // also if the original abstract variant comes from the class being searched, then skip it
        if (!nqc || v->getClass() == nqc)
            continue;

        QoreMethod* m = nqc->priv->parseFindLocalMethod(name);
        if (m) {
            MethodFunctionBase* f = m->getFunction();
            MethodVariantBase* ov = f->parseHasVariantWithSignature(v);
            if (ov && !ov->isAbstract())
                return ov;
        }
        if (nqc->priv->scl) {
            MethodVariantBase* ov = nqc->priv->scl->matchNonAbstractVariant(name, v);
            if (ov)
                return ov;
        }
    }

    return nullptr;
}

int BCAList::execBaseClassConstructorArgs(BCEAList* bceal, ExceptionSink* xsink) const {
   for (auto& i : *this) {
      if (bceal->add(i->classid, i->getArgs(), i->getVariant(), i->loc, xsink))
         return -1;
   }
   return 0;
}

bool QoreClass::hasAbstract() const {
   return priv->hasAbstract();
}

const QoreMethod* QoreClass::parseGetConstructor() const {
   const_cast<QoreClass*>(this)->priv->initialize();
   if (priv->constructor)
      return priv->constructor;
   return priv->parseFindLocalMethod("constructor");
}

bool QoreClass::has_delete_blocker() const {
   return priv->has_delete_blocker;
}

BCSMList* QoreClass::getBCSMList() const {
   return priv->scl ? &priv->scl->sml : nullptr;
}

const QoreMethod* QoreClass::findLocalStaticMethod(const char* nme) const {
   CurrentProgramRuntimeParseContextHelper pch;
   return priv->findLocalCommittedStaticMethod(nme);
}

const QoreMethod* QoreClass::findLocalMethod(const char* nme) const {
   CurrentProgramRuntimeParseContextHelper pch;
   return priv->findLocalCommittedMethod(nme);
}

const QoreMethod* QoreClass::findStaticMethod(const char* nme) const {
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !priv->runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;

   CurrentProgramRuntimeParseContextHelper pch;
   ClassAccess access;
   return priv->runtimeFindCommittedStaticMethod(nme, access, class_ctx);
}

const QoreMethod* QoreClass::findStaticMethod(const char* nme, bool& priv_flag) const {
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !priv->runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;

   CurrentProgramRuntimeParseContextHelper pch;
   ClassAccess access;
   const QoreMethod* rv = priv->runtimeFindCommittedStaticMethod(nme, access, class_ctx);
   priv_flag = access > Public;
   return rv;
}

const QoreMethod* QoreClass::findStaticMethod(const char* nme, ClassAccess& access) const {
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !priv->runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;
   CurrentProgramRuntimeParseContextHelper pch;
   return priv->runtimeFindCommittedStaticMethod(nme, access, class_ctx);
}

const QoreMethod* QoreClass::findMethod(const char* nme) const {
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !priv->runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;
   ClassAccess access = Public;
   return priv->runtimeFindCommittedMethod(nme, access, class_ctx);
}

const QoreMethod* QoreClass::findMethod(const char* nme, bool& priv_flag) const {
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !priv->runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;
   ClassAccess access = Public;
   const QoreMethod* rv = priv->runtimeFindCommittedMethod(nme, access, class_ctx);
   priv_flag = access > Public;
   return rv;
}

const QoreMethod* QoreClass::findMethod(const char* nme, ClassAccess& access) const {
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !priv->runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;
   return priv->runtimeFindCommittedMethod(nme, access, class_ctx);
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

void QoreClass::setSystem() {
   priv->sys = true;
   priv->committed = true;
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

void QoreClass::addBuiltinBaseClass(QoreClass* qc, QoreListNode* xargs) {
   assert(!xargs);
   if (!priv->scl)
      priv->scl = new BCList;
   priv->scl->push_back(new BCNode(&loc_builtin, qc));
   priv->scl->sml.add(this, qc, false);
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
   priv->scl->push_back(new BCNode(&loc_builtin, qc, true));

   if (qc->priv->scl && qc->priv->scl->valid)
      qc->priv->scl->addBaseClassesToSubclass(qc, this, true);
   priv->scl->sml.add(this, qc, true);
}

void qore_class_private::parseCheckAbstractNew(const QoreProgramLocation* loc) const {
    // we must defer the check until all classes are completely resolved
    qore_root_ns_private::get(*getRootNS())->deferParseCheckAbstractNew(this, loc);
}

const QoreMethod* qore_class_private::parseFindNormalMethod(const char* nme, const qore_class_private* class_ctx) {
    // if we have a class context, first we have to check here for an internal method
    if (class_ctx) {
        const QoreMethod* m = class_ctx->parseFindLocalMethod(nme);
        if (m && qore_method_private::getAccess(*m) == Internal)
            return m;
    }

    return parseFindNormalMethodIntern(nme, class_ctx);
}

const QoreMethod* qore_class_private::parseFindStaticMethod(const char* nme, const qore_class_private* class_ctx) {
    // if we have a class context, first we have to check here for an internal method
    if (class_ctx) {
        const QoreMethod* m = class_ctx->parseFindLocalStaticMethod(nme);
        if (m && qore_method_private::getAccess(*m) == Internal)
            return m;
    }

    return parseFindStaticMethodIntern(nme, class_ctx);
}

const QoreMethod* qore_class_private::parseFindAnyMethod(const char* nme, const qore_class_private* class_ctx) {
    const QoreMethod* m = nullptr;

    // if we have a class context, first we have to check here for an internal method
    if (class_ctx) {
        m = class_ctx->parseFindAnyLocalMethod(nme);
        if (m && qore_method_private::getAccess(*m) != Internal)
            m = nullptr;
    }

    if (!m)
        m = parseFindNormalMethodIntern(nme, class_ctx);

    //printd(5, "qore_class_private::parseFindAnyMethod() cls: '%s' nme: '%s' class_ctx: %p m: %p (%s::%s) %s\n", name.c_str(), nme, class_ctx, m, m ? m->getClass()->getName() : "n/a", m ? m->getName() : "n/a", m ? privpub(qore_method_private::getAccess(*m)) : "n/a");

    if (m && strcmp(nme, "constructor") && strcmp(nme, "destructor") && strcmp(nme, "copy") && ((qore_method_private::getAccess(*m) == Public) || class_ctx))
        return m;

    m = parseFindStaticMethodIntern(nme, class_ctx);
    return m && ((qore_method_private::getAccess(*m) == Public) || class_ctx) ? m : 0;
}

const QoreMethod* qore_class_private::parseFindAnyMethodStaticFirst(const char* nme, const qore_class_private* class_ctx) {
   const QoreMethod* m = 0;

   // if we have a class context, first we have to check here for an internal method
   if (class_ctx) {
      m = class_ctx->parseFindLocalStaticMethod(nme);
      if (m && qore_method_private::getAccess(*m) != Internal) {
         m = class_ctx->parseFindLocalMethod(nme);
         if (m && qore_method_private::getAccess(*m) != Internal) {
            m = 0;
         }
      }
   }

   if (!m)
      m = parseFindStaticMethodIntern(nme, class_ctx);

   if (m && ((qore_method_private::getAccess(*m) == Public) || class_ctx))
      return m;

   m = parseFindNormalMethodIntern(nme, class_ctx);

   if (m && (!strcmp(nme, "constructor") || !strcmp(nme, "destructor") || !strcmp(nme, "copy")))
      m = 0;

   return m && ((qore_method_private::getAccess(*m) == Public) || class_ctx) ? m : 0;
}

// searches all methods, both pending and comitted
const QoreMethod* qore_class_private::parseResolveSelfMethod(const QoreProgramLocation* loc, const char* nme, const qore_class_private* class_ctx) {
    initialize();

    const QoreMethod* m = nullptr;

    if (class_ctx) {
        const QoreMethod* m = class_ctx->parseFindAnyLocalMethod(nme);
        if (m && qore_method_private::getAccess(*m) != Internal)
            m = nullptr;
    }
    if (!m)
        m = parseResolveSelfMethodIntern(loc, nme, class_ctx);

    if (!m) {
        parse_error(*loc, "no method %s::%s() has been defined; if you want to make a call to a method that will be defined in an inherited class, then use 'self.%s()' instead", name.c_str(), nme, nme);
        return nullptr;
    }
    printd(5, "qore_class_private::parseResolveSelfMethod(%s) resolved to %s::%s() %p (static: %d)\n", nme, name.c_str(), nme, m, m->isStatic());

    const char* mname = m->getName();
    // make sure we're not calling a method that cannot be called directly
    if (!m->isStatic() && (!strcmp(mname, "constructor") || !strcmp(mname, "destructor") || !strcmp(mname, "copy"))) {
        parse_error(*loc, "explicit calls to %s() methods are not allowed", nme);
        return nullptr;
    }

    return m;
}

const QoreMethod* qore_class_private::parseFindSelfMethod(const char* nme) {
    initialize();

    return parseFindAnyMethod(nme, this);
}

const QoreMethod* qore_class_private::parseResolveSelfMethod(const QoreProgramLocation* loc, NamedScope* nme) {
    // first find class
    QoreClass* qc = qore_root_ns_private::parseFindScopedClassWithMethod(loc, *nme, true);
    if (!qc)
        return nullptr;

    // see if class is base class of this class
    if (qc != cls && (!scl || !scl->isBaseClass(qc, true))) {
        parse_error(*loc, "'%s' is not a base class of '%s'", qc->getName(), name.c_str());
        return nullptr;
    }

    return qc->priv->parseResolveSelfMethod(loc, nme->getIdentifier(), this);
}

// finds a non-static method in the class hierarchy at parse time, optionally initializes classes
const QoreMethod* qore_class_private::parseFindNormalMethodIntern(const char* mname, const qore_class_private* class_ctx) {
    initialize();
    const QoreMethod* m = parseFindLocalMethod(mname);
    if (m && doParseMethodAccess(m, class_ctx)) {
        //printd(5, "qore_class_private::parseFindNormalMethodIntern() this: %p '%s' mname: '%s' m: %p\n", this, name.c_str(), mname, m);
        return m;
    }
    //printd(5, "qore_class_private::parseFindNormalMethodIntern() this: %p '%s' mname: '%s' NOT FOUND\n", this, name.c_str(), mname);
    if (scl) {
        m = scl->parseFindNormalMethod(mname, class_ctx, class_ctx == this);
        if (m && doParseMethodAccess(m, class_ctx)) {
            return m;
        }
    }
    return nullptr;
}

// finds a static method in the class hierarchy at parse time, optionally initializes classes
const QoreMethod* qore_class_private::parseFindStaticMethodIntern(const char* mname, const qore_class_private* class_ctx) {
    initialize();
    const QoreMethod* m = parseFindLocalStaticMethod(mname);
    if (m && doParseMethodAccess(m, class_ctx))
        return m;
    if (scl) {
        m = scl->parseFindStaticMethod(mname, class_ctx, class_ctx == this);
        if (m && doParseMethodAccess(m, class_ctx))
            return m;
    }
    return nullptr;
}

const QoreMethod* qore_class_private::parseResolveSelfMethodIntern(const QoreProgramLocation* loc, const char* nme, const qore_class_private* class_ctx) {
    const QoreMethod* m = parseFindLocalMethod(nme);
    if (m && doMethodAccess(m, qore_method_private::getAccess(*m), class_ctx))
        return m;
    m = parseFindLocalStaticMethod(nme);
    if (m && doMethodAccess(m, qore_method_private::getAccess(*m), class_ctx))
        return m;
    if (scl) {
        m = scl->parseResolveSelfMethod(loc, nme, class_ctx, this == class_ctx);
        if (m && doMethodAccess(m, qore_method_private::getAccess(*m), class_ctx))
            return m;
    }
    return nullptr;
}

int qore_class_private::parseCheckClassHierarchyMembers(const char* mname, const QoreMemberInfo& b_mi, const qore_class_private& b_qc, const QoreMemberInfo& l_mi) {
      if (l_mi.access != b_mi.access) {
      // raise an exception only if parse exceptions are enabled
      if (getProgram()->getParseExceptionSink()) {
         qore_program_private::makeParseException(getProgram(), *l_mi.loc, "PARSE-ERROR", new QoreStringNodeMaker("class '%s' cannot be combined with class '%s' in the same hierarchy because member '%s' is declared both %s and %s, respectively", l_mi.getClass(this)->name.c_str(), b_mi.getClass(&b_qc)->name.c_str(), mname, privpub(l_mi.access), privpub(b_mi.access)));
      }
      return -1;
   }
   if (l_mi.parseHasTypeInfo() || b_mi.parseHasTypeInfo()) {
      //printd(5, "qore_class_private::parseCheckClassHierarchyMembers() this: %p '%s' mname: '%s' l: '%s' %p b: '%s' %p ('%s' %p)\n", this, name.c_str(), mname, l_mi.getClass(this)->name.c_str(), l_mi.getClass(this), b_qc.name.c_str(), &b_qc, b_mi.getClass(&b_qc)->name.c_str(), b_mi.getClass(&b_qc));
      // raise an exception only if parse exceptions are enabled
      if (getProgram()->getParseExceptionSink()) {
         qore_program_private::makeParseException(getProgram(), l_mi.parseHasTypeInfo() ? *l_mi.loc : *b_mi.loc, "PARSE-ERROR", new QoreStringNodeMaker("class '%s' cannot be combined with class '%s' in the same hierarchy because member '%s' is declared with a type definition", l_mi.getClass(this)->name.c_str(), b_mi.getClass(&b_qc)->name.c_str(), mname));
      }
      return -1;
   }
   return 0;
}

void qore_class_private::parseImportMembers(qore_class_private& qc, ClassAccess access) {
   // issue #2657: ensure that parent class members are initialied before merging
   //printd(5, "qore_class_private::parseImportMembers() this: %p '%s' members: %p init qc: %p '%s' qc.members: %p\n", this, name.c_str(), &members, &qc, qc.name.c_str(), &qc.members);
   qc.members.parseInit();
   for (QoreMemberMap::DeclOrderIterator i = qc.members.beginDeclOrder(), e = qc.members.endDeclOrder(); i != e; ++i) {
      if (i->second->access == Internal)
         continue;
      const QoreMemberInfo* mi = parseFindLocalPublicPrivateMemberNoInit(i->first);
      if (mi) {
         if (mi->access == Internal)
            continue;
         if (!mi->getClass(this)->equal(*i->second->getClass(&qc)))
            parseCheckClassHierarchyMembers(i->first, *(i->second), qc, *mi);
         continue;
      }
      //printd(5, "qore_class_private::parseImportMembers() this: %p '%s' members: %p add '%s' %p '%s' (%d)\n", this, name.c_str(), &members, i->first, i->second->exp, get_type_name(i->second->exp), get_node_type(i->second->exp));
      members.addInheritedNoCheck(strdup(i->first), i->second->copy(i->first, &qc, access));
   }
}

void qore_class_private::parseRollback() {
    if (parse_init_called)
        parse_init_called = false;

    if (parse_init_partial_called)
        parse_init_partial_called = false;

    if (has_sig_changes)
        has_sig_changes = false;

    if (!has_new_user_changes) {
#ifdef DEBUG
        // verify status
        for (hm_method_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
            assert(i->second->priv->func->pendingEmpty());
        for (hm_method_t::iterator i = shm.begin(), e = shm.end(); i != e; ++i)
            assert(i->second->priv->func->pendingEmpty());
#endif
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

    // delete all static vars
    if (!vars.empty()) {
        vars.del();
        //printd(5, "qore_class_private::parseRollback() '%s' deleted all vars: %d\n", name.c_str(), vars.empty());
    }

    // set flags
    if (pending_has_public_memdecl)
        pending_has_public_memdecl = false;

    has_new_user_changes = false;
}

QoreMethod::QoreMethod(const QoreClass* n_parent_class, MethodFunctionBase* n_func, bool n_static) : priv(new qore_method_private(n_parent_class, n_func, n_static)) {
}

QoreMethod::~QoreMethod() {
    delete priv;
}

MethodFunctionBase* QoreMethod::getFunction() const {
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

ClassAccess QoreMethod::getAccess() const {
   return priv->func->getAccess();
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

QoreValue QoreMethod::execManaged(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
   // to ensure the object does not get referenced for the call
   ObjectSubstitutionHelper osh(self, qore_class_private::get(*priv->parent_class));
   return priv->eval(xsink, self, args);
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

const QoreTypeInfo* QoreMethod::getUniqueReturnTypeInfo() const {
   return priv->getUniqueReturnTypeInfo();
}

static const QoreClass* getStackClass() {
   const qore_class_private* qc = runtime_get_class();
   return qc ? qc->cls : nullptr;
}

void QoreClass::addMember(const char* mname, ClassAccess access, const QoreTypeInfo* n_typeInfo, QoreValue initial_value) {
    priv->addMember(mname, access, n_typeInfo, initial_value);
}

BCSMList::BCSMList(const BCSMList& old) {
    reserve(old.size());
    for (auto& i : old) {
        push_back(i);
        i.first->priv->ref();
    }
}

BCSMList::~BCSMList() {
    for (auto& i : *this) {
        i.first->priv->deref(false, false);
    }
}

int BCSMList::runtimeInitInternalMembers(QoreObject& o, bool& need_scan, ExceptionSink* xsink) const {
    for (auto& i : *this) {
        if (i.first->priv->runtimeInitLocalMembers(o, need_scan, true, xsink)) {
            return -1;
        }
    }
    return 0;
}

void BCSMList::alignBaseClassesInSubclass(QoreClass* thisclass, QoreClass* child, bool is_virtual) {
    //printd(5, "BCSMList::alignBaseClassesInSubclass(this: %s, sc: %s) size: %d\n", thisclass->getName(), sc->getName());
    // we must iterate with offsets, because the vector can be reallocated during this iteration
    for (unsigned i = 0; i < (*this).size(); ++i) {
        bool virt = is_virtual || (*this)[i].second;
        //printd(5, "BCSMList::alignBaseClassesInSubclass() %s child: %s virt: %d\n", thisclass->getName(), sc->getName(), virt);
        child->priv->scl->sml.align(child, (*this)[i].first, virt);
    }
}

void BCSMList::align(QoreClass* thisclass, QoreClass* qc, bool is_virtual) {
    assert(thisclass->getID() != qc->getID());

    // see if class already exists in vector
    for (auto& i : *this) {
        if (i.first->getID() == qc->getID())
            return;
        assert(i.first->getID() != thisclass->getID());
    }
    qc->priv->ref();

    // append to the end of the vector
    //printd(5, "BCSMList::align() adding %p '%s' (virt: %d) as a base class of %p '%s'\n", qc, qc->getName(), is_virtual, thisclass, thisclass->getName());
    push_back(std::make_pair(qc, is_virtual));
}

int BCSMList::addBaseClassesToSubclass(QoreClass* thisclass, QoreClass* sc, bool is_virtual) {
    //printd(5, "BCSMList::addBaseClassesToSubclass(this: %s, sc: %s) size: %d\n", thisclass->getName(), sc->getName());
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
        parse_error(*thisclass->priv->loc, "class '%s' cannot inherit itself", thisclass->getName());
        return -1;
    }

    // see if class already exists in list
    class_list_t::const_iterator i = begin();
    while (i != end()) {
        if (i->first->getID() == qc->getID())
            return 0;
        if (i->first->getID() == thisclass->getID()) {
            thisclass->priv->scl->valid = false;
            parse_error(*thisclass->priv->loc, "circular reference in class hierarchy, '%s' is an ancestor of itself", thisclass->getName());
            return -1;
        }
        ++i;
    }
    qc->priv->ref();

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
      //printd(5, "BCSMList::execSystemDestructors() %s::destructor() o: %p virt: %s (subclass %s)\n", i->first->getName(), o, i->second ? "true" : "false", o->getClass()->getName());
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

QoreClass::QoreClass(qore_class_private* priv) : priv(priv) {
    priv->cls = this;
}

QoreClass::QoreClass(const QoreClass& old) : priv(old.priv) {
    priv->pgmRef();

    // ensure atomicity when writing to qcset
    AutoLocker al(priv->gate.asl_lock);
    priv->qcset.insert(this);
}

QoreClass::QoreClass(const char* nme, int64 dom) : priv(new qore_class_private(this, std::string(nme), dom)) {
   priv->orNothingTypeInfo = new QoreClassOrNothingTypeInfo(this, nme);
   priv->owns_ornothingtypeinfo = true;
}

QoreClass::QoreClass(const char* nme, int dom) : priv(new qore_class_private(this, std::string(nme), dom)) {
   priv->orNothingTypeInfo = new QoreClassOrNothingTypeInfo(this, nme);
   priv->owns_ornothingtypeinfo = true;
}

QoreClass::QoreClass(const char* nme, int64 dom, const QoreTypeInfo* typeInfo) {
    assert(typeInfo);
    priv = new qore_class_private(this, std::string(nme), dom, const_cast<QoreTypeInfo*>(typeInfo));

    printd(5, "QoreClass::QoreClass() this: %p creating '%s' with custom typeinfo\n", this, priv->name.c_str());

    // see if typeinfo already accepts NOTHING
    if (QoreTypeInfo::parseAcceptsReturns(typeInfo, NT_NOTHING))
        priv->orNothingTypeInfo = const_cast<QoreTypeInfo*>(typeInfo);
    else {
        priv->orNothingTypeInfo = new QoreClassOrNothingTypeInfo(this, nme);
        priv->owns_ornothingtypeinfo = true;
    }
}

QoreClass::QoreClass() : priv(new qore_class_private(this, parse_pop_name())) {
    priv->orNothingTypeInfo = new QoreClassOrNothingTypeInfo(this, priv->name.c_str());
    priv->owns_ornothingtypeinfo = true;
}

QoreClass::~QoreClass() {
    // dereference the private data if still present
    if (priv) {
        {
            // ensure atomicity when writing to qcset
            AutoLocker al(priv->gate.asl_lock);
            qc_set_t::iterator i = priv->qcset.find(this);
            if (i != priv->qcset.end()) {
                priv->qcset.erase(i);
            }
        }

        priv->deref(true, true, true);
    }
}

void QoreClass::setUserData(const void *n_ptr) {
   priv->setUserData(n_ptr);
}

const void *QoreClass::getUserData() const {
   return priv->getUserData();
}

void QoreClass::setManagedUserData(AbstractQoreClassUserData *n_mud) {
   priv->setManagedUserData(n_mud);
}

AbstractQoreClassUserData* QoreClass::getManagedUserData() const {
   return priv->getManagedUserData();
}

QoreClass* QoreClass::getClass(qore_classid_t cid) const {
   if (cid == priv->classID)
      return (QoreClass* )this;
   return priv->scl ? priv->scl->sml.getClass(cid) : 0;
}

const QoreClass* QoreClass::getClass(qore_classid_t cid, bool& cpriv) const {
   ClassAccess access = Public;
   const QoreClass* qc = priv->getClassIntern(cid, access, true);
   cpriv = (access > Public);
   return qc;
}

const QoreClass* QoreClass::getClass(const QoreClass& qc, bool& cpriv) const {
   ClassAccess access = Public;
   const QoreClass* rv = priv->getClassIntern(*qc.priv, access, true);
   cpriv = (access > Public);
   return rv;
}

bool QoreMethod::existsVariant(const type_vec_t &paramTypeInfo) const {
   return priv->func->existsVariant(paramTypeInfo);
}

void QoreClass::insertMethod(QoreMethod* m) {
   priv->insertBuiltinMethod(m);
}

void QoreClass::insertStaticMethod(QoreMethod* m) {
   priv->insertBuiltinStaticMethod(m);
}

const QoreClass* qore_class_private::parseGetClass(const qore_class_private& qc, ClassAccess& n_access) const {
   n_access = Public;
   const_cast<qore_class_private*>(this)->initialize();
   if (parseEqual(qc))
      return (QoreClass*)cls;
   return scl ? scl->parseGetClass(qc, n_access, true) : 0;
}

bool qore_class_private::runtimeHasCallableMethod(const char* m, int mask) const {
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;

   const QoreMethod* w = 0;
   ClassAccess access;
   CurrentProgramRuntimeParseContextHelper pch;

   if (mask & QCCM_NORMAL)
      w = runtimeFindCommittedMethod(m, access, class_ctx);

   if (!w && (mask & QCCM_STATIC))
      w = runtimeFindCommittedStaticMethod(m, access, class_ctx);

   return !w || (!class_ctx && (access > Public)) ? false : true;
}

const QoreMethod* qore_class_private::getMethodForEval(const char* nme, QoreProgram* pgm, ExceptionSink* xsink) const {
   //printd(5, "qore_class_private::getMethodForEval() %s::%s() %s call attempted\n", name.c_str(), nme, runtimeCheckPrivateClassAccess() ? "external" : "internal" );

   const QoreMethod* w;
   ClassAccess access;

   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;
   //printd(5, "qore_class_private::getMethodForEval() %s::%s() class_ctx: %p %s\n", name.c_str(), nme, class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a");

   {
      ProgramRuntimeParseContextHelper pch(xsink, pgm);
      if (*xsink)
         return 0;

      if (!(w = runtimeFindCommittedMethod(nme, access, class_ctx)) && !(w = runtimeFindCommittedStaticMethod(nme, access, class_ctx)))
         return 0;
   }

   //printd(5, "QoreClass::getMethodForEval() %s::%s() found method %p class %s\n", name.c_str(), nme, w, w->getClassName());

   // check for illegal explicit call
   if (w == constructor || w == destructor || w == deleteBlocker) {
      xsink->raiseException("ILLEGAL-EXPLICIT-METHOD-CALL", "explicit calls to ::%s() methods are not allowed", nme);
      return 0;
   }

   if (w->isPrivate() && !class_ctx) {
      xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s() is private and cannot be accessed externally", name.c_str(), nme);
      return 0;
   }
   else if ((access > Public) && !class_ctx) {
      xsink->raiseException("BASE-CLASS-IS-PRIVATE", "%s() is a method of a privately-inherited class %s", nme, name.c_str());
      return 0;
   }

   return w;
}

bool qore_class_private::runtimeIsPrivateMemberIntern(const char* str, bool toplevel) const {
   QoreMemberInfo* info = members.find(str);
   if (info) {
      ClassAccess ma = info->getAccess();
      if (ma != Internal || toplevel)
         return ma > Public;
   }

   return !scl ? false : scl->runtimeIsPrivateMember(str, toplevel);
}

QoreValue QoreClass::evalMethod(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
    QORE_TRACE("QoreClass::evalMethod()");
    assert(self);

    if (!strcmp(nme, "copy")) {
        if (args) {
            xsink->raiseException("COPY-ERROR", "while calling %s::copy(): it is illegal to pass arguments to copy methods", self->getClassName());
            return QoreValue();
        }
        return execCopy(self, xsink);
    }

    const QoreMethod* w = priv->getMethodForEval(nme, self->getProgram(), xsink);
    if (*xsink)
        return QoreValue();

    if (w)
        return qore_method_private::eval(*w, xsink, self, args);

    // first see if there is a pseudo-method for this
    QoreClass* qc = nullptr;
    w = pseudo_classes_find_method(NT_OBJECT, nme, qc);
    if (w)
        return qore_method_private::evalPseudoMethod(*w, xsink, 0, self, args);
    else if (priv->methodGate && !priv->methodGate->inMethod(self)) // call methodGate with unknown method name and arguments
        return evalMethodGate(self, nme, args, xsink);

    xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() has been defined and no pseudo-method <object>::%s() is available", self->getClassName(), nme, nme);
    return QoreValue();
}

QoreValue QoreClass::evalMethodGate(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
    printd(5, "QoreClass::evalMethodGate() method: %s args: %p\n", nme, args);

    ReferenceHolder<QoreListNode> args_holder(xsink);

    // build new argument list
    if (args) {
        if (args->needs_eval())
            args_holder = args->evalList(xsink);
        else
            args_holder = args->copy();
        if (*xsink)
            return QoreValue();
    }
    else
        args_holder = new QoreListNode;

    args_holder->insert(new QoreStringNode(nme), nullptr);

    if (priv->gate_access)
        args_holder->insert(priv->runtimeCheckPrivateClassAccess() ? true : false, nullptr);

    return self->evalMethod(*priv->methodGate, *args_holder, xsink);
}

bool QoreClass::isPrivateMember(const char* str) const {
   return priv->runtimeIsPrivateMemberIntern(str, true);
}

QoreValue QoreClass::evalMemberGate(QoreObject* self, const QoreString *nme, ExceptionSink* xsink) const {
   assert(nme && nme->getEncoding() == QCS_DEFAULT);

   printd(5, "QoreClass::evalMemberGate() member: %s\n", nme->getBuffer());
   // do not run memberGate method if we are already in it...
   if (!priv->memberGate || priv->memberGate->inMethod(self))
      return QoreValue();

   ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
   args->push(new QoreStringNode(*nme), nullptr);
   if (priv->gate_access)
      args->push(priv->runtimeCheckPrivateClassAccess() ? true : false, nullptr);

   return self->evalMethod(*priv->memberGate, *args, xsink);
}

void QoreClass::execMemberNotification(QoreObject* self, const char* mem, ExceptionSink* xsink) const {
   // cannot run this method when executing from within the class
   assert((this != getStackClass()));

   //printd(5, "QoreClass::execMemberNotification() member: %s\n", mem);

   ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
   args->push(new QoreStringNode(mem), nullptr);
   self->evalMethod(*priv->memberNotification, *args, xsink).discard(xsink);
}

// FIXME: remove
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
   printd(5, "qore_class_private::execDeleteBlocker(self: %p) this: %p '%s' has_delete_blocker: %s deleteBlocker: %p\n", self, this, name.c_str(), has_delete_blocker ? "true" : "false", deleteBlocker);
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
   //printd(5, "qore_class_private::execDestructor() %s::destructor() o: %p scl: %p sml: %p, self: %p, destructor: %p, isSystemObject: %d\n", name.c_str(), self, scl, scl ? &scl->sml : 0, self, destructor, self->isSystemObject());

   // we use a new, blank exception sink to ensure all destructor code gets executed
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;

   if (self->isSystemObject()) {
      if (destructor) {
         destructor->priv->evalSystemDestructor(self, &de);
      } else {
         self->defaultSystemDestructor(classID, &de);
      }
      // execute superclass destructors
      if (scl) {
         scl->sml.execSystemDestructors(self, &de);
      }
   }
   else {
      if (destructor) {
         destructor->priv->evalDestructor(self, &de);
      } else if (sys) {
         self->defaultSystemDestructor(classID, &de);
      }
      // execute superclass destructors
      if (scl) {
         scl->sml.execDestructors(self, &de);
      }
   }

   xsink->assimilate(de);
}

void qore_class_private::execBaseClassDestructor(QoreObject* self, ExceptionSink* xsink) const {
   // we use a new, blank exception sink to ensure all destructor code gets executed
   // in case there were already exceptions in the current exceptionsink
   ExceptionSink de;
   //printd(5, "qore_class_private::execBaseClassDestructor() %s::destructor(), destructor: %p, sys: %d\n", name.c_str(), destructor, sys);
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

int qore_class_private::addBaseClassesToSubclass(QoreClass* child, bool is_virtual) {
   //printd(5, "qore_class_private::addBaseClassesToSubclass() this: %p '%s' sc: %p '%s' is_virtual: %d scl: %p\n", this, name.c_str(), sc, sc->getName(), is_virtual, scl);
   if (scl && scl->addBaseClassesToSubclass(cls, child, is_virtual))
      return -1;
   assert(child->priv->scl);
   return child->priv->scl->sml.add(child, cls, is_virtual);
}

int qore_class_private::addUserMethod(const char* mname, MethodVariantBase* f, bool n_static) {
    // FIXME: set class name at parse time
    const char* tname = name.c_str();
    printd(5, "QoreClass::addUserMethod(%s, umv: %p, priv: %d, static: %d) this: %p %s\n", mname, f, f->isPrivate(), n_static, this, tname);

    std::unique_ptr<MethodVariantBase> func(f);

    if (sys || committed) {
        parseCheckSystemCommitted(static_cast<UserSignature*>(f->getSignature())->getParseLocation());
        return -1;
    }

    if (f->isAbstract()) {
        if (initialized) {
            parseException(*static_cast<UserSignature*>(f->getSignature())->getParseLocation(), "ILLEGAL-ABSTRACT-METHOD", "abstract %s::%s(): abstract methods cannot be added to a class once the class has been committed", name.c_str(), mname);
            return -1;
        }
        if (n_static) {
            parseException(*static_cast<UserSignature*>(f->getSignature())->getParseLocation(), "ILLEGAL-ABSTRACT-METHOD", "abstract %s::%s(): abstract methods cannot be static", name.c_str(), mname);
            return -1;
        }
    }

    bool dst = !strcmp(mname, "destructor");
    bool con = dst ? false : !strcmp(mname, "constructor");

    // check for illegal static method
    if (n_static) {
        if ((con || dst || checkSpecialStaticIntern(mname))) {
            parseException(*static_cast<UserSignature*>(f->getSignature())->getParseLocation(), "ILLEGAL-STATIC-METHOD", "%s methods cannot be static", mname);
            return -1;
        }
    }

    bool cpy = dst || con ? false : !strcmp(mname, "copy");
    // check for illegal method overloads
    if (sys && (con || cpy)) {
        parseException(*static_cast<UserSignature*>(f->getSignature())->getParseLocation(), "ILLEGAL-METHOD-OVERLOAD", "class %s is builtin; %s methods in builtin classes cannot be overloaded; create a subclass instead", name.c_str(), mname);
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

    // we cannot initialize the class here
    QoreMethod* m = const_cast<QoreMethod*>(!n_static ? parseFindLocalMethod(mname) : parseFindLocalStaticMethod(mname));
    if (!n_static && m && (dst || cpy || methGate || memGate || hasMemberNotification)) {
        parseException(*static_cast<UserSignature*>(f->getSignature())->getParseLocation(), "ILLEGAL-METHOD-OVERLOAD", "a %s::%s() method has already been defined; cannot overload %s methods", tname, mname, mname);
        return -1;
    }

    // now we add the new variant to a method, creating the method if necessary

    if (!has_new_user_changes)
        has_new_user_changes = true;
    if (!has_sig_changes)
        has_sig_changes = true;

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

    //printd(5, "qore_class_private::addUserMethod() %s %s::%s(%s) f: %p (%d) new: %d\n", privpub(f->getAccess()), tname, mname, f->getSignature()->getSignatureText(), f, ((QoreReferenceCounter*)f)->reference_count(), is_new);

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
    else if (!n_static && !con && !dst)
        ahm.parseOverrideAbstractVariant(mname, f);

    return 0;
}

void QoreClass::addMethod(const char* nme, q_method_n_t m, ClassAccess access, int64 flags, int64 domain, const QoreTypeInfo* returnTypeInfo, unsigned num_params, ...) {
    type_vec_t typeList;
    arg_vec_t defaultArgList;
    name_vec_t nameList;
    if (num_params) {
        va_list args;
        va_start(args, num_params);
        qore_process_params(num_params, typeList, defaultArgList, nameList, args);
        va_end(args);
    }

    priv->addBuiltinMethod(nme, new BuiltinNormalMethodValueVariant(m, access, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addMethod(const void* ptr, const char* nme, q_external_method_t m, ClassAccess access, int64 flags, int64 domain, const QoreTypeInfo* returnTypeInfo, const type_vec_t& typeList, const arg_vec_t& defaultArgList, const name_vec_t& nameList) {
    priv->addBuiltinMethod(nme, new BuiltinExternalNormalMethodValueVariant(ptr, m, access, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addStaticMethod(const char* nme, q_func_n_t m, ClassAccess access, int64 flags, int64 domain, const QoreTypeInfo* returnTypeInfo, unsigned num_params, ...) {
    type_vec_t typeList;
    arg_vec_t defaultArgList;
    name_vec_t nameList;
    if (num_params) {
        va_list args;
        va_start(args, num_params);
        qore_process_params(num_params, typeList, defaultArgList, nameList, args);
        va_end(args);
    }

    priv->addBuiltinStaticMethod(nme, new BuiltinStaticMethodValueVariant(m, access, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addStaticMethod(const void* ptr, const char* nme, q_external_static_method_t m, ClassAccess access, int64 flags, int64 domain, const QoreTypeInfo* returnTypeInfo, const type_vec_t& typeList, const arg_vec_t& defaultArgList, const name_vec_t& nameList) {
    priv->addBuiltinStaticMethod(nme, new BuiltinExternalStaticMethodValueVariant(ptr, m, access, false, flags, domain, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addConstructor(q_constructor_n_t m, ClassAccess access, int64 n_flags, int64 n_domain, unsigned num_params, ...) {
    type_vec_t typeList;
    arg_vec_t defaultArgList;
    name_vec_t nameList;
    if (num_params) {
        va_list args;
        va_start(args, num_params);
        qore_process_params(num_params, typeList, defaultArgList, nameList, args);
        va_end(args);
    }
    priv->addBuiltinConstructor(new BuiltinConstructorValueVariant(m, access, n_flags, n_domain, typeList, defaultArgList, nameList));
}

void QoreClass::addConstructor(const void* ptr, q_external_constructor_t m, ClassAccess access, int64 n_flags, int64 n_domain, const type_vec_t& typeList, const arg_vec_t& defaultArgList, const name_vec_t& nameList) {
    priv->addBuiltinConstructor(new BuiltinExternalConstructorValueVariant(ptr, m, access, n_flags, n_domain, typeList, defaultArgList, nameList));
}

void QoreClass::addAbstractMethod(const char *n_name, ClassAccess access, int64 n_flags, const QoreTypeInfo* returnTypeInfo, unsigned num_params, ...) {
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

    priv->addBuiltinMethod(n_name, new BuiltinAbstractMethodVariant(access, n_flags, returnTypeInfo, typeList, defaultArgList, nameList));
}

void QoreClass::addAbstractMethod(const char* n_name, ClassAccess access, int64 n_flags, const QoreTypeInfo* returnTypeInfo, const type_vec_t& typeList, const arg_vec_t& defaultArgList, const name_vec_t& nameList) {
    priv->addBuiltinMethod(n_name, new BuiltinAbstractMethodVariant(access, n_flags, returnTypeInfo, typeList, defaultArgList, nameList));
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor(q_destructor_t m) {
    priv->addBuiltinDestructor(new BuiltinDestructorVariant(m));
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor(const void *ptr, q_external_destructor_t m) {
    priv->addBuiltinDestructor(new BuiltinExternalDestructorVariant(ptr, m));
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy(q_copy_t m) {
    priv->addBuiltinCopyMethod(new BuiltinCopyVariant(this, m));
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy(const void* ptr, q_external_copy_t m) {
    priv->addBuiltinCopyMethod(new BuiltinExternalCopyVariant(ptr, this, m));
}

// sets the delete_blocker function
void QoreClass::setDeleteBlocker(q_delete_blocker_t m) {
    priv->setDeleteBlocker(m);
}

// sets the final flag
void QoreClass::setFinal() {
    qore_class_private::setFinal(*this);
}

void QoreClass::setSystemConstructor(q_system_constructor_t m) {
    priv->setBuiltinSystemConstructor(new BuiltinSystemConstructor(this, m));
}

QoreListNode* QoreClass::getMethodList() const {
    QoreListNode* l = new QoreListNode(stringTypeInfo);

    for (hm_method_t::const_iterator i = priv->hm.begin(), e = priv->hm.end(); i != e; ++i)
        l->push(new QoreStringNode(i->first), nullptr);
    return l;
}

QoreListNode* QoreClass::getStaticMethodList() const {
    QoreListNode* l = new QoreListNode(stringTypeInfo);

    for (hm_method_t::const_iterator i = priv->shm.begin(), e = priv->shm.end(); i != e; ++i)
        l->push(new QoreStringNode(i->first), nullptr);
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
                    std::unique_ptr<AbstractMethod> m(new AbstractMethod);
                    // see if there are pending normal variants...
                    hm_method_t::iterator mi = hm.find(ai->first);
                    //printd(5, "qore_class_private::parseInitPartialIntern() this: %p '%s' looking for local '%s': %d\n", this, name.c_str(), ai->first.c_str(), mi != hm.end());
                    m->parseMergeBase(*(ai->second), mi == hm.end() ? 0 : mi->second->getFunction());
                    if (!m->empty()) {
                        ahm.insert(amap_t::value_type(ai->first, m.release()));
                    }
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
}

void qore_class_private::parseInit() {
    // make sure initialize() is called first
    initialize();

    //printd(5, "qore_class_private::parseInit() this: %p '%s' parse_init_called: %d parse_init_partial_called: %d\n", this, name.c_str(), parse_init_called, parse_init_partial_called);
    if (parse_init_called) {
        return;
    }

    parse_init_called = true;

    if (has_new_user_changes) {
        NamespaceParseContextHelper nspch(ns);
        QoreParseClassHelper qpch(cls);

        if (!parse_init_partial_called)
            parseInitPartialIntern();

        // initialize constants
        constlist.parseInit();

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
}

void qore_class_private::parseResolveHierarchy() {
    if (!parse_resolve_hierarchy) {
        parse_resolve_hierarchy = true;

        if (!scl)
            return;

        qcp_set_t qcp_set = {this};
        // issue #2657: initialize class hierarchy first before initializing code and members
        initializeHierarchy(qcp_set);
    }
}

void qore_class_private::parseResolveAbstract() {
    if (!parse_resolve_abstract) {
        parse_resolve_abstract = true;

        if (!scl)
            return;

        scl->parseResolveAbstract();

        // search for new concrete variants of abstract variants last
        ahm.parseInit(*this, scl);
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

int qore_class_private::checkExistingVarMember(const char* dname, const QoreMemberInfoBaseAccess* mi, const QoreMemberInfoBaseAccess* omi, const qore_class_private* qc, ClassAccess oaccess, bool var) const {
    //printd(5, "qore_class_private::checkExistingVarMember() name: %s priv: %d is_priv: %d sclass: %s\n", name.c_str(), priv, is_priv, sclass->getName());

    // here we know that the member or var already exists, so either it will be a
    // duplicate declaration, in which case it is ignored if it has no type info, or it is a
    // contradictory declaration, in which case a parse exception is raised

    // if the var was previously declared public
    if (mi->access != oaccess) {
        // raise an exception only if parse exceptions are enabled
        if (getProgram()->getParseExceptionSink()) {
            QoreStringNode* desc = new QoreStringNode;
            desc->sprintf("class '%s' ", name.c_str());
            desc->concat("cannot declare ");
            desc->sprintf("%s %s ", privpub(mi->access), var ? "static variable" : "member");
            desc->sprintf("'%s' when ", dname);
            if (qc == this)
                desc->concat("this class");
            else
                desc->sprintf("base class '%s'", qc->name.c_str());
            desc->sprintf(" already declared this %s as %s", var ? "variable" : "member", privpub(oaccess));
            qore_program_private::makeParseException(getProgram(), *mi->loc, "PARSE-ERROR", desc);
        }
        return -1;
    }
    else if ((mi && mi->parseHasTypeInfo()) || (omi && omi->parseHasTypeInfo())) {
        if (getProgram()->getParseExceptionSink()) {
            QoreStringNode* desc = new QoreStringNode;
            desc->sprintf("%s %s ", privpub(mi->access), var ? "static variable" : "member");
            desc->sprintf("'%s' was already declared in ", dname);
            if (qc == this)
                desc->concat("this class");
            else
                desc->sprintf("base class '%s'", qc->name.c_str());
            if (mi && mi->parseHasTypeInfo())
                desc->sprintf(" with a type definition");
            desc->concat(" and cannot be declared again");
            desc->sprintf(" in class '%s'", name.c_str());
            desc->concat(" if the declaration has a type definition");

            qore_program_private::makeParseException(getProgram(), *mi->loc, "PARSE-TYPE-ERROR", desc);
        }
        return -1;
    }

    return 0;
}

QoreValue qore_class_private::evalPseudoMethod(const QoreValue n, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const {
   QORE_TRACE("qore_class_private::evalPseudoMethod()");

   const QoreMethod* m = runtimeFindPseudoMethod(n, nme, xsink);
   if (!m)
      return QoreValue();

   //printd(5, "qore_class_private::evalPseudoMethod() %s::%s() found method %p class %s\n", priv->name, nme, w, w->getClassName());

   return qore_method_private::evalPseudoMethod(*m, xsink, 0, n, args);
}

QoreValue qore_class_private::evalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const QoreValue n, const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::evalPseudoMethod(*m, xsink, variant, n, args);
}

bool qore_class_private::parseCheckPrivateClassAccess(const qore_class_private* opc) const {
   // see if shouldBeClass is a parent class of the class currently being parsed
   //ClassAccess access1 = Public;
   //printd(5, "qore_class_private::parseCheckPrivateClassAccess(%p '%s') pc: %p '%s' found: %p\n", this, name.c_str(), opc, opc ? opc->name.c_str() : "n/a", opc ? opc->getClass(*this, access1) : nullptr);

   if (!opc)
      return false;

   ClassAccess access = Public;
   return opc->parseGetClass(*this, access) || (scl && scl->parseGetClass(*opc, access, true));
}

bool qore_class_private::runtimeCheckPrivateClassAccess(const qore_class_private* qc) const {
   if (!qc) {
      //printd(5, "runtimeCheckPrivateClassAccess() this: %p '%s' no runtime class context: failed\n", this, name.c_str());
      return false;
   }

   ClassAccess access = Public;
   //printd(5, "runtimeCheckPrivateClassAccess() qc: %p '%s' test: %p '%s' okl: %d okr: %d\n", qc, qc->name.c_str(), this, name.c_str(), qc->getClassIntern(*this, access, true), (scl && scl->getClass(*qc, access, true)));
   return qc->getClassIntern(*this, access, true) || (scl && scl->getClass(*qc, access, true));
}

qore_type_result_e qore_class_private::parseCheckCompatibleClass(const qore_class_private& oc, bool& may_not_match) const {
   qore_type_result_e rv = parseCheckCompatibleClassIntern(oc, may_not_match);
   if (rv != QTI_NOT_EQUAL)
      return rv;
   if (injectedClass) {
      rv = injectedClass->parseCheckCompatibleClass(oc, may_not_match);
      if (rv != QTI_NOT_EQUAL)
         return rv;
   }
   // FIXME: with %strict-types, this check should not be made (cast<>s are obligatory in that case)
   if (oc.injectedClass) {
      qore_type_result_e rv = oc.injectedClass->parseCheckCompatibleClass(*this, may_not_match);
      if (!may_not_match && rv != QTI_NOT_EQUAL)
         may_not_match = true;
      return rv;
   }
   return QTI_NOT_EQUAL;
}

qore_type_result_e qore_class_private::parseCheckCompatibleClassIntern(const qore_class_private& oc, bool& may_not_match) const {
   // make sure both classes are initialized
   const_cast<qore_class_private*>(this)->initialize();
   const_cast<qore_class_private&>(oc).initialize();

#ifdef DEBUG_SKIP
   QoreString h1, h2;
   hash.toString(h1);
   oc.hash.toString(h2);
   printd(5, "qore_class_private::parseCheckCompatibleClass() %p '%s' (%d %s) == %p '%s' (%d %s)\n", this, name.c_str(), classID, h1.getBuffer(), &oc, oc.name.c_str(), oc.classID, h2.getBuffer());
#endif

   if (parseEqual(oc))
      return QTI_IDENT;

   // FIXME: with %strict-types, the second check should not be made (cast<>s are obligatory in that case)
   ClassAccess access;
   if (!parseGetClass(oc, access)) {
      if (!oc.parseGetClass(*this, access))
         return QTI_NOT_EQUAL;
      may_not_match = true;
   }

   if (access == Public)
      return QTI_AMBIGUOUS;

   if (parseCheckPrivateClassAccess())
      return QTI_AMBIGUOUS;

   if (!parseCheckPrivateClassAccess()) {
      if (may_not_match)
         may_not_match = false;
      return QTI_NOT_EQUAL;
   }

   return QTI_AMBIGUOUS;
}

qore_type_result_e qore_class_private::runtimeCheckCompatibleClass(const qore_class_private& oc) const {
   qore_type_result_e rv = runtimeCheckCompatibleClassIntern(oc);
   if (rv != QTI_NOT_EQUAL)
      return rv;
   if (injectedClass) {
      rv = injectedClass->runtimeCheckCompatibleClassIntern(oc);
      if (rv != QTI_NOT_EQUAL)
         return rv;
   }
   return QTI_NOT_EQUAL;
}

qore_type_result_e qore_class_private::runtimeCheckCompatibleClassIntern(const qore_class_private& oc) const {
   if (equal(oc))
      return QTI_IDENT;

   ClassAccess access = Public;
   if (!oc.scl || !oc.scl->getClass(*this, access, true))
      return QTI_NOT_EQUAL;

   if (access == Public)
      return QTI_AMBIGUOUS;

   return runtimeCheckPrivateClassAccess() ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
}

bool QoreClass::hasParentClass() const {
   return (bool)priv->scl;
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

const QoreTypeInfo* QoreClass::getTypeInfo() const {
   return priv->getTypeInfo();
}

const QoreTypeInfo* QoreClass::getOrNothingTypeInfo() const {
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
   if (!priv->has_new_user_changes)
      priv->has_new_user_changes = true;
}

bool QoreClass::isPublicOrPrivateMember(const char* str, bool& priv_member) const {
   ClassAccess access;
   bool internal_member;
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !priv->runtimeCheckPrivateClassAccess(class_ctx))
      class_ctx = 0;
   bool rv = (bool)priv->runtimeGetMemberClass(str, access, class_ctx, internal_member);
   priv_member = access > Public;
   return rv;
}

bool QoreClass::hasPrivateCopyMethod() const {
   return priv->copyMethod && priv->copyMethod->isPrivate();
}

bool QoreClass::parseHasPrivateCopyMethod() const {
   return priv->copyMethod && (qore_method_private::getAccess(*priv->copyMethod) > Public);
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

void QoreClass::addBuiltinConstant(const char* name, QoreValue value, ClassAccess access, const QoreTypeInfo* typeInfo) {
    priv->addBuiltinConstant(name, value.takeNode(), access, typeInfo);
}

void QoreClass::addBuiltinStaticVar(const char* name, QoreValue value, ClassAccess access, const QoreTypeInfo* typeInfo) {
    priv->addBuiltinStaticVar(name, value.takeNode(), access, typeInfo);
}

void QoreClass::rescanParents() {
   // rebuild parent class data
   if (priv->scl)
      priv->scl->rescanParents(this);
}

void QoreClass::setPublicMemberFlag() {
   priv->has_public_memdecl = true;
}

void QoreClass::setGateAccessFlag() {
   priv->gate_access = true;
}

void MethodFunctionBase::parseInit() {
    QoreFunction::parseInit(qore_class_private::get(*qc)->ns);
}

void MethodFunctionBase::parseCommit() {
   QoreFunction::parseCommit();
}

void MethodFunctionBase::parseRollback() {
   QoreFunction::parseRollback();
}

int MethodFunctionBase::checkFinalVariant(const MethodFunctionBase* m, const MethodVariantBase* v) const {
   if (!v->isFinal())
      return 0;

   const AbstractFunctionSignature* sig = v->getSignature();
   const AbstractFunctionSignature* vs = nullptr;
   int rc = parseCompareResolvedSignature(vlist, sig, vs);
   if (rc == QTI_NOT_EQUAL)
      return 0;

   const char* stat = isStatic() ? "static " : "";
   // can only be overridden with a user variant
   assert(dynamic_cast<const UserSignature*>(vs));
   parse_error(*static_cast<const UserSignature*>(vs)->getParseLocation(), "child class method %s%s::%s(%s) cannot override parent class method final %s%s::%s(%s)", stat, qc->getName(), getName(), vs->getSignatureText(), stat, m->qc->getName(), getName(), sig->getSignatureText());
   return -1;
}

void MethodFunctionBase::checkFinal() const {
   // only check if we have new pending variants in this method
   if (vlist.empty()) {
      //printd(5, "MethodFunctionBase::checkFinal() %s::%s() pending list is empty\n", qc->getName(), getName());
      return;
   }

   ilist_t::const_iterator i = ilist.begin(), e = ilist.end();
   ++i;
   for (; i != e; ++i) {
      const MethodFunctionBase* m = METHFB_const((*i).func);
      //printd(5, "MethodFunctionBase::checkFinal() base method %s::%s() has_final: %d against child %s::%s()\n", m->qc->getName(), getName(), m->has_final, qc->getName(), getName());
      if (m->has_final) {
         for (vlist_t::const_iterator i = m->vlist.begin(), e = m->vlist.end(); i != e; ++i) {
            if (checkFinalVariant(m, METHVB_const(*i)))
               return;
         }
      }
   }
}

void MethodFunctionBase::addBuiltinMethodVariant(MethodVariantBase* variant) {
    ClassAccess ma = variant->getAccess();
    if (access > ma)
        access = ma;
    if (!has_final && variant->isFinal())
        has_final = true;
    addBuiltinVariant(variant);
}

int MethodFunctionBase::parseAddUserMethodVariant(MethodVariantBase* variant) {
    int rc = addPendingVariant(variant);
    if (!rc) {
        ClassAccess ma = variant->getAccess();
        if (access > ma)
            access = ma;
        if (!has_final && variant->isFinal())
            has_final = true;
    }
    return rc;
}

static void do_variant_sig(QoreString& csig, const std::string& name, const MethodVariantBase* v, const char* mod) {
    if (v->isAbstract()) {
        csig.concat("abstract ");
    }
    switch (v->getAccess()) {
        case Public: csig.concat("pub ");
        case Private: csig.concat("priv ");
        case Internal: csig.concat("int ");
    }
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

void MethodFunctionBase::parseSignatures(QoreString& csig, const char* mod) const {
    for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
        const MethodVariantBase* v = METHVB_const(*i);
        do_variant_sig(csig, name, v, mod);
    }
}

void MethodFunctionBase::parseCommitMethod(QoreString& csig, const char* mod) {
    parseSignatures(csig, mod);
    parseCommitMethod();
}

void MethodFunctionBase::parseCommitMethod() {
    parseCommit();
}

void MethodFunctionBase::parseRollbackMethod() {
    parseRollback();
}

void MethodFunctionBase::replaceAbstractVariantIntern(MethodVariantBase* variant) {
    variant->ref();
    AbstractFunctionSignature& sig = *(variant->getSignature());
    for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
        (*i)->parseResolveUserSignature();
        if ((*i)->isSignatureIdentical(sig)) {
            pending_save.push_back(*i);
            vlist.erase(i);
            vlist.push_back(variant);
            //printd(5, "MethodFunctionBase::replaceAbstractVariantIntern() this: %p replacing %p ::%s%s in vlist\n", this, variant, getName(), variant->getAbstractSignature());
            return;
        }
    }
    //printd(5, "MethodFunctionBase::replaceAbstractVariantIntern() this: %p adding %p ::%s%s to vlist\n", this, variant, getName(), variant->getAbstractSignature());
    vlist.push_back(variant);
}

void MethodFunctionBase::replaceAbstractVariant(MethodVariantBase* variant) {
    replaceAbstractVariantIntern(variant);

    ClassAccess ma = variant->getAccess();
    if (access > ma)
        access = ma;

    if (!has_final && variant->isFinal())
        has_final = true;
}

// if an identical signature is found to the passed variant, then it is removed from the abstract list
MethodVariantBase* MethodFunctionBase::parseHasVariantWithSignature(MethodVariantBase* v) const {
    v->parseResolveUserSignature();
    AbstractFunctionSignature& sig = *(v->getSignature());
    for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
        (*i)->parseResolveUserSignature();
        if ((*i)->isSignatureIdentical(sig))
            return reinterpret_cast<MethodVariantBase*>(*i);
    }
    return nullptr;
}

QoreValue UserMethodVariant::evalMethod(QoreObject* self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
    VRMutexOptionalLockHelper vrmolh(synchronized ? (self ? qore_object_private::get(*self)->getGate() : ceh.getClass()->getGate()) : 0, xsink);
    if (*xsink)
        return QoreValue();
    //printd(5, "UserMethodVariant::evalMethod() this: %p %s::%s()\n", this, getClassPriv()->name.c_str(), qmethod->getName());
    return eval(qmethod->getName(), &ceh, self, xsink, getClassPriv());
}

void BuiltinConstructorValueVariant::evalConstructor(const QoreClass& thisclass, QoreObject* self, CodeEvaluationHelper &ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const {
    CodeContextHelper cch(xsink, CT_BUILTIN, "constructor", self, qore_class_private::get(thisclass));

    if (constructorPrelude(thisclass, ceh, self, bcl, bceal, xsink))
        return;

    constructor(self, ceh.getArgs(), ceh.getRuntimeFlags(), xsink);
}

void BuiltinExternalConstructorValueVariant::evalConstructor(const QoreClass& thisclass, QoreObject* self, CodeEvaluationHelper &ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const {
    CodeContextHelper cch(xsink, CT_BUILTIN, "constructor", self, qore_class_private::get(thisclass));

    if (constructorPrelude(thisclass, ceh, self, bcl, bceal, xsink))
        return;

    constructor(*qmethod, ptr, self, ceh.getArgs(), ceh.getRuntimeFlags(), xsink);
}

int ConstructorMethodVariant::constructorPrelude(const QoreClass& thisclass, CodeEvaluationHelper& ceh, QoreObject* self, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const {
    if (bcl) {
        const BCAList* bcal = getBaseClassArgumentList();
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

void UserConstructorVariant::evalConstructor(const QoreClass &thisclass, QoreObject* self, CodeEvaluationHelper &ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const {
   UserVariantExecHelper uveh(this, &ceh, xsink);
   if (!uveh)
      return;

   CodeContextHelper cch(xsink, CT_USER, "constructor", self, qore_class_private::get(thisclass), false);

   // instantiate "self" before executing base class constructors in case base class constructor arguments reference "self"
   assert(signature.selfid);
   signature.selfid->instantiateSelf(self);

   // instantiate argv and push id on stack for base class constructors
   if (bcl) {
      ReferenceHolder<QoreListNode>& argv = uveh.getArgv();
      signature.argvid->instantiate(argv ? argv->refSelf() : 0);
      ArgvContextHelper argv_helper(argv ? argv->listRefSelf() : 0, xsink);
   }

   if (!constructorPrelude(thisclass, ceh, self, bcl, bceal, xsink)) {
      evalIntern(uveh.getArgv(), 0, xsink).discard(xsink);
   }

   // uninstantiate argv
   if (bcl)
      signature.argvid->uninstantiate(xsink);

   // if self then uninstantiate
   signature.selfid->uninstantiateSelf();
}

void UserConstructorVariant::parseInit(QoreFunction* f) {
   MethodFunctionBase* mf = static_cast<MethodFunctionBase*>(f);
   const QoreClass& parent_class = *(mf->MethodFunctionBase::getClass());

   signature.resolve();
   assert(!signature.getReturnTypeInfo() || signature.getReturnTypeInfo() == nothingTypeInfo);

   // push return type on stack (no return value can be used)
   ParseCodeInfoHelper rtih("constructor", nothingTypeInfo);

   if (bcal && !parent_class.hasParentClass()) {
      parse_error(*signature.getParseLocation(), "base constructor arguments given for class '%s' that has no parent classes", parent_class.getName());
      delete bcal;
      bcal = nullptr;
   }

   //printd(5, "UserConstructorVariant::parseInitConstructor() this: %p %s::constructor() params: %d\n", this, parent_class.getName(), signature.numParams());
   // must be called even if statements is NULL
   statements->parseInitConstructor(parent_class.getTypeInfo(), this, bcal, parent_class);

   // recheck types against committed types if necessary
   if (recheck)
      f->parseCheckDuplicateSignatureCommitted(&signature);
}

void BuiltinDestructorVariant::evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const {
   CodeContextHelper cch(xsink, CT_BUILTIN, "destructor", self, qore_class_private::get(thisclass));

   AbstractPrivateData* private_data = self->getAndClearPrivateData(thisclass.getID(), xsink);
   //printd(5, "BuiltinDestructorVariant::evalDestructor() o: %p, v: %d, classid: %d, private: %p\n", self, self->isValid(), thisclass.getID(), private_data);
   if (!private_data)
      return;
   destructor(self, private_data, xsink);
}

void BuiltinExternalDestructorVariant::evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const {
    CodeContextHelper cch(xsink, CT_BUILTIN, "destructor", self, qore_class_private::get(thisclass));

    AbstractPrivateData* private_data = self->getAndClearPrivateData(thisclass.getID(), xsink);
    //printd(5, "BuiltinExternalDestructorVariant::evalDestructor() o: %p, v: %d, private: %p\n", self, self->isValid(), private_data);
    if (!private_data)
        return;
    destructor(thisclass, ptr, self, private_data, xsink);
}

void UserCopyVariant::evalCopy(const QoreClass& thisclass, QoreObject* self, QoreObject* old, CodeEvaluationHelper& ceh, BCList* scl, ExceptionSink* xsink) const {
   // there can only be max 1 param
   assert(signature.numParams() <= 1);

   QoreListNode* args = new QoreListNode;
   args->push(self->refSelf(), nullptr);
   ceh.setArgs(args);

   UserVariantExecHelper uveh(this, &ceh, xsink);
   if (!uveh)
      return;

   CodeContextHelper cch(xsink, CT_USER, "copy", self, qore_class_private::get(thisclass));

   if (scl) {
      scl->sml.execCopyMethods(self, old, xsink);
      if (*xsink)
         return;
      ceh.restorePosition();
   }

   evalIntern(uveh.getArgv(), self, xsink).discard(xsink);
}

void UserCopyVariant::parseInit(QoreFunction* f) {
   MethodFunctionBase* mf = static_cast<MethodFunctionBase*>(f);
   const QoreClass& parent_class = *(mf->MethodFunctionBase::getClass());

   signature.resolve();

   // make sure there is max one parameter in the copy method
   if (signature.numParams() > 1)
      parse_error(*signature.getParseLocation(), "maximum of one parameter may be defined in class copy methods (%d defined); this parameter will be assigned to the old object when the method is executed", signature.numParams());

   // push return type on stack (no return value can be used)
   ParseCodeInfoHelper rtih("copy", nothingTypeInfo);

   // must be called even if statements is NULL
   statements->parseInitMethod(parent_class.getTypeInfo(), this);

   // see if there is a type specification for the sole parameter and make sure it matches the class if there is
   if (signature.numParams()) {
      const QoreTypeInfo* typeInfo = signature.getParamTypeInfo(0);
      if (typeInfo) {
         if (QoreTypeInfo::parseReturns(typeInfo, &parent_class) == QTI_NOT_EQUAL) {
            // raise parse exception if parse exceptions have not been suppressed
            if (getProgram()->getParseExceptionSink()) {
               QoreStringNode* desc = new QoreStringNode("the copy constructor will be passed ");
               QoreTypeInfo::getThisType(parent_class.getTypeInfo(), *desc);
               desc->concat(", but the object's parameter was defined expecting ");
               QoreTypeInfo::getThisType(typeInfo, *desc);
               desc->concat(" instead");
               qore_program_private::makeParseException(getProgram(), *signature.getParseLocation(), "PARSE-TYPE-ERROR", desc);
            }
         }
      }
      else { // set to class' type
         signature.setFirstParamType(parent_class.getTypeInfo());
      }
   }

   // only 1 variant is possible, no need to recheck types
}

void BuiltinCopyVariantBase::evalCopy(const QoreClass& thisclass, QoreObject* self, QoreObject* old, CodeEvaluationHelper& ceh, BCList* scl, ExceptionSink* xsink) const {
   CodeContextHelper cch(xsink, CT_BUILTIN, "copy", self, qore_class_private::get(thisclass));

   if (scl) {
      scl->sml.execCopyMethods(self, old, xsink);
      if (*xsink)
         return;
      ceh.restorePosition();
   }

   old->evalCopyMethodWithPrivateData(thisclass, this, self, xsink);
}

void ConstructorMethodFunction::evalConstructor(const AbstractQoreFunctionVariant* variant, const QoreClass& thisclass, QoreObject* self, const QoreListNode* args, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const {
   // setup call, save runtime position, and evaluate arguments
   CodeEvaluationHelper ceh(xsink, this, variant, "constructor", args, self, qore_class_private::get(thisclass));
   if (*xsink)
      return;

   CONMV_const(variant)->evalConstructor(thisclass, self, ceh, bcl, bceal, xsink);
}

void CopyMethodFunction::evalCopy(const QoreClass& thisclass, QoreObject* self, QoreObject* old, BCList* scl, ExceptionSink* xsink) const {
   assert(vlist.singular());

   const AbstractQoreFunctionVariant* variant = first();
   qore_call_t ct = variant->getCallType();

   // setup call, save runtime position
   CodeEvaluationHelper ceh(xsink, this, variant, "copy", (QoreListNode*)nullptr, self, qore_class_private::get(thisclass), ct, true);
   if (*xsink) return;

   COPYMV_const(variant)->evalCopy(thisclass, self, old, ceh, scl, xsink);
}

void DestructorMethodFunction::evalDestructor(const QoreClass& thisclass, QoreObject* self, ExceptionSink* xsink) const {
   assert(vlist.singular());

   const AbstractQoreFunctionVariant* variant = first();
   qore_call_t ct = variant->getCallType();

   // setup call, save runtime position
   //printd(5, "DestructorMethodFunction::evalDestructor() %s::%s() o: %p, v: %d, ct: %d\n", getClassName(), getName(), self, self->isValid(),ct);
   CodeEvaluationHelper ceh(xsink, this, variant, "destructor", (QoreListNode*)nullptr, self, qore_class_private::get(thisclass), ct);
   if (*xsink) return;

   DESMV_const(variant)->evalDestructor(thisclass, self, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
QoreValue NormalMethodFunction::evalMethod(ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, QoreObject* self, const QoreListNode* args, const qore_class_private* cctx) const {
    const char* cname = getClassName();
    const char* mname = getName();
    //printd(5, "NormalMethodFunction::evalMethod() %s::%s() v: %d\n", cname, mname, self->isValid());

    CodeEvaluationHelper ceh(xsink, this, variant, mname, args, self, qore_class_private::get(*qc), CT_UNUSED, false, cctx);
    if (*xsink)
        return QoreValue();

    const MethodVariant* mv = METHV_const(variant);
    if (mv->isAbstract()) {
        xsink->raiseException("ABSTRACT-VARIANT-ERROR", "cannot call abstract variant %s::%s(%s) directly", cname, mname, mv->getSignature()->getSignatureText());
        return QoreValue();
    }
    //printd(5, "NormalMethodFunction::evalMethod() %s::%s(%s) (self: %s, v: %s) variant: %p, mv: %p priv: %d access: %d (%p %s)\n",getClassName(), mname, mv->getSignature()->getSignatureText(), self->getClass()->getName(), variant->getClass()->getName(), variant, mv, mv->isPrivate(), qore_class_private::runtimeCheckPrivateClassAccess(*mv->getClass()), runtime_get_class(), runtime_get_class() ? runtime_get_class()->name.c_str() : "n/a");

    return mv->evalMethod(self, ceh, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
QoreValue NormalMethodFunction::evalMethodTmpArgs(ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, QoreObject* self, QoreListNode* args, const qore_class_private* cctx) const {
    const char* cname = getClassName();
    const char* mname = getName();
    //printd(5, "NormalMethodFunction::evalMethod() %s::%s() v: %d\n", cname, mname, self->isValid());

    CodeEvaluationHelper ceh(xsink, this, variant, mname, args, self, qore_class_private::get(*qc), CT_UNUSED, false, cctx);
    if (*xsink)
        return QoreValue();

    const MethodVariant* mv = METHV_const(variant);
    if (mv->isAbstract()) {
        xsink->raiseException("ABSTRACT-VARIANT-ERROR", "cannot call abstract variant %s::%s(%s) directly", cname, mname, mv->getSignature()->getSignatureText());
        return QoreValue();
    }
    //printd(5, "NormalMethodFunction::evalMethod() %s::%s(%s) (self: %s) variant: %p, mv: %p priv: %d access: %d (%p %s)\n",getClassName(), mname, mv->getSignature()->getSignatureText(), self->getClass()->getName(), variant, mv, mv->isPrivate(), qore_class_private::runtimeCheckPrivateClassAccess(*mv->getClass()), runtime_get_class(), runtime_get_class() ? runtime_get_class()->name.c_str() : "n/a");

    return mv->evalMethod(self, ceh, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
QoreValue NormalMethodFunction::evalPseudoMethod(ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, const QoreValue n, const QoreListNode* args, const qore_class_private* cctx) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, 0, qore_class_private::get(*qc), CT_UNUSED, false, cctx);
   if (*xsink)
      return QoreValue();

   return METHV_const(variant)->evalPseudoMethod(n, ceh, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
QoreValue StaticMethodFunction::evalMethod(ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, const QoreListNode* args, const qore_class_private* cctx) const {
   const char* mname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, mname, args, 0, qore_class_private::get(*qc), CT_UNUSED, false, cctx);
   if (*xsink)
      return QoreValue();

   return METHV_const(variant)->evalMethod(0, ceh, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
QoreValue StaticMethodFunction::evalMethodTmpArgs(ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, QoreListNode* args, const qore_class_private* cctx) const {
    const char* mname = getName();
    CodeEvaluationHelper ceh(xsink, this, variant, mname, args, 0, qore_class_private::get(*qc), CT_UNUSED, false, cctx);
    if (*xsink)
        return QoreValue();

    return METHV_const(variant)->evalMethod(0, ceh, xsink);
}

const qore_class_private* MethodVariantBase::getClassPriv() const {
    return qore_class_private::get(*(qmethod->getClass()));
}

const char* MethodVariantBase::getAbstractSignature() {
    if (asig.empty())
        getSignature()->addAbstractParameterSignature(asig);
    return asig.c_str();
}

QoreValue BuiltinNormalMethodVariantBase::evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
    CodeContextHelper cch(xsink, CT_BUILTIN, qmethod->getName(), self, qore_class_private::get(*qmethod->getClass()));
    return qore_object_private::evalBuiltinMethodWithPrivateData(*self, *qmethod, this, ceh.getArgs(), ceh.getRuntimeFlags(), xsink);
}

QoreValue BuiltinNormalMethodVariantBase::evalPseudoMethod(const QoreValue n, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
    CodeContextHelper cch(xsink, CT_BUILTIN, qmethod->getName());
    return evalImpl(NULL, (AbstractPrivateData*)&n, ceh.getArgs(), ceh.getRuntimeFlags(), xsink);
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
#define HMI_CAST(p) (reinterpret_cast<qmi_priv*>(p))

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

void QoreMemberInfo::parseInit(const char* name) {
   if (init)
      return;
   init = true;

   if (!typeInfo) {
      typeInfo = QoreParseTypeInfo::resolveAndDelete(parseTypeInfo, loc);
      parseTypeInfo = 0;
   }
#ifdef DEBUG
   else assert(!parseTypeInfo);
#endif

   if (exp) {
      const QoreTypeInfo* argTypeInfo = 0;
      int lvids = 0;
      //printd(5, "QoreMemberInfo::parseInit() this: %p '%s' %p '%s' %d\n", this, name, exp, get_type_name(exp), get_node_type(exp));
      parse_init_value(exp, 0, 0, lvids, argTypeInfo);
      if (lvids) {
         parse_error(*loc, "illegal local variable declaration in member initialization expression");
         while (lvids--)
            pop_local_var();
      }
      // throw a type exception only if parse exceptions are enabled
      if (!QoreTypeInfo::parseAccepts(typeInfo, argTypeInfo) && getProgram()->getParseExceptionSink()) {
         QoreStringNode* desc = new QoreStringNode("initialization expression for ");
         desc->sprintf("%s member '%s' returns ", privpub(access), name);
         QoreTypeInfo::getThisType(argTypeInfo, *desc);
         desc->concat(", but the member was declared as ");
         QoreTypeInfo::getThisType(typeInfo, *desc);
         qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", desc);
      }
   }
}

void QoreVarInfo::parseInit(const char* name) {
   if (QoreMemberInfoBaseAccess::init)
      return;
   QoreMemberInfoBaseAccess::init = true;

   if (!typeInfo) {
      typeInfo = QoreParseTypeInfo::resolveAndDelete(parseTypeInfo, loc);
      parseTypeInfo = 0;
   }
#ifdef DEBUG
   else assert(!parseTypeInfo);
#endif

   val.set(typeInfo);

   if (exp) {
      const QoreTypeInfo* argTypeInfo = 0;
      int lvids = 0;
      parse_init_value(exp, 0, 0, lvids, argTypeInfo);
      if (lvids) {
         parse_error(*loc, "illegal local variable declaration in class static variable initialization expression");
         while (lvids--)
            pop_local_var();
      }
      // throw a type exception only if parse exceptions are enabled
      if (!QoreTypeInfo::parseAccepts(typeInfo, argTypeInfo) && getProgram()->getParseExceptionSink()) {
         QoreStringNode* desc = new QoreStringNode("initialization expression for ");
         desc->sprintf("%s class static variable '%s' returns ", privpub(access), name);
         QoreTypeInfo::getThisType(argTypeInfo, *desc);
         desc->concat(", but the variable was declared as ");
         QoreTypeInfo::getThisType(typeInfo, *desc);
         qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", desc);
      }
   }
}

QoreParseClassHelper::QoreParseClassHelper(QoreClass* cls) : old(parse_get_class()), oldns(cls ? parse_get_ns() : 0), rn(cls) {
   setParseClass(cls);
   if (cls)
      parse_set_ns(qore_class_private::get(*cls)->ns);
}

QoreParseClassHelper::~QoreParseClassHelper() {
   if (rn)
      parse_set_ns(oldns);
   setParseClass(old);
}

void QoreMemberMap::parseInit() {
    //printd(5, "QoreMemberMap::parseInit() this: %p init: %d\n", this, init);
    if (init)
        return;
    init = true;
    for (SigOrderIterator i = beginSigOrder(), e = endSigOrder(); i != e; ++i) {
        printd(5, "QoreMemberMap::parseInit() this: %p mem: '%s' (%p) type: %s (%d)\n", this, i->first, i->second, i->second->exp.getTypeName(), i->second->exp.getType());
        if (i->second)
            i->second->parseInit(i->first);
    }
}

void QoreMemberMap::moveAllTo(QoreClass* qc, ClassAccess access) {
   if (empty() && access == Public) {
      qc->parseSetEmptyPublicMemberDeclaration();
      return;
   }
   for (DeclOrderIterator i = beginDeclOrder(); i != endDeclOrder(); ++i) {
      qore_class_private::parseAddMember(*qc, i->first, access, i->second);
   }
   map.clear();
   list.clear();
}

void QoreVarMap::parseCommitRuntimeInit(ExceptionSink* xsink) {
    if (init) {
        return;
    }
    init = true;
    assert(xsink);
    for (QoreVarMap::DeclOrderIterator i = beginDeclOrder(), e = endDeclOrder(); i != e; ++i) {
        //printd(5, "qore_class_private::parseCommitRuntimeInit() %s committing %s var %p %s\n", name.c_str(), privpub(i->second->access), l->first, l->first);
        // initialize variable
        //initVar(i->first, *(i->second), xsink);
        const char* vname = i->first;
        QoreVarInfo& vi = *(i->second);

        if (vi.exp) {
            // evaluate expression
            ValueEvalRefHolder val(vi.exp, xsink);
            if (*xsink) {
                continue;
            }
            if (QoreTypeInfo::mayRequireFilter(vi.getTypeInfo(), *val)) {
                val.ensureReferencedValue();
                QoreTypeInfo::acceptInputMember(vi.getTypeInfo(), vname, *val, xsink);
                if (*xsink) {
                    continue;
                }
            }

            discard(vi.assignInit(val.takeReferencedValue()), xsink);
        }
        else {
            vi.init();
        }
    }
}

void QoreVarMap::moveAllTo(QoreClass* qc, ClassAccess access) {
   if (empty() && access == Public) {
      qc->parseSetEmptyPublicMemberDeclaration();
      return;
   }
   for (DeclOrderIterator i = beginDeclOrder(); i != endDeclOrder(); ++i) {
      qore_class_private::parseAddStaticVar(qc, i->first, access, i->second);
   }
   map.clear();
   list.clear();
}

QoreClassHolder::~QoreClassHolder() {
   if (c) {
      qore_class_private::get(*c)->deref(true, true);
   }
}

QoreBuiltinClass::QoreBuiltinClass(const char* name, int64 n_domain) : QoreClass(name, n_domain) {
   setSystem();
}

QoreBuiltinClass::QoreBuiltinClass(const QoreBuiltinClass& old) : QoreClass(old) {
}
