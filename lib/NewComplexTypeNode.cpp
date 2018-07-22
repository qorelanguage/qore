/* -*- indent-tabs-mode: nil -*- */
/*
    NewComplexTypeNode.cpp

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
#include "qore/intern/typed_hash_decl_private.h"
#include "qore/intern/ScopedObjectCallNode.h"
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/qore_list_private.h"

void ParseNewComplexTypeNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    typeInfo = QoreParseTypeInfo::resolveAndDelete(pti, loc);
    pti = nullptr;

    {
        const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(typeInfo);
        if (qc) {
            ReferenceHolder<> holder(this, nullptr);
            val = new ScopedObjectCallNode(loc, qc, takeArgs());
            parse_init_value(val, oflag, pflag, lvids, typeInfo);
            return;
        }
    }
    {
        const TypedHashDecl* hd = QoreTypeInfo::getUniqueReturnHashDecl(typeInfo);
        if (hd) {
            ReferenceHolder<> holder(this, nullptr);
            bool runtime_check;
            lvids += typed_hash_decl_private::get(*hd)->parseInitHashDeclInitialization(loc, oflag, pflag, args, runtime_check);
            val = new NewHashDeclNode(loc, hd, takeArgs(), runtime_check);
            return;
        }
    }
    {
        const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexHash(typeInfo);
        if (ti) {
            ReferenceHolder<> holder(this, nullptr);
            lvids += qore_hash_private::parseInitComplexHashInitialization(loc, oflag, pflag, args, ti);
            val = new NewComplexHashNode(loc, typeInfo, takeArgs());
            return;
        }
    }
    {
        const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexList(typeInfo);
        if (ti) {
            ReferenceHolder<> holder(this, nullptr);
            QoreValue new_args;
            lvids += qore_list_private::parseInitComplexListInitialization(loc, oflag, pflag, takeArgs(), new_args, ti);
            val = new NewComplexListNode(loc, typeInfo, new_args);
            return;
        }
    }

    parse_error(*loc, "type '%s' does not support instantiation with the new operator", QoreTypeInfo::getName(typeInfo));
}

QoreValue NewHashDeclNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    return typed_hash_decl_private::get(*hd)->newHash(args, runtime_check, xsink);
}

QoreValue NewComplexHashNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    return qore_hash_private::newComplexHash(typeInfo, args, xsink);
}

QoreValue NewComplexListNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    return qore_list_private::newComplexList(typeInfo, args, xsink);
}
