/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  typed_hash_decl_private.h

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

#ifndef _QORE_INTERN_TYPED_HASH_DECL_PRIVATE_H

#define _QORE_INTERN_TYPED_HASH_DECL_PRIVATE_H

#include "qore/intern/QoreClassIntern.h"

#include <string>

class typed_hash_decl_private;

class HashDeclMemberInfo : public QoreMemberInfoBase {
public:
   DLLLOCAL HashDeclMemberInfo(const QoreProgramLocation& loc, const QoreTypeInfo* n_typeInfo = nullptr, QoreParseTypeInfo* n_parseTypeInfo = nullptr, AbstractQoreNode* e = nullptr) : QoreMemberInfoBase(loc, n_typeInfo, n_parseTypeInfo, e) {
   }

   DLLLOCAL HashDeclMemberInfo(const HashDeclMemberInfo& old) : QoreMemberInfoBase(old) {
   }

   DLLLOCAL void parseInit(const char* name, bool priv);
};

typedef QoreMemberMapBase<HashDeclMemberInfo> HashDeclMemberMap;

class typed_hash_decl_private {
public:
    DLLLOCAL typed_hash_decl_private(const QoreProgramLocation& loc) : loc(loc) {
    }

    DLLLOCAL typed_hash_decl_private(const QoreProgramLocation& loc, const char* n, TypedHashDecl* thd) : loc(loc), name(n), thd(thd) {
    }

    DLLLOCAL typed_hash_decl_private(const typed_hash_decl_private& old, TypedHashDecl* thd);

    DLLLOCAL TypedHashDecl* newTypedHashDecl(const char* n) {
        assert(name.empty());
        assert(!thd);
        name = n;
        return thd = new TypedHashDecl(this);
    }

   DLLLOCAL bool injected() const {
      return inject;
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
        if (parse_init_done)
            return;
        parse_init_done = true;

        // initialize new members
        for (HashDeclMemberMap::DeclOrderIterator i = members.beginDeclOrder(), e = members.endDeclOrder(); i != e; ++i) {
            if (i->second)
                i->second->parseInit(i->first, true);
            // check new members for conflicts in base hashdecls
            //parseCheckMemberInBaseHashDecl(i->first, i->second);
        }
    }

    DLLLOCAL int runtimeInitMembers(QoreHashNode& h, ExceptionSink* xsink) const;

    DLLLOCAL void parseAdd(std::pair<char*, HashDeclMemberInfo*> pair) {
        members.addNoCheck(pair);
    }

    DLLLOCAL bool hasMember(const char* name) {
        return members.inList(name);
    }

    DLLLOCAL const char* getName() const {
        return name.c_str();
    }

    DLLLOCAL const QoreProgramLocation& getParseLocation() const {
        return loc;
    }

    DLLLOCAL static typed_hash_decl_private* get(TypedHashDecl& hashdecl) {
        return hashdecl.priv;
    }

    DLLLOCAL static const typed_hash_decl_private* get(const TypedHashDecl& hashdecl) {
        return hashdecl.priv;
    }

protected:
    mutable QoreReferenceCounter refs;       // references
    QoreProgramLocation loc;
    std::string name;
    TypedHashDecl* thd = nullptr;
    HashDeclMemberMap members;

    bool pub = false;
    bool sys = false;
    bool inject = false;

    bool parse_init_done = false;
};

#endif