/* -*- indent-tabs-mode: nil -*- */
/*
  QoreParseHashNode.cpp

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
#include "qore/intern/QoreParseHashNode.h"
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/qore_program_private.h"

AbstractQoreNode* QoreParseHashNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    assert(keys.size() == values.size());
    bool needs_eval = false;

    // turn off "return value ignored" flag before performing parse init
    pflag &= ~PF_RETURN_VALUE_IGNORED;

    // initialize value type vector
    vtypes.resize(keys.size());

    // try to find a common value type, if any
    bool vcommon = false;

    for (size_t i = 0; i < keys.size(); ++i) {
        const QoreTypeInfo* argTypeInfo = 0;
        AbstractQoreNode* p = keys[i];
        keys[i] = keys[i]->parseInit(oflag, pflag, lvids, argTypeInfo);

        if (p != keys[i] && (!keys[i] || keys[i]->is_value())) {
            QoreStringValueHelper key(keys[i]);
            checkDup(lvec[i], key->getBuffer());
        }
        else if (!needs_eval && keys[i] && keys[i]->needs_eval())
            needs_eval = true;

        if (!QoreTypeInfo::canConvertToScalar(argTypeInfo)) {
            QoreStringMaker str("key number %ld (starting from 0) in the hash is ", i);
            argTypeInfo->doNonStringWarning(lvec[i], str.getBuffer());
        }

        argTypeInfo = nullptr;
        values[i] = values[i]->parseInit(oflag, pflag, lvids, vtypes[i]);

        if (!i) {
            if (QoreTypeInfo::hasType(vtypes[i])) {
                vtype = vtypes[i];
                vcommon = true;
            }
        }
        else if (vcommon && !QoreTypeInfo::matchCommonType(vtype, vtypes[i]))
            vcommon = false;

        if (!needs_eval && values[i] && values[i]->needs_eval())
            needs_eval = true;
    }

    kmap.clear();

    if (vtype && !QoreTypeInfo::hasType(vtype))
        vtype = nullptr;

    if (vtype) {
        this->typeInfo = typeInfo = qore_program_private::get(*getProgram())->getComplexHashType(vtype);
    }
    else {
        this->typeInfo = typeInfo = hashTypeInfo;
    }

    if (needs_eval)
        return this;

    // evaluate immediately
    ReferenceHolder<QoreHashNode> h(new QoreHashNode(vtype), nullptr);
    qore_hash_private* ph = qore_hash_private::get(**h);
    for (size_t i = 0; i < keys.size(); ++i) {
        QoreStringValueHelper key(keys[i]);
        discard(ph->swapKeyValue(key->c_str(), values[i], nullptr), nullptr);
        values[i] = nullptr;
    }

    deref();
    return h.release();
}

QoreValue QoreParseHashNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    assert(keys.size() == values.size());
    ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);

    // issue #2106 we must calculate the runtime type again because lvalues can return NOTHING despite their declared type
    const QoreTypeInfo* vtype = nullptr;
    // try to find a common value type, if any
    bool vcommon = false;

    for (size_t i = 0; i < keys.size(); ++i) {
        QoreNodeEvalOptionalRefHolder k(keys[i], xsink);
        if (xsink && *xsink)
            return QoreValue();

        QoreNodeEvalOptionalRefHolder v(values[i], xsink);
        if (xsink && *xsink)
            return QoreValue();

        QoreStringValueHelper key(*k);
        AbstractQoreNode* val = v.getReferencedValue();
        h->setKeyValue(key->c_str(), val, xsink);
        if (xsink && *xsink)
            return QoreValue();

        if (!i) {
            vtype = getTypeInfoForValue(val);
            vcommon = true;
        }
        else if (vcommon && !QoreTypeInfo::matchCommonType(vtype, getTypeInfoForValue(val)))
            vcommon = false;
    }

    if (vcommon && QoreTypeInfo::hasType(vtype))
       qore_hash_private::get(**h)->complexTypeInfo = qore_program_private::get(*getProgram())->getComplexHashType(vtype);

    return h.release();
}
