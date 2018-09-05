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
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/typed_hash_decl_private.h"

#include <string>

static QoreHashNode* serialization_get_index(imap_t::iterator i) {
    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hashdeclIndexedObjectSerializationInfo, nullptr), nullptr);
    qore_hash_private* h = qore_hash_private::get(**rv);
    h->setKeyValueIntern("_index", new QoreStringNode(i->second.c_str()));
    return rv.release();
}

QoreValue QoreSerializable::serializeValue(const QoreValue val, ReferenceHolder<QoreHashNode>& index, imap_t& imap, ExceptionSink* xsink) {
    switch (val.getType()) {
        case NT_INT:
        case NT_STRING:
        case NT_BOOLEAN:
        case NT_FLOAT:
        case NT_NUMBER:
        case NT_NOTHING:
        case NT_NULL:
        case NT_BINARY:
        case NT_DATE:
            return val.refSelf();

        case NT_OBJECT: {
            // see if object is in index
            const QoreObject* obj = val.get<const QoreObject>();
            imap_t::iterator i;
            if (!imap.empty()) {
                QoreString str;
                qore_get_ptr_hash(str, obj);
                i = imap.lower_bound(str.c_str());
                if (i != imap.end() && i->first == str.c_str()) {
                    return serialization_get_index(i);
                }
            }
            else {
                i = imap.end();
            }

            ReferenceHolder<QoreHashNode> rv(serializeObjectToData(*obj, index, imap, i, xsink), xsink);
            if (*xsink) {
                return -1;
            }
            return rv.release();
        }

        case NT_HASH: {
            ReferenceHolder<QoreHashNode> rv(serializeHashToData(*val.get<const QoreHashNode>(), index, imap, xsink), xsink);
            if (*xsink) {
                return QoreValue();
            }
            return rv.release();
        }

        case NT_LIST: {
            ReferenceHolder<QoreListNode> rv(serializeListToData(*val.get<const QoreListNode>(), index, imap, xsink), xsink);
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

QoreHashNode* QoreSerializable::serializeToData(const QoreValue val, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hashdeclSerializationInfo, xsink), xsink);
    ReferenceHolder<QoreHashNode> index(xsink);

    imap_t imap;

    ValueHolder data(serializeValue(val, index, imap, xsink), xsink);
    if (*xsink) {
        return nullptr;
    }

    if (index) {
        rv->setKeyValue("_index", index.release(), xsink);
    }

    rv->setKeyValue("_data", data.release(), xsink);

    return rv.release();
}

QoreHashNode* QoreSerializable::serializeObjectToData(const QoreObject& self, ReferenceHolder<QoreHashNode>& index, imap_t& imap, imap_t::iterator hint, ExceptionSink* xsink) {
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

    // first write object to index
    imap_t::iterator i;
    std::string index_str;
    {
        QoreString str;
        qore_get_ptr_hash(str, &self);

        assert(imap.find(str.c_str()) == imap.end());
        index_str = std::to_string(imap.size());
        i = imap.insert(hint, imap_t::value_type(str.c_str(), index_str));
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
            const QoreExternalNormalMember* m = mi.getMember();
            // skip members marked as transient
            if (m->isTransient()) {
                continue;
            }
            const char* mname = mi.getName();

            ValueHolder val(self.getReferencedMemberNoMethod(mname, current_cls, xsink), xsink);
            if (*xsink) {
                return nullptr;
            }

            ValueHolder new_val(serializeValue(*val, index, imap, xsink), xsink);
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

    if (!index) {
        index = new QoreHashNode(hashdeclObjectSerializationInfo->getTypeInfo());
    }
    index->setKeyValue(index_str.c_str(), h.release(), xsink);

    // return an index to the object
    return serialization_get_index(i);
}

QoreHashNode* QoreSerializable::serializeHashToData(const QoreHashNode& h, ReferenceHolder<QoreHashNode>& index, imap_t& imap, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hashdeclHashSerializationInfo, xsink), xsink);

    const TypedHashDecl* thd = h.getHashDecl();
    rv->setKeyValue("_hash", thd ? new QoreStringNode(thd->getNamespacePath()) : new QoreStringNode("^hash^"), xsink);

    // serialize hash members
    ReferenceHolder<QoreHashNode> hash_members(xsink);

    ConstHashIterator hi(h);
    while (hi.next()) {
        ValueHolder new_val(serializeValue(hi.get(), index, imap, xsink), xsink);
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

QoreListNode* QoreSerializable::serializeListToData(const QoreListNode& l, ReferenceHolder<QoreHashNode>& index, imap_t& imap, ExceptionSink* xsink) {
    ReferenceHolder<QoreListNode> rv(new QoreListNode(autoTypeInfo), xsink);

    ConstListIterator li(l);
    while (li.next()) {
        ValueHolder new_val(serializeValue(li.getValue(), index, imap, xsink), xsink);
        if (*xsink) {
            return nullptr;
        }

        rv->push(new_val.release(), xsink);
    }

    return rv.release();
}

QoreValue QoreSerializable::deserialize(const QoreHashNode* h, ExceptionSink* xsink) {
    assert(h->getHashDecl() == hashdeclSerializationInfo);

    ObjectIndexMap oimap(xsink);

    QoreProgram* pgm = getProgram();

    QoreValue val = h->getKeyValue("_index");
    if (val) {
        assert(val.getType() == NT_HASH);
        const QoreHashNode* index = val.get<const QoreHashNode>();
        // first create the objects and their indices
        ConstHashIterator hi(index);
        while (hi.next()) {
            const char* key = hi.getKey();
            const QoreHashNode* oh = hi.get().get<const QoreHashNode>();
            assert(oh->getHashDecl() == hashdeclObjectSerializationInfo);
            QoreValue v = oh->getKeyValue("_class");
            assert(v.getType() == NT_STRING);
            const QoreClass* cls = pgm->findClass(v.get<const QoreStringNode>()->c_str(), xsink);
            if (!cls) {
                assert(*xsink);
                return QoreValue();
            }
            QoreObject* obj = new QoreObject(cls, pgm);
            assert(oimap.find(key) == oimap.end());
            oimap.insert(oimap_t::value_type(key, obj));
        }

        while (hi.next()) {
            const char* key = hi.getKey();
            QoreObject* obj = oimap.find(key)->second;

            const QoreHashNode* oh = hi.get().get<const QoreHashNode>();
            const QoreClass* cls = obj->getClass();

            // deserialize each class in the hierarchy separately
            QoreValue v = oh->getKeyValue("_members");
            if (v) {
                assert(v.getType() == NT_HASH);
                const QoreHashNode* mh = v.get<const QoreHashNode>();

                ConstHashIterator mhi(mh);
                while (mhi.next()) {
                    const QoreClass* mcls = pgm->findClass(mhi.getKey(), xsink);
                    if (!cls) {
                        assert(*xsink);
                        return QoreValue();
                    }

                    // make sure that the class is part of the object's hierarchy
                    {
                        ClassAccess access;
                        if (!cls->inHierarchy(*mcls, access)) {
                            xsink->raiseException("DESERIALIZATION-ERROR", "object to be deserialized is of class '%s' which does not contain class '%s' in its hierarchy when being deserialized", obj->getClassName(), mcls->getName());
                            return QoreValue();
                        }
                    }

                    if (cls->isSystem()) {
                        // TODO: add support for serialization to builtin classes
                        assert(false);
                    }
                    else {
                        v = mhi.get();
                        if (v) {
                            assert(v.getType() == NT_HASH);
                            const QoreHashNode* cmh = v.get<const QoreHashNode>();
                            ConstHashIterator cmhi(cmh);
                            while (cmhi.next()) {
                                ValueHolder vh(deserializeData(cmhi.get(), oimap, xsink), xsink);
                                if (*xsink) {
                                    return QoreValue();
                                }

                                obj->setMemberValue(cmhi.getKey(), mcls, *vh, xsink);
                                if (*xsink) {
                                    return QoreValue();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return deserializeData(h->getKeyValue("_data"), oimap, xsink);
}

QoreValue QoreSerializable::deserializeData(const QoreValue val, const oimap_t& oimap, ExceptionSink* xsink) {
    if (val.getType() == NT_HASH) {
        const QoreHashNode* h = val.get<const QoreHashNode>();
        QoreValue v = h->getKeyValue("_hash");
        if (v) {
            if (v.getType() != NT_STRING) {
                xsink->raiseException("DESERIALIZATION-ERROR", "'_hash' key has invalid type '%s'; expecting 'string'", v.getTypeName());
                return QoreValue();
            }

            return deserializeHashData(v.get<const QoreStringNode>(), h, oimap, xsink);
        }
        v = h->getKeyValue("_index");
        if (v) {
            if (v.getType() != NT_STRING) {
                xsink->raiseException("DESERIALIZATION-ERROR", "'_index' key has invalid type '%s'; expecting 'string'", v.getTypeName());
                return QoreValue();
            }
            const char* key = v.get<const QoreStringNode>()->c_str();

            oimap_t::const_iterator i = oimap.find(key);
            if (i == oimap.end()) {
                xsink->raiseException("DESERIALIZATION-ERROR", "'_index' value '%s' is invalid; no such index exists", key);
                return QoreValue();
            }

            return i->second->refSelf();
        }

        xsink->raiseException("DESERIALIZATION-ERROR", "hash hash no type information for deserialization; expecting either '_hash' or '_index' keys; neither was found");
        return QoreValue();
    }

    return val.refSelf();
}

QoreValue QoreSerializable::deserializeHashData(const QoreStringNode* type, const QoreHashNode* h, const oimap_t& oimap, ExceptionSink* xsink) {
    // get members, if any
    const QoreValue members = h->getKeyValue("_members");

    if (members && members.getType() != NT_HASH) {
        xsink->raiseException("DESERIALIZATION-ERROR", "'_members' key has invalid type '%s'; expecting 'hash' or 'nothing'", members.getTypeName());
        return QoreValue();
    }

    if (*type == "^hash^") {
        return members ? members.refSelf() : new QoreHashNode(autoTypeInfo);
    }

    const QoreNamespace* pns = nullptr;
    const TypedHashDecl* hd = getProgram()->findHashDecl(type->c_str(), pns);
    if (!hd) {
        xsink->raiseException("DESERIALIZATION-ERROR", "'_hash' key indicates that a '%s' typed hash should be deserialized, but no such typed hash (hashdecl) could be found in the current Program object", type->c_str());
        return QoreValue();
    }

    // do the runtime cast
    return typed_hash_decl_private::get(*hd)->newHash(members.get<const QoreHashNode>(), true, xsink);

    ReferenceHolder<QoreHashNode> rv(new QoreHashNode(hd), xsink);

    return rv.release();
}

void QoreSerializable::serialize(QoreObject* self, OutputStream* stream, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> h(serializeToData(self, xsink), xsink);
    if (*xsink) {
        return;
    }

    xsink->raiseException("SERIALIZATION-ERROR", "unimplemented");
}

QoreValue QoreSerializable::deserialize(InputStream* stream, ExceptionSink* xsink) {
    xsink->raiseException("DESERIALIZATION-ERROR", "unimplemented");
    return QoreValue();
}
