/*
    ReferenceHelper.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2024 Qore Technologies, s.r.o.

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

struct qore_type_safe_ref_helper_priv_t : public LValueHelper {
    DLLLOCAL qore_type_safe_ref_helper_priv_t(const ReferenceNode* ref, ExceptionSink* xsink) : LValueHelper(*ref, xsink) {
    }

    DLLLOCAL qore_type_safe_ref_helper_priv_t(const AbstractQoreNode* exp, ExceptionSink* xsink) : LValueHelper(exp, xsink) {
    }

    DLLLOCAL ~qore_type_safe_ref_helper_priv_t() {
    }

    DLLLOCAL AbstractQoreNode* getUnique(ExceptionSink *xsink) {
        ensureUnique();
        return LValueHelper::getNodeValue();
    }

    DLLLOCAL int assign(QoreValue val) {
        return LValueHelper::assign(val, "<reference>");
    }

    DLLLOCAL const QoreValue getValue() const {
        return LValueHelper::getValue();
    }

    DLLLOCAL qore_type_t getType() const {
        return LValueHelper::getType();
    }

    DLLLOCAL const char* getTypeName() const {
        return LValueHelper::getTypeName();
    }
};

QoreTypeSafeReferenceHelper::QoreTypeSafeReferenceHelper(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink) : priv(new qore_type_safe_ref_helper_priv_t(ref, xsink)) {
}

QoreTypeSafeReferenceHelper::QoreTypeSafeReferenceHelper(const ReferenceNode *ref, ExceptionSink *xsink) : priv(new qore_type_safe_ref_helper_priv_t(ref, xsink)) {
}

QoreTypeSafeReferenceHelper::~QoreTypeSafeReferenceHelper() {
   delete priv;
}

AbstractQoreNode *QoreTypeSafeReferenceHelper::getUnique(ExceptionSink *xsink) {
   return priv->getUnique(xsink);
}

int QoreTypeSafeReferenceHelper::assign(QoreValue val) {
   return priv->assign(val);
}

const QoreValue QoreTypeSafeReferenceHelper::getValue() const {
   return priv->getValue();
}

QoreTypeSafeReferenceHelper::operator bool() const {
   return *priv;
}

qore_type_t QoreTypeSafeReferenceHelper::getType() const {
   return priv->getType();
}

const char* QoreTypeSafeReferenceHelper::getTypeName() const {
   return priv->getTypeName();
}
