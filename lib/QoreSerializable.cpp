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
#include "qore/intern/QC_Serializable.h"

#include <string>

QoreValue QoreSerializable::serializeValue(const QoreValue val, ReferenceHolder<QoreHashNode>& index, ExceptionSink* xsink) {
    switch (val.getType()) {
        case NT_INT:
        case NT_STRING:
        case NT_BOOLEAN:
        case NT_FLOAT:
        case NT_NUMBER:
        case NT_NOTHING:
        case NT_NULL:
        case NT_BINARY:
            return val.refSelf();

        case NT_OBJECT: {
            // see if object is in index
            const QoreObject* obj = val.get<const QoreObject>();
            if (*index) {
                QoreString str;
                qore_get_ptr_hash(str, obj);
                if (index->getKeyValue(str.c_str())) {
                    // object already present, return an index
                    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hashdeclIndexedObjectSerializationInfo, xsink), xsink);
                    size_t size = str.size();
                    rv->setKeyValue("_index", new QoreStringNode(str.giveBuffer(), size, size + 1, QCS_DEFAULT), xsink);
                    return rv.release();
                }
            }

            ReferenceHolder<QoreHashNode> rv(serializeObjectToData(*obj, index, xsink), xsink);
            if (*xsink) {
                return -1;
            }
            return rv.release();
        }

        case NT_HASH: {
            ReferenceHolder<QoreHashNode> rv(serializeHashToData(*val.get<const QoreHashNode>(), index, xsink), xsink);
            if (*xsink) {
                return QoreValue();
            }
            return rv.release();
        }

        default:
            break;
    }

    xsink->raiseException("SERIALIZATION-ERROR", "cannot serialize type '%s'; type is not supported for serialization",
        val.getTypeName());
    return QoreValue();
}

QoreHashNode* QoreSerializable::serializeToData(QoreObject* self, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hashdeclSerializationInfo, xsink), xsink);
    ReferenceHolder<QoreHashNode> index(xsink);

    ReferenceHolder<QoreHashNode> data(serializeObjectToData(*self, index, xsink), xsink);
    if (*xsink) {
        return nullptr;
    }

    if (index) {
        rv->setKeyValue("_index", index.release(), xsink);
    }

    rv->setKeyValue("_data", data.release(), xsink);

    return rv.release();
}

QoreHashNode* QoreSerializable::serializeObjectToData(const QoreObject& self, ReferenceHolder<QoreHashNode>& index, ExceptionSink* xsink) {
    const QoreClass* cls = self.getClass();

    // check if the class inherits Serializable and throw an exception if not
    {
        bool priv = false;
        if (!cls->getClass(*QC_SERIALIZABLE, priv)) {
            xsink->raiseException("SERIALIZATION-ERROR", "cannot serialize class '%s' as it does not inherit 'Serializable' and therefore is not eligible for serialization; to correct this error, declare Serializable as a parent class of '%s'",
                cls->getName(), cls->getName());
            return nullptr;
        }
    }

    ReferenceHolder<QoreHashNode> h(new QoreHashNode(hashdeclObjectSerializationInfo, xsink), xsink);
    h->setKeyValue("_class", new QoreStringNode(cls->getNamespacePath()), xsink);

    ReferenceHolder<QoreHashNode> members_per_class(xsink);

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

            ValueHolder val(self.getReferencedMemberNoMethod(mname, current_cls, xsink), xsink);
            if (*xsink) {
                return nullptr;
            }

            ValueHolder new_val(serializeValue(*val, index, xsink), xsink);
            if (*xsink) {
                return nullptr;
            }

            if (!class_members) {
                class_members = new QoreHashNode(autoTypeInfo);
            }
            class_members->setKeyValue(mname, new_val.release(), xsink);
        }

        if (class_members) {
            if (!members_per_class) {
                members_per_class = new QoreHashNode(autoHashTypeInfo);
            }
            std::string class_path = current_cls->getNamespacePath();
            members_per_class->setKeyValue(class_path.c_str(), class_members.release(), xsink);
        }
    }

    if (members_per_class) {
        h->setKeyValue("_members", members_per_class.release(), xsink);
    }

    // write object to index
    QoreString str;
    qore_get_ptr_hash(str, &self);

    if (!index) {
        index = new QoreHashNode(hashdeclObjectSerializationInfo->getTypeInfo());
    }
    index->setKeyValue(str.c_str(), h.release(), xsink);

    // return an index to the object
    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hashdeclIndexedObjectSerializationInfo, xsink), xsink);
    size_t size = str.size();
    rv->setKeyValue("_index", new QoreStringNode(str.giveBuffer(), size, size + 1, QCS_DEFAULT), xsink);
    return rv.release();
}

QoreHashNode* QoreSerializable::serializeHashToData(const QoreHashNode& h, ReferenceHolder<QoreHashNode>& index, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hashdeclHashSerializationInfo, xsink), xsink);

    const TypedHashDecl* thd = h.getHashDecl();
    rv->setKeyValue("_hash", thd ? new QoreStringNode(thd->getNamespacePath()) : new QoreStringNode("^hash^"), xsink);

    // serialize hash members
    ReferenceHolder<QoreHashNode> hash_members(xsink);

    ConstHashIterator hi(h);
    while (hi.next()) {
        ValueHolder new_val(serializeValue(hi.get(), index, xsink), xsink);
        if (*xsink) {
            return nullptr;
        }

        if (!hash_members) {
            hash_members = new QoreHashNode(autoTypeInfo);
        }
        hash_members->setKeyValue(hi.getKey(), new_val.release(), xsink);
    }

    if (hash_members) {
        rv->setKeyValue("_members", hash_members.release(), xsink);
    }

    return rv.release();
}

BinaryNode* QoreSerializable::serialize(QoreObject* self, ExceptionSink* xsink) {
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