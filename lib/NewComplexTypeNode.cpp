/* -*- indent-tabs-mode: nil -*- */
/*
  NewComplexTypeNode.cpp

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
#include "qore/intern/ScopedObjectCallNode.h"

AbstractQoreNode* ParseNewComplexTypeNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    typeInfo = QoreParseTypeInfo::resolveAndDelete(pti, loc);
    pti = nullptr;

    {
        const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(typeInfo);
        if (qc) {
            ReferenceHolder<> holder(this, nullptr);
            return (new ScopedObjectCallNode(loc, qc, takeArgs()))->parseInitImpl(oflag, pflag, lvids, typeInfo);
        }
    }
    {
        const TypedHashDecl* hd = QoreTypeInfo::getUniqueReturnHashDecl(typeInfo);
        if (hd) {
            bool runtime_check;
            lvids += typed_hash_decl_private::get(*hd)->parseInitImpliedConstructor(loc, oflag, pflag, args, runtime_check);
            return new NewHashDeclNode(loc, hd, takeArgs(), runtime_check);
        }
    }

    parse_error(loc, "type '%s' does not support instantiation with the new operator", QoreTypeInfo::getName(typeInfo));

    return this;
}

QoreValue NewHashDeclNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    return typed_hash_decl_private::get(*hd)->newHash(args, runtime_check, xsink);
}

