/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  TypedHashDecl.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#include "qore/intern/typed_hash_decl_private.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreParseHashNode.h"
#include "qore/intern/QoreHashNodeIntern.h"

bool HashDeclMemberInfo::equal(const HashDeclMemberInfo& other) const {
    return QoreTypeInfo::equal(typeInfo, other.typeInfo);
}

void HashDeclMemberInfo::parseInit(const char* name, bool priv) {
    if (!typeInfo) {
        typeInfo = QoreParseTypeInfo::resolveAndDelete(parseTypeInfo, loc);
        parseTypeInfo = nullptr;
    }
#ifdef DEBUG
    else assert(!parseTypeInfo);
#endif

    if (exp) {
        const QoreTypeInfo* argTypeInfo = nullptr;
        int lvids = 0;
        exp = exp->parseInit(nullptr, 0, lvids, argTypeInfo);
        if (lvids) {
            parse_error(loc, "illegal local variable declaration in hashdecl member initialization expression");
            while (lvids--)
                pop_local_var();
        }
        // throw a type exception only if parse exceptions are enabled
        if (!QoreTypeInfo::parseAccepts(typeInfo, argTypeInfo) && getProgram()->getParseExceptionSink()) {
            QoreStringNode* desc = new QoreStringNode("initialization expression for ");
            desc->sprintf("hashdecl member '%s' returns ", name);
            QoreTypeInfo::getThisType(argTypeInfo, *desc);
            desc->concat(", but the member was declared as ");
            QoreTypeInfo::getThisType(typeInfo, *desc);
            qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", desc);
        }
    }
}

typed_hash_decl_private::typed_hash_decl_private(const typed_hash_decl_private& old, TypedHashDecl* thd) : loc(old.loc), name(old.name), thd(thd), typeInfo(new QoreHashDeclTypeInfo(thd, old.name.c_str())), orNothingTypeInfo(new QoreHashDeclOrNothingTypeInfo(thd, old.name.c_str())), pub(old.pub), sys(old.sys) {
    // copy member list
    for (HashDeclMemberMap::DeclOrderIterator i = old.members.beginDeclOrder(), e = old.members.endDeclOrder(); i != e; ++i)
        members.addNoCheck(strdup(i->first), i->second ? new HashDeclMemberInfo(*i->second) : nullptr);
}

int typed_hash_decl_private::parseInitHashDeclInitialization(const QoreProgramLocation& loc, LocalVar *oflag, int pflag, QoreParseListNode* args, bool& runtime_check) const {
    runtime_check = false;

    int lvids = 0;
    const QoreTypeInfo* argTypeInfo = nullptr;
    const AbstractQoreNode* arg;
    if (!qore_hash_private::parseInitHashInitialization(loc, oflag, pflag, lvids, args, argTypeInfo, arg))
        parseCheckHashDeclInitialization(loc, argTypeInfo, arg, "initializer value", runtime_check, true);

    return lvids;
}

void typed_hash_decl_private::parseCheckHashDeclInitialization(const QoreProgramLocation& loc, const QoreTypeInfo* expTypeInfo, const AbstractQoreNode* exp, const char* context_action, bool& runtime_check, bool strict_check) const {
    const TypedHashDecl* hd2 = QoreTypeInfo::getUniqueReturnHashDecl(expTypeInfo);
    if (hd2)
        parseCheckHashDeclAssignment(loc, *hd2->priv, context_action, runtime_check, strict_check);
    else
        parseCheckHashDeclAssignment(loc, exp, context_action, runtime_check, strict_check);
}

// see if the assignment is valid
void typed_hash_decl_private::parseCheckHashDeclAssignment(const QoreProgramLocation& loc, const typed_hash_decl_private& hd, const char* context, bool& needs_runtime_check, bool strict_check) const {
    for (HashDeclMemberMap::DeclOrderIterator i = hd.members.beginDeclOrder(), e = hd.members.endDeclOrder(); i != e; ++i) {
        HashDeclMemberInfo* m = members.find(i->first);
        if (!m) {
            if (!strict_check && QoreTypeInfo::parseReturns(i->second->getTypeInfo(), NT_NOTHING))
                continue;
            parse_error(loc, "hashdecl '%s' cannot be initialized from %s with hashdecl '%s' due to key '%s' present in hashdecl '%s' but not in the target hashdecl '%s'", name.c_str(), context, hd.name.c_str(), i->first, hd.name.c_str(), name.c_str());
        }
        else {
            bool may_not_match = false;
            qore_type_result_e res = QoreTypeInfo::parseAccepts(m->getTypeInfo(), i->second->getTypeInfo(), may_not_match);

            if (res == QTI_IDENT
                || ((res == QTI_AMBIGUOUS)
                    && (!strict_check || !may_not_match)))
                continue;

            if (res == QTI_AMBIGUOUS && may_not_match)
                parse_error(loc, "hashdecl '%s' initializer value for key '%s' from hashdecl '%s' from %s has incompatible type '%s'; expecting '%s'; types may not be compatible at runtime; use cast<hash<%s>>() to force a runtime check", name.c_str(), i->first, hd.name.c_str(), context, QoreTypeInfo::getName(i->second->getTypeInfo()), QoreTypeInfo::getName(m->getTypeInfo()), name.c_str());
            else
                parse_error(loc, "hashdecl '%s' initializer value for key '%s' from hashdecl '%s' from %s has incompatible type '%s'; expecting '%s'", name.c_str(), i->first, hd.name.c_str(), context, QoreTypeInfo::getName(i->second->getTypeInfo()), QoreTypeInfo::getName(m->getTypeInfo()));
        }
    }
}

// see if the assignment is valid
void typed_hash_decl_private::parseCheckHashDeclAssignment(const QoreProgramLocation& loc, const AbstractQoreNode* n, const char* context, bool& runtime_check, bool strict_check) const {
    assert(!runtime_check);

    switch (get_node_type(n)) {
        case NT_HASH: {
            ConstHashIterator i(reinterpret_cast<const QoreHashNode*>(n));
            while (i.next()) {
                HashDeclMemberInfo* m = members.find(i.getKey());
                if (!m)
                    parse_error(loc, "hashdecl '%s' initializer value from %s contains unknown key '%s'", name.c_str(), context, i.getKey());
                else {
                    const QoreTypeInfo* kti = getTypeInfoForValue(i.getValue());
                    bool may_not_match = false;
                    qore_type_result_e res = QoreTypeInfo::parseAccepts(m->getTypeInfo(), kti, may_not_match);
                    if (may_not_match && !runtime_check)
                        runtime_check = true;
                    if (res == QTI_IDENT
                        || ((res == QTI_AMBIGUOUS)
                            && (!strict_check || !may_not_match)))
                        continue;
                    parse_error(loc, "hashdecl '%s' initializer value from %s cannot be assigned from key '%s' with incompatible value type '%s'; expecting '%s'", name.c_str(), context, i.getKey(), QoreTypeInfo::getName(kti), QoreTypeInfo::getName(m->getTypeInfo()));
                }
            }
            break;
        }
        case NT_PARSE_HASH: {
            const QoreParseHashNode* phn = reinterpret_cast<const QoreParseHashNode*>(n);
            const QoreParseHashNode::nvec_t& keys = phn->getKeys();
            const QoreParseHashNode::tvec_t& vtypes = phn->getValueTypes();
            assert(keys.size() == vtypes.size());

            for (unsigned i = 0; i < keys.size(); ++i) {
                // check key
                const AbstractQoreNode* kn = keys[i];
                const QoreStringNode* key = get_node_type(kn) == NT_STRING ? reinterpret_cast<const QoreStringNode*>(kn) : nullptr;
                if (key) {
                    const HashDeclMemberInfo* m = members.find(key->c_str());
                    if (!m) {
                        parse_error(loc, "hashdecl '%s' hash initializer value from %s contains unknown key '%s'", name.c_str(), context, key);
                        continue;
                    }
                    // check value type
                    const QoreTypeInfo* vti = vtypes[i];
                    bool may_not_match = false;
                    qore_type_result_e res = QoreTypeInfo::parseAccepts(m->getTypeInfo(), vti, may_not_match);
                    if (may_not_match && !runtime_check)
                        runtime_check = true;
                    if (res == QTI_IDENT
                        || ((res == QTI_AMBIGUOUS)
                            && (!strict_check || !may_not_match)))
                        continue;

                    if (res == QTI_AMBIGUOUS && may_not_match)
                        parse_error(loc, "hashdecl '%s' initializer value for key '%s' from %s has incompatible type '%s'; expecting '%s'; types may not be compatible at runtime; use cast<hash<%s>>() to force a runtime check", name.c_str(), key->c_str(), context, QoreTypeInfo::getName(vti), QoreTypeInfo::getName(m->getTypeInfo()), name.c_str());
                    else
                        parse_error(loc, "hashdecl '%s' initializer value for key '%s' from %s has incompatible type '%s'; expecting '%s'; types may not be compatible at runtime", name.c_str(), key->c_str(), context, QoreTypeInfo::getName(vti), QoreTypeInfo::getName(m->getTypeInfo()), name.c_str());
                }
                else if (!runtime_check)
                    runtime_check = true;
            }
            break;
        }
        default:
            runtime_check = true;
            break;
    }
}

void typed_hash_decl_private::parseCheckComplexHashAssignment(const QoreProgramLocation& loc, const QoreTypeInfo* vti) const {
    assert(QoreTypeInfo::hasType(vti));
    for (HashDeclMemberMap::DeclOrderIterator i = members.beginDeclOrder(), e = members.endDeclOrder(); i != e; ++i) {
        if (!QoreTypeInfo::parseAccepts(vti, i->second->getTypeInfo())) {
            parse_error(loc, "cannot initialize a hash<string, %s> value from hashdecl '%s' due to member '%s' with incompatible type '%s'", QoreTypeInfo::getName(vti), name.c_str(), i->first, QoreTypeInfo::getName(i->second->getTypeInfo()));
        }
    }
}

int typed_hash_decl_private::parseCheckMemberAccess(const QoreProgramLocation& loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, int pflag) const {
    const_cast<typed_hash_decl_private*>(this)->parseInit();
    const HashDeclMemberInfo* m = members.find(mem);

    if (!m) {
        parse_error(loc, "illegal access to unknown member '%s' in hashdecl '%s'", mem, name.c_str());
        return -1;
    }

    memberTypeInfo = m->getTypeInfo();
    return 0;
}

QoreHashNode* typed_hash_decl_private::newHash(const QoreParseListNode* args, bool runtime_check, ExceptionSink* xsink) const {
    assert(!args || args->empty() || args->size() == 1);
    ValueEvalRefHolder a(args && !args->empty() ? args->get(0) : nullptr, xsink);
    if (*xsink)
        return nullptr;

    const QoreHashNode* init = nullptr;
    if (a->getType() != NT_NOTHING) {
        if (runtime_check && a->getType() != NT_HASH) {
            xsink->raiseException("HASHDECL-INIT-ERROR", "hashdecl '%s' hash initializer value must be a hash; got type '%s' instead", name.c_str(), a->getTypeName());
            return nullptr;
        }

        init = a->get<const QoreHashNode>();
    }

    return newHash(init, runtime_check, xsink);
}

QoreHashNode* typed_hash_decl_private::newHash(const QoreHashNode* init, bool runtime_check, ExceptionSink* xsink) const {
    if (runtime_check) {
        ConstHashIterator i(init);
        while (i.next()) {
            if (!members.find(i.getKey())) {
                xsink->raiseException("HASHDECL-INIT-ERROR", "hashdecl '%s' hash initializer value contains unknown key '%s'", name.c_str(), i.getKey());
                return nullptr;
            }
        }
    }

    ReferenceHolder<QoreHashNode> h(qore_hash_private::newHashDecl(thd), xsink);
    initHash(*h, init, xsink);
    return *xsink ? nullptr : h.release();
}

int typed_hash_decl_private::initHash(QoreHashNode* h, const QoreHashNode* init, ExceptionSink* xsink) const {
#ifdef QORE_MANAGE_STACK
    if (check_stack(xsink))
        return -1;
#endif

    for (HashDeclMemberMap::DeclOrderIterator i = members.beginDeclOrder(), e = members.endDeclOrder(); i != e; ++i) {
        // first try to use value given in init hash
        if (init) {
            const qore_hash_private* hi = qore_hash_private::get(*init);
            bool exists;
            ReferenceHolder<> val(hi->getReferencedKeyValueIntern(i->first, exists), xsink);
            if (exists) {
                // check types
                QoreValue qv(val.release());
                QoreTypeInfo::acceptInputMember(i->second->getTypeInfo(), i->first, qv, xsink);
                val = qv.takeNode();
                if (*xsink)
                    return -1;
                AbstractQoreNode** v = h->getKeyValuePtr(i->first);
                assert(!*v);
                *v = val.release();
                continue;
            }
        }

        if (!i->second)
            continue;

        if (i->second->exp) {
            AbstractQoreNode** v = h->getKeyValuePtr(i->first);
            assert(!*v);

            ReferenceHolder<> val(i->second->exp->eval(xsink), xsink);
            if (*xsink)
                return -1;
            // check types
            QoreValue qv(val.release());
            QoreTypeInfo::acceptInputMember(i->second->getTypeInfo(), i->first, qv, xsink);
            val = qv.takeNode();
            if (*xsink)
                return -1;
            *v = val.release();
        }
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
        else {
            AbstractQoreNode** v = h->getKeyValuePtr(i->first);
            assert(!*v);
            *v = QoreTypeInfo::getDefaultQoreValue(i->second->getTypeInfo()).takeNode();
        }
#endif
    }

    return 0;
}

TypedHashDecl::TypedHashDecl(const char* name) : priv(new typed_hash_decl_private(get_runtime_location(), name, this)) {
}

TypedHashDecl::TypedHashDecl(typed_hash_decl_private* p) : priv(p) {
}

TypedHashDecl::TypedHashDecl(const TypedHashDecl& old) : priv(new typed_hash_decl_private(*old.priv, this)) {
}

TypedHashDecl::~TypedHashDecl() {
    delete priv;
}

void TypedHashDecl::addMember(const char* name, const QoreTypeInfo* memberTypeInfo, QoreValue init_val) {
    priv->addMember(name, memberTypeInfo, init_val);
}

const QoreTypeInfo* TypedHashDecl::getTypeInfo(bool or_nothing) const {
    return priv->getTypeInfo(or_nothing);
}

const char* TypedHashDecl::getName() const {
    return priv->getName();
}

bool TypedHashDecl::isSystem() const {
    return priv->isSystem();
}

TypedHashDeclHolder::~TypedHashDeclHolder() {
    if (thd)
        typed_hash_decl_private::get(*thd)->deref();
}

TypedHashDecl* TypedHashDeclHolder::operator=(TypedHashDecl* nhd) {
    if (thd)
        typed_hash_decl_private::get(*thd)->deref();
    return thd = nhd;
}

