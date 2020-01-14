/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    typed_hash_decl_private.h

    Qore Programming Language

    Copyright (C) 2003 - 2020 Qore Technologies, s.r.o.

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

#ifndef _QORE_INTERN_TYPED_HASH_DECL_PRIVATE_H

#define _QORE_INTERN_TYPED_HASH_DECL_PRIVATE_H

#include "qore/intern/QoreClassIntern.h"

#include <string>

class typed_hash_decl_private;

class HashDeclMemberInfo : public QoreMemberInfoBase {
public:
    DLLLOCAL HashDeclMemberInfo(const QoreProgramLocation* loc, const QoreTypeInfo* n_typeInfo = nullptr, QoreParseTypeInfo* n_parseTypeInfo = nullptr, QoreValue e = QoreValue()) : QoreMemberInfoBase(loc, n_typeInfo, n_parseTypeInfo, e) {
    }

    DLLLOCAL HashDeclMemberInfo(const HashDeclMemberInfo& old) : QoreMemberInfoBase(old) {
    }

    DLLLOCAL bool equal(const HashDeclMemberInfo& other) const;

    DLLLOCAL void parseInit(const char* name, bool priv);
};

typedef QoreMemberMapBase<HashDeclMemberInfo> HashDeclMemberMap;

class typed_hash_decl_private {
friend class typed_hash_decl_member_iterator;
friend class TypedHashDecl;
public:
    DLLLOCAL typed_hash_decl_private(const QoreProgramLocation* loc) : loc(loc), orig(this) {
        assignModule();
    }

    DLLLOCAL typed_hash_decl_private(const QoreProgramLocation* loc, const char* n, TypedHashDecl* thd) :
        loc(loc), name(n), thd(thd), orig(this),
        typeInfo(new QoreHashDeclTypeInfo(thd, n)),
        orNothingTypeInfo(new QoreHashDeclOrNothingTypeInfo(thd, n)) {
        assignModule();
    }

    DLLLOCAL typed_hash_decl_private(const typed_hash_decl_private& old, TypedHashDecl* thd);

    DLLLOCAL ~typed_hash_decl_private() {
        delete typeInfo;
        delete orNothingTypeInfo;
    }

    DLLLOCAL TypedHashDecl* newTypedHashDecl(const char* n) {
        assert(name.empty());
        assert(!thd);
        name = n;
        thd = new TypedHashDecl(this);
        typeInfo = new QoreHashDeclTypeInfo(thd, n);
        orNothingTypeInfo = new QoreHashDeclOrNothingTypeInfo(thd, n);
        return thd;
    }

    DLLLOCAL bool equal(const typed_hash_decl_private& other) const {
        if (name != other.name || members.size() != other.members.size())
            return false;

        for (HashDeclMemberMap::const_iterator ti = members.member_list.begin(), oi = other.members.member_list.begin(), te = members.member_list.end(); ti != te; ++ti, ++oi) {
            // if the member's name is different, return false
            if (strcmp(oi->first, ti->first)) {
               return false;
            }
            // if the member's type is different, return false
            if (!ti->second->equal(*oi->second)) {
               return false;
            }
        }

        return true;
    }

    DLLLOCAL bool parseEqual(const typed_hash_decl_private& other) const {
        const_cast<typed_hash_decl_private*>(this)->parseInit();
        return equal(other);
    }

    DLLLOCAL const QoreTypeInfo* getTypeInfo(bool or_nothing = false) const {
        return or_nothing ? orNothingTypeInfo : typeInfo;
    }

    DLLLOCAL bool isPublic() const {
        return pub;
    }

    DLLLOCAL bool isUserPublic() const {
        return pub && !sys;
    }

    DLLLOCAL void setPublic() {
        assert(!pub);
        pub = true;
    }

    DLLLOCAL void setPrivate() {
        assert(pub);
        pub = false;
    }

    DLLLOCAL bool isSystem() const {
        return sys;
    }

    DLLLOCAL void ref() const {
        refs.ROreference();
    }

    DLLLOCAL bool deref() {
        if (refs.ROdereference()) {
            delete thd;
            return true;
        }
        return false;
    }

    DLLLOCAL void parseInit() {
        if (parse_init_done || sys)
            return;
        parse_init_done = true;

        // initialize new members
        for (auto& i : members.member_list) {
            if (i.second) {
                i.second->parseInit(i.first, true);
            }
            // check new members for conflicts in base hashdecls
            //parseCheckMemberInBaseHashDecl(i.first, i.second);
        }
    }

    DLLLOCAL int parseInitHashDeclInitialization(const QoreProgramLocation* loc, LocalVar *oflag, int pflag, QoreParseListNode* args, bool& runtime_check) const;

    DLLLOCAL void parseCheckHashDeclInitialization(const QoreProgramLocation* loc, const QoreTypeInfo* expTypeInfo, QoreValue exp, const char* context_action, bool& runtime_check, bool strict_check = true) const;

    DLLLOCAL void parseCheckHashDeclAssignment(const QoreProgramLocation* loc, const typed_hash_decl_private& hd, const char* context, bool& needs_runtime_check, bool strict_check = true) const;

    DLLLOCAL void parseCheckHashDeclAssignment(const QoreProgramLocation* loc, QoreValue n, const char* context, bool& needs_runtime_check, bool strict_check = true) const;

    DLLLOCAL void parseCheckComplexHashAssignment(const QoreProgramLocation* loc, const QoreTypeInfo* vti) const;

    DLLLOCAL QoreHashNode* newHash(const QoreParseListNode* args, bool runtime_check, ExceptionSink* xsink) const;

    DLLLOCAL QoreHashNode* newHash(const QoreHashNode* init, bool runtime_check, ExceptionSink* xsink) const;

    DLLLOCAL int initHash(QoreHashNode* h, const QoreHashNode* init, ExceptionSink* xsink) const;

    DLLLOCAL int runtimeAssignKey(const char* key, ValueHolder& val, ExceptionSink* xsink) const {
        const HashDeclMemberInfo* mem = members.find(key);
        if (!mem) {
            xsink->raiseException("HASHDECL-KEY-ERROR", "cannot assign unknown key '%s' to hashdecl '%s'", key, name.c_str());
            return -1;
        }
        QoreTypeInfo::acceptInputKey(mem->getTypeInfo(), key, *val, xsink);
        if (*xsink) {
            xsink->appendLastDescription(" (while assigning to hashdecl '%s')", name.c_str());
            return -1;
        }
        return 0;
    }

    DLLLOCAL int parseCheckMemberAccess(const QoreProgramLocation* loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, int pflag) const;

    DLLLOCAL const HashDeclMemberInfo* findMember(const char* m) const {
        return members.find(m);
    }

    DLLLOCAL void parseAdd(std::pair<char*, HashDeclMemberInfo*> pair) {
        members.addNoCheck(pair);
    }

    DLLLOCAL bool hasMember(const char* name) const {
        return members.inList(name);
    }

    DLLLOCAL void setSystemPublic() {
        assert(!sys);
        assert(!pub);
        sys = true;
        pub = true;
    }

    DLLLOCAL const char* getName() const {
        return name.c_str();
    }

    DLLLOCAL const std::string& getNameStr() const {
        return name;
    }

    DLLLOCAL const QoreProgramLocation* getParseLocation() const {
        return loc;
    }

    DLLLOCAL static typed_hash_decl_private* get(TypedHashDecl& hashdecl) {
        return hashdecl.priv;
    }

    DLLLOCAL static const typed_hash_decl_private* get(const TypedHashDecl& hashdecl) {
        return hashdecl.priv;
    }

    DLLLOCAL void addMember(const char* name, const QoreTypeInfo* memberTypeInfo, QoreValue init_val) {
        assert(!members.find(name));
        members.addNoCheck(std::make_pair(strdup(name), new HashDeclMemberInfo(&loc_builtin, memberTypeInfo, nullptr, init_val)));
    }

    DLLLOCAL void setName(const char* n) {
        name = n;
    }

    DLLLOCAL const HashDeclMemberMap& getMembers() const {
        return members;
    }

    DLLLOCAL const HashDeclMemberInfo* findLocalMember(const char* name) const {
        return members.find(name);
    }

    DLLLOCAL void setNamespace(qore_ns_private* n) {
        ns = n;
    }

    DLLLOCAL const qore_ns_private* getNamespace() const {
        return ns;
    }

    DLLLOCAL const char* getModuleName() const {
        return from_module.empty() ? nullptr : from_module.c_str();
    }

protected:
    // references
    mutable QoreReferenceCounter refs;
    const QoreProgramLocation* loc;
    std::string name;
    TypedHashDecl* thd = nullptr;
    // parent namespace
    qore_ns_private* ns = nullptr;
    // the module that defined this class, if any
    std::string from_module;

    // original declaration
    const typed_hash_decl_private* orig;

    // type information
    QoreHashDeclTypeInfo* typeInfo = nullptr;
    QoreHashDeclOrNothingTypeInfo* orNothingTypeInfo = nullptr;

    // member information
    HashDeclMemberMap members;

    bool pub = false;
    bool sys = false;

    bool parse_init_done = false;

    DLLLOCAL void assignModule() {
        const char* mod_name = get_module_context_name();
        if (mod_name) {
            from_module = mod_name;
        }
    }

    DLLLOCAL int initHashIntern(QoreHashNode* h, const QoreHashNode* init, ExceptionSink* xsink) const;
};

#endif
