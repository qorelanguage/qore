/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreSerializable.h

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

#ifndef _QORE_CLASS_INTERN_QORESERIALIZABLE_H

#define _QORE_CLASS_INTERN_QORESERIALIZABLE_H

#include <qore/Qore.h>
#include "qore/intern/StreamReader.h"
#include "qore/intern/StreamWriter.h"

#include <map>
#include <string>
#include <set>

// maps from index strings to containers
typedef std::map<std::string, QoreValue> oimap_t;

// maps from object hashes to index strings
typedef std::map<std::string, std::string> imap_t;

// set of modules to load
typedef std::set<std::string> mset_t;

namespace {
// qore serialization stream data type constants
enum qore_stream_type : unsigned char {
    HASH = 0,
    HASHDECL = 1,
    STRING = 2,
    UTF8_STRING = 3,
    LIST = 4,
    BOOLEAN_TRUE = 5,
    BOOLEAN_FALSE = 6,
    INT1 = 7,
    INT2 = 8,
    INT4 = 9,
    INT8 = 10,
    FLOAT = 11,
    QORENUMBER = 12,
    QOREBINARY = 13,
    ABSDATE = 14,
    RELDATE = 15,
    SQLNULL = 16,
    NOTHING = 17,
};
}

class ObjectIndexMap : public oimap_t {
public:
    DLLLOCAL ObjectIndexMap(ExceptionSink* xs) : xs(xs) {
    }

    DLLLOCAL ~ObjectIndexMap();

protected:
    ExceptionSink* xs;
};

class QoreSerializable : public AbstractPrivateData {
friend class QoreInternalSerializationContext;
friend class QoreInternalDeserializationContext;

public:
    DLLLOCAL static QoreHashNode* serializeToData(QoreValue val, int64 flags, ExceptionSink* xsink);

    DLLLOCAL static void serialize(const QoreObject& self, OutputStream& stream, int64 flags, ExceptionSink* xsink);

    DLLLOCAL static void serialize(const QoreValue val, OutputStream& stream, int64 flags, ExceptionSink* xsink);

    DLLLOCAL static QoreHashNode* deserializeToData(InputStream& stream, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserialize(InputStream& stream, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserialize(const QoreHashNode& h, ExceptionSink* xsink);

    DLLLOCAL static int serializeValueToStream(const QoreValue val, OutputStream& stream, ExceptionSink* xsink);

    // note: does not serialiaze the stream header; just the value
    DLLLOCAL static int serializeValueToStream(const QoreValue val, StreamWriter& writer, ExceptionSink* xsink);

    // note: reads a raw value; assumes the stream header has already been read
    DLLLOCAL static QoreValue deserializeValueFromStream(StreamReader& reader, ExceptionSink* xsink);

protected:
    DLLLOCAL virtual ~QoreSerializable() {}

    DLLLOCAL static QoreValue serializeObjectToData(const QoreObject& obj, bool weak,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, ExceptionSink* xsink);

    DLLLOCAL static imap_t::iterator serializeObjectToIndex(const QoreObject& obj,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, ExceptionSink* xsink);

    DLLLOCAL static imap_t::iterator serializeObjectToIndexIntern(const QoreObject& self,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, const QoreString& str,
            imap_t::iterator hint, ExceptionSink* xsink);

    DLLLOCAL static QoreValue serializeValue(const QoreValue val, ReferenceHolder<QoreHashNode>& index, int64 flags,
            imap_t& imap, mset_t& mset, ExceptionSink* xsink);

    DLLLOCAL static QoreValue serializeHashToData(const QoreHashNode& h, bool weak,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, ExceptionSink* xsink);

    DLLLOCAL static imap_t::iterator serializeHashToIndex(const QoreHashNode& h,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, ExceptionSink* xsink);

    DLLLOCAL static imap_t::iterator serializeHashToIndexIntern(const QoreHashNode& h,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, const QoreString& str,
            imap_t::iterator hint, ExceptionSink* xsink);

    DLLLOCAL static QoreValue serializeListToData(const QoreListNode& l, bool weak,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, ExceptionSink* xsink);

    DLLLOCAL static imap_t::iterator serializeListToIndex(const QoreListNode& l,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, ExceptionSink* xsink);

    DLLLOCAL static imap_t::iterator serializeListToIndexIntern(const QoreListNode& h,
            ReferenceHolder<QoreHashNode>& index, int64 flags, imap_t& imap, mset_t& mset, const QoreString& str,
            imap_t::iterator hint, ExceptionSink* xsink);

    DLLLOCAL static int readStringFromStream(StreamReader& reader, QoreString& str, const char* type,
            ExceptionSink* xsink);
    DLLLOCAL static int64 readIntFromStream(ExceptionSink* xsink, StreamReader& reader, const char* type,
            bool can_be_negative = false);

    DLLLOCAL static int serializeHashToStream(const QoreHashNode& h, StreamWriter& writer, ExceptionSink* xsink);
    DLLLOCAL static int serializeStringToStream(const QoreStringNode& str, StreamWriter& writer,
            ExceptionSink* xsink);
    DLLLOCAL static int serializeStringToStream(StreamWriter& writer, const char* key, size_t len,
            const QoreEncoding* enc, ExceptionSink* xsink);
    DLLLOCAL static int serializeIntToStream(int64 i, StreamWriter& writer, ExceptionSink* xsink);
    DLLLOCAL static int serializeBoolToStream(bool b, StreamWriter& writer, ExceptionSink* xsink);
    DLLLOCAL static int serializeListToStream(const QoreListNode& l, StreamWriter& writer, ExceptionSink* xsink);
    DLLLOCAL static int serializeFloatToStream(double f, StreamWriter& writer, ExceptionSink* xsink);
    DLLLOCAL static int serializeNumberToStream(const QoreNumberNode& n, StreamWriter& writer, ExceptionSink* xsink);
    DLLLOCAL static int serializeDateToStream(const DateTimeNode& n, StreamWriter& writer, ExceptionSink* xsink);
    DLLLOCAL static int serializeBinaryToStream(const BinaryNode& n, StreamWriter& writer, ExceptionSink* xsink);

    DLLLOCAL static void serializeToStream(const QoreHashNode& h, OutputStream& stream, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserializeData(const QoreValue val, const oimap_t& oimap, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserializeHashData(const QoreStringNode& type, const QoreHashNode& h,
            const oimap_t& oimap, ExceptionSink* xsink, QoreHashNode* rv = nullptr);

    DLLLOCAL static QoreValue deserializeListData(QoreValue v, const QoreHashNode& h, const oimap_t& oimap,
            ExceptionSink* xsink, QoreListNode* rv = nullptr);

    DLLLOCAL static QoreValue deserializeListData(const QoreListNode& l, const oimap_t& oimap, ExceptionSink* xsink,
            QoreListNode* rv = nullptr);

    DLLLOCAL static QoreValue deserializeIndexedContainer(const char* key, const oimap_t& oimap,
            ExceptionSink* xsink);
    DLLLOCAL static QoreValue deserializeIndexedWeakReference(const char* key, const oimap_t& oimap,
            ExceptionSink* xsink);

    DLLLOCAL static QoreHashNode* deserializeHashFromStream(StreamReader& reader, qore_stream_type code,
            ExceptionSink* xsink);

    DLLLOCAL static QoreStringNode* deserializeStringFromStream(StreamReader& reader, qore_stream_type code,
            ExceptionSink* xsink);

    DLLLOCAL static int64 deserializeIntFromStream(StreamReader& reader, qore_stream_type code, ExceptionSink* xsink);

    DLLLOCAL static QoreListNode* deserializeListFromStream(StreamReader& reader, ExceptionSink* xsink);

    DLLLOCAL static double deserializeFloatFromStream(StreamReader& reader, ExceptionSink* xsink);

    DLLLOCAL static QoreNumberNode* deserializeNumberFromStream(StreamReader& reader, ExceptionSink* xsink);

    DLLLOCAL static DateTimeNode* deserializeAbsDateFromStream(StreamReader& reader, ExceptionSink* xsink);

    DLLLOCAL static DateTimeNode* deserializeRelDateFromStream(StreamReader& reader, ExceptionSink* xsink);

    DLLLOCAL static BinaryNode* deserializeBinaryFromStream(StreamReader& reader, ExceptionSink* xsink);
};

#endif // _QORE_CLASS_INTERN_QORESERIALIZABLE_H
