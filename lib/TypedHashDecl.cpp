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

typed_hash_decl_private::typed_hash_decl_private(const typed_hash_decl_private& old, TypedHashDecl* thd) : loc(old.loc), name(old.name), thd(thd), pub(old.pub), sys(old.sys) {
    // copy member list
    for (HashDeclMemberMap::DeclOrderIterator i = old.members.beginDeclOrder(), e = old.members.endDeclOrder(); i != e; ++i)
        members.addNoCheck(strdup(i->first), i->second ? new HashDeclMemberInfo(*i->second) : nullptr);
}

int typed_hash_decl_private::runtimeInitMembers(QoreHashNode& h, ExceptionSink* xsink) const {
    for (HashDeclMemberMap::DeclOrderIterator i = members.beginDeclOrder(), e = members.endDeclOrder(); i != e; ++i) {
        if (!i->second)
            continue;

        AbstractQoreNode** v = h.getKeyValuePtr(i->first);
        assert(!*v);

        if (i->second->exp) {
            ReferenceHolder<AbstractQoreNode> val(i->second->exp->eval(xsink), xsink);
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
