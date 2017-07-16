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

int typed_hash_decl_private::parseInitImpliedConstructor(const QoreProgramLocation& loc, LocalVar *oflag, int pflag, QoreListNode* args, bool& runtime_check) const {
    runtime_check = false;

    if (!args || args->empty())
        return 0;
    if (args->size() > 1) {
        parse_error(loc, "illegal arguments to implied hashdecl constructor; a single hash argument is expected; %d arguments supplied instead", (int)args->size());
        return 0;
    }

    int lvids = 0;

    // initialize argument
    AbstractQoreNode** n = args->get_entry_ptr(0);
    const QoreTypeInfo* argTypeInfo;
    (*n) = (*n)->parseInit(oflag, pflag & ~(PF_RETURN_VALUE_IGNORED), lvids, argTypeInfo);

    if (!QoreTypeInfo::parseReturns(argTypeInfo, NT_HASH)) {
        parse_error(loc, "illegal argument to implied hashdecl constructor; a single hash argument is expected; got type '%s' instead", QoreTypeInfo::getName(argTypeInfo));
    }
    else {
        const TypedHashDecl* hd2 = QoreTypeInfo::getUniqueReturnHashDecl(argTypeInfo);
        if (hd2)
            parseCheckHashDeclAssignment(loc, *hd2->priv, "initializer value", runtime_check);
        else
            parseCheckHashDeclAssignment(loc, *n, "initializer value", runtime_check);
    }
    return lvids;
}

// see if the assignment is valid
void typed_hash_decl_private::parseCheckHashDeclAssignment(const QoreProgramLocation& loc, const typed_hash_decl_private& hd, const char* context, bool& needs_runtime_check) const {
    for (HashDeclMemberMap::DeclOrderIterator i = hd.members.beginDeclOrder(), e = hd.members.endDeclOrder(); i != e; ++i) {
        HashDeclMemberInfo* m = members.find(i->first);
        if (!m) {
            parse_error(loc, "hashdecl '%s' cannot be initialized from %s with hashdecl '%s' due to key '%s' present in hashdecl '%s' but not in the target hashdecl '%s'", name.c_str(), context, hd.name.c_str(), i->first, hd.name.c_str(), name.c_str());
        }
        else {
            bool may_not_match = false;
            if (!QoreTypeInfo::parseAccepts(m->getTypeInfo(), i->second->getTypeInfo(), may_not_match))
                parse_error(loc, "hashdecl '%s' initializer value for key '%s' from hashdecl '%s' from %s has an incomaptible type; expecting '%s'; got '%s'", name.c_str(), i->first, hd.name.c_str(), context, QoreTypeInfo::getName(m->getTypeInfo()), QoreTypeInfo::getName(i->second->getTypeInfo()));
            else if (may_not_match && !needs_runtime_check)
               needs_runtime_check = true;
        }
    }
}

// see if the assignment is valid
void typed_hash_decl_private::parseCheckHashDeclAssignment(const QoreProgramLocation& loc, const AbstractQoreNode* n, const char* context, bool& needs_runtime_check) const {
    needs_runtime_check = false;

    switch (get_node_type(n)) {
        case NT_HASH: {
            ConstHashIterator i(reinterpret_cast<const QoreHashNode*>(n));
            while (i.next()) {
                if (!members.find(i.getKey()))
                    parse_error(loc, "hashdecl '%s' initializer value from %s contains unknown key '%s'", name.c_str(), context, i.getKey());
            }
            break;
        }
        case NT_PARSE_HASH: {
                const QoreParseHashNode::nvec_t& keys = reinterpret_cast<const QoreParseHashNode*>(n)->getKeys();
                for (auto& i : keys) {
                    if (get_node_type(i) == NT_STRING) {
                        const char* key = reinterpret_cast<const QoreStringNode*>(i)->c_str();
                        if (!members.find(key))
                            parse_error(loc, "hashdecl '%s' hash initializer value from %s contains unknown key '%s'", name.c_str(), context, key);
                    }
                    else if (!needs_runtime_check)
                        needs_runtime_check = true;
                }
                break;
        }
        default:
            needs_runtime_check = true;
            break;
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

QoreHashNode* typed_hash_decl_private::newHash(const QoreListNode* args, bool runtime_check, ExceptionSink* xsink) const {
    assert(!args || args->empty() || args->size() == 1);
    QoreListNodeEvalOptionalRefHolder a(args, xsink);
    if (*xsink)
        return nullptr;

    const QoreHashNode* init = nullptr;
    if (runtime_check && *a && !a->empty()) {
        const AbstractQoreNode* n = a->retrieve_entry(0);
        if (get_node_type(n) != NT_HASH) {
            xsink->raiseException("HASHDECL-INIT-ERROR", "hashdecl hash initializer value must be a hash; got type '%s' instead", get_type_name(n));
            return nullptr;
        }

        init = reinterpret_cast<const QoreHashNode*>(n);
    }
    else if (*a && !a->empty()) {
        assert(a->size() == 1);
        assert(get_node_type(a->retrieve_entry(0)) == NT_HASH);
        init = reinterpret_cast<const QoreHashNode*>(a->retrieve_entry(0));
    }

    return newHash(init, runtime_check, xsink);
}

QoreHashNode* typed_hash_decl_private::newHash(const QoreHashNode* init, bool runtime_check, ExceptionSink* xsink) const {
    ConstHashIterator i(init);
    while (i.next()) {
        if (!members.find(i.getKey())) {
            xsink->raiseException("HASHDECL-INIT-ERROR", "hashdecl hash initializer value contains unknown key '%s'", i.getKey());
            return nullptr;
        }
    }

    ReferenceHolder<QoreHashNode> h(qore_hash_private::newHashDecl(thd), xsink);

    for (HashDeclMemberMap::DeclOrderIterator i = members.beginDeclOrder(), e = members.endDeclOrder(); i != e; ++i) {
        // first try to use value given in init hash
        if (init) {
            bool exists;
            ReferenceHolder<> val(init->getReferencedKeyValue(i->first, exists), xsink);
            if (exists) {
                // check types
                QoreValue qv(val.release());
                QoreTypeInfo::acceptInputMember(i->second->getTypeInfo(), i->first, qv, xsink);
                val = qv.takeNode();
                if (*xsink)
                    return nullptr;
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
                return nullptr;
            // check types
            QoreValue qv(val.release());
            QoreTypeInfo::acceptInputMember(i->second->getTypeInfo(), i->first, qv, xsink);
            val = qv.takeNode();
            if (*xsink)
                return nullptr;
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

    return h.release();
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
