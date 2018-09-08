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
#include "qore/intern/QoreObjectIntern.h"

#include <string>
#include <set>
#include <cstdint>
#include <cctype>
#include <cstdlib>

static QoreString QoreSerializationTypeString("QS");
static QoreString QoreSerializationVersionString("1.0");

typedef std::set<std::string> strset_t;

// qore serialization stream data type constants
#define QSSDT_HASH 0
#define QSSDT_HASHDECL 1
#define QSSDT_STRING 2
#define QSSDT_UTF8_STRING 3
#define QSSDT_LIST 4
#define QSSDT_BOOLEAN 5
#define QSSDT_INT1 6
#define QSSDT_INT2 7
#define QSSDT_INT4 8
#define QSSDT_INT8 9
#define QSSDT_FLOAT 10
#define QSSDT_NUMBER 11
#define QSSDT_BINARY 12
#define QSSDT_ABSDATE 13
#define QSSDT_RELDATE 14
#define QSSDT_NULL 15
#define QSSDT_NOTHING 16

#define CODE_IS_INT(x) ((x) == QSSDT_INT1 || (x) == QSSDT_INT2 || (x) == QSSDT_INT4 || (x) == QSSDT_INT8)
#define CODE_IS_STRING(x) ((x) == QSSDT_STRING || (x) == QSSDT_UTF8_STRING)

ObjectIndexMap::~ObjectIndexMap() {
    for (auto& i : *this) {
        if (*xs) {
            // in case of an exception, we need to obliterate the object before dereferencing
            qore_object_private::get(*i.second)->obliterate(xs);
        }
        else {
            // otherwise we just need to dereference
            i.second->deref(xs);
        }
    }
}

static int stream_read_string(ExceptionSink* xsink, InputStream& stream, QoreString& str, int max_size = -1) {
    assert(max_size);
    str.clear();

    unsigned char c;
    while (true) {
        int64 rc = stream.read(&c, 1, xsink);
        if (*xsink) {
            return -1;
        }
        if (!rc) {
            xsink->raiseException("DESERIALIZATION-ERROR", "end of stream found while reading string value");
            return -1;
        }

        if (!c) {
            break;
        }

        if (max_size > 0 && str.size() == (size_t)max_size) {
            xsink->raiseException("DESERIALIZATION-ERROR", "string exceeded maximum size of %d while reading string value", max_size);
            return -1;
        }

        str.concat(c);
    }

    return 0;
}

static int check_deserialization_string(const QoreString& str, const QoreString& expected, const char* type, ExceptionSink* xsink) {
    if (str != expected) {
        bool bin = false;

        // see if string has binary data in it
        for (const char* p = str.c_str(), * e = p + str.size(); p < e; ++p) {
            if (*p < 32 || *p > 126) {
                bin = true;
                break;
            }
        }

        if (bin) {
            xsink->raiseException("DESERIALIZATION-ERROR", "stream data does not match expected %s string value '%s'; read binary (non-string) data instead", type, expected.c_str());
        }
        else {
            xsink->raiseException("DESERIALIZATION-ERROR", "expecting expecting %s string value '%s' from stream; got '%s' instead", type, expected.c_str(), str.c_str());
        }
        return -1;
    }
    return 0;
}


class QoreInternalSerializationContext {
public:
    ReferenceHolder<QoreHashNode>& index;
    imap_t& imap;

    DLLLOCAL QoreInternalSerializationContext(ReferenceHolder<QoreHashNode>& index, imap_t& imap) : index(index), imap(imap) {
    }

    DLLLOCAL int serializeObject(const QoreObject& obj, std::string& index_str, ExceptionSink* xsink) {
        imap_t::iterator i = QoreSerializable::serializeObjectToIndex(obj, index, imap, xsink);
        if (*xsink) {
            return -1;
        }
        index_str = i->first;
        return 0;
    }

    QoreValue serializeValue(const QoreValue val, ExceptionSink* xsink) {
        return QoreSerializable::serializeValue(val, index, imap, xsink);
    }
};

class QoreInternalDeserializationContext {
public:
    const oimap_t& oimap;

    DLLLOCAL QoreInternalDeserializationContext(const oimap_t& oimap) : oimap(oimap) {
    }

    DLLLOCAL QoreObject* deserializeObject(const char* index_str, ExceptionSink* xsink) {
        return QoreSerializable::deserializeIndexedObject(index_str, oimap, xsink);
    }

    DLLLOCAL QoreValue deserializeValue(const QoreValue val, ExceptionSink* xsink) {
        return QoreSerializable::deserializeData(val, oimap, xsink);
    }
};

int QoreSerializationContext::serializeObject(const QoreObject& obj, std::string& index, ExceptionSink* xsink) {
    return reinterpret_cast<QoreInternalSerializationContext*>(this)->serializeObject(obj, index, xsink);
}

QoreValue QoreSerializationContext::serializeValue(const QoreValue val, ExceptionSink* xsink) {
    return reinterpret_cast<QoreInternalSerializationContext*>(this)->serializeValue(val, xsink);
}

QoreObject* QoreDeserializationContext::deserializeObject(const char* index, ExceptionSink* xsink) {
    return reinterpret_cast<QoreInternalDeserializationContext*>(this)->deserializeObject(index, xsink);
}

QoreValue QoreDeserializationContext::deserializeValue(const QoreValue val, ExceptionSink* xsink) {
    return reinterpret_cast<QoreInternalDeserializationContext*>(this)->deserializeValue(val, xsink);
}

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
            return serializeObjectToData(*val.get<const QoreObject>(), index, imap, xsink);
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

imap_t::iterator QoreSerializable::serializeObjectToIndex(const QoreObject& obj, ReferenceHolder<QoreHashNode>& index, imap_t& imap, ExceptionSink* xsink) {
    // see if object is in index
    QoreString str;
    qore_get_ptr_hash(str, &obj);

    imap_t::iterator i = imap.lower_bound(str.c_str());

    return i != imap.end() && i->first == str.c_str()
        ? i
        : serializeObjectToIndexIntern(obj, index, imap, str, i, xsink);
}

QoreValue QoreSerializable::serializeObjectToData(const QoreObject& obj, ReferenceHolder<QoreHashNode>& index, imap_t& imap, ExceptionSink* xsink) {
    imap_t::iterator i = serializeObjectToIndex(obj, index, imap, xsink);
    return *xsink ? QoreValue() : serialization_get_index(i);
}

imap_t::iterator QoreSerializable::serializeObjectToIndexIntern(const QoreObject& self, ReferenceHolder<QoreHashNode>& index, imap_t& imap, const QoreString& str, imap_t::iterator hint, ExceptionSink* xsink) {
    const QoreClass* cls = self.getClass();

    // first write object to index
    assert(imap.find(str.c_str()) == imap.end());
    std::string index_str = std::to_string(imap.size());
    imap_t::iterator i = imap.insert(hint, imap_t::value_type(str.c_str(), index_str));

    ReferenceHolder<QoreHashNode> h(new QoreHashNode(hashdeclObjectSerializationInfo, xsink), xsink);
    h->setKeyValue("_class", new QoreStringNode(cls->getNamespacePath()), xsink);

    ReferenceHolder<QoreHashNode> class_data(xsink);

    QoreClassHierarchyIterator ci(cls);
    while (ci.next()) {
        // do not process virtual classes
        if (ci.isVirtual()) {
            continue;
        }

        const QoreClass* current_cls = ci.get();

        // check if the class inherits Serializable and throw an exception if not
        {
            bool priv = false;
            if (!current_cls->getClass(*QC_SERIALIZABLE, priv)) {
                xsink->raiseException("SERIALIZATION-ERROR", "cannot serialize class '%s' as it does not inherit 'Serializable' and therefore is not eligible for serialization; to correct this error, declare Serializable as a parent class of '%s'",
                    current_cls->getName(), current_cls->getName());
                return imap.end();
            }
        }

        // serialize class members for each member of the hierarchy separately
        ReferenceHolder<QoreHashNode> class_members(xsink);

        if (current_cls->isSystem()) {
            q_serializer_t serializer = current_cls->getSerializer();
            assert(serializer);

            // get class private data for serialization call
            ReferenceHolder<AbstractPrivateData> private_data(self.getReferencedPrivateData(current_cls->getID(), xsink), xsink);
            if (*xsink) {
                return imap.end();
            }

            QoreInternalSerializationContext context(index, imap);

            class_members = serializer(self, **private_data, reinterpret_cast<QoreSerializationContext&>(context), xsink);
            if (*xsink) {
                return imap.end();
            }
        }
        else {
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
                    return imap.end();
                }

                ValueHolder new_val(serializeValue(*val, index, imap, xsink), xsink);
                if (*xsink) {
                    return imap.end();
                }

                if (!class_members) {
                    class_members = new QoreHashNode(autoTypeInfo);
                }
                class_members->setKeyValue(mname, new_val.release(), xsink);
            }
        }

        if (class_members) {
            if (!class_data) {
                class_data = new QoreHashNode(autoHashTypeInfo);
            }
            std::string class_path = current_cls->getNamespacePath();
            class_data->setKeyValue(class_path.c_str(), class_members.release(), xsink);
        }
    }

    if (class_data) {
        h->setKeyValue("_class_data", class_data.release(), xsink);
    }

    if (!index) {
        index = new QoreHashNode(hashdeclObjectSerializationInfo->getTypeInfo());
    }
    index->setKeyValue(index_str.c_str(), h.release(), xsink);

    return i;
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

QoreValue QoreSerializable::deserialize(const QoreHashNode& h, ExceptionSink* xsink) {
    assert(hashdeclSerializationInfo->equal(h.getHashDecl()));

    ObjectIndexMap oimap(xsink);

    QoreProgram* pgm = getProgram();

    QoreValue val = h.getKeyValue("_index");
    if (val) {
        assert(val.getType() == NT_HASH);
        const QoreHashNode* index = val.get<const QoreHashNode>();
        // first create the objects and their indices
        ConstHashIterator hi(index);
        while (hi.next()) {
            const char* key = hi.getKey();
            const QoreHashNode* oh = hi.get().get<const QoreHashNode>();
            assert(hashdeclObjectSerializationInfo->equal(oh->getHashDecl()));
            QoreValue v = oh->getKeyValue("_class");
            assert(v.getType() == NT_STRING);
            const char* cname = v.get<const QoreStringNode>()->c_str();
            const QoreClass* cls = pgm->findClass(cname, xsink);
            if (!cls) {
                if (!*xsink) {
                    xsink->raiseException("DESERIALIZATION-ERROR", "cannot find class '%s' required for deserialization", cname);
                }
                return QoreValue();
            }
            QoreObject* obj = new QoreObject(cls, pgm);
            assert(oimap.find(key) == oimap.end());
            oimap.insert(oimap_t::value_type(key, obj));
        }

        // iterate the hash again
        while (hi.next()) {
            const char* key = hi.getKey();
            QoreObject* obj = oimap.find(key)->second;

            const QoreHashNode* oh = hi.get().get<const QoreHashNode>();
            const QoreClass* cls = obj->getClass();

            // deserialize each class in the hierarchy separately
            QoreValue v = oh->getKeyValue("_class_data");
            assert(!v || v.getType() == NT_HASH);
            const QoreHashNode* mh = v ? v.get<const QoreHashNode>() : nullptr;
            // make sure we use all the keys in the hash
            size_t found = 0;

            // ensure that the serialization hash includes all classes necessary for the parsent class
            QoreClassHierarchyIterator chi(cls);

            while (chi.next()) {
                // do not process virtual classes
                if (chi.isVirtual()) {
                    continue;
                }

                const QoreClass* mcls = chi.get();

                //printd(5, "iterating %p '%s' GOT CHILD %p '%s'\n", cls, cls->getName(), mcls, mcls->getNamespacePath().c_str());

                // check if the class inherits Serializable and throw an exception if not
                {
                    bool priv = false;
                    if (!mcls->getClass(*QC_SERIALIZABLE, priv)) {
                        xsink->raiseException("DESERIALIZATION-ERROR", "cannot deserialize class '%s' as it does not inherit 'Serializable' and therefore is not eligible for deserialization'",
                            mcls->getName());
                        return QoreValue();
                    }
                }

                std::string class_path = mcls->getNamespacePath();

                QoreHashNode* cmh = nullptr;

                QoreValue cv;
                if (mh) {
                    bool exists;
                    cv = mh->getKeyValueExistence(class_path.c_str(), exists);
                    if (exists) {
                        ++found;
                        if (cv) {
                            if (cv.getType() != NT_HASH) {
                                xsink->raiseException("DESERIALIZATION-ERROR", "serialized data for class '%s' has type '%s'; expecting 'hash' or 'nothing'",
                                    mcls->getName(), cv.getTypeName());
                                return QoreValue();
                            }
                            cmh = cv.get<QoreHashNode>();
                        }
                    }
                }

                if (mcls->isSystem()) {
                    q_deserializer_t deserializer = mcls->getDeserializer();
                    assert(deserializer);

                    QoreInternalDeserializationContext context(oimap);

                    deserializer(*obj, cmh, reinterpret_cast<QoreDeserializationContext&>(context), xsink);
                    if (*xsink) {
                        return QoreValue();
                    }
                }
                else {
                    if (cmh) {
                        // deserialize members
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

                        // initialize transient members
                        if (mcls->hasTransientMember()) {
                            QoreClassMemberIterator mi(mcls);
                            while (mi.next()) {
                                const QoreExternalNormalMember* mem = mi.getMember();
                                if (!mem->isTransient()) {
                                    continue;
                                }
                                ValueHolder val(mem->getDefaultValue(xsink), xsink);
                                if (*xsink) {
                                    return QoreValue();
                                }

                                obj->setMemberValue(mi.getName(), mcls, *val, xsink);
                                if (*xsink) {
                                    return QoreValue();
                                }
                            }
                        }
                    }
                }
            }

            // get hierarchy size in terms of serialized data
            size_t hsize = mh ? mh->size() : 0;
            // throw an exception if we did not use all the keys in the serialization hash
            if (found < hsize) {
                // get list of "extra" classes in serialization hash
                strset_t strset;

                // first get a set of all classes in the serialization hash
                ConstHashIterator chi2(mh);
                while (chi2.next()) {
                    strset.insert(chi2.getKey());
                }

                // remove all matching classes in the hierarchy
                QoreClassHierarchyIterator chi3(cls);
                while (chi3.next()) {
                    strset_t::iterator i = strset.find(chi3.get()->getNamespacePath());
                    if (i != strset.end()) {
                        strset.erase(i);
                    }
                }

                // create the error string
                SimpleRefHolder<QoreStringNode> desc(new QoreStringNodeMaker("incompatible class hierarchy; %d class%s in serialization data, but %d used for deserialization; unmatched classes: ", static_cast<int>(hsize), hsize == 1 ? "" : "es", static_cast<int>(found)));

                for (auto& i : strset) {
                    desc->sprintf("'%s', ", i.c_str());
                }

                desc->terminate(desc->size() - 2);

                xsink->raiseException("DESERIALIZATION-ERROR", desc.release());
                return QoreValue();
            }
        }
    }

    return deserializeData(h.getKeyValue("_data"), oimap, xsink);
}

QoreObject* QoreSerializable::deserializeIndexedObject(const char* key, const oimap_t& oimap, ExceptionSink* xsink) {
    oimap_t::const_iterator i = oimap.find(key);
    if (i == oimap.end()) {
        xsink->raiseException("DESERIALIZATION-ERROR", "'_index' value '%s' is invalid; no such index exists", key);
        return nullptr;
    }

    i->second->ref();
    return i->second;
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

            return deserializeHashData(*v.get<const QoreStringNode>(), *h, oimap, xsink);
        }
        v = h->getKeyValue("_index");
        if (v) {
            if (v.getType() != NT_STRING) {
                xsink->raiseException("DESERIALIZATION-ERROR", "'_index' key has invalid type '%s'; expecting 'string'", v.getTypeName());
                return QoreValue();
            }
            const char* key = v.get<const QoreStringNode>()->c_str();
            return deserializeIndexedObject(key, oimap, xsink);
        }

        xsink->raiseException("DESERIALIZATION-ERROR", "hash hash no type information for deserialization; expecting either '_hash' or '_index' keys; neither was found");
        return QoreValue();
    }

    if (val.getType() == NT_LIST) {
        return deserializeListData(*val.get<const QoreListNode>(), oimap, xsink);
    }

    return val.refSelf();
}

QoreValue QoreSerializable::deserializeHashData(const QoreStringNode& type, const QoreHashNode& h, const oimap_t& oimap, ExceptionSink* xsink) {
    // get members, if any
    const QoreValue members = h.getKeyValue("_members");

    if (members && members.getType() != NT_HASH) {
        xsink->raiseException("DESERIALIZATION-ERROR", "'_members' key has invalid type '%s'; expecting 'hash' or 'nothing'", members.getTypeName());
        return QoreValue();
    }

    if (type == "^hash^") {
        return members ? members.refSelf() : new QoreHashNode(autoTypeInfo);
    }

    const QoreNamespace* pns = nullptr;
    const TypedHashDecl* hd = getProgram()->findHashDecl(type.c_str(), pns);
    if (!hd) {
        xsink->raiseException("DESERIALIZATION-ERROR", "'_hash' key indicates that a '%s' typed hash should be deserialized, but no such typed hash (hashdecl) could be found in the current Program object", type.c_str());
        return QoreValue();
    }

    // do the runtime cast
    return typed_hash_decl_private::get(*hd)->newHash(members.get<const QoreHashNode>(), true, xsink);
}

QoreValue QoreSerializable::deserializeListData(const QoreListNode& l, const oimap_t& oimap, ExceptionSink* xsink) {
    ReferenceHolder<QoreListNode> rv(new QoreListNode(autoTypeInfo), xsink);

    ConstListIterator li(l);
    while (li.next()) {
        ValueHolder val(deserializeData(li.getValue(), oimap, xsink), xsink);
        if (*xsink) {
            return QoreValue();
        }
        rv->push(val.release(), xsink);
    }

    return rv.release();
}

void QoreSerializable::serialize(const QoreValue val, OutputStream& stream, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> h(serializeToData(val, xsink), xsink);
    if (*xsink) {
        return;
    }

    serializeToStream(**h, stream, xsink);
}


void QoreSerializable::serialize(const QoreObject& self, OutputStream& stream, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> h(serializeToData(&self, xsink), xsink);
    if (*xsink) {
        return;
    }

    serializeToStream(**h, stream, xsink);
}

void QoreSerializable::serializeToStream(const QoreHashNode& h, OutputStream& stream, ExceptionSink* xsink) {
    assert(hashdeclSerializationInfo->equal(h.getHashDecl()));

    serializeValueToStream(&h, stream, xsink);
    //xsink->raiseException("SERIALIZATION-ERROR", "unimplemented");
}

int QoreSerializable::serializeValueToStream(const QoreValue val, OutputStream& stream, ExceptionSink* xsink) {
    if (!stream.check(xsink)) {
        return -1;
    }

    // write header to output stream
    stream.write(QoreSerializationTypeString.c_str(), QoreSerializationTypeString.size() + 1, xsink);
    if (*xsink) {
        return -1;
    }
    stream.write(QoreSerializationVersionString.c_str(), QoreSerializationVersionString.size() + 1, xsink);
    if (*xsink) {
        return -1;
    }

    // must reference the output stream for the assignment to StreamWriter
    stream.ref();
    ReferenceHolder<StreamWriter> writer(new StreamWriter(xsink, &stream, QCS_UTF8), xsink);

    return serializeValueToStream(val, **writer, xsink);
}

int QoreSerializable::serializeValueToStream(const QoreValue val, StreamWriter& writer, ExceptionSink* xsink) {
    switch (val.getType()) {
        case NT_HASH:
            return serializeHashToStream(*val.get<QoreHashNode>(), writer, xsink);

        case NT_STRING: {
            return serializeStringToStream(*val.get<QoreStringNode>(), writer, xsink);
        }

        case NT_INT: {
            return serializeIntToStream(val.v.i, writer, xsink);
        }

        case NT_BOOLEAN: {
            return serializeBoolToStream(val.v.b, writer, xsink);
        }

        case NT_LIST: {
            return serializeListToStream(*val.get<QoreListNode>(), writer, xsink);
        }

        case NT_FLOAT: {
            return serializeFloatToStream(val.v.f, writer, xsink);
        }

        case NT_NUMBER: {
            return serializeNumberToStream(*val.get<QoreNumberNode>(), writer, xsink);
        }

        case NT_DATE: {
            return serializeDateToStream(*val.get<DateTimeNode>(), writer, xsink);
        }

        default:
            break;
    }

    xsink->raiseException("SERIALIZATION-ERROR", "cannot serialize type '%s'; type is not supported for serialization",
        val.getTypeName());
    return -1;
}

int QoreSerializable::serializeHashToStream(const QoreHashNode& h, StreamWriter& writer, ExceptionSink* xsink) {
    // write hashdecl type to stream, if any, otherwise -1
    {
        const TypedHashDecl* thd = h.getHashDecl();
        if (thd) {
            // write data type code to stream
            if (writer.writei1(QSSDT_HASHDECL, xsink)) {
                return -1;
            }

            std::string path = thd->getNamespacePath();

            // write hashdecl string size to stream
            if (serializeIntToStream(path.size(), writer, xsink)) {
                return -1;
            }

            // write string
            if (writer.write(path.c_str(), path.size(), xsink)) {
                return -1;
            }
        }
        else {
            // write data type code to stream
            if (writer.writei1(QSSDT_HASH, xsink)) {
                return -1;
            }
        }
    }

    // write hash size to stream
    if (serializeIntToStream(h.size(), writer, xsink)) {
        return -1;
    }

    ConstHashIterator hi(h);
    while (hi.next()) {
        // write key string to stream
        if (serializeStringToStream(writer, hi.getKey(), strlen(hi.getKey()), QCS_DEFAULT, xsink)) {
            return -1;
        }

        if (serializeValueToStream(hi.get(), writer, xsink)) {
            return -1;
        }
    }

    return 0;
}

int QoreSerializable::serializeStringToStream(const QoreStringNode& str, StreamWriter& writer, ExceptionSink* xsink) {
    return serializeStringToStream(writer, str.c_str(), str.size(), str.getEncoding(), xsink);
}

int QoreSerializable::serializeStringToStream(StreamWriter& writer, const char* str, size_t len, const QoreEncoding* enc, ExceptionSink* xsink) {
    // write string encoding if not UTF-8
    if (enc && enc != QCS_UTF8) {
        // write data type code to stream
        if (writer.writei1(QSSDT_STRING, xsink)) {
            return -1;
        }
        // get encoding string
        const char* enc_str = enc->getCode();
        size_t enc_len = strlen(enc_str);

        // write encoding string size to stream
        if (QoreSerializable::serializeIntToStream(enc_len, writer, xsink)) {
            return -1;
        }

        // write string
        if (writer.write(enc_str, enc_len, xsink)) {
            return -1;
        }
    }
    else {
        // write data type code to stream
        if (writer.writei1(QSSDT_UTF8_STRING, xsink)) {
            return -1;
        }
    }

    // write string size
    if (QoreSerializable::serializeIntToStream(len, writer, xsink)) {
        return -1;
    }

    // write string
    return writer.write(str, len, xsink);
}

int QoreSerializable::serializeIntToStream(int64 i, StreamWriter& writer, ExceptionSink* xsink) {
    if (i >= -128 & i <= 127) {
        // write data type code to stream
        if (!writer.writei1(QSSDT_INT1, xsink)) {
            // write integer to stream
            return writer.writei1((signed char)i, xsink);
        }
    }
    else if (i >= -32768 && i <= 32767) {
        // write data type code to stream
        if (!writer.writei1(QSSDT_INT2, xsink)) {
            // write integer to stream
            return writer.writei2((int16_t)i, xsink);
        }
    }
    else if (i >= -2147483648 && i <= 2147483647) {
        // write data type code to stream
        if (!writer.writei1(QSSDT_INT4, xsink)) {
            // write integer to stream
            return writer.writei4((int32_t)i, xsink);
        }
    }
    else {
        // write data type code to stream
        if (!writer.writei1(QSSDT_INT8, xsink)) {
            // write integer to stream
            return writer.writei8((int64_t)i, xsink);
        }
    }

    return -1;
}

int QoreSerializable::serializeBoolToStream(bool b, StreamWriter& writer, ExceptionSink* xsink) {
    // write data type code to stream
    if (!writer.writei1(QSSDT_BOOLEAN, xsink)) {
        // write bool to stream
        return writer.writei1((signed char)b, xsink);
    }

    return -1;
}

int QoreSerializable::serializeListToStream(const QoreListNode& l, StreamWriter& writer, ExceptionSink* xsink) {
    // write data type code to stream
    if (writer.writei1(QSSDT_LIST, xsink)) {
        return -1;
    }

    // write list size to stream
    if (serializeIntToStream(l.size(), writer, xsink)) {
        return -1;
    }

    ConstListIterator i(l);
    while (i.next()) {
        if (serializeValueToStream(i.getValue(), writer, xsink)) {
            return -1;
        }
    }

    return 0;
}

int QoreSerializable::serializeFloatToStream(double f, StreamWriter& writer, ExceptionSink* xsink) {
    // write data type code to stream
    if (!writer.writei1(QSSDT_FLOAT, xsink)) {
        // write value to stream
        int64* i = reinterpret_cast<int64*>(&f);
        return writer.writei8(*i, xsink);
    }

    return -1;
}

int QoreSerializable::serializeNumberToStream(const QoreNumberNode& n, StreamWriter& writer, ExceptionSink* xsink) {
    // write data type code to stream
    if (writer.writei1(QSSDT_NUMBER, xsink)) {
        return -1;
    }

    QoreString tmp;
    n.toString(tmp, QORE_NF_SCIENTIFIC|QORE_NF_RAW);
    if (tmp == "inf") {
        tmp.set("@inf@n");
    }
    else if (tmp == "-inf") {
        tmp.set("-@inf@n");
    }
    else if (tmp == "nan") {
        tmp.set("@nan@n");
    }
    else {
        tmp.concat('n');
    }
    // append precision
    tmp.sprintf("{%u}", n.getPrec());

    // write number string size
    if (QoreSerializable::serializeIntToStream(tmp.size(), writer, xsink)) {
        return -1;
    }

    // write number string
    return writer.write(tmp.c_str(), tmp.size(), xsink);
}

// absolute dates:
//   utc zones:    int epoch | int us | int offset
//   region zones: int epoch | int us | string region
// relative dates:
//   int size "P..."
int QoreSerializable::serializeDateToStream(const DateTimeNode& n, StreamWriter& writer, ExceptionSink* xsink) {
    if (n.isAbsolute()) {
        // write data type code to stream
        if (writer.writei1(QSSDT_ABSDATE, xsink)) {
            return -1;
        }

        // write epoch to stream
        int64 epoch = n.getEpochMicrosecondsUTC();
        if (serializeIntToStream(epoch, writer, xsink)) {
            return -1;
        }

        // write microseconds to stream
        int us = n.getMicrosecond();
        if (serializeIntToStream(us, writer, xsink)) {
            return -1;
        }

        // write zone type to stream
        const AbstractQoreZoneInfo* zone = n.getZone();
        if (dynamic_cast<const QoreOffsetZoneInfo*>(zone)) {
            int utc_offset = AbstractQoreZoneInfo::getUTCOffset(zone);

            // write UTC offset to stream
            if (serializeIntToStream(utc_offset, writer, xsink)) {
                return -1;
            }
        }
        else {
            const char* region = AbstractQoreZoneInfo::getRegionName(zone);
            assert(region);

            if (serializeStringToStream(writer, region, strlen(region), nullptr, xsink)) {
                return -1;
            }
        }

        return 0;
    }

    // relative date
    if (writer.writei1(QSSDT_RELDATE, xsink)) {
        return -1;
    }

    QoreString str;

    if (n.hasValue()) {
        qore_tm info;
        n.getInfo(info);

        if (info.year) {
            str.sprintf("%dY", info.year);
        }
        if (info.month) {
            str.sprintf("%dM", info.month);
        }
        if (info.day) {
            str.sprintf("%dD", info.day);
        }

        bool has_t = false;

        if (info.hour) {
            str.sprintf("T%dH", info.hour);
            has_t = true;
        }
        if (info.minute) {
            if (!has_t) {
                str.concat('T');
                has_t = true;
            }
            str.sprintf("%dM", info.minute);
        }
        if (info.second) {
            if (!has_t) {
                str.concat('T');
                has_t = true;
            }
            str.sprintf("%dS", info.second);
        }
        if (info.us) {
            if (!has_t) {
                str.concat('T');
                has_t = true;
            }
            str.sprintf("%du", info.us);
        }
    }
    else {
        str.concat("0D");
    }

    // write string size
    if (QoreSerializable::serializeIntToStream(str.size(), writer, xsink)) {
        return -1;
    }

    // write string
    return writer.write(str.c_str(), str.size(), xsink);
}

QoreValue QoreSerializable::deserialize(InputStream& stream, ExceptionSink* xsink) {
    if (!stream.check(xsink)) {
        return QoreValue();
    }

    // read serialization stream type
    QoreString str;
    if (stream_read_string(xsink, stream, str, 50)) {
        return QoreValue();
    }

    if (check_deserialization_string(str, QoreSerializationTypeString, "header type", xsink)) {
        return QoreValue();
    }

    // read serialization stream version
    if (stream_read_string(xsink, stream, str, 50)) {
        return QoreValue();
    }

    if (check_deserialization_string(str, QoreSerializationVersionString, "header version", xsink)) {
        return QoreValue();
    }

    // must reference the input stream for the assignment to StreamReader
    stream.ref();
    ReferenceHolder<StreamReader> reader(new StreamReader(xsink, &stream, QCS_UTF8), xsink);

    ValueHolder val(deserializeValueFromStream(**reader, xsink), xsink);
    if (*xsink) {
        return QoreValue();
    }
    const QoreHashNode* h = val->getType() == NT_HASH ? val->get<const QoreHashNode>() : nullptr;

    //printd(5, "QoreSerializable::deserialize() h: %p val: '%s' hd: %p '%s' (%p %d)\n", h, val->getFullTypeName(), h->getHashDecl(), h->getHashDecl() ? h->getHashDecl()->getName() : "n/a", hashdeclSerializationInfo, h->getHashDecl() == hashdeclSerializationInfo);
    if (!h || !hashdeclSerializationInfo->equal(h->getHashDecl())) {
        xsink->raiseException("DESERIALIZATION-ERROR", "expecting a SerializationInfo hash from the serialization stream; got type '%s' instead", val->getFullTypeName());
        return QoreValue();
    }

    return deserialize(*h, xsink);
}

QoreValue QoreSerializable::deserializeValueFromStream(StreamReader& reader, ExceptionSink* xsink) {
    // read data type code
    int64 code = reader.readi1(xsink);
    if (*xsink) {
        return -1;
    }

    switch (code) {
        case QSSDT_HASH:
        case QSSDT_HASHDECL:
            return deserializeHashFromStream(reader, code, xsink);

        case QSSDT_STRING:
        case QSSDT_UTF8_STRING:
            return deserializeStringFromStream(reader, code, xsink);

        case QSSDT_INT1:
        case QSSDT_INT2:
        case QSSDT_INT4:
        case QSSDT_INT8:
            return deserializeIntFromStream(reader, code, xsink);

        case QSSDT_BOOLEAN:
            return deserializeBoolFromStream(reader, xsink);

        case QSSDT_LIST:
            return deserializeListFromStream(reader, xsink);

        case QSSDT_FLOAT:
            return deserializeFloatFromStream(reader, xsink);

        case QSSDT_NUMBER:
            return deserializeNumberFromStream(reader, xsink);

        case QSSDT_ABSDATE:
            return deserializeAbsDateFromStream(reader, xsink);

        case QSSDT_RELDATE:
            return deserializeRelDateFromStream(reader, xsink);

        default:
            break;
    }

    xsink->raiseException("DESERIALIZATION-ERROR", "invalid serialization type code " QLLD, code);
    return QoreValue();
}

int64 QoreSerializable::readIntFromStream(StreamReader& reader, const char* type, ExceptionSink* xsink) {
    // read int code
    int64 code = reader.readi1(xsink);
    if (*xsink) {
        return 0;
    }

    if (!CODE_IS_INT(code)) {
        xsink->raiseException("DESERIALIZATION-ERROR", "expecting integer code for the %s value; got invalid code %d instead", type, code);
        return 0;
    }

    int64 size = deserializeIntFromStream(reader, code, xsink);
    if (*xsink) {
        return 0;
    }

    if (size < 0) {
        xsink->raiseException("DESERIALIZATION-ERROR", "stream data gives an invalid %s value (" QLLD ")", size);
        return 0;
    }

    return size;
}

QoreHashNode* QoreSerializable::deserializeHashFromStream(StreamReader& reader, int64 code, ExceptionSink* xsink) {
    const TypedHashDecl* thd;
    int64 size;
    if (code == QSSDT_HASHDECL) {
        // read hashdecl path string
        QoreString path_str;
        if (readStringFromStream(reader, path_str, "hashdecl tag", xsink)) {
            return nullptr;
        }

        const QoreNamespace* pns = nullptr;
        thd = getProgram()->findHashDecl(path_str.c_str(), pns);
        if (!thd) {
            xsink->raiseException("DESERIALIZATION-ERROR", "stream data indicates that a '%s' typed hash should be deserialized, but no such typed hash (hashdecl) could be found in the current Program object", path_str.c_str());
            return nullptr;
        }
    }
    else {
        assert(code == QSSDT_HASH);
        thd = nullptr;
    }

    // now read in the number of hash keys
    size = readIntFromStream(reader, "hash keys", xsink);
    if (*xsink) {
        return nullptr;
    }

    ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);

    // read in hash keys
    for (int64 i = 0; i < size; ++i) {
        // read key type (must be string)
        code = reader.readi1(xsink);
        if (*xsink) {
            return nullptr;
        }
        if (code != QSSDT_UTF8_STRING && code != QSSDT_STRING) {
            xsink->raiseException("DESERIALIZATION-ERROR", "expecting string type for hash key; got type %d instead", static_cast<int>(code));
            return nullptr;
        }

        SimpleRefHolder<QoreStringNode> key(deserializeStringFromStream(reader, code, xsink));
        if (*xsink) {
            return nullptr;
        }

        // convert key string to QCS_DEFAULT if necessary
        TempEncodingHelper tstr(**key, QCS_DEFAULT, xsink);
        if (*xsink) {
            return nullptr;
        }

        ValueHolder val(deserializeValueFromStream(reader, xsink), xsink);
        if (*xsink) {
            return nullptr;
        }

        h->setKeyValue(tstr->c_str(), val.release(), xsink);
    }

    return thd ? typed_hash_decl_private::get(*thd)->newHash(*h, true, xsink) : h.release();
}

int QoreSerializable::readStringFromStream(StreamReader& reader, QoreString& str, const char* type, ExceptionSink* xsink) {
    // read string size
    int64 size = readIntFromStream(reader, "string size", xsink);
    if (*xsink) {
        return -1;
    }

    // read in string data
    str.reserve(size);
    if (reader.read(xsink, const_cast<char*>(str.c_str()), size) == -1) {
        return -1;
    }
    str.terminate(size);
    return 0;
}

QoreStringNode* QoreSerializable::deserializeStringFromStream(StreamReader& reader, int64 code, ExceptionSink* xsink) {
    SimpleRefHolder<QoreStringNode> str(new QoreStringNode);

    int64 size;
    const QoreEncoding* enc;
    if (code == QSSDT_STRING) {
        // read encoding string
        QoreString enc_str;
        if (readStringFromStream(reader, enc_str, "encoding", xsink)) {
            return nullptr;
        }
        enc = QEM.findCreate(enc_str.c_str());
    }
    else {
        assert(code == QSSDT_UTF8_STRING);
        enc = QCS_UTF8;
    }

    if (readStringFromStream(reader, **str, "data", xsink)) {
        return nullptr;
    }

    str->setEncoding(enc);
    return str.release();
}

int64 QoreSerializable::deserializeIntFromStream(StreamReader& reader, int64 code, ExceptionSink* xsink) {
    int64 i;
    switch (code) {
        case QSSDT_INT1: i = reader.readi1(xsink); break;
        case QSSDT_INT2: i = reader.readi2(xsink); break;
        case QSSDT_INT4: i = reader.readi4(xsink); break;
        case QSSDT_INT8: i = reader.readi8(xsink); break;
        default:
            assert(false);
            break;
    }

    return *xsink ? 0 : i;
}

bool QoreSerializable::deserializeBoolFromStream(StreamReader& reader, ExceptionSink* xsink) {
    return (bool)reader.readi1(xsink);
}

QoreListNode* QoreSerializable::deserializeListFromStream(StreamReader& reader, ExceptionSink* xsink) {
    // read list size from stream
    int64 size = readIntFromStream(reader, "list size", xsink);
    if (*xsink) {
        return nullptr;
    }

    ReferenceHolder<QoreListNode> rv(new QoreListNode(autoTypeInfo), xsink);

    for (int64 i = 0; i < size; ++i) {
        ValueHolder val(deserializeValueFromStream(reader, xsink), xsink);
        if (*xsink) {
            return nullptr;
        }
    }

    return rv.release();
}

double QoreSerializable::deserializeFloatFromStream(StreamReader& reader, ExceptionSink* xsink) {
    int64 i = reader.readi8(xsink);
    double* f = reinterpret_cast<double*>(&i);
    return *f;
}

static unsigned is_prec(const char* str, size_t len) {
    //printd(5, "is_prec str: '%s' len: %d strlen: %d\n", str, len, strlen(str));
    if (*str != '{') {
        return 0;
    }

    const char* p = str + 1;
    while (isdigit(*p)) {
        ++p;
    }
    if (p == (str + 1) || *p != '}' || static_cast<size_t>(p - str + 1) != len) {
        return 0;
    }

    return static_cast<unsigned>(atoi(str + 1));
}

QoreNumberNode* QoreSerializable::deserializeNumberFromStream(StreamReader& reader, ExceptionSink* xsink) {
    QoreString tmp;
    if (readStringFromStream(reader, tmp, "arbitrary-precision number", xsink)) {
        return nullptr;
    }

    const char* val = tmp.c_str();
    bool sign = (*val == '-' || *val == '+');

    // check for @inf@ and @nan@
    if (!strncasecmp(val + sign, "@nan@", 5) || !strncasecmp(val + sign, "@inf@", 5)) {
        if (val[5 + sign] == 'n') {
            if (tmp.size() == static_cast<size_t>(6 + sign)) {
                return new QoreNumberNode(val);
            }
            else {
                unsigned prec = is_prec(val + sign + 6, tmp.size() - sign - 6);
                if (prec) {
                    return new QoreNumberNode(val, prec);
                }
            }
        }
        xsink->raiseException("DESERIALIZATION-ERROR", "invalid number string '%s' read from stream", tmp.c_str());
        return nullptr;
    }

    const char* p = strchr(val, '{');
    if (p) {
        unsigned prec = is_prec(p, tmp.size() - (p - val));
        //printd(5, "%s prec %d\n", val, prec);
        if (prec) {
            return new QoreNumberNode(val, prec);
        }
        else {
            xsink->raiseException("DESERIALIZATION-ERROR", "invalid number string '%s' read from stream", tmp.c_str());
            return nullptr;
        }
    }

    return new QoreNumberNode(val);
}

// absolute dates:
//   utc zones:    int epoch | int us | int1 zt = 0 | int offset
//   region zones: int epoch | int us | int1 zt = 1 | string region
DateTimeNode* QoreSerializable::deserializeAbsDateFromStream(StreamReader& reader, ExceptionSink* xsink) {
    // read in epoch
    int64 epoch = readIntFromStream(reader, "absolute date epoch", xsink);
    if (*xsink) {
        return nullptr;
    }

    int64 us = readIntFromStream(reader, "absolute date microseconds", xsink);
    if (*xsink) {
        return nullptr;
    }

    // read key type (must be string)
    int64 zt = reader.peek(xsink);
    if (*xsink) {
        return nullptr;
    }

    if (!CODE_IS_INT(zt) && zt != QSSDT_UTF8_STRING) {
        xsink->raiseException("DESERIALIZATION-ERROR", "invalid absolute date zone type " QLLD " read from stream; expecting an integer or UTF-8 string type", zt);
        return nullptr;
    }

    const AbstractQoreZoneInfo* zone;
    if (CODE_IS_INT(zt)) {
        // read UTC offset
        int64 seconds_east = readIntFromStream(reader, "absolute date UTC offset", xsink);
        if (*xsink) {
            return nullptr;
        }

        zone = QTZM.findCreateOffsetZone(seconds_east);
    }
    else {
        // read data type code
        int64 code = reader.readi1(xsink);
        if (*xsink) {
            return nullptr;
        }
        assert(code = QSSDT_UTF8_STRING);

        QoreString region;
        if (readStringFromStream(reader, region, "absolute date region", xsink)) {
            return nullptr;
        }

        bool is_path = (!region.empty() && region.c_str()[0] == '.') || q_absolute_path(region.c_str());
        if (is_path && runtime_check_parse_option(PO_NO_FILESYSTEM)) {
            xsink->raiseException("ILLEGAL-FILESYSTEM-ACCESS", "cannot create a TimeZone object from absolute path '%s' when sandboxing restriction PO_NO_FILESYSTEM is set", region.c_str());
        }

        zone = is_path
            ? QTZM.findLoadRegionFromPath(region.c_str(), xsink)
            : QTZM.findLoadRegion(region.c_str(), xsink);

        if (*xsink) {
            return nullptr;
        }
    }

    return DateTimeNode::makeAbsolute(zone, epoch, us);
}

// relative dates:
//   int size "P..."
DateTimeNode* QoreSerializable::deserializeRelDateFromStream(StreamReader& reader, ExceptionSink* xsink) {
    QoreString str;
    if (readStringFromStream(reader, str, "relative date", xsink)) {
        return nullptr;
    }

    return new DateTimeNode(str.c_str());
}
