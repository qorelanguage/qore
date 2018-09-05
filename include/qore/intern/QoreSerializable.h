/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreSerializable.h

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

#ifndef _QORE_CLASS_INTERN_QORESERIALIZABLE_H

#define _QORE_CLASS_INTERN_QORESERIALIZABLE_H

#include <qore/Qore.h>

#include <map>
#include <string>

// maps from object hashes to index strings
typedef std::map<std::string, std::string> imap_t;

// maps from index strings to objects
typedef std::map<std::string, QoreObject*> oimap_t;

class ObjectIndexMap : public oimap_t {
public:
    ObjectIndexMap(ExceptionSink* xs) : xs(xs) {
    }

    ~ObjectIndexMap() {
        for (auto& i : *this) {
            i.second->deref(xs);
        }
    }

protected:
    ExceptionSink* xs;
};

class QoreSerializable : public AbstractPrivateData {
public:
    DLLLOCAL static QoreHashNode* serializeToData(QoreValue val, ExceptionSink* xsink);

    DLLLOCAL static void serialize(QoreObject* self, OutputStream* stream, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserialize(InputStream* stream, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserialize(const QoreHashNode* h, ExceptionSink* xsink);

protected:
    DLLLOCAL virtual ~QoreSerializable() {}

    DLLLOCAL static QoreValue serializeValue(const QoreValue val, ReferenceHolder<QoreHashNode>& index, imap_t& imap, ExceptionSink* xsink);

    DLLLOCAL static QoreHashNode* serializeObjectToData(const QoreObject& self, ReferenceHolder<QoreHashNode>& index, imap_t& imap, imap_t::iterator hint, ExceptionSink* xsink);
    DLLLOCAL static QoreHashNode* serializeHashToData(const QoreHashNode& h, ReferenceHolder<QoreHashNode>& index, imap_t& imap, ExceptionSink* xsink);
    DLLLOCAL static QoreListNode* serializeListToData(const QoreListNode& l, ReferenceHolder<QoreHashNode>& index, imap_t& imap, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserializeData(const QoreValue val, const oimap_t& oimap, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserializeHashData(const QoreStringNode* type, const QoreHashNode* h, const oimap_t& oimap, ExceptionSink* xsink);

    DLLLOCAL static QoreValue deserializeListData(const QoreListNode* l, const oimap_t& oimap, ExceptionSink* xsink);
};

#endif // _QORE_CLASS_INTERN_QORESERIALIZABLE_H
