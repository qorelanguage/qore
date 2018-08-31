/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreSerializable.qpp

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

#include "qore/Qore.h"
#include "qore/intern/QoreSerializable.h"

#include <string>

int QoreSerializable::serializeMemberValue(ValueHolder& val, ReferenceHolder<QoreHashNode>& index, const QoreClass* current_cls, const char* mname, ExceptionSink* xsink) const {
    switch (val->getType()) {
        case NT_INT:
        case NT_STRING:
        case NT_BOOLEAN:
        case NT_FLOAT:
        case NT_NUMBER:
        case NT_NOTHING:
        case NT_NULL:
        case NT_BINARY:
            return 0;

        default:
            break;
    }

    xsink->raiseException("SERIALIZATION-ERROR", "cannot serialize class '%s' member '%s' with type '%s'; type cannot be serialized",
        current_cls->getName(), mname, val->getTypeName());
    return -1;
}

QoreHashNode* QoreSerializable::serializeToData(QoreObject* self, ExceptionSink* xsink) const {
    const QoreClass* cls = self->getClass();
    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hashdeclSerializationInfo, xsink), xsink);
    rv->setKeyValue("_class", new QoreStringNode(cls->getNamespacePath()), xsink);

    ReferenceHolder<QoreHashNode> index(xsink);

    ReferenceHolder<QoreHashNode> members_per_class(new QoreHashNode(autoHashTypeInfo), xsink);

    QoreClassHierarchyIterator ci(cls);
    while (ci.next()) {
        const QoreClass* current_cls = ci.get();

        // serialize class members for each member of the hierarchy separately
        ReferenceHolder<QoreHashNode> class_members(xsink);

        // iterate all nornal members in the class
        QoreClassMemberIterator mi(ci.get());
        while (mi.next()) {
            const QoreExternalMemberVarBase* m = mi.getMember();
            const char* mname = mi.getName();

            ValueHolder val(self->getReferencedMemberNoMethod(mname, current_cls, xsink), xsink);
            if (*xsink) {
                return nullptr;
            }

            if (serializeMemberValue(val, index, current_cls, mname, xsink)) {
                return nullptr;
            }

            if (!class_members) {
                class_members = new QoreHashNode(autoTypeInfo);
            }
            class_members->setKeyValue(mname, val.release(), xsink);
        }

        if (class_members) {
            std::string class_path = current_cls->getNamespacePath();
            members_per_class->setKeyValue(class_path.c_str(), class_members.release(), xsink);
        }
    }

    if (index) {
        rv->setKeyValue("_index", index.release(), xsink);
    }

    rv->setKeyValue("_members", members_per_class.release(), xsink);

    return rv.release();
}

BinaryNode* QoreSerializable::serialize(QoreObject* self, ExceptionSink* xsink) const {
    ReferenceHolder<QoreHashNode> h(serializeToData(self, xsink), xsink);
    if (*xsink) {
        return nullptr;
    }

    xsink->raiseException("SERIALIZATION-ERROR", "unimplemented");
    return nullptr;
}

QoreObject* QoreSerializable::deserialize(const BinaryNode* b, ExceptionSink* xsink) {
    xsink->raiseException("DESERIALIZATION-ERROR", "unimplemented");
    return nullptr;
}


QoreObject* QoreSerializable::deserialize(const QoreHashNode* h, ExceptionSink* xsink) {
    xsink->raiseException("DESERIALIZATION-ERROR", "unimplemented");
    return nullptr;
}