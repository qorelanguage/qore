/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreTypeInfo.h

    Qore Programming Language

    Copyright (C) 2003 - 2020 Qore Technologies, s.r.o.

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

#ifndef _QORE_QORETYPEINFO_H

#define _QORE_QORETYPEINFO_H

#include <map>
#include <vector>
#include <utility>
#include <functional>

#define NO_TYPE_INFO "any"

// forward references
class LValueHelper;

DLLLOCAL int64 parse_get_parse_options();

// adds external types to global type map
DLLLOCAL void add_to_type_map(qore_type_t t, const QoreTypeInfo* typeInfo);

enum q_typespec_t : unsigned char {
   QTS_TYPE = 0,
   QTS_CLASS = 1,
   QTS_HASHDECL = 2,
   QTS_COMPLEXHASH = 3,
   QTS_COMPLEXLIST = 4,
   QTS_COMPLEXREF = 5,
   QTS_COMPLEXSOFTLIST = 6,
   QTS_EMPTYLIST = 7,
   QTS_EMPTYHASH = 8,
};

class QoreTypeInfo;
typedef std::function<void (QoreValue&, ExceptionSink*)> q_type_map_t;

static q_type_map_t null_to_nothing = [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); };

// returns type info for base types
DLLLOCAL const QoreTypeInfo* getTypeInfoForType(qore_type_t t);
// returns type info information for parse types (values)
DLLLOCAL const QoreTypeInfo* getTypeInfoForValue(const AbstractQoreNode* n);
// returns an "or nothing" type for the given non-or-nothing type or nullptr if not possible
DLLLOCAL const QoreTypeInfo* get_or_nothing_type(const QoreTypeInfo* typeInfo);

class QoreTypeSpec {
public:
    DLLLOCAL QoreTypeSpec(qore_type_t t) : typespec(QTS_TYPE) {
        u.t = t;
    }

    DLLLOCAL QoreTypeSpec(const QoreClass* qc) : typespec(QTS_CLASS) {
        u.qc = qc;
    }

    DLLLOCAL QoreTypeSpec(const TypedHashDecl* hd) : typespec(QTS_HASHDECL) {
        u.hd = hd;
    }

    DLLLOCAL q_typespec_t getTypeSpec() const {
        return typespec;
    }

    DLLLOCAL const char* getName() const;

    DLLLOCAL const char* getSimpleName() const;

    DLLLOCAL qore_type_t getType() const {
        switch (typespec) {
            case QTS_TYPE:
            case QTS_EMPTYLIST:
            case QTS_EMPTYHASH:
                return u.t;
            case QTS_CLASS:
                return NT_OBJECT;
            case QTS_COMPLEXHASH:
            case QTS_HASHDECL:
                return NT_HASH;
            case QTS_COMPLEXLIST:
            case QTS_COMPLEXSOFTLIST:
                return NT_LIST;
            case QTS_COMPLEXREF:
                return NT_REFERENCE;
        }
        assert(false);
        return NT_NOTHING;
    }

    DLLLOCAL const QoreClass* getClass() const {
        return typespec == QTS_CLASS ? u.qc : nullptr;
    }

    DLLLOCAL const TypedHashDecl* getHashDecl() const {
        return typespec == QTS_HASHDECL ? u.hd : nullptr;
    }

    DLLLOCAL const QoreTypeInfo* getComplexHash() const {
        return typespec == QTS_COMPLEXHASH ? u.ti : nullptr;
    }

    DLLLOCAL const QoreTypeInfo* getComplexList() const {
        return typespec == QTS_COMPLEXLIST || typespec == QTS_COMPLEXSOFTLIST ? u.ti : nullptr;
    }

    DLLLOCAL const QoreTypeInfo* getComplexSoftList() const {
        return typespec == QTS_COMPLEXSOFTLIST ? u.ti : nullptr;
    }

    DLLLOCAL const QoreTypeInfo* getComplexReference() const {
        return typespec == QTS_COMPLEXREF ? u.ti : nullptr;
    }

    //! returns the element type, if any (nullptr if not applicable)
    DLLLOCAL const QoreTypeInfo* getElementType() const {
        switch (typespec) {
            case QTS_COMPLEXHASH:
            case QTS_COMPLEXLIST:
            case QTS_COMPLEXSOFTLIST:
                return u.ti;
            default:
                break;
        }
        return nullptr;
    }

    // returns true if the type has auto in it somewhere (even for complex types)
    DLLLOCAL bool isAutoType() const;

    DLLLOCAL bool isComplex() const {
        return typespec == QTS_HASHDECL
                || typespec == QTS_COMPLEXHASH
                || typespec == QTS_COMPLEXLIST
                || typespec == QTS_COMPLEXSOFTLIST
                || typespec == QTS_COMPLEXREF;
    }

    DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
        switch (typespec) {
            case QTS_HASHDECL:
                return u.hd->getTypeInfo();

            case QTS_COMPLEXLIST:
            case QTS_COMPLEXSOFTLIST:
                return qore_get_complex_list_type(u.ti);

            case QTS_COMPLEXHASH:
                return qore_get_complex_hash_type(u.ti);

            case QTS_COMPLEXREF:
                return qore_get_complex_reference_type(u.ti);

            case QTS_CLASS:
                return u.qc->getTypeInfo();

            case QTS_TYPE:
                return getTypeInfoForType(u.t);

            case QTS_EMPTYLIST:
                return emptyListTypeInfo;

            case QTS_EMPTYHASH:
                return emptyHashTypeInfo;
        }
        assert(false);
        return nullptr;
    }

    DLLLOCAL const QoreTypeInfo* getBaseTypeInfo() const {
        switch (typespec) {
            case QTS_HASHDECL:
            case QTS_COMPLEXHASH:
                return hashTypeInfo;
            case QTS_COMPLEXLIST:
            case QTS_COMPLEXSOFTLIST:
                return listTypeInfo;
            case QTS_CLASS:
                return objectTypeInfo;
            case QTS_COMPLEXREF:
                return referenceTypeInfo;
            case QTS_TYPE:
                return getTypeInfoForType(u.t);
            case QTS_EMPTYLIST:
                return emptyListTypeInfo;
            case QTS_EMPTYHASH:
                return emptyHashTypeInfo;
        }
        assert(false);
        return nullptr;
    }

    DLLLOCAL qore_type_result_e matchType(qore_type_t t) const {
        if (typespec == QTS_CLASS)
            return t == NT_OBJECT ? QTI_IDENT : QTI_NOT_EQUAL;
        else if (typespec == QTS_HASHDECL || typespec == QTS_COMPLEXHASH)
            return t == NT_HASH ? QTI_IDENT : QTI_NOT_EQUAL;
        else if (typespec == QTS_COMPLEXLIST || typespec == QTS_COMPLEXSOFTLIST)
            return t == NT_LIST ? QTI_IDENT : QTI_NOT_EQUAL;
        else if (typespec == QTS_COMPLEXREF)
            return t == NT_REFERENCE ? QTI_IDENT : QTI_NOT_EQUAL;
        if (u.t == NT_ALL)
            return QTI_WILDCARD;
        return u.t == t ? QTI_IDENT : QTI_NOT_EQUAL;
    }

    // this is the "expecting" type, t is the type to match
    // ex: this = class, t = NT_OBJECT, result = AMBIGU`OUS
    // ex: this = NT_OBJECT, t = class, result = IDENT
    DLLLOCAL qore_type_result_e match(const QoreTypeSpec& t) const {
        bool may_not_match = false;
        bool may_need_filter = false;
        return match(t, may_not_match, may_need_filter);
    }

    // this is the "expecting" type, t is the type to match
    // ex: this = class, t = NT_OBJECT, result = AMBIGU`OUS
    // ex: this = NT_OBJECT, t = class, result = IDENT
    DLLLOCAL qore_type_result_e match(const QoreTypeSpec& t, bool& may_not_match) const {
        bool may_need_filter = false;
        return match(t, may_not_match, may_need_filter);
    }

    // this is the "expecting" type, t is the type to match
    // ex: this = class, t = NT_OBJECT, result = AMBIGU`OUS
    // ex: this = NT_OBJECT, t = class, result = IDENT
    DLLLOCAL qore_type_result_e match(const QoreTypeSpec& t, bool& may_not_match, bool& may_need_filter) const;

    DLLLOCAL qore_type_result_e runtimeAcceptsValue(const QoreValue& n, bool exact) const;

    // returns true if there is a match or if an error has been raised
    DLLLOCAL bool acceptInput(ExceptionSink* xsink, const QoreTypeInfo& typeInfo, q_type_map_t map, const char* arg_type, bool obj, int param_num, const char* param_name, QoreValue& n, LValueHelper* lvhelper = nullptr) const;

    DLLLOCAL bool operator==(const QoreTypeSpec& other) const;
    DLLLOCAL bool operator!=(const QoreTypeSpec& other) const;

protected:
    DLLLOCAL QoreTypeSpec(const QoreTypeInfo* ti, q_typespec_t t) : typespec(t) {
        u.ti = ti;
    }

    DLLLOCAL QoreTypeSpec(qore_type_t t, q_typespec_t ts) : typespec(ts) {
        u.t = t;
    }

private:
    union {
        qore_type_t t;
        const QoreClass* qc;
        const TypedHashDecl* hd;
        const QoreTypeInfo* ti;
    } u;
    q_typespec_t typespec;
};

class QoreComplexHashTypeSpec : public QoreTypeSpec {
public:
   DLLLOCAL QoreComplexHashTypeSpec(const QoreTypeInfo* ti) : QoreTypeSpec(ti, QTS_COMPLEXHASH) {
   }
};

class QoreComplexListTypeSpec : public QoreTypeSpec {
public:
   DLLLOCAL QoreComplexListTypeSpec(const QoreTypeInfo* ti) : QoreTypeSpec(ti, QTS_COMPLEXLIST) {
   }
};

class QoreComplexSoftListTypeSpec : public QoreTypeSpec {
public:
   DLLLOCAL QoreComplexSoftListTypeSpec(const QoreTypeInfo* ti) : QoreTypeSpec(ti, QTS_COMPLEXSOFTLIST) {
   }
};

class QoreComplexReferenceTypeSpec : public QoreTypeSpec {
public:
   DLLLOCAL QoreComplexReferenceTypeSpec(const QoreTypeInfo* ti) : QoreTypeSpec(ti, QTS_COMPLEXREF) {
   }
};

class QoreEmptyListTypeSpec : public QoreTypeSpec {
public:
   DLLLOCAL QoreEmptyListTypeSpec() : QoreTypeSpec(NT_LIST, QTS_EMPTYLIST) {
   }
};

class QoreEmptyHashTypeSpec : public QoreTypeSpec {
public:
   DLLLOCAL QoreEmptyHashTypeSpec() : QoreTypeSpec(NT_HASH, QTS_EMPTYHASH) {
   }
};

struct QoreReturnSpec {
   const QoreTypeSpec spec;
   bool exact = false;

   DLLLOCAL QoreReturnSpec(const QoreTypeSpec&& spec, bool exact = false) : spec(spec), exact(exact) {
   }
};

typedef std::vector<QoreReturnSpec> q_return_vec_t;

struct QoreAcceptSpec {
   const QoreTypeSpec spec;
   const q_type_map_t map;
   bool exact = false;

   DLLLOCAL QoreAcceptSpec(const QoreTypeSpec&& spec, const q_type_map_t&& map, bool exact = false) : spec(spec), map(map), exact(exact) {
   }
};
typedef std::vector<QoreAcceptSpec> q_accept_vec_t;

template <typename T>
DLLLOCAL bool typespec_vec_compare(const T& a, const T& b);

DLLLOCAL bool accept_vec_compare(const q_accept_vec_t& a, const q_accept_vec_t& b);
DLLLOCAL bool return_vec_compare(const q_return_vec_t& a, const q_return_vec_t& b);

class QoreTypeInfo {
protected:
    QoreString tname;

    DLLLOCAL QoreTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec, const QoreString& tname)
        : tname(tname), accept_vec(a_vec), return_vec(r_vec) {
    }

public:
    const q_accept_vec_t accept_vec;
    const q_return_vec_t return_vec;

    DLLLOCAL QoreTypeInfo(const char* name, const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec) : tname(name), accept_vec(a_vec), return_vec(r_vec) {
    }

    DLLLOCAL virtual ~QoreTypeInfo() = default;

    // static version of method, checking for null pointer
    DLLLOCAL static QoreHashNode* getAcceptTypes(const QoreTypeInfo* ti) {
        ReferenceHolder<QoreHashNode> rv(new QoreHashNode(boolTypeInfo), nullptr);
        if (ti && hasType(ti)) {
            ti->getAcceptTypes(rv);
        } else {
            rv->setKeyValue("any", true, nullptr);
        }
        return rv.release();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static QoreHashNode* getReturnTypes(const QoreTypeInfo* ti) {
        ReferenceHolder<QoreHashNode> rv(new QoreHashNode(boolTypeInfo), nullptr);
        if (ti && hasType(ti)) {
            ti->getReturnTypes(rv);
        } else {
            rv->setKeyValue("any", true, nullptr);
        }
        return rv.release();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_t getSingleType(const QoreTypeInfo* ti) {
        return ti && hasType(ti) ? ti->getSingleType() : NT_ALL;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_t getBaseType(const QoreTypeInfo* ti) {
        return ti && hasType(ti) ? ti->getBaseType() : NT_ALL;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool parseAcceptsReturns(const QoreTypeInfo* ti, qore_type_t t) {
        return ti && hasType(ti) ? ti->parseAcceptsReturns(t) : true;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_result_e parseReturns(const QoreTypeInfo* ti, QoreTypeSpec t) {
        return ti && hasType(ti) ? ti->parseReturns(t) : QTI_WILDCARD;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool isReference(const QoreTypeInfo* ti) {
        return ti && ti->return_vec[0].spec.getType() == NT_REFERENCE;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getReferenceTarget(const QoreTypeInfo* ti) {
        if (isReference(ti)) {
            const QoreTypeInfo* rv = ti->return_vec[0].spec.getComplexReference();
            return rv ? rv : anyTypeInfo;
        }
        return nullptr;
    }

    // static version of method, checking for null pointer
    // returns true if this type only returns the type given
    DLLLOCAL static bool isType(const QoreTypeInfo* ti, qore_type_t t) {
        return ti ? ti->isType(t) : false;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_result_e runtimeAcceptsValue(const QoreTypeInfo* ti, const QoreValue n) {
        return ti && hasType(ti) ? ti->runtimeAcceptsValue(n) : QTI_WILDCARD;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_result_e parseAccepts(const QoreTypeInfo* first, const QoreTypeInfo* second) {
        bool may_not_match = false;
        bool may_need_filter = false;
        return parseAccepts(first, second, may_not_match, may_need_filter);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_result_e parseAccepts(const QoreTypeInfo* first, const QoreTypeInfo* second, bool& may_not_match) {
        bool may_need_filter = false;
        return parseAccepts(first, second, may_not_match, may_need_filter);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_result_e parseAccepts(const QoreTypeInfo* first, const QoreTypeInfo* second, bool& may_not_match, bool& may_need_filter) {
        if (first == autoTypeInfo) {
            return QTI_WILDCARD;
        }
        if (!hasType(first)) {
            if (!may_need_filter && isComplex(second))
                may_need_filter = true;
            return QTI_WILDCARD;
        }
        if (!hasType(second)) {
            if (!may_need_filter) {
                // check if we could need a runtime filter
                for (auto& i : first->accept_vec) {
                    if (i.map) {
                        may_need_filter = true;
                        break;
                    }
                }
            }
            // if the type may not match at runtime, then return no match with %strict-types
            if (parse_get_parse_options() & PO_STRICT_TYPES) {
                return QTI_NOT_EQUAL;
            }
            may_not_match = true;
            return QTI_AMBIGUOUS;
        }
        if (first == second)
            return QTI_IDENT;
        return first->parseAccepts(second, may_not_match, may_need_filter);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_result_e runtimeTypeMatch(const QoreTypeInfo* first, const QoreTypeInfo* second) {
        //printd(5, "QoreTypeInfo::runtimeTypeMatch() %p '%s' (ht: %d) <=> %p '%s' (ht: %d)\n", first, QoreTypeInfo::getName(first), QoreTypeInfo::hasType(first), second, QoreTypeInfo::getName(second), QoreTypeInfo::hasType(second));
        if (first == second) {
            return QTI_IDENT;
        }
        if (!hasType(second)) {
            return !hasType(first) ? QTI_NEAR : QTI_AMBIGUOUS;
        }
        return QoreTypeInfo::hasType(first) ? first->runtimeTypeMatch(second) : QTI_AMBIGUOUS;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static qore_type_t getSingleReturnType(const QoreTypeInfo* ti) {
        if (!hasType(ti) || ti->return_vec.size() > 1)
            return NT_ALL;
        return ti->return_vec[0].spec.getType();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool returnsSingle(const QoreTypeInfo* ti) {
        return ti && hasType(ti) ? ti->return_vec.size() == 1 : false;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool acceptsSingle(const QoreTypeInfo* ti) {
        return ti && hasType(ti) ? ti->accept_vec.size() == 1 : false;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreClass* getUniqueReturnClass(const QoreTypeInfo* ti) {
        if (!ti || ti->return_vec.size() > 1 || !hasType(ti))
            return nullptr;
        return ti->return_vec[0].spec.getClass();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const TypedHashDecl* getUniqueReturnHashDecl(const QoreTypeInfo* ti) {
        if (!ti || ti->return_vec.size() > 1 || !hasType(ti))
            return nullptr;
        return ti->return_vec[0].spec.getHashDecl();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getUniqueReturnComplexHash(const QoreTypeInfo* ti) {
        if (!ti || ti->return_vec.size() > 1 || !hasType(ti))
            return nullptr;
        return ti == autoHashTypeInfo ? autoTypeInfo : ti->return_vec[0].spec.getComplexHash();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getUniqueReturnComplexList(const QoreTypeInfo* ti) {
        if (!ti || ti->return_vec.size() > 1 || !hasType(ti))
            return nullptr;
        return ti == autoListTypeInfo || ti == softAutoListTypeInfo
            ? autoTypeInfo
            : ti->return_vec[0].spec.getComplexList();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getUniqueReturnComplexSoftList(const QoreTypeInfo* ti) {
        if (!ti || ti->return_vec.size() > 1 || !hasType(ti))
            return nullptr;
        return ti == softAutoListTypeInfo ? autoTypeInfo : ti->return_vec[0].spec.getComplexSoftList();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getUniqueReturnComplexReference(const QoreTypeInfo* ti) {
        if (!ti || ti->return_vec.size() > 1 || !hasType(ti))
            return nullptr;
        return ti->return_vec[0].spec.getComplexReference();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getReturnComplexHashOrNothing(const QoreTypeInfo* ti) {
        if (!ti || !hasType(ti)) {
            return nullptr;
        }
        if (ti->return_vec.size() > 1) {
            if (ti->return_vec.size() != 2 || (ti->return_vec[1].spec.match(NT_NOTHING) != QTI_IDENT))
                return nullptr;
        }
        return ti == autoHashTypeInfo || ti == autoHashOrNothingTypeInfo
            ? autoTypeInfo
            : ti->return_vec[0].spec.getComplexHash();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getReturnComplexListOrNothing(const QoreTypeInfo* ti) {
        if (!ti || !hasType(ti))
            return nullptr;
        if (ti->return_vec.size() > 1) {
            if (ti->return_vec.size() != 2 || (ti->return_vec[1].spec.match(NT_NOTHING) != QTI_IDENT))
                return nullptr;
        }
        return ti == autoListTypeInfo
            || ti == autoListOrNothingTypeInfo
            || ti == softAutoListTypeInfo
            || ti == softAutoListOrNothingTypeInfo
            ? autoTypeInfo
            : ti->return_vec[0].spec.getComplexList();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool hasComplexType(const QoreTypeInfo* ti) {
        if (!ti) {
            return false;
        }
        return ti == autoTypeInfo || isComplex(ti);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool hasType(const QoreTypeInfo* ti) {
        return !ti || ti->accept_vec[0].spec.getType() == NT_ALL ? false : true;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool needsScan(const QoreTypeInfo* ti) {
        return ti && hasType(ti) ? ti->needsScanImpl() : true;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const char* getName(const QoreTypeInfo* ti) {
        return ti ? ti->tname.c_str() : NO_TYPE_INFO;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void getThisType(const QoreTypeInfo* ti, QoreString& str) {
        if (ti)
            ti->getThisTypeImpl(str);
        else
            str.sprintf("no value");
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void acceptInputParam(const QoreTypeInfo* ti, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) {
        if (hasType(ti)) {
            ti->acceptInputIntern(xsink, "parameter", false, param_num, param_name, n);
        } else if (ti != autoTypeInfo) {
            stripTypeInfo(n, xsink);
        }
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void acceptInputMember(const QoreTypeInfo* ti, const char* member_name, QoreValue& n, ExceptionSink* xsink) {
        if (hasType(ti)) {
            ti->acceptInputIntern(xsink, "member", true, -1, member_name, n);
        } else if (ti != autoTypeInfo) {
            stripTypeInfo(n, xsink);
        }
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void acceptInputKey(const QoreTypeInfo* ti, const char* member_name, QoreValue& n, ExceptionSink* xsink) {
        if (hasType(ti)) {
            ti->acceptInputIntern(xsink, "key", false, -1, member_name, n);
        } else if (ti != autoTypeInfo) {
            stripTypeInfo(n, xsink);
        }
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void acceptAssignment(const QoreTypeInfo* ti, const char* text, QoreValue& n, ExceptionSink* xsink, LValueHelper* lvhelper = nullptr) {
        assert(text && text[0] == '<');
        if (hasType(ti)) {
            ti->acceptInputIntern(xsink, "lvalue", false, -1, text, n);
        } else if (ti != autoTypeInfo) {
            stripTypeInfo(n, xsink, lvhelper);
        }
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool hasDefaultValue(const QoreTypeInfo* ti) {
        return ti ? ti->hasDefaultValueImpl() : false;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static QoreValue getDefaultQoreValue(const QoreTypeInfo* ti) {
        return ti ? ti->getDefaultQoreValueImpl() : QoreValue();
    }

    // returns true if the type has auto in it somewhere (even for complex types)
    DLLLOCAL static bool isAutoType(const QoreTypeInfo* ti) {
        if (!ti) {
            return false;
        }
        if (ti == autoTypeInfo) {
            return true;
        }
        return ti->isAutoType();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool mayRequireFilter(const QoreTypeInfo* ti, const QoreValue& n) {
        if (!hasType(ti) && ti != autoTypeInfo) {
            return isComplex(n.getTypeInfo());
        }
        assert(ti);
        return ti->mayRequireFilter(n);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool equal(const QoreTypeInfo* a, const QoreTypeInfo* b) {
        if (a == b)
            return true;
        bool hta = hasType(a);
        bool htb = hasType(b);
        if (hta && htb)
            return accept_vec_compare(a->accept_vec, b->accept_vec) && return_vec_compare(a->return_vec, b->return_vec);
        return hta || htb ? false : true;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool isInputIdentical(const QoreTypeInfo* a, const QoreTypeInfo* b) {
        if (a == b)
            return true;
        bool hta = hasType(a);
        bool htb = hasType(b);
        if (hta && htb)
            return accept_vec_compare(a->accept_vec, b->accept_vec);
        return hta || htb ? false : true;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool isOutputIdentical(const QoreTypeInfo* a, const QoreTypeInfo* b) {
        if (a == b)
            return true;
        bool hta = hasType(a);
        bool htb = hasType(b);
        if (hta && htb)
            return return_vec_compare(a->return_vec, b->return_vec);
        return hta || htb ? false : true;
    }

    // if second's return type is compatible with first's return type
    // static version of method, checking for null pointer
    DLLLOCAL static bool isOutputCompatible(const QoreTypeInfo* first, const QoreTypeInfo* second) {
        if (hasType(first) && hasType(second)) {
            return parseAccepts(first, second);
        }
        return true;
    }

    // if first is a superset of second with a strict interpretation that the order of accept and return declarations must also be the same
    // static version of method, checking for null pointer
    DLLLOCAL static bool superSetOf(const QoreTypeInfo* first, const QoreTypeInfo* second) {
        if (hasType(first)) {
            if (hasType(second))
                return first->superSetOf(second);
            return false;
        }
        return hasType(second);
    }

    // returns true if first's output is a superset of second's output with a strict interpretation that the order of return declarations must also be the same
    DLLLOCAL static bool outputSuperSetOf(const QoreTypeInfo* first, const QoreTypeInfo* second) {
        if (!hasType(first))
            return true;

        if (hasType(second))
            return first == second ? true : first->outputSuperSetOf(second);
        return false;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool canConvertToScalar(const QoreTypeInfo* ti) {
        return ti ? ti->canConvertToScalarImpl() : true;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void checkDoNonNumericWarning(const QoreTypeInfo* ti, const QoreProgramLocation* loc, const char* preface) {
        if (ti && !ti->canConvertToScalarImpl())
            ti->doNonNumericWarning(loc, preface);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void checkDoNonBooleanWarning(const QoreTypeInfo* ti, const QoreProgramLocation* loc, const char* preface) {
        if (ti && !ti->canConvertToScalarImpl())
            ti->doNonBooleanWarning(loc, preface);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void checkDoNonStringWarning(const QoreTypeInfo* ti, const QoreProgramLocation* loc, const char* preface) {
        if (ti && !ti->canConvertToScalarImpl())
            ti->doNonStringWarning(loc, preface);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void concatName(const QoreTypeInfo* ti, std::string& str) {
        if (ti && (hasType(ti) || ti == autoTypeInfo))
            str.append(ti->tname.c_str());
        else
            str.append(NO_TYPE_INFO);
    }

    // check for a common type
    DLLLOCAL static bool matchCommonType(const QoreTypeInfo*& ctype, const QoreTypeInfo* ntype) {
        // issue #3005: if the first element had no type, then there is no common type
        if (!ctype || ctype == anyTypeInfo) {
            ctype = nullptr;
            return false;
        }
        if (ctype == ntype) {
            return true;
        }
        if (!QoreTypeInfo::hasType(ntype)) {
            // issue #2791: when performing type folding, do not set to type "any" but rather use "auto"
            ctype = ntype == anyTypeInfo ? nullptr : ntype;
            return false;
        }

        // ctype |* NOTHING -> *type
        if (!QoreTypeInfo::parseReturns(ctype, NT_NOTHING) && QoreTypeInfo::isType(ntype, NT_NOTHING)) {
            const QoreTypeInfo* ti = get_or_nothing_type(ctype);
            ctype = ti;
            return ctype != autoTypeInfo ? true : false;
        }

        // ctype==NOTHING | type -> *type
        if (QoreTypeInfo::isType(ctype, NT_NOTHING)) {
            ctype = get_or_nothing_type_check(ntype);
            return ctype != autoTypeInfo ? true : false;
        }

        // ctype |* *ctype -> *ctype
        // if the new type is a superset of the existing common type, then use the new type
        if (ntype->superSetOf(ctype)) {
            ctype = ntype;
            return true;
        }

        // try to find a common base type
        // if we're dealing with types that return multiple types, then they are not compatible
        if (ctype->return_vec.size() > 1 || ntype->return_vec.size() > 1) {
            // issue #2791: when performing type folding, do not set to type "any" but rather use "auto"
            ctype = autoTypeInfo;
            return false;
        }

        // see if we have a complex type
        const QoreTypeInfo* bti = ctype->return_vec[0].spec.getBaseTypeInfo();
        if (bti == ntype->return_vec[0].spec.getBaseTypeInfo()) {
            // issue #3429: when performing type folding with complex types, do not use subtype "any" but rather use "auto"
            if (bti == hashTypeInfo) {
                ctype = autoHashTypeInfo;
            } else if (bti == listTypeInfo) {
                ctype = autoListTypeInfo;
            } else if (bti == softListTypeInfo) {
                ctype = softAutoListTypeInfo;
            } else if (bti == hashOrNothingTypeInfo) {
                ctype = autoHashOrNothingTypeInfo;
            } else if (bti == listOrNothingTypeInfo) {
                ctype = autoListOrNothingTypeInfo;
            } else if (bti == softListOrNothingTypeInfo) {
                ctype = softAutoListOrNothingTypeInfo;
            } else {
                ctype = bti;
            }
            return true;
        }
        // issue #2791: when performing type folding, do not set to type "any" but rather use "auto"
        ctype = autoTypeInfo;
        return false;
    }

    //! returns true if ti could return a complex type
    DLLLOCAL static bool isComplex(const QoreTypeInfo* ti) {
        if (!ti)
            return false;
        for (auto& i : ti->return_vec) {
            if (i.spec.isComplex())
                return true;
        }
        return false;
    }

    //! returns true if the type is an explicit list type
    DLLLOCAL static bool isListType(const QoreTypeInfo* ti) {
        if (!hasType(ti)) {
            return false;
        }
        return ti->return_vec[0].spec.getType() == NT_LIST;
    }

    //! returns the element type, if any (nullptr if not applicable)
    DLLLOCAL static const QoreTypeInfo* getElementType(const QoreTypeInfo* ti) {
        if (!hasType(ti)) {
            return nullptr;
        }
        assert(!ti->return_vec.empty());
        if (ti == autoListTypeInfo
            || ti == autoListOrNothingTypeInfo
            || ti == softAutoListTypeInfo
            || ti == softAutoListOrNothingTypeInfo
            || ti == autoHashTypeInfo
            || ti == autoHashOrNothingTypeInfo) {
            return autoTypeInfo;
        }
        return ti->return_vec[0].spec.getElementType();
    }

    //! returns the typed hash ptr, if applicable
    DLLLOCAL static const TypedHashDecl* getTypedHash(const QoreTypeInfo* ti) {
        if (!hasType(ti)) {
            return nullptr;
        }
        assert(!ti->return_vec.empty());
        return ti->return_vec[0].spec.getHashDecl();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreClass* getReturnClass(const QoreTypeInfo* ti) {
        if (!hasType(ti)) {
            return nullptr;
        }
        assert(!ti->return_vec.empty());
        return ti->return_vec[0].spec.getClass();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getComplexHashValueType(const QoreTypeInfo* ti) {
        if (!hasType(ti)) {
            return nullptr;
        }
        assert(!ti->return_vec.empty());
        if (ti == autoHashTypeInfo || ti == autoHashOrNothingTypeInfo) {
            return autoTypeInfo;
        }
        return ti->return_vec[0].spec.getComplexHash();
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* getComplexListValueType(const QoreTypeInfo* ti) {
        if (!hasType(ti)) {
            return nullptr;
        }
        assert(!ti->return_vec.empty());
        if (ti == autoListTypeInfo || ti == autoListOrNothingTypeInfo) {
            return autoTypeInfo;
        }
        return ti->return_vec[0].spec.getComplexList();
    }

    DLLLOCAL void getAcceptTypes(ReferenceHolder<QoreHashNode>& h) const {
        for (auto& i : accept_vec) {
            const char* simple_type_name = i.spec.getSimpleName();
            h->setKeyValue(simple_type_name, true, nullptr);
            if (!strcmp(simple_type_name, "int")) {
                h->setKeyValue("integer", true, nullptr);
            }
        }
    }

    DLLLOCAL void getReturnTypes(ReferenceHolder<QoreHashNode>& h) const {
        for (auto& i : return_vec) {
            const char* simple_type_name = i.spec.getSimpleName();
            h->setKeyValue(simple_type_name, true, nullptr);
            if (!strcmp(simple_type_name, "int")) {
                h->setKeyValue("integer", true, nullptr);
            }
        }
    }

    DLLLOCAL int doAcceptError(bool priv_error, const char* arg_type, bool obj, int param_num, const char* param_name, const QoreValue& n, ExceptionSink* xsink) const {
        if (priv_error) {
            if (obj) {
                doObjectPrivateClassException(param_name, xsink);
            } else {
                doPrivateClassException(arg_type, param_num + 1, param_name, xsink);
            }
        } else {
            if (obj) {
                doObjectHashDeclTypeException(arg_type, param_name, n, xsink);
            } else {
                doTypeException(arg_type, param_num + 1, param_name, n, xsink);
            }
        }
        return -1;
    }

    DLLLOCAL int doTypeException(const char* arg_type, int param_num, const char* param_name, const QoreValue& n, ExceptionSink* xsink) const {
        // xsink may be null in case parse exceptions have been disabled in the QoreProgram object
        // for example if there was a "requires" error
        if (!xsink)
            return -1;

        QoreStringNode* desc = new QoreStringNode;
        QoreTypeInfo::ptext(*desc, arg_type, param_num, param_name);
        desc->sprintf("expects type '%s', but got ", tname.c_str());
        QoreTypeInfo::getNodeType(*desc, n);
        desc->concat(" instead");
        xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
        return -1;
    }

    DLLLOCAL void acceptInputIntern(ExceptionSink* xsink, const char* arg_type, bool obj, int param_num, const char* param_name, QoreValue& n, LValueHelper* lvhelper = nullptr) const {
        for (auto& t : accept_vec) {
            if (t.spec.acceptInput(xsink, *this, t.map, arg_type, obj, param_num, param_name, n, lvhelper)) {
                return;
            }
        }
        doAcceptError(false, arg_type, obj, param_num, param_name, n, xsink);
    }

    DLLLOCAL void doNonNumericWarning(const QoreProgramLocation* loc, const char* preface) const;
    DLLLOCAL void doNonBooleanWarning(const QoreProgramLocation* loc, const char* preface) const;
    DLLLOCAL void doNonStringWarning(const QoreProgramLocation* loc, const char* preface) const;
    DLLLOCAL void doNonStringError(const QoreProgramLocation* loc, const char* preface) const;

protected:
    DLLLOCAL int doObjectPrivateClassException(const char* param_name, ExceptionSink* xsink) const {
        assert(xsink);
        QoreStringNode* desc = new QoreStringNode;
        desc->sprintf("member '%s' expects type '%s', but got an object where this class is privately inherited instead", param_name, tname.c_str());
        xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
        return -1;
    }

    DLLLOCAL int doPrivateClassException(const char* arg_type, int param_num, const char* param_name, ExceptionSink* xsink) const {
        // xsink may be null in case that parse exceptions have been disabled in the QoreProgram object
        // for example if there was a "requires" error
        if (!xsink)
            return -1;

        QoreStringNode* desc = new QoreStringNode;
        QoreTypeInfo::ptext(*desc, arg_type, param_num, param_name);
        desc->sprintf("expects type '%s', but got an object where this class is privately inherited instead", tname.c_str());
        xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
        return -1;
    }

    DLLLOCAL int doObjectHashDeclTypeException(const char* arg_type, const char* param_name, const QoreValue& n, ExceptionSink* xsink) const {
        assert(xsink);
        QoreStringNode* desc = new QoreStringNode;
        desc->sprintf("%s '%s' expects type '%s', but got ", arg_type, param_name, tname.c_str());
        QoreTypeInfo::getNodeType(*desc, n);
        desc->concat(" instead");
        xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
        return -1;
    }

    // returns true if the type has auto in it somewhere (even for complex types)
    DLLLOCAL bool isAutoType() const {
        for (auto& i : accept_vec) {
            if (i.spec.isAutoType()) {
                return true;
            }
        }
        return false;
    }

    // returns true if "this" is a superset of the argument with a strict interpretation that the order of accept and return declarations must also be the same
    DLLLOCAL bool superSetOf(const QoreTypeInfo* t) const {
        if (accept_vec.size() < t->accept_vec.size() || return_vec.size() < t->return_vec.size())
            return false;

        for (unsigned i = 0; i < t->accept_vec.size(); ++i) {
            if (t->accept_vec[i].spec != accept_vec[i].spec)
                return false;
        }

        for (unsigned i = 0; i < t->return_vec.size(); ++i) {
            if (t->return_vec[i].spec != return_vec[i].spec)
                return false;
        }

        return true;
    }

    // returns true if "this"'s output is a superset of the argument's output with a strict interpretation that the order of return declarations must also be the same
    DLLLOCAL bool outputSuperSetOf(const QoreTypeInfo* t) const {
        if (return_vec.size() < t->return_vec.size())
            return false;

        for (unsigned i = 0; i < t->return_vec.size(); ++i) {
            bool may_not_match = false;
            bool may_need_filter = false;
            if (!return_vec[i].spec.match(t->return_vec[i].spec, may_not_match, may_need_filter))
                return false;
            if (may_not_match)
                return false;
        }

        return true;
    }

    DLLLOCAL qore_type_t getSingleType() const {
        if (accept_vec.size() == 1 && return_vec.size() == 1) {
            qore_type_t qt = accept_vec[0].spec.getType();
            if (qt == return_vec[0].spec.getType())
                return qt;
        }
        return NT_ALL;
    }

    DLLLOCAL qore_type_t getBaseType() const {
        assert(!return_vec.empty());
        return return_vec[0].spec.getType();
    }

    DLLLOCAL bool parseAcceptsReturns(qore_type_t t) const {
        bool ok = false;
        for (auto& i : accept_vec) {
            if (i.spec.matchType(t) != QTI_NOT_EQUAL) {
                ok = true;
                break;
            }
        }
        if (!ok)
            return false;
        for (auto& i : return_vec) {
            if (i.spec.matchType(t) != QTI_NOT_EQUAL)
                return true;
        }
        return false;
    }

    DLLLOCAL qore_type_result_e parseReturns(QoreTypeSpec t) const {
        if (return_vec.size() == 1)
            return t.match(return_vec[0].spec);
        for (auto& i : return_vec) {
            qore_type_result_e rv = t.match(i.spec);
            if (rv != QTI_NOT_EQUAL)
                return i.exact ? QTI_IDENT : QTI_AMBIGUOUS;
        }
        return QTI_NOT_EQUAL;
    }

    // returns true if this type only returns the type given
    DLLLOCAL bool isType(qore_type_t t) const {
        if (return_vec.size() > 1)
            return false;
        return t == return_vec[0].spec.getType();
    }

    // returns true if the type matches an accept type with a filter (type only checked)
    DLLLOCAL bool mayRequireFilter(const QoreValue& n) const {
        for (auto& at : accept_vec) {
            if (at.map && at.spec.matchType(n.getType()) != QTI_NOT_EQUAL)
                return true;
        }
        return false;
    }

    DLLLOCAL qore_type_result_e parseAccepts(const QoreTypeInfo* typeInfo, bool& may_not_match, bool& may_need_filter) const {
        //printd(5, "QoreTypeInfo::parseAccepts() '%s' <- '%s'\n", tname.c_str(), typeInfo->tname.c_str());
        if (typeInfo->return_vec.size() > accept_vec.size()) {
            // if the type may not match at runtime, then return no match with %strict-types
            if (parse_get_parse_options() & PO_STRICT_TYPES) {
                return QTI_NOT_EQUAL;
            }
            may_not_match = true;
        }

        bool ok = false;
        for (auto& rt : typeInfo->return_vec) {
            bool t_no_match = true;
            for (auto& at : accept_vec) {
                qore_type_result_e res = parseAcceptsIntern(at, rt, may_not_match, may_need_filter, t_no_match, ok);
                if (res == QTI_IDENT)
                    return res;
                else if (res == QTI_AMBIGUOUS || res == QTI_NEAR || res == QTI_WILDCARD) {
                    assert(ok);
                    if (may_not_match)
                        return res;
                    break;
                }
            }
            if (t_no_match) {
                if (!may_not_match) {
                    // if the type may not match at runtime, then return no match with %strict-types
                    if (parse_get_parse_options() & PO_STRICT_TYPES) {
                        return QTI_NOT_EQUAL;
                    }
                    may_not_match = true;
                    if (ok)
                        return QTI_AMBIGUOUS;
                }
            }
        }
        if (ok)
            return QTI_AMBIGUOUS;
        may_not_match = false;
        return QTI_NOT_EQUAL;
    }

    // checks that types are near identical at runtime
    /** accept and return types match in the same order and with no match worse than QTI_NEAR
    */
    DLLLOCAL qore_type_result_e runtimeTypeMatch(const QoreTypeInfo* typeInfo) const {
        //printd(5, "QoreTypeInfo::runtimeTypeMatch() '%s' <=> '%s'\n", tname.c_str(), typeInfo->tname.c_str());
        if (typeInfo->return_vec.size() != return_vec.size() || typeInfo->accept_vec.size() != accept_vec.size()) {
            return QTI_NOT_EQUAL;
        }

        // check accept types
        qore_type_result_e rc = QTI_IDENT;
        for (size_t i = 0; i < accept_vec.size(); ++i) {
            bool may_not_match = false;
            bool may_need_filter = false;
            qore_type_result_e res = accept_vec[i].spec.match(typeInfo->accept_vec[i].spec, may_not_match, may_need_filter);
            //printd(5, " + accept: %s %d <=> %s %d: %d\n", QoreTypeInfo::getName(accept_vec[i].spec.getBaseTypeInfo()), accept_vec[i].spec.getTypeSpec(), QoreTypeInfo::getName(typeInfo->accept_vec[i].spec.getBaseTypeInfo()), typeInfo->accept_vec[i].spec.getTypeSpec(), res);
            if (res < QTI_NEAR) {
                return QTI_NOT_EQUAL;
            }
            if (res < rc) {
                rc = res;
            }
        }

        // check return types
        for (size_t i = 0; i < return_vec.size(); ++i) {
            bool may_not_match = false;
            bool may_need_filter = false;
            qore_type_result_e res = return_vec[i].spec.match(typeInfo->return_vec[i].spec, may_not_match, may_need_filter);
            //printd(5, " + return: %s %d <=> %s %d: %d\n", QoreTypeInfo::getName(return_vec[i].spec.getBaseTypeInfo()), return_vec[i].spec.getTypeSpec(), QoreTypeInfo::getName(typeInfo->return_vec[i].spec.getBaseTypeInfo()), typeInfo->return_vec[i].spec.getTypeSpec(), res);
            if (res < QTI_NEAR) {
                return QTI_NOT_EQUAL;
            }
            if (res < rc) {
                rc = res;
            }
        }

        return rc;
    }

    DLLLOCAL qore_type_result_e runtimeAcceptsValue(const QoreValue& n) const;

    // returns true if this type could contain an object or a closure
    DLLLOCAL virtual bool needsScanImpl() const {
        return true;
    }

    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.sprintf("type '%s'", tname.c_str());
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return false;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
        return QoreValue();
    }

    // returns true if there is no type or if the type can be converted to a scalar (numeric, bool, int, string, etc) value, false true if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const = 0;

    DLLLOCAL static qore_type_result_e parseAcceptsIntern(const QoreAcceptSpec& at, const QoreReturnSpec& rt, bool& may_not_match, bool& may_need_filter, bool& t_no_match, bool& ok) {
        //printd(5, "QoreTypeInfo::parseAcceptsIntern() at: %d rt: %d rc: %d\n", (int)at.spec.getTypeSpec(), (int)rt.spec.getTypeSpec(), at.spec.match(rt.spec, may_not_match, may_need_filter));
        qore_type_result_e res = at.spec.match(rt.spec, may_not_match, may_need_filter);
        switch (res) {
            case QTI_IDENT:
                if (at.exact && rt.exact) {
                    if (at.map && !may_need_filter)
                        may_need_filter = true;
                    return QTI_IDENT;
                }
            // fall down to next case
            case QTI_NEAR:
            case QTI_AMBIGUOUS:
            case QTI_WILDCARD:
                if (at.map && !may_need_filter)
                    may_need_filter = true;
                if (t_no_match) {
                    t_no_match = false;
                    if (!ok) {
                        ok = true;
                        if (may_not_match)
                            return res;
                    }
                }

            // fall down to default
            default:
                break;
        }
        return QTI_NOT_EQUAL;
    }

    DLLLOCAL static void getNodeType(QoreString& str, const QoreValue& n) {
        qore_type_t nt = n.getType();
        if (nt == NT_NOTHING) {
            str.concat("no value");
            return;
        }
        if (nt != NT_OBJECT) {
            str.sprintf("type '%s'", n.getTypeName());
            return;
        }
        str.sprintf("an object of class '%s'", n.get<const QoreObject>()->getClassName());
    }

    DLLLOCAL static void ptext(QoreString& str, const char* arg_type, int param_num, const char* param_name) {
        if (!param_num && param_name && param_name[0] == '<') {
            str.concat(param_name);
            str.concat(' ');
            return;
        }
        if (param_name && param_name[0] == '<') {
            str.concat(param_name);
            str.concat(' ');
        }
        if (param_num) {
            str.sprintf("parameter %d ", param_num);
            if (param_name && param_name[0] != '<')
                str.sprintf("('%s') ", param_name);
        }
        else if (param_name)
            str.sprintf("%s '%s' ", arg_type, param_name);
        else {
            str.concat(arg_type);
            str.concat(' ');
        }
    }

    DLLLOCAL static void stripTypeInfo(QoreValue& n, ExceptionSink* xsink, LValueHelper* lvhelper = nullptr);
};

class QoreParseTypeInfo;
typedef std::vector<QoreParseTypeInfo*> parse_type_vec_t;

// this is basically just a wrapper around NamedScope
class QoreParseTypeInfo {
protected:
    std::string tname;

    DLLLOCAL QoreParseTypeInfo(const NamedScope* n_cscope) : cscope(n_cscope->copy()), or_nothing(false) {
        setName();
    }

    DLLLOCAL void setName() {
        if (or_nothing)
            tname = "*";
        tname += cscope->ostr;
        if (!subtypes.empty()) {
            tname += '<';
            tname += subtypes[0]->getName();
            for (unsigned i = 1; i < subtypes.size(); ++i) {
                tname += ", ";
                tname += subtypes[i]->getName();
            }
            tname += '>';
        }
    }

public:
    NamedScope* cscope; // namespace scope for class
    parse_type_vec_t subtypes;
    bool or_nothing;

    DLLLOCAL QoreParseTypeInfo(char* n_cscope, bool n_or_nothing = false) : cscope(new NamedScope(n_cscope)), or_nothing(n_or_nothing) {
        setName();
        //printd(5, "QoreParseTypeInfo::QoreParseTypeInfo() %s\n", tname.c_str());
    }

    DLLLOCAL QoreParseTypeInfo(char* n_cscope, bool n_or_nothing, parse_type_vec_t&& subtypes) : cscope(new NamedScope(n_cscope)), subtypes(subtypes), or_nothing(n_or_nothing) {
        setName();

        //printd(5, "QoreParseTypeInfo::QoreParseTypeInfo() %s\n", tname.c_str());
    }

    DLLLOCAL QoreParseTypeInfo(const QoreParseTypeInfo& old) : tname(old.tname), cscope(old.cscope ? new NamedScope(*old.cscope) : nullptr), or_nothing(old.or_nothing) {
        // copy subtypes
        for (const auto& i : old.subtypes)
            subtypes.push_back(new QoreParseTypeInfo(*i));
    }

    DLLLOCAL ~QoreParseTypeInfo() {
        delete cscope;
        for (auto& i : subtypes)
            delete i;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool parseStageOneIdenticalWithParsed(const QoreParseTypeInfo* pti, const QoreTypeInfo* typeInfo, bool& recheck) {
        if (pti && typeInfo)
            return pti->parseStageOneIdenticalWithParsed(typeInfo, recheck);
        else if (pti)
            return false;
        else if (typeInfo)
            return false;
        else
            return true;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static bool parseStageOneIdentical(const QoreParseTypeInfo* pti, const QoreParseTypeInfo* typeInfo, bool& recheck) {
        if (pti && typeInfo)
            return pti->parseStageOneIdentical(typeInfo, recheck);
        else
            return !(pti || typeInfo);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* resolveAndDelete(QoreParseTypeInfo* pti, const QoreProgramLocation* loc) {
        return pti ? pti->resolveAndDelete(loc) : nullptr;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* resolve(QoreParseTypeInfo* pti, const QoreProgramLocation* loc) {
        return pti ? pti->resolve(loc) : nullptr;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const QoreTypeInfo* resolveAny(QoreParseTypeInfo* pti, const QoreProgramLocation* loc) {
        return pti ? pti->resolveAny(loc) : nullptr;
    }

    DLLLOCAL static const QoreTypeInfo* resolveRuntime(QoreParseTypeInfo* pti) {
        return pti ? pti->resolveRuntime() : nullptr;
    }

#ifdef DEBUG
    DLLLOCAL const char* getCID() const { return cscope ? cscope->getIdentifier() : "n/a"; }

    // static version of method, checking for null pointer
    DLLLOCAL static const char* getCID(const QoreParseTypeInfo* pti) { return pti ? pti->getCID() : "n/a"; }
#endif

    DLLLOCAL QoreParseTypeInfo* copy() const {
        return new QoreParseTypeInfo(cscope);
    }

    // static version of method, checking for null pointer
    DLLLOCAL static const char* getName(const QoreParseTypeInfo* pti) {
        return pti ? pti->getName() : NO_TYPE_INFO;
    }

    // static version of method, checking for null pointer
    DLLLOCAL static void concatName(const QoreParseTypeInfo* pti, std::string& str) {
        if (pti)
            pti->concatName(str);
        else
            str.append(NO_TYPE_INFO);
    }

private:
    // used when parsing user code to find duplicate signatures
    DLLLOCAL bool parseStageOneIdenticalWithParsed(const QoreTypeInfo* typeInfo, bool& recheck) const {
        if (!typeInfo)
            return false;

        const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(typeInfo);
        if (!qc) {
            const TypedHashDecl* hd = QoreTypeInfo::getUniqueReturnHashDecl(typeInfo);
            if (!hd) {
                const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexHash(typeInfo);
                if (!ti) {
                    return false;
                }
                if (subtypes.size() == 2 && !strcmp(cscope->getIdentifier(), "hash"))
                    return recheck = true;
                return false;
            }
            if (subtypes.size() == 1 && !strcmp(cscope->getIdentifier(), "hash"))
                return recheck = true;
            return false;
        }

        // both have class info
        if (!strcmp(cscope->getIdentifier(), qc->getName()))
            return recheck = true;
        return false;
    }

    // used when parsing user code to find duplicate signatures
    DLLLOCAL bool parseStageOneIdentical(const QoreParseTypeInfo* typeInfo, bool& recheck) const {
        //printd(5, "QoreParseTypeInfo::parseStageOneIdentical() this: %p '%s' == %p '%s'\n", this, tname.c_str(), typeInfo, typeInfo->tname.c_str());
        if (tname == typeInfo->tname) {
            return true;
        }
        // issue #3861: check if they could potentially refer to the same declaration; if the shorter string is the same as the
        // longer string, and the longer string is prefixed by "::", then the declarations are ambiguous and must be rechecked
        if (tname.size() > typeInfo->tname.size()) {
            recheck = checkAmbiguous(tname, typeInfo->tname);
        } else if (typeInfo->tname.size() > tname.size()) {
            recheck = checkAmbiguous(typeInfo->tname, tname);
        }
        return false;
    }

    DLLLOCAL static bool checkAmbiguous(const std::string& longer, const std::string& shorter) {
        // if the previous two character in longer are not '::', then the strings are not ambiguous
        if (longer.size() - shorter.size() < 2) {
            return false;
        }
        if (longer.compare(longer.size() - shorter.size() - 2, 2, "::")) {
            return false;
        }
        return !longer.compare(longer.size() - shorter.size(), shorter.size(), shorter);
    }

    // resolves complex types (classes, hashdecls, etc)
    DLLLOCAL const QoreTypeInfo* resolve(const QoreProgramLocation* loc) const;
    // also resolves base types
    DLLLOCAL const QoreTypeInfo* resolveAny(const QoreProgramLocation* loc) const;
    // resolves the current type to an QoreTypeInfo pointer and deletes itself
    DLLLOCAL const QoreTypeInfo* resolveAndDelete(const QoreProgramLocation* loc);
    DLLLOCAL const QoreTypeInfo* resolveSubtype(const QoreProgramLocation* loc) const;

    DLLLOCAL const QoreTypeInfo* resolveRuntime() const;
    DLLLOCAL const QoreTypeInfo* resolveRuntimeSubtype() const;
    DLLLOCAL static const QoreTypeInfo* resolveRuntimeClass(const NamedScope& cscope, bool or_nothing);

    DLLLOCAL const char* getName() const {
        return tname.c_str();
    }

    DLLLOCAL void concatName(std::string& str) const {
        assert(!tname.empty());
        str.append(tname);
    }

    DLLLOCAL static const QoreTypeInfo* resolveClass(const QoreProgramLocation* loc, const NamedScope& cscope, bool or_nothing);
};

class QoreAnyTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreAnyTypeInfo() : QoreTypeInfo("any", q_accept_vec_t {{NT_ALL, nullptr}}, q_return_vec_t {{NT_ALL}}) {
   }

protected:
   DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
      str.concat("no value");
   }

   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }
};

class QoreAutoTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreAutoTypeInfo() : QoreTypeInfo("auto", q_accept_vec_t {{NT_ALL, nullptr}}, q_return_vec_t {{NT_ALL}}) {
   }

protected:
   DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
      str.concat("no value");
   }

   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }
};

class QoreClassTypeInfo : public QoreTypeInfo {
public:
    DLLLOCAL QoreClassTypeInfo(const QoreClass* qc, const char* name)
        : QoreTypeInfo(q_accept_vec_t {{qc, nullptr, true}}, q_return_vec_t {{qc, true}},
            QoreStringMaker("object<%s>", name)) {
    }

protected:
    DLLLOCAL QoreClassTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec, const QoreString& tname)
        : QoreTypeInfo(std::move(a_vec), std::move(r_vec), tname) {
    }

    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.concat(&tname);
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }
};

class QoreClassOrNothingTypeInfo : public QoreClassTypeInfo {
public:
    DLLLOCAL QoreClassOrNothingTypeInfo(const QoreClass* qc, const char* name) : QoreClassTypeInfo(q_accept_vec_t {
            {qc, nullptr},
            {NT_NOTHING, nullptr},
            {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
        }, q_return_vec_t {{qc}, {NT_NOTHING}}, QoreStringMaker("*object<%s>", name)) {
    }

protected:
   DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
      str.sprintf("an object of class '%s' or no value (NOTHING)", accept_vec[0].spec.getClass()->getName());
   }

   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }
};

class QoreHashDeclTypeInfo : public QoreTypeInfo {
public:
    DLLLOCAL QoreHashDeclTypeInfo(const TypedHashDecl* hd, const char* name)
        : QoreTypeInfo(q_accept_vec_t {{hd, nullptr, true}}, q_return_vec_t {{hd, true}},
        QoreStringMaker("hash<%s>", name)) {
    }

protected:
    DLLLOCAL QoreHashDeclTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec, const QoreString& tname)
        : QoreTypeInfo(std::move(a_vec), std::move(r_vec), tname) {
    }

    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.concat(&tname);
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return true;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const;
};

class QoreHashDeclOrNothingTypeInfo : public QoreHashDeclTypeInfo {
public:
    DLLLOCAL QoreHashDeclOrNothingTypeInfo(const TypedHashDecl* hd, const char* name) : QoreHashDeclTypeInfo(q_accept_vec_t {
            {hd, nullptr},
            {NT_NOTHING, nullptr},
            {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
        }, q_return_vec_t {{hd}, {NT_NOTHING}}, QoreStringMaker("*hash<%s>", name)) {
    }

protected:
    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.sprintf("hash<%s> or no value (NOTHING)", accept_vec[0].spec.getHashDecl()->getName());
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return false;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
        return QoreValue();
    }
};

class QoreComplexHashTypeInfo : public QoreTypeInfo {
public:
    DLLLOCAL QoreComplexHashTypeInfo(const QoreTypeInfo* vti)
        : QoreTypeInfo(q_accept_vec_t {{QoreComplexHashTypeSpec(vti), nullptr, true}},
            q_return_vec_t {{QoreComplexHashTypeSpec(vti), true}},
            vti == autoTypeInfo
                ? QoreString("hash<auto>")
                : QoreStringMaker("hash<string, %s>", QoreTypeInfo::getName(vti))) {
    }

protected:
    DLLLOCAL QoreComplexHashTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec,
        const QoreString& tname) : QoreTypeInfo(std::move(a_vec), std::move(r_vec), tname) {
    }

    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.concat(&tname);
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return true;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
        return new QoreHashNode(accept_vec[0].spec.getComplexHash());
    }
};

class QoreComplexHashOrNothingTypeInfo : public QoreComplexHashTypeInfo {
public:
    DLLLOCAL QoreComplexHashOrNothingTypeInfo(const QoreTypeInfo* vti) : QoreComplexHashTypeInfo(q_accept_vec_t {
            {QoreComplexHashTypeSpec(vti), nullptr, true},
            {NT_NOTHING, nullptr},
            {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
            }, q_return_vec_t {{QoreComplexHashTypeSpec(vti)}, {NT_NOTHING}},
            vti == autoTypeInfo
                ? QoreString("*hash<auto>")
                : QoreStringMaker("*hash<string, %s>", QoreTypeInfo::getName(vti))) {
    }

protected:
    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.sprintf("hash<string, %s> or no value (NOTHING)", QoreTypeInfo::getName(accept_vec[0].spec.getComplexHash()));
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return false;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
        return QoreValue();
    }
};

class QoreAutoHashTypeInfo : public QoreComplexHashTypeInfo {
public:
   DLLLOCAL QoreAutoHashTypeInfo() : QoreComplexHashTypeInfo(autoTypeInfo) {
   }
};

class QoreAutoHashOrNothingTypeInfo : public QoreComplexHashOrNothingTypeInfo {
public:
   DLLLOCAL QoreAutoHashOrNothingTypeInfo() : QoreComplexHashOrNothingTypeInfo(autoTypeInfo) {
   }
};

class QoreComplexListTypeInfo : public QoreTypeInfo {
public:
    DLLLOCAL QoreComplexListTypeInfo(const QoreTypeInfo* vti)
        : QoreTypeInfo(q_accept_vec_t {{QoreComplexListTypeSpec(vti), nullptr, true}},
            q_return_vec_t {{QoreComplexListTypeSpec(vti), true}},
            QoreStringMaker("list<%s>", QoreTypeInfo::getName(vti))) {
        assert(vti);
    }

protected:
    DLLLOCAL QoreComplexListTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec,
        const QoreString& tname) : QoreTypeInfo(std::move(a_vec), std::move(r_vec), tname) {
    }

    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.concat(&tname);
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return true;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
        return new QoreListNode(accept_vec[0].spec.getComplexList());
    }
};

class QoreComplexListOrNothingTypeInfo : public QoreComplexListTypeInfo {
public:
    DLLLOCAL QoreComplexListOrNothingTypeInfo(const QoreTypeInfo* vti) : QoreComplexListTypeInfo(q_accept_vec_t {
            {QoreComplexListTypeSpec(vti), nullptr},
            {NT_NOTHING, nullptr},
            {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
            }, q_return_vec_t {{QoreComplexListTypeSpec(vti)}, {NT_NOTHING}},
            QoreStringMaker("*list<%s>", QoreTypeInfo::getName(vti))) {
        assert(vti);
    }

protected:
    DLLLOCAL QoreComplexListOrNothingTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec,
        const QoreString& tname) : QoreComplexListTypeInfo(std::move(a_vec), std::move(r_vec), tname) {
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return false;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
        return QoreValue();
    }
};

class QoreAutoListTypeInfo : public QoreComplexListTypeInfo {
public:
   DLLLOCAL QoreAutoListTypeInfo() : QoreComplexListTypeInfo(autoTypeInfo) {
   }
};

class QoreAutoListOrNothingTypeInfo : public QoreComplexListOrNothingTypeInfo {
public:
   DLLLOCAL QoreAutoListOrNothingTypeInfo() : QoreComplexListOrNothingTypeInfo(autoTypeInfo) {
   }
};

class QoreComplexSoftListTypeInfo : public QoreComplexListTypeInfo {
public:
    DLLLOCAL QoreComplexSoftListTypeInfo(const QoreTypeInfo* vti);

protected:
    DLLLOCAL QoreComplexSoftListTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec,
        const QoreString& tname)
        : QoreComplexListTypeInfo(std::move(a_vec), std::move(r_vec), tname) {
    }
};

class QoreComplexSoftListOrNothingTypeInfo : public QoreComplexListOrNothingTypeInfo {
public:
    DLLLOCAL QoreComplexSoftListOrNothingTypeInfo(const QoreTypeInfo* vti);

protected:
    DLLLOCAL QoreComplexSoftListOrNothingTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec,
        const QoreString& tname)
         : QoreComplexListOrNothingTypeInfo(std::move(a_vec), std::move(r_vec), tname) {
    }
};

class QoreComplexReferenceTypeInfo : public QoreTypeInfo {
public:
    DLLLOCAL QoreComplexReferenceTypeInfo(const QoreTypeInfo* vti)
        : QoreTypeInfo(q_accept_vec_t {{QoreComplexReferenceTypeSpec(vti), nullptr, true}},
            q_return_vec_t {{QoreComplexReferenceTypeSpec(vti), true}},
            QoreStringMaker("reference<%s>", QoreTypeInfo::getName(vti))) {
        assert(vti);
    }

protected:
    DLLLOCAL QoreComplexReferenceTypeInfo(const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec,
        const QoreString& tname) : QoreTypeInfo(std::move(a_vec), std::move(r_vec), tname) {
    }

    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.concat(&tname);
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }
};

class QoreComplexReferenceOrNothingTypeInfo : public QoreComplexReferenceTypeInfo {
public:
    DLLLOCAL QoreComplexReferenceOrNothingTypeInfo(const QoreTypeInfo* vti)
        : QoreComplexReferenceTypeInfo(q_accept_vec_t {
                {QoreComplexReferenceTypeSpec(vti), nullptr, true},
                {NT_NOTHING, nullptr},
                {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
            },
            q_return_vec_t {{QoreComplexReferenceTypeSpec(vti)}, {NT_NOTHING}},
            QoreStringMaker("*reference<%s>", QoreTypeInfo::getName(vti))) {
        assert(vti);
    }

protected:
    DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const {
        str.sprintf("reference<%s> or no value (NOTHING)", QoreTypeInfo::getName(accept_vec[0].spec.getComplexReference()));
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }
};

class QoreBaseTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreBaseTypeInfo(const char* name, qore_type_t t) : QoreTypeInfo(name, q_accept_vec_t {{t, nullptr, true}}, q_return_vec_t {{t, true}}) {
   }

protected:
   DLLLOCAL QoreBaseTypeInfo(const char* name, q_accept_vec_t&& a_vec, q_return_vec_t&& r_vec) : QoreTypeInfo(name, std::move(a_vec), std::move(r_vec)) {
   }
};

class QoreBaseOrNothingTypeInfo : public QoreBaseTypeInfo {
public:
   DLLLOCAL QoreBaseOrNothingTypeInfo(const char* name, qore_type_t t) : QoreBaseTypeInfo(name, q_accept_vec_t {
         {t, nullptr},
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      },
      q_return_vec_t {{t}, {NT_NOTHING}}) {
   }
};

class QoreBaseConvertTypeInfo : public QoreBaseTypeInfo {
public:
   DLLLOCAL QoreBaseConvertTypeInfo(const char* name, qore_type_t qt) : QoreBaseTypeInfo(name, qt) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }
};

class QoreBaseOrNothingConvertTypeInfo : public QoreBaseOrNothingTypeInfo {
public:
   DLLLOCAL QoreBaseOrNothingConvertTypeInfo(const char* name, qore_type_t qt) : QoreBaseOrNothingTypeInfo(name, qt) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }
};

class QoreBaseNoConvertTypeInfo : public QoreBaseTypeInfo {
public:
   DLLLOCAL QoreBaseNoConvertTypeInfo(const char* name, qore_type_t qt) : QoreBaseTypeInfo(name, qt) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }
};

class QoreObjectTypeInfo : public QoreBaseTypeInfo {
public:
   DLLLOCAL QoreObjectTypeInfo() : QoreBaseTypeInfo("object", NT_OBJECT) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }
};

class QoreBaseOrNothingNoConvertTypeInfo : public QoreBaseOrNothingTypeInfo {
public:
   DLLLOCAL QoreBaseOrNothingNoConvertTypeInfo(const char* name, qore_type_t qt) : QoreBaseOrNothingTypeInfo(name, qt) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }
};

class QoreBigIntTypeInfo : public QoreBaseConvertTypeInfo {
public:
   DLLLOCAL QoreBigIntTypeInfo() : QoreBaseConvertTypeInfo("int", NT_INT) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return 0LL;
   }
};

class QoreBigIntOrNothingTypeInfo : public QoreBaseOrNothingConvertTypeInfo {
public:
   DLLLOCAL QoreBigIntOrNothingTypeInfo() : QoreBaseOrNothingConvertTypeInfo("*int", NT_INT) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreStringTypeInfo : public QoreBaseConvertTypeInfo {
public:
   DLLLOCAL QoreStringTypeInfo() : QoreBaseConvertTypeInfo("string", NT_STRING) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return NullString->refSelf();
   }
};

class QoreStringOrNothingTypeInfo : public QoreBaseOrNothingConvertTypeInfo {
public:
   DLLLOCAL QoreStringOrNothingTypeInfo() : QoreBaseOrNothingConvertTypeInfo("*string", NT_STRING) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreBoolTypeInfo : public QoreBaseConvertTypeInfo {
public:
   DLLLOCAL QoreBoolTypeInfo() : QoreBaseConvertTypeInfo("bool", NT_BOOLEAN) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return false;
   }
};

class QoreBoolOrNothingTypeInfo : public QoreBaseOrNothingConvertTypeInfo {
public:
   DLLLOCAL QoreBoolOrNothingTypeInfo() : QoreBaseOrNothingConvertTypeInfo("*bool", NT_BOOLEAN) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

};

class QoreBinaryTypeInfo : public QoreBaseNoConvertTypeInfo {
public:
   DLLLOCAL QoreBinaryTypeInfo() : QoreBaseNoConvertTypeInfo("binary", NT_BINARY) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return new BinaryNode;
   }
};

class QoreBinaryOrNothingTypeInfo : public QoreBaseOrNothingNoConvertTypeInfo {
public:
   DLLLOCAL QoreBinaryOrNothingTypeInfo() : QoreBaseOrNothingNoConvertTypeInfo("*binary", NT_BINARY) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreObjectOrNothingTypeInfo : public QoreBaseOrNothingNoConvertTypeInfo {
public:
   DLLLOCAL QoreObjectOrNothingTypeInfo() : QoreBaseOrNothingNoConvertTypeInfo("*object", NT_OBJECT) {
   }
};

class QoreDateTypeInfo : public QoreBaseConvertTypeInfo {
public:
   DLLLOCAL QoreDateTypeInfo() : QoreBaseConvertTypeInfo("date", NT_DATE) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return ZeroDate->refSelf();
   }
};

class QoreDateOrNothingTypeInfo : public QoreBaseOrNothingConvertTypeInfo {
public:
   DLLLOCAL QoreDateOrNothingTypeInfo() : QoreBaseOrNothingConvertTypeInfo("*date", NT_DATE) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

DLLLOCAL void map_get_plain_hash(QoreValue&, ExceptionSink*);
DLLLOCAL void map_get_plain_hash_lvalue(QoreValue&, ExceptionSink*, LValueHelper*);

class QoreHashTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreHashTypeInfo() : QoreTypeInfo("hash", q_accept_vec_t {
         {NT_HASH, map_get_plain_hash, true}},
      q_return_vec_t {{NT_HASH, true}}) {
   }

protected:
   DLLLOCAL QoreHashTypeInfo(const char* name, const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec) :
      QoreTypeInfo(name, std::move(a_vec), std::move(r_vec)) {
   }

   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return emptyHash->hashRefSelf();
   }
};

class QoreEmptyHashTypeInfo : public QoreHashTypeInfo {
public:
    DLLLOCAL QoreEmptyHashTypeInfo() : QoreHashTypeInfo("hash", q_accept_vec_t {
            {NT_HASH, map_get_plain_hash, true},
        },
        q_return_vec_t {{QoreEmptyHashTypeSpec(), true}}) {
    }
};

class QoreHashOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreHashOrNothingTypeInfo() : QoreTypeInfo("*hash", q_accept_vec_t {
         {NT_HASH, map_get_plain_hash},
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      },
      q_return_vec_t {{NT_HASH}, {NT_NOTHING}}) {
   }

protected:
   DLLLOCAL QoreHashOrNothingTypeInfo(const char* name, const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec) :
      QoreTypeInfo(name, std::move(a_vec), std::move(r_vec)) {
   }

   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }
};

DLLLOCAL void map_get_plain_list(QoreValue&, ExceptionSink*);
DLLLOCAL void map_get_plain_list_lvalue(QoreValue&, ExceptionSink*, LValueHelper*);

class QoreListTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreListTypeInfo() : QoreTypeInfo("list",
      q_accept_vec_t {
         {NT_LIST, map_get_plain_list, true},
      },
      q_return_vec_t {{NT_LIST, true}}) {
   }

protected:
   DLLLOCAL QoreListTypeInfo(const char* name, const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec) :
      QoreTypeInfo(name, std::move(a_vec), std::move(r_vec)) {
   }

   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return emptyList->listRefSelf();
   }
};

class QoreEmptyListTypeInfo : public QoreListTypeInfo {
public:
    DLLLOCAL QoreEmptyListTypeInfo() : QoreListTypeInfo("list", q_accept_vec_t {
            {NT_LIST, nullptr, true},
        },
        q_return_vec_t {{QoreEmptyListTypeSpec(), true}}) {
    }
};

class QoreListOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreListOrNothingTypeInfo() : QoreTypeInfo("*list",
      q_accept_vec_t {
         {NT_LIST, map_get_plain_list},
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      },
      q_return_vec_t {{NT_LIST}, {NT_NOTHING}}) {
   }

protected:
   DLLLOCAL QoreListOrNothingTypeInfo(const char* name, const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec) :
      QoreTypeInfo(name, std::move(a_vec), std::move(r_vec)) {
   }

   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }
};

class QoreNothingTypeInfo : public QoreBaseNoConvertTypeInfo {
public:
   DLLLOCAL QoreNothingTypeInfo() : QoreBaseNoConvertTypeInfo("nothing", NT_NOTHING) {
   }
};

class QoreNullTypeInfo : public QoreBaseNoConvertTypeInfo {
public:
   DLLLOCAL QoreNullTypeInfo() : QoreBaseNoConvertTypeInfo("null", NT_NULL) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return &Null;
   }
};

class QoreNullOrNothingTypeInfo : public QoreBaseOrNothingNoConvertTypeInfo {
public:
   DLLLOCAL QoreNullOrNothingTypeInfo() : QoreBaseOrNothingNoConvertTypeInfo("*null", NT_NULL) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreClosureTypeInfo : public QoreBaseNoConvertTypeInfo {
public:
   DLLLOCAL QoreClosureTypeInfo() : QoreBaseNoConvertTypeInfo("closure", NT_RUNTIME_CLOSURE) {
   }
};

class QoreClosureOrNothingTypeInfo : public QoreBaseOrNothingNoConvertTypeInfo {
public:
   DLLLOCAL QoreClosureOrNothingTypeInfo() : QoreBaseOrNothingNoConvertTypeInfo("*closure", NT_RUNTIME_CLOSURE) {
   }
};

class QoreCallReferenceTypeInfo : public QoreBaseNoConvertTypeInfo {
public:
   DLLLOCAL QoreCallReferenceTypeInfo() : QoreBaseNoConvertTypeInfo("callref", NT_FUNCREF) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreCallReferenceOrNothingTypeInfo : public QoreBaseOrNothingNoConvertTypeInfo {
public:
   DLLLOCAL QoreCallReferenceOrNothingTypeInfo() : QoreBaseOrNothingNoConvertTypeInfo("*callref", NT_FUNCREF) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreReferenceTypeInfo : public QoreBaseNoConvertTypeInfo {
public:
   DLLLOCAL QoreReferenceTypeInfo() : QoreBaseNoConvertTypeInfo("reference", NT_REFERENCE) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreReferenceOrNothingTypeInfo : public QoreBaseOrNothingNoConvertTypeInfo {
public:
   DLLLOCAL QoreReferenceOrNothingTypeInfo() : QoreBaseOrNothingNoConvertTypeInfo("*reference", NT_REFERENCE) {
   }

protected:
   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreNumberTypeInfo : public QoreBaseTypeInfo {
public:
   DLLLOCAL QoreNumberTypeInfo() : QoreBaseTypeInfo("number", q_accept_vec_t {
         {NT_NUMBER, nullptr, true},
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) { discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink); }}, {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) { discard(n.assign(new QoreNumberNode(n.getAsBigInt())), xsink); }},
      },
      q_return_vec_t {{NT_NUMBER, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return ZeroNumber->refSelf();
   }
};

class QoreNumberOrNothingTypeInfo : public QoreBaseTypeInfo {
public:
   DLLLOCAL QoreNumberOrNothingTypeInfo() :
      QoreBaseTypeInfo("*number", q_accept_vec_t {
         {NT_NUMBER, nullptr},
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
              discard(n.assign(new QoreNumberNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               n.assignNothing();
            }
         },
      }, q_return_vec_t {{NT_NUMBER}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreFloatTypeInfo : public QoreBaseTypeInfo {
public:
   DLLLOCAL QoreFloatTypeInfo() : QoreBaseTypeInfo("float", q_accept_vec_t {
         {NT_FLOAT, nullptr, true},
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign((double)n.getAsBigInt()), xsink);
            }
         },
      }, q_return_vec_t {{NT_FLOAT, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return 0.0;
   }
};

class QoreFloatOrNothingTypeInfo : public QoreBaseTypeInfo {
public:
   DLLLOCAL QoreFloatOrNothingTypeInfo() : QoreBaseTypeInfo("*float", q_accept_vec_t {
         {NT_FLOAT, nullptr},
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign((double)n.getAsBigInt()), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               n.assignNothing();
            }
         },
      }, q_return_vec_t {{NT_FLOAT}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreCodeTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreCodeTypeInfo() : QoreTypeInfo("code", q_accept_vec_t {
         {NT_RUNTIME_CLOSURE, nullptr},
         {NT_FUNCREF, nullptr},
      }, q_return_vec_t {{NT_RUNTIME_CLOSURE}, {NT_FUNCREF}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return true;
   }
};

class QoreCodeOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreCodeOrNothingTypeInfo() : QoreTypeInfo("*code", q_accept_vec_t {
         {NT_RUNTIME_CLOSURE, nullptr},
         {NT_FUNCREF, nullptr},
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      }, q_return_vec_t {{NT_RUNTIME_CLOSURE}, {NT_FUNCREF}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return true;
   }
};

class QoreDataTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreDataTypeInfo() : QoreTypeInfo("data", q_accept_vec_t {
         {NT_STRING, nullptr},
         {NT_BINARY, nullptr},
      }, q_return_vec_t {{NT_STRING}, {NT_BINARY}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreDataOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreDataOrNothingTypeInfo() : QoreTypeInfo("*data", q_accept_vec_t {
         {NT_STRING, nullptr},
         {NT_BINARY, nullptr},
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      }, q_return_vec_t {{NT_STRING}, {NT_BINARY}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreSoftBigIntTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftBigIntTypeInfo() : QoreTypeInfo("softint", q_accept_vec_t {
         {NT_INT, nullptr, true},
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(0ll), xsink);
            }
         },
      }, q_return_vec_t {{NT_INT, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return 0ll;
   }
};

class QoreSoftBigIntOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftBigIntOrNothingTypeInfo() : QoreTypeInfo("*softint", q_accept_vec_t {
         {NT_INT, nullptr},
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBigInt()), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      }, q_return_vec_t {{NT_INT}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreSoftFloatTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftFloatTypeInfo() : QoreTypeInfo("softfloat", q_accept_vec_t {
         {NT_FLOAT, nullptr, true},
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(0.0), xsink);
            }
         },
      }, q_return_vec_t {{NT_FLOAT, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return 0.0;
   }
};

class QoreSoftFloatOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftFloatOrNothingTypeInfo() : QoreTypeInfo("*softfloat", q_accept_vec_t {
         {NT_FLOAT, nullptr},
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsFloat()), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      }, q_return_vec_t {{NT_FLOAT}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreSoftNumberTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftNumberTypeInfo() : QoreTypeInfo("softnumber", q_accept_vec_t {
         {NT_NUMBER, nullptr, true},
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.get<const QoreStringNode>()->c_str())), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
            }
         },
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
            }
         },
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(0.0)), xsink);
            }
         },
      }, q_return_vec_t {{NT_NUMBER, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return ZeroNumber->refSelf();
   }
};

class QoreSoftNumberOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftNumberOrNothingTypeInfo() : QoreTypeInfo("*softnumber", q_accept_vec_t {
         {NT_NUMBER, nullptr},
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.get<const QoreStringNode>()->c_str())), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
            }
         },
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      }, q_return_vec_t {{NT_NUMBER}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreSoftBinaryTypeInfo : public QoreTypeInfo {
public:
    DLLLOCAL QoreSoftBinaryTypeInfo() : QoreTypeInfo("softbinary",
        q_accept_vec_t {
            {NT_BINARY, nullptr, true},
            {NT_STRING,
                [] (QoreValue& n, ExceptionSink* xsink) {
                    const QoreStringNode* str = n.get<const QoreStringNode>();
                    SimpleRefHolder<BinaryNode> bn(new BinaryNode);
                    bn->append((const void*)str->c_str(), str->size());
                    discard(n.assign(bn.release()), xsink);
                }
            },
            {NT_NULL,
                [] (QoreValue& n, ExceptionSink* xsink) {
                    discard(n.assign(new BinaryNode), xsink);
                }
            },
        }, q_return_vec_t {{NT_BINARY, true}}) {
    }

protected:
    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return true;
    }

    // returns true if this type could contain an object or a closure
    DLLLOCAL virtual bool needsScanImpl() const {
        return false;
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return true;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
        return new BinaryNode();
    }
};

class QoreSoftBinaryOrNothingTypeInfo : public QoreTypeInfo {
public:
    DLLLOCAL QoreSoftBinaryOrNothingTypeInfo() : QoreTypeInfo("*softbinary",
        q_accept_vec_t {
            {NT_BINARY, nullptr, true},
            {NT_STRING,
                [] (QoreValue& n, ExceptionSink* xsink) {
                    const QoreStringNode* str = n.get<const QoreStringNode>();
                    SimpleRefHolder<BinaryNode> bn(new BinaryNode);
                    bn->append((const void*)str->c_str(), str->size());
                    discard(n.assign(bn.release()), xsink);
                }
            },
            {NT_NOTHING, nullptr},
            {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
        }, q_return_vec_t {{NT_BINARY}, {NT_NOTHING}}) {
    }

protected:
    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return true;
    }

    // returns true if this type could contain an object or a closure
    DLLLOCAL virtual bool needsScanImpl() const {
        return false;
    }
};

class QoreSoftBoolTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftBoolTypeInfo() : QoreTypeInfo("softbool", q_accept_vec_t {
         {NT_BOOLEAN, nullptr, true},
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               n.assign(false);
            }
         },
      }, q_return_vec_t {{NT_BOOLEAN, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return false;
   }
};

class QoreSoftBoolOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftBoolOrNothingTypeInfo() : QoreTypeInfo("*softbool", q_accept_vec_t {
         {NT_BOOLEAN, nullptr},
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(n.getAsBool()), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      }, q_return_vec_t {{NT_BOOLEAN}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreSoftStringTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftStringTypeInfo() : QoreTypeInfo("softstring", q_accept_vec_t {
         {NT_STRING, nullptr, true},
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreStringNodeMaker(QLLD, n.getAsBigInt())), xsink);
            }
         },
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(q_fix_decimal(new QoreStringNodeMaker("%.9g", n.getAsFloat()), 0)), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreStringNodeMaker(QLLD, n.getAsBigInt())), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               QoreStringNodeValueHelper str(n.getInternalNode());
               discard(n.assign(str.getReferencedValue()), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               QoreStringNodeValueHelper str(n.getInternalNode());
               discard(n.assign(str.getReferencedValue()), xsink);
            }
         },
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               n.assign(NullString->stringRefSelf());
            }
         },
      }, q_return_vec_t {{NT_STRING, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return NullString->refSelf();
   }
};

class QoreSoftStringOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftStringOrNothingTypeInfo() : QoreTypeInfo("*softstring", q_accept_vec_t {
         {NT_STRING, nullptr},
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreStringNodeMaker(QLLD, n.getAsBigInt())), xsink);
            }
         },
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(q_fix_decimal(new QoreStringNodeMaker("%.9g", n.getAsFloat()), 0)), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new QoreStringNodeMaker(QLLD, n.getAsBigInt())), xsink);
            }
         },
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               QoreStringNodeValueHelper str(n.getInternalNode());
               discard(n.assign(str.getReferencedValue()), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               QoreStringNodeValueHelper str(n.getInternalNode());
               discard(n.assign(str.getReferencedValue()), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      }, q_return_vec_t {{NT_STRING}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreSoftDateTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftDateTypeInfo() : QoreTypeInfo("softdate", q_accept_vec_t {
         {NT_DATE, nullptr, true},
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               DateTimeNodeValueHelper dt(n.getInternalNode());
               discard(n.assign(dt.getReferencedValue()), xsink);
            }
         },
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new DateTimeNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new DateTimeNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new DateTimeNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               DateTimeNodeValueHelper dt(n.getInternalNode());
               discard(n.assign(dt.getReferencedValue()), xsink);
            }
         },
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               n.assign(new DateTimeNode(0ll));
            }
         },
      }, q_return_vec_t {{NT_DATE, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return ZeroDate->refSelf();
   }
};

class QoreSoftDateOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftDateOrNothingTypeInfo() : QoreTypeInfo("*softdate", q_accept_vec_t {
         {NT_DATE, nullptr},
         {NT_STRING, [] (QoreValue& n, ExceptionSink* xsink) {
               DateTimeNodeValueHelper dt(n.getInternalNode());
               discard(n.assign(dt.getReferencedValue()), xsink);
            }
         },
         {NT_BOOLEAN, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new DateTimeNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_FLOAT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new DateTimeNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_INT, [] (QoreValue& n, ExceptionSink* xsink) {
               discard(n.assign(new DateTimeNode(n.getAsBigInt())), xsink);
            }
         },
         {NT_NUMBER, [] (QoreValue& n, ExceptionSink* xsink) {
               DateTimeNodeValueHelper dt(n.getInternalNode());
               discard(n.assign(dt.getReferencedValue()), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
      }, q_return_vec_t {{NT_DATE}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreSoftListTypeInfo : public QoreTypeInfo {
public:
    DLLLOCAL QoreSoftListTypeInfo() : QoreTypeInfo("softlist", q_accept_vec_t {
            {NT_LIST, map_get_plain_list, true},
            {NT_NOTHING, [] (QoreValue& n, ExceptionSink* xsink) {
                QoreListNode* l = new QoreListNode;
                n.assign(l);
                }
            },
            {NT_ALL, [] (QoreValue& n, ExceptionSink* xsink) {
                QoreListNode* l = new QoreListNode;
                l->push(n, nullptr);
                n.assign(l);
                }
            },
        }, q_return_vec_t {{NT_LIST, true}}) {
    }

protected:
    DLLLOCAL QoreSoftListTypeInfo(const char* name, const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec) : QoreTypeInfo(name, std::move(a_vec), std::move(r_vec)) {
    }

    // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
    DLLLOCAL virtual bool canConvertToScalarImpl() const {
        return false;
    }

    // returns true if this type could contain an object or a closure
    DLLLOCAL virtual bool needsScanImpl() const {
        return true;
    }

    DLLLOCAL virtual bool hasDefaultValueImpl() const {
        return true;
    }

    DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
        return emptyList->listRefSelf();
    }
};

class QoreSoftListOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreSoftListOrNothingTypeInfo() : QoreTypeInfo("*softlist", q_accept_vec_t {
         {NT_LIST, map_get_plain_list},
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
         {NT_ALL, [] (QoreValue& n, ExceptionSink* xsink) {
               QoreListNode* l = new QoreListNode;
               l->push(n, nullptr);
               n.assign(l);
            }
         },
      }, q_return_vec_t {{NT_LIST}, {NT_NOTHING}}) {
   }

protected:
   DLLLOCAL QoreSoftListOrNothingTypeInfo(const char* name, const q_accept_vec_t&& a_vec, const q_return_vec_t&& r_vec) : QoreTypeInfo(name, std::move(a_vec), std::move(r_vec)) {
   }

   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return false;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return true;
   }
};

class QoreSoftAutoListTypeInfo : public QoreComplexSoftListTypeInfo {
public:
   DLLLOCAL QoreSoftAutoListTypeInfo() : QoreComplexSoftListTypeInfo(q_accept_vec_t {
         {QoreComplexListTypeSpec(autoTypeInfo), nullptr, true},
         {NT_NOTHING, [] (QoreValue& n, ExceptionSink* xsink) {
               QoreListNode* l = new QoreListNode(autoTypeInfo);
               n.assign(l);
            }
         },
         {NT_ALL, [] (QoreValue& n, ExceptionSink* xsink) {
               QoreListNode* l = new QoreListNode(autoTypeInfo);
               l->push(n, nullptr);
               n.assign(l);
            }
         },
      }, q_return_vec_t {{QoreComplexListTypeSpec(autoTypeInfo), true}}, QoreString("softlist<auto>")) {
   }
};

class QoreSoftAutoListOrNothingTypeInfo : public QoreComplexSoftListTypeInfo {
public:
   DLLLOCAL QoreSoftAutoListOrNothingTypeInfo() : QoreComplexSoftListTypeInfo(q_accept_vec_t {
         {QoreComplexListTypeSpec(autoTypeInfo), nullptr},
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
         {NT_ALL, [] (QoreValue& n, ExceptionSink* xsink) {
               QoreListNode* l = new QoreListNode(autoTypeInfo);
               l->push(n, nullptr);
               n.assign(l);
            }
         },
      }, q_return_vec_t {{QoreComplexListTypeSpec(autoTypeInfo)}, {NT_NOTHING}}, QoreString("*softlist<auto>")) {
   }
};

class QoreTimeoutTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreTimeoutTypeInfo() : QoreTypeInfo("timeout", q_accept_vec_t {
         {NT_INT, nullptr},
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               int64 ms = n.get<const DateTimeNode>()->getRelativeMilliseconds();
               discard(n.assign(ms), xsink);
            }
         },
      }, q_return_vec_t {{NT_INT, true}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return true;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return 0ll;
   }
};

class QoreTimeoutOrNothingTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreTimeoutOrNothingTypeInfo() : QoreTypeInfo("*timeout", q_accept_vec_t {
         {NT_INT, nullptr},
         {NT_DATE, [] (QoreValue& n, ExceptionSink* xsink) {
               int64 ms = n.get<const DateTimeNode>()->getRelativeMilliseconds();
               discard(n.assign(ms), xsink);
            }
         },
         {NT_NOTHING, nullptr},
         {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) {
               n.assignNothing();
            }
         },
      }, q_return_vec_t {{NT_INT}, {NT_NOTHING}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreIntOrFloatTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreIntOrFloatTypeInfo() : QoreTypeInfo("int|float", q_accept_vec_t {
         {NT_INT, nullptr},
         {NT_FLOAT, nullptr},
      }, q_return_vec_t {{NT_INT}, {NT_FLOAT}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreIntFloatOrNumberTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreIntFloatOrNumberTypeInfo() : QoreTypeInfo("int|float|number", q_accept_vec_t {
         {NT_INT, nullptr},
         {NT_FLOAT, nullptr},
         {NT_NUMBER, nullptr},
      }, q_return_vec_t {{NT_INT}, {NT_FLOAT}, {NT_NUMBER}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

class QoreFloatOrNumberTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL QoreFloatOrNumberTypeInfo() : QoreTypeInfo("float|number", q_accept_vec_t {
         {NT_FLOAT, nullptr},
         {NT_NUMBER, nullptr},
      }, q_return_vec_t {{NT_FLOAT}, {NT_NUMBER}}) {
   }

protected:
   // returns true if there is no type or if the type can be converted to a scalar value, false if otherwise
   DLLLOCAL virtual bool canConvertToScalarImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }
};

#endif // _QORE_QORETYPEINFO_H
