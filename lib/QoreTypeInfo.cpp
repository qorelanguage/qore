/*
    QoreTypeInfo.cpp

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

#include <qore/Qore.h>
#include <qore/QoreRWLock.h>
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/qore_number_private.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/typed_hash_decl_private.h"
#include "qore/intern/qore_list_private.h"
#include "qore/intern/QoreHashNodeIntern.h"

const QoreAnyTypeInfo staticAnyTypeInfo;
const QoreAutoTypeInfo staticAutoTypeInfo;

const QoreBigIntTypeInfo staticBigIntTypeInfo;
const QoreBigIntOrNothingTypeInfo staticBigIntOrNothingTypeInfo;

const QoreStringTypeInfo staticStringTypeInfo;
const QoreStringOrNothingTypeInfo staticStringOrNothingTypeInfo;

const QoreBoolTypeInfo staticBoolTypeInfo;
const QoreBoolOrNothingTypeInfo staticBoolOrNothingTypeInfo;

const QoreBinaryTypeInfo staticBinaryTypeInfo;
const QoreBinaryOrNothingTypeInfo staticBinaryOrNothingTypeInfo;

const QoreSoftBinaryTypeInfo staticSoftBinaryTypeInfo;
const QoreSoftBinaryOrNothingTypeInfo staticSoftBinaryOrNothingTypeInfo;

const QoreObjectTypeInfo staticObjectTypeInfo;
const QoreObjectOrNothingTypeInfo staticObjectOrNothingTypeInfo;

const QoreDateTypeInfo staticDateTypeInfo;
const QoreDateOrNothingTypeInfo staticDateOrNothingTypeInfo;

const QoreHashTypeInfo staticHashTypeInfo;
const QoreHashOrNothingTypeInfo staticHashOrNothingTypeInfo;
const QoreEmptyHashTypeInfo staticEmptyHashTypeInfo;

const QoreAutoHashTypeInfo staticAutoHashTypeInfo;
const QoreAutoHashOrNothingTypeInfo staticAutoHashOrNothingTypeInfo;

const QoreListTypeInfo staticListTypeInfo;
const QoreListOrNothingTypeInfo staticListOrNothingTypeInfo;
const QoreEmptyListTypeInfo staticEmptyListTypeInfo;

const QoreAutoListTypeInfo staticAutoListTypeInfo;
const QoreAutoListOrNothingTypeInfo staticAutoListOrNothingTypeInfo;

const QoreNothingTypeInfo staticNothingTypeInfo;

const QoreNullTypeInfo staticNullTypeInfo;
const QoreNullOrNothingTypeInfo staticNullOrNothingTypeInfo;

const QoreClosureTypeInfo staticClosureTypeInfo;
const QoreClosureOrNothingTypeInfo staticClosureOrNothingTypeInfo;

const QoreCallReferenceTypeInfo staticCallReferenceTypeInfo;
const QoreCallReferenceOrNothingTypeInfo staticCallReferenceOrNothingTypeInfo;

const QoreReferenceTypeInfo staticReferenceTypeInfo;
const QoreReferenceOrNothingTypeInfo staticReferenceOrNothingTypeInfo;

const QoreHardReferenceTypeInfo staticHardReferenceTypeInfo;
const QoreHardReferenceOrNothingTypeInfo staticHardReferenceOrNothingTypeInfo;

const QoreNumberTypeInfo staticNumberTypeInfo;
const QoreNumberOrNothingTypeInfo staticNumberOrNothingTypeInfo;

const QoreFloatTypeInfo staticFloatTypeInfo;
const QoreFloatOrNothingTypeInfo staticFloatOrNothingTypeInfo;

const QoreCodeTypeInfo staticCodeTypeInfo;
const QoreCodeOrNothingTypeInfo staticCodeOrNothingTypeInfo;

const QoreDataTypeInfo staticDataTypeInfo;
const QoreDataOrNothingTypeInfo staticDataOrNothingTypeInfo;

const QoreSoftBigIntTypeInfo staticSoftBigIntTypeInfo;
const QoreSoftBigIntOrNothingTypeInfo staticSoftBigIntOrNothingTypeInfo;

const QoreSoftFloatTypeInfo staticSoftFloatTypeInfo;
const QoreSoftFloatOrNothingTypeInfo staticSoftFloatOrNothingTypeInfo;

const QoreSoftNumberTypeInfo staticSoftNumberTypeInfo;
const QoreSoftNumberOrNothingTypeInfo staticSoftNumberOrNothingTypeInfo;

const QoreSoftBoolTypeInfo staticSoftBoolTypeInfo;
const QoreSoftBoolOrNothingTypeInfo staticSoftBoolOrNothingTypeInfo;

const QoreSoftStringTypeInfo staticSoftStringTypeInfo;
const QoreSoftStringOrNothingTypeInfo staticSoftStringOrNothingTypeInfo;

const QoreSoftDateTypeInfo staticSoftDateTypeInfo;
const QoreSoftDateOrNothingTypeInfo staticSoftDateOrNothingTypeInfo;

const QoreSoftListTypeInfo staticSoftListTypeInfo;
const QoreSoftListOrNothingTypeInfo staticSoftListOrNothingTypeInfo;

const QoreSoftAutoListTypeInfo staticSoftAutoListTypeInfo;
const QoreSoftAutoListOrNothingTypeInfo staticSoftAutoListOrNothingTypeInfo;

const QoreTimeoutTypeInfo staticTimeoutTypeInfo;
const QoreTimeoutOrNothingTypeInfo staticTimeoutOrNothingTypeInfo;

const QoreIntOrFloatTypeInfo staticIntOrFloatTypeInfo;

const QoreIntFloatOrNumberTypeInfo staticIntFloatOrNumberTypeInfo;

const QoreFloatOrNumberTypeInfo staticFloatOrNumberTypeInfo;

const QoreTypeInfo* anyTypeInfo = &staticAnyTypeInfo,
   *autoTypeInfo = &staticAutoTypeInfo,
   *bigIntTypeInfo = &staticBigIntTypeInfo,
   *floatTypeInfo = &staticFloatTypeInfo,
   *boolTypeInfo = &staticBoolTypeInfo,
   *stringTypeInfo = &staticStringTypeInfo,
   *binaryTypeInfo = &staticBinaryTypeInfo,
   *dateTypeInfo = &staticDateTypeInfo,
   *objectTypeInfo = &staticObjectTypeInfo,
   *hashTypeInfo = &staticHashTypeInfo,
   *emptyHashTypeInfo = &staticEmptyHashTypeInfo,
   *autoHashTypeInfo = &staticAutoHashTypeInfo,
   *listTypeInfo = &staticListTypeInfo,
   *autoListTypeInfo = &staticAutoListTypeInfo,
   *emptyListTypeInfo = &staticEmptyListTypeInfo,
   *nothingTypeInfo = &staticNothingTypeInfo,
   *nullTypeInfo = &staticNullTypeInfo,
   *numberTypeInfo = &staticNumberTypeInfo,
   *runTimeClosureTypeInfo = &staticClosureTypeInfo,
   *callReferenceTypeInfo = &staticCallReferenceTypeInfo,
   *referenceTypeInfo = &staticReferenceTypeInfo,
   *codeTypeInfo = &staticCodeTypeInfo,
   *softBinaryTypeInfo = &staticSoftBinaryTypeInfo,
   *softBigIntTypeInfo = &staticSoftBigIntTypeInfo,
   *softFloatTypeInfo = &staticSoftFloatTypeInfo,
   *softNumberTypeInfo = &staticSoftNumberTypeInfo,
   *softBoolTypeInfo = &staticSoftBoolTypeInfo,
   *softStringTypeInfo = &staticSoftStringTypeInfo,
   *softDateTypeInfo = &staticSoftDateTypeInfo,
   *softListTypeInfo = &staticSoftListTypeInfo,
   *softAutoListTypeInfo = &staticSoftAutoListTypeInfo,
   *dataTypeInfo = &staticDataTypeInfo,
   *timeoutTypeInfo = &staticTimeoutTypeInfo,
   *bigIntOrFloatTypeInfo = &staticIntOrFloatTypeInfo,
   *bigIntFloatOrNumberTypeInfo = &staticIntFloatOrNumberTypeInfo,
   *floatOrNumberTypeInfo = &staticFloatOrNumberTypeInfo,

   *bigIntOrNothingTypeInfo = &staticBigIntOrNothingTypeInfo,
   *floatOrNothingTypeInfo = &staticFloatOrNothingTypeInfo,
   *numberOrNothingTypeInfo = &staticNumberOrNothingTypeInfo,
   *stringOrNothingTypeInfo = &staticStringOrNothingTypeInfo,
   *boolOrNothingTypeInfo = &staticBoolOrNothingTypeInfo,
   *binaryOrNothingTypeInfo = &staticBinaryOrNothingTypeInfo,
   *objectOrNothingTypeInfo = &staticObjectOrNothingTypeInfo,
   *dateOrNothingTypeInfo = &staticDateOrNothingTypeInfo,
   *hashOrNothingTypeInfo = &staticHashOrNothingTypeInfo,
   *autoHashOrNothingTypeInfo = &staticAutoHashOrNothingTypeInfo,
   *listOrNothingTypeInfo = &staticListOrNothingTypeInfo,
   *autoListOrNothingTypeInfo = &staticAutoListOrNothingTypeInfo,
   *nullOrNothingTypeInfo = &staticNullOrNothingTypeInfo,
   *codeOrNothingTypeInfo = &staticCodeOrNothingTypeInfo,
   *dataOrNothingTypeInfo = &staticDataOrNothingTypeInfo,
   *referenceOrNothingTypeInfo = &staticReferenceOrNothingTypeInfo,

   *softBinaryOrNothingTypeInfo = &staticSoftBinaryOrNothingTypeInfo,
   *softBigIntOrNothingTypeInfo = &staticSoftBigIntOrNothingTypeInfo,
   *softFloatOrNothingTypeInfo = &staticSoftFloatOrNothingTypeInfo,
   *softNumberOrNothingTypeInfo = &staticSoftNumberOrNothingTypeInfo,
   *softBoolOrNothingTypeInfo = &staticSoftBoolOrNothingTypeInfo,
   *softStringOrNothingTypeInfo = &staticSoftStringOrNothingTypeInfo,
   *softDateOrNothingTypeInfo = &staticSoftDateOrNothingTypeInfo,
   *softListOrNothingTypeInfo = &staticSoftListOrNothingTypeInfo,
   *softAutoListOrNothingTypeInfo = &staticSoftAutoListOrNothingTypeInfo,
   *timeoutOrNothingTypeInfo = &staticTimeoutOrNothingTypeInfo;

QoreListNode* emptyList;
QoreHashNode* emptyHash;
QoreStringNode* NullString;
DateTimeNode* ZeroDate, * OneDate;
QoreNumberNode* ZeroNumber, * NaNumber, * InfinityNumber, * piNumber;

// map from names used when parsing to types
typedef std::map<const char* , const QoreTypeInfo* , ltstr> str_typeinfo_map_t;
static str_typeinfo_map_t str_typeinfo_map;
static str_typeinfo_map_t str_ornothingtypeinfo_map;

// map from types to type info
typedef std::map<qore_type_t, const QoreTypeInfo* > type_typeinfo_map_t;
static type_typeinfo_map_t type_typeinfo_map;
static type_typeinfo_map_t type_ornothingtypeinfo_map;

// global external type map
static type_typeinfo_map_t extern_type_info_map;

// map from types to names
typedef std::map<qore_type_t, const char* > type_str_map_t;
static type_str_map_t type_str_map;

// map from simple types to "or nothing" types
typedef std::map<const QoreTypeInfo*, const QoreTypeInfo*> typeinfo_map_t;
static typeinfo_map_t typeinfo_map;

static QoreThreadLock ctl; // complex type lock

typedef std::map<const QoreTypeInfo*, QoreTypeInfo*> tmap_t;
tmap_t ch_map,          // complex hash map
   chon_map,            // complex hash or nothing map
   cl_map,              // complex list map
   clon_map,            // complex list or nothing map
   chr_map,             // complex hard reference map
   cr_map,              // complex reference map
   cron_map,            // complex reference or nothing map
   csl_map,             // complex softlist map
   cslon_map;           // complex softlist or nothing map

// rwlock for global type map
static QoreRWLock extern_type_info_map_lock;

static void do_maps(qore_type_t t, const char* name, const QoreTypeInfo* typeInfo, const QoreTypeInfo* orNothingTypeInfo) {
   str_typeinfo_map[name]          = typeInfo;
   str_ornothingtypeinfo_map[name] = orNothingTypeInfo;
   type_typeinfo_map[t]            = typeInfo;
   type_ornothingtypeinfo_map[t]   = orNothingTypeInfo;
   type_str_map[t]                 = name;
   typeinfo_map[typeInfo]          = orNothingTypeInfo;
}

// at least the NullString must be created after the default character encoding is set
void init_qore_types() {
   // initialize global default values
   NullString     = new QoreStringNode;
   ZeroDate       = DateTimeNode::makeAbsolute(0, 0, 0);
   OneDate        = DateTimeNode::makeAbsolute(0, 0, 0, 0, 0, 1);
   ZeroNumber     = new QoreNumberNode;
   NaNumber       = qore_number_private::getNaNumber();
   InfinityNumber = qore_number_private::getInfinity();
   piNumber       = qore_number_private::getPi();

   emptyList      = new QoreListNode;
   emptyHash      = new QoreHashNode;

   do_maps(NT_INT,         "int", bigIntTypeInfo, bigIntOrNothingTypeInfo);
   do_maps(NT_STRING,      "string", stringTypeInfo, stringOrNothingTypeInfo);
   do_maps(NT_BOOLEAN,     "bool", boolTypeInfo, boolOrNothingTypeInfo);
   do_maps(NT_FLOAT,       "float", floatTypeInfo, floatOrNothingTypeInfo);
   do_maps(NT_NUMBER,      "number", numberTypeInfo, numberOrNothingTypeInfo);
   do_maps(NT_BINARY,      "binary", binaryTypeInfo, binaryOrNothingTypeInfo);
   do_maps(NT_LIST,        "list", listTypeInfo, listOrNothingTypeInfo);
   do_maps(NT_HASH,        "hash", hashTypeInfo, hashOrNothingTypeInfo);
   do_maps(NT_OBJECT,      "object", objectTypeInfo, objectOrNothingTypeInfo);
   do_maps(NT_ALL,         "any", anyTypeInfo, anyTypeInfo);
   do_maps(NT_ALL,         "auto", autoTypeInfo, autoTypeInfo);
   do_maps(NT_DATE,        "date", dateTypeInfo, dateOrNothingTypeInfo);
   do_maps(NT_CODE,        "code", codeTypeInfo, codeOrNothingTypeInfo);
   do_maps(NT_DATA,        "data", dataTypeInfo, dataOrNothingTypeInfo);
   do_maps(NT_REFERENCE,   "reference", referenceTypeInfo, referenceOrNothingTypeInfo);
   do_maps(NT_NULL,        "null", nullTypeInfo, nullOrNothingTypeInfo);
   do_maps(NT_NOTHING,     "nothing", nothingTypeInfo, nothingTypeInfo);

   do_maps(NT_SOFTINT,     "softint", softBigIntTypeInfo, softBigIntOrNothingTypeInfo);
   do_maps(NT_SOFTFLOAT,   "softfloat", softFloatTypeInfo, softFloatOrNothingTypeInfo);
   do_maps(NT_SOFTNUMBER,  "softnumber", softNumberTypeInfo, softNumberOrNothingTypeInfo);
   do_maps(NT_SOFTBOOLEAN, "softbool", softBoolTypeInfo, softBoolOrNothingTypeInfo);
   do_maps(NT_SOFTSTRING,  "softstring", softStringTypeInfo, softStringOrNothingTypeInfo);
   do_maps(NT_SOFTDATE,    "softdate", softDateTypeInfo, softDateOrNothingTypeInfo);
   do_maps(NT_SOFTLIST,    "softlist", softListTypeInfo, softListOrNothingTypeInfo);
   do_maps(NT_SOFTBINARY,  "softbinary", softBinaryTypeInfo, softBinaryOrNothingTypeInfo);

   do_maps(NT_TIMEOUT,     "timeout", timeoutTypeInfo, timeoutOrNothingTypeInfo);

   // map the closure and callref strings to codeTypeInfo to ensure that these
   // types are always interchangeable
   do_maps(NT_RUNTIME_CLOSURE, "closure", codeTypeInfo, codeOrNothingTypeInfo);
   do_maps(NT_FUNCREF, "callref", codeTypeInfo, codeOrNothingTypeInfo);
}

void delete_qore_types() {
    // dereference global default values
    NullString->deref();
    piNumber->deref();
    InfinityNumber->deref();
    NaNumber->deref();
    ZeroNumber->deref();
    OneDate->deref();
    ZeroDate->deref();
    emptyList->deref(nullptr);
    emptyHash->deref(nullptr);

    // delete stored type information
    for (auto& i : ch_map)
        delete i.second;
    for (auto& i : chon_map)
        delete i.second;
    for (auto& i : cl_map)
        delete i.second;
    for (auto& i : clon_map)
        delete i.second;
    for (auto& i : chr_map)
        delete i.second;
    for (auto& i : cr_map)
        delete i.second;
    for (auto& i : cron_map)
        delete i.second;
    for (auto& i : csl_map)
        delete i.second;
    for (auto& i : cslon_map)
        delete i.second;
}

void add_to_type_map(qore_type_t t, const QoreTypeInfo* typeInfo) {
   QoreAutoRWWriteLocker al(extern_type_info_map_lock);
   assert(extern_type_info_map.find(t) == extern_type_info_map.end());
   extern_type_info_map[t] = typeInfo;
}

// public API
const QoreTypeInfo* qore_get_or_nothing_type(const QoreTypeInfo* typeInfo) {
   return get_or_nothing_type_check(typeInfo);
}

const QoreTypeInfo* get_or_nothing_type_check(const QoreTypeInfo* typeInfo) {
   return QoreTypeInfo::parseAcceptsReturns(typeInfo, NT_NOTHING) ? typeInfo : get_or_nothing_type(typeInfo);
}

const QoreTypeInfo* get_or_nothing_type(const QoreTypeInfo* typeInfo) {
    assert(!QoreTypeInfo::parseAcceptsReturns(typeInfo, NT_NOTHING));

    typeinfo_map_t::iterator i = typeinfo_map.find(typeInfo);
    if (i != typeinfo_map.end())
        return i->second;

    // see if we have a complex type
    {
        const TypedHashDecl* hd = QoreTypeInfo::getUniqueReturnHashDecl(typeInfo);
        if (hd)
            return hd->getTypeInfo(true);
    }

    {
        const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(typeInfo);
        if (qc) {
            return qc->getOrNothingTypeInfo();
        }
    }

    {
        const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexHash(typeInfo);
        if (ti)
            return qore_get_complex_hash_or_nothing_type(ti);
    }

    {
        const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexSoftList(typeInfo);
        if (ti)
            return qore_get_complex_softlist_or_nothing_type(ti);
    }

    {
        const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexList(typeInfo);
        if (ti)
            return qore_get_complex_list_or_nothing_type(ti);
    }

    {
        const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexReference(typeInfo);
        if (ti)
            return qore_get_complex_reference_or_nothing_type(ti);
    }

    // issue #2791: when performing type folding, do not set to type "any" but rather use "auto"
    return autoTypeInfo;
}

const QoreTypeInfo* qore_get_complex_hash_type(const QoreTypeInfo* vti) {
    if (vti == autoTypeInfo) {
        return autoHashTypeInfo;
    }
    if (vti == anyTypeInfo || !vti) {
        return hashTypeInfo;
    }

    AutoLocker al(ctl);

    tmap_t::iterator i = ch_map.lower_bound(vti);
    if (i != ch_map.end() && i->first == vti)
        return i->second;

    QoreComplexHashTypeInfo* ti = new QoreComplexHashTypeInfo(vti);
    ch_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_get_complex_hash_or_nothing_type(const QoreTypeInfo* vti) {
    if (vti == autoTypeInfo) {
        return autoHashOrNothingTypeInfo;
    }
    if (vti == anyTypeInfo || !vti) {
        return hashOrNothingTypeInfo;
    }

    AutoLocker al(ctl);

    tmap_t::iterator i = chon_map.lower_bound(vti);
    if (i != chon_map.end() && i->first == vti)
        return i->second;

    QoreComplexHashOrNothingTypeInfo* ti = new QoreComplexHashOrNothingTypeInfo(vti);
    chon_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_get_complex_list_type(const QoreTypeInfo* vti) {
    if (vti == autoTypeInfo) {
        return autoListTypeInfo;
    }
    if (vti == anyTypeInfo || !vti) {
        return listTypeInfo;
    }

    AutoLocker al(ctl);

    tmap_t::iterator i = cl_map.lower_bound(vti);
    if (i != cl_map.end() && i->first == vti)
        return i->second;

    QoreComplexListTypeInfo* ti = new QoreComplexListTypeInfo(vti);
    cl_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_get_complex_list_or_nothing_type(const QoreTypeInfo* vti) {
    if (vti == autoTypeInfo) {
        return autoListOrNothingTypeInfo;
    }
    if (vti == anyTypeInfo || !vti) {
        return listOrNothingTypeInfo;
    }

    AutoLocker al(ctl);

    tmap_t::iterator i = clon_map.lower_bound(vti);
    if (i != clon_map.end() && i->first == vti)
        return i->second;

    QoreComplexListOrNothingTypeInfo* ti = new QoreComplexListOrNothingTypeInfo(vti);
    clon_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_get_complex_softlist_type(const QoreTypeInfo* vti) {
    if (vti == autoTypeInfo) {
        return softAutoListTypeInfo;
    }
    if (vti == anyTypeInfo || !vti) {
        return softListTypeInfo;
    }

    AutoLocker al(ctl);

    tmap_t::iterator i = csl_map.lower_bound(vti);
    if (i != csl_map.end() && i->first == vti)
        return i->second;

    QoreComplexSoftListTypeInfo* ti = new QoreComplexSoftListTypeInfo(vti);
    csl_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_get_complex_softlist_or_nothing_type(const QoreTypeInfo* vti) {
    if (vti == autoTypeInfo) {
        return softAutoListOrNothingTypeInfo;
    }
    if (vti == anyTypeInfo || !vti) {
        return softListOrNothingTypeInfo;
    }

    AutoLocker al(ctl);

    tmap_t::iterator i = cslon_map.lower_bound(vti);
    if (i != cslon_map.end() && i->first == vti)
        return i->second;

    QoreComplexSoftListOrNothingTypeInfo* ti = new QoreComplexSoftListOrNothingTypeInfo(vti);
    cslon_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_get_complex_hard_reference_type(const QoreTypeInfo* vti) {
    AutoLocker al(ctl);

    tmap_t::iterator i = chr_map.lower_bound(vti);
    if (i != chr_map.end() && i->first == vti)
        return i->second;

    QoreComplexHardReferenceTypeInfo* ti = new QoreComplexHardReferenceTypeInfo(vti);
    chr_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_get_complex_reference_type(const QoreTypeInfo* vti) {
    AutoLocker al(ctl);

    tmap_t::iterator i = cr_map.lower_bound(vti);
    if (i != cr_map.end() && i->first == vti)
        return i->second;

    QoreComplexReferenceTypeInfo* ti = new QoreComplexReferenceTypeInfo(vti);
    cr_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_get_complex_reference_or_nothing_type(const QoreTypeInfo* vti) {
    AutoLocker al(ctl);

    tmap_t::iterator i = cron_map.lower_bound(vti);
    if (i != cron_map.end() && i->first == vti)
        return i->second;

    QoreComplexReferenceOrNothingTypeInfo* ti = new QoreComplexReferenceOrNothingTypeInfo(vti);
    cron_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

static const QoreTypeInfo* getExternalTypeInfoForType(qore_type_t t) {
    QoreAutoRWReadLocker al(extern_type_info_map_lock);
    type_typeinfo_map_t::iterator i = extern_type_info_map.find(t);
    return (i == extern_type_info_map.end() ? nullptr : i->second);
}

const QoreTypeInfo* getTypeInfoForType(qore_type_t t) {
    type_typeinfo_map_t::iterator i = type_typeinfo_map.find(t);
    return i != type_typeinfo_map.end() ? i->second : getExternalTypeInfoForType(t);
}

const QoreTypeInfo* getTypeInfoForValue(const AbstractQoreNode* n) {
    qore_type_t t = get_node_type(n);
    switch (t) {
        case NT_OBJECT:
            return static_cast<const QoreObject*>(n)->getClass()->getTypeInfo();
        case NT_WEAKREF:
            return static_cast<const WeakReferenceNode*>(n)->get()->getClass()->getTypeInfo();
        case NT_HASH:
            return static_cast<const QoreHashNode*>(n)->getTypeInfo();
        case NT_LIST:
            return static_cast<const QoreListNode*>(n)->getTypeInfo();
        case NT_REFERENCE:
            return static_cast<const ReferenceNode*>(n)->getTypeInfo();
        default:
            break;
    }
    return getTypeInfoForType(t);
}

const QoreTypeInfo* getBuiltinUserTypeInfo(const char* str) {
    str_typeinfo_map_t::iterator i = str_typeinfo_map.find(str);
    if (i == str_typeinfo_map.end())
        return nullptr;

    const QoreTypeInfo* rv = i->second;
    // return type "any" for reference types if PO_BROKEN_REFERENCES is set
    if (rv == referenceTypeInfo && (parse_get_parse_options() & PO_BROKEN_REFERENCES))
        rv = anyTypeInfo;
    return rv;
}

const QoreTypeInfo* getBuiltinUserOrNothingTypeInfo(const char* str) {
    str_typeinfo_map_t::iterator i = str_ornothingtypeinfo_map.find(str);
    if (i == str_ornothingtypeinfo_map.end())
        return nullptr;

    const QoreTypeInfo* rv = i->second;
    // return type "any" for reference types if PO_BROKEN_REFERENCES is set
    if (rv == referenceOrNothingTypeInfo && (parse_get_parse_options() & PO_BROKEN_REFERENCES))
        rv = anyTypeInfo;

    return rv;
}

const char* getBuiltinTypeName(qore_type_t type) {
    type_str_map_t::iterator i = type_str_map.find(type);
    if (i != type_str_map.end())
        return i->second;

    const QoreTypeInfo* typeInfo = getExternalTypeInfoForType(type);
    if (typeInfo)
        return QoreTypeInfo::getName(typeInfo);
    return "<unknown type>";
}

// only called for complex hashes and lists
static qore_type_result_e match_type(const QoreTypeInfo* this_type, const QoreTypeInfo* that_type, bool& may_not_match, bool& may_need_filter) {
    //printd(5, "match_type() '%s' <- '%s'\n", QoreTypeInfo::getName(this_type), QoreTypeInfo::getName(that_type));
    qore_type_result_e res = QoreTypeInfo::parseAccepts(this_type, that_type, may_not_match, may_need_filter);
    // if the type may not match at runtime, then return no match with %strict-types
    if (may_not_match && (parse_get_parse_options() & PO_STRICT_TYPES)) {
        return QTI_NOT_EQUAL;
    }

    // with strict-types, may not match must be interpreted as no match
    // however if we interpret "may not match" as "no match" here, then we introduce an incompatibility with
    // non-complex types
    // even if types are 100% compatible, if they are not equal, then we perform type folding
    if (res == QTI_IDENT && !may_need_filter && !QoreTypeInfo::equal(this_type, that_type)) {
        may_need_filter = true;
        res = QTI_AMBIGUOUS;
    }
    return res;
}

const char* QoreTypeSpec::getName() const {
    return QoreTypeInfo::getName(getTypeInfo());
}

const char* QoreTypeSpec::getSimpleName() const {
    switch (typespec) {
        case QTS_CLASS:
            return u.qc->getName();

        default:
            return QoreTypeInfo::getName(getTypeInfo());
    }
    assert(false);
    return nullptr;
}

qore_type_result_e QoreTypeSpec::match(const QoreTypeSpec& t, bool& may_not_match, bool& may_need_filter) const {
    //printd(5, "QoreTypeSpec::match() typespec: %d t.typespec: %d\n", (int)typespec, (int)t.typespec);
    switch (typespec) {
        case QTS_CLASS: {
            switch (t.typespec) {
                case QTS_CLASS:
                    return qore_class_private::get(*t.u.qc)->parseCheckCompatibleClass(*qore_class_private::get(*u.qc), may_not_match);
                default: {
                    qore_type_t tt = t.getType();
                    if (tt == NT_ALL || tt == NT_OBJECT) {
                        // if the type may not match at runtime, then return no match with %strict-types
                        if (parse_get_parse_options() & PO_STRICT_TYPES) {
                            return QTI_NOT_EQUAL;
                        }
                        may_not_match = true;
                        return QTI_AMBIGUOUS;
                    }
                    break;
                }
            }
            // see if the right side is a reference type
            return tryMatchReferenceType(t, may_not_match);
        }
        case QTS_HASHDECL: {
            switch (t.typespec) {
                /** NOTE: QTS_COMPLEXHASH with auto (hash<auto>) is not allowed on the RHS, even with
                    may_not_match = true, because ATM it would also match with any immediate hash without
                    a specific value type
                */
                case QTS_HASHDECL:
                    return typed_hash_decl_private::get(*t.u.hd)->parseEqual(*typed_hash_decl_private::get(*u.hd)) ? QTI_IDENT : QTI_NOT_EQUAL;
                case QTS_TYPE:
                    if (t.getType() == NT_ALL || t.getType() == NT_HASH) {
                        // if the type may not match at runtime, then return no match with %strict-types
                        if (parse_get_parse_options() & PO_STRICT_TYPES) {
                            return QTI_NOT_EQUAL;
                        }
                        may_not_match = true;
                        return QTI_AMBIGUOUS;
                    }
                    // fall down to the next case
                default: {
                    break;
                }
            }
            // see if the right side is a reference type
            return tryMatchReferenceType(t, may_not_match);
        }
        case QTS_COMPLEXHASH: {
            //printd(5, "QoreTypeSpec::match() %d: t.typespec: %d '%s'\n", typespec, (int)t.typespec, QoreTypeInfo::getName(u.ti));
            switch (t.typespec) {
                case QTS_COMPLEXHASH:
                    //printd(5, "QoreTypeSpec::match() t.typespec: complexlist <- %d '%s' <- '%s' rc: %d)\n", (int)t.typespec, QoreTypeInfo::getName(u.ti), QoreTypeInfo::getName(t.u.ti), match_type(u.ti, t.u.ti, may_not_match, may_need_filter));
                    return u.ti == autoTypeInfo
                        ? QTI_NEAR
                        : match_type(u.ti, t.u.ti, may_not_match, may_need_filter);
                case QTS_EMPTYHASH:
                    return QTI_NEAR;
                case QTS_HASHDECL:
                    return u.ti == autoTypeInfo
                        ? QTI_NEAR
                        : QTI_NOT_EQUAL;
                case QTS_TYPE:
                    if (t.getType() == NT_HASH && u.ti == autoTypeInfo) {
                        return QTI_NEAR;
                    }
                    if (t.getType() == NT_ALL) {
                        // if the type may not match at runtime, then return no match with %strict-types
                        if (parse_get_parse_options() & PO_STRICT_TYPES) {
                            return QTI_NOT_EQUAL;
                        }
                        may_not_match = true;
                        return QTI_AMBIGUOUS;
                    }
                    // fall down to the next case
                default: {
                    break;
                }
            }
            // see if the right side is a reference type
            return tryMatchReferenceType(t, may_not_match);
        }
        case QTS_COMPLEXSOFTLIST:
        case QTS_COMPLEXLIST: {
            //printd(5, "QoreTypeSpec::match() %d: t.typespec: %d '%s'\n", typespec, (int)t.typespec, QoreTypeInfo::getName(u.ti));
            switch (t.typespec) {
                case QTS_COMPLEXSOFTLIST:
                case QTS_COMPLEXLIST:
                    //printd(5, "QoreTypeSpec::match() t.typespec: complexlist <- %d '%s' <- '%s' rc: %d)\n", (int)t.typespec, QoreTypeInfo::getName(u.ti), QoreTypeInfo::getName(t.u.ti), match_type(u.ti, t.u.ti, may_not_match, may_need_filter));
                    return u.ti == autoTypeInfo
                        ? QTI_NEAR
                        : match_type(u.ti, t.u.ti, may_not_match, may_need_filter);
                case QTS_EMPTYLIST:
                    return QTI_NEAR;
                case QTS_TYPE:
                    if (t.getType() == NT_LIST && u.ti == autoTypeInfo) {
                        return QTI_NEAR;
                    }
                    if (t.getType() == NT_ALL) {
                        // if the type may not match at runtime, then return no match with %strict-types
                        if (parse_get_parse_options() & PO_STRICT_TYPES) {
                            return QTI_NOT_EQUAL;
                        }
                        may_not_match = true;
                        return QTI_AMBIGUOUS;
                    }
                    // fall down to the next case
                default: {
                    break;
                }
            }
            // see if the right side is a reference type
            return tryMatchReferenceType(t, may_not_match);
        }
        case QTS_COMPLEXREF: {
            //printd(5, "QoreTypeSpec::match() t.typespec: %d '%s'\n", (int)t.typespec, QoreTypeInfo::getName(u.ti));
            switch (t.typespec) {
                case QTS_COMPLEXHARDREF:
                case QTS_COMPLEXREF: {
                    //printd(5, "pcr: '%s' '%s' eq: %d ss: %d\n", QoreTypeInfo::getName(t.u.ti), QoreTypeInfo::getName(u.ti), QoreTypeInfo::equal(u.ti, t.u.ti), QoreTypeInfo::outputSuperSetOf(t.u.ti, u.ti));
                    // the passed argument's type must be a superset or equal to the reference type's subtype
                    // that is; if the types are different, the reference type's subtype must be more restrictive than the passed type's
                    qore_type_result_e ref_res = QoreTypeInfo::runtimeTypeMatch(t.u.ti, u.ti);
                    if (ref_res != QTI_NOT_EQUAL) {
                        return ref_res;
                    }
                    return QoreTypeInfo::outputSuperSetOf(t.u.ti, u.ti) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
                }
                case QTS_HARDREF: {
                    return QTI_IDENT;
                }
                case QTS_TYPE:
                    if (t.getType() == NT_REFERENCE) {
                        // if the type may not match at runtime, then return no match with %strict-types
                        if (parse_get_parse_options() & PO_STRICT_TYPES) {
                            return QTI_NOT_EQUAL;
                        }
                        may_not_match = true;
                        return QTI_AMBIGUOUS;
                    }
                    // check if types match
                    return checkMatchType(t, may_not_match);

                case QTS_EMPTYLIST:
                case QTS_EMPTYHASH: {
                    // check if types match
                    if (!u.ti || u.ti->accept_vec.empty()) {
                        return QTI_AMBIGUOUS;
                    }
                    qore_type_result_e rv = QTI_NOT_EQUAL;
                    for (auto& at : u.ti->accept_vec) {
                        qore_type_result_e res = at.spec.checkMatchType(t, may_not_match);
                        if (res == QTI_NOT_EQUAL && !may_not_match) {
                            may_not_match = true;
                        }
                        if (res > rv) {
                            rv = res;
                        }
                    }
                    if (u.ti->accept_vec.size() == 1) {
                        return rv;
                    }
                    if (rv > QTI_AMBIGUOUS) {
                        return QTI_AMBIGUOUS;
                    }
                    return rv;
                }

                default:
                    return QTI_NOT_EQUAL;
            }
            return QTI_NOT_EQUAL;
        }
        case QTS_TYPE:
            if (u.t == NT_REFERENCE) {
                return QTI_AMBIGUOUS;
            }
            // fall down to next case
        case QTS_EMPTYLIST:
        case QTS_EMPTYHASH: {
            qore_type_result_e rv = checkMatchType(t, may_not_match);
            if (rv == QTI_NOT_EQUAL) {
                // see if the right side is a reference type
                rv = tryMatchReferenceType(t, may_not_match);
            }
            return rv;
        }
        case QTS_COMPLEXHARDREF: {
            switch (t.typespec) {
                case QTS_HARDREF: {
                    return QTI_IDENT;
                }
                case QTS_COMPLEXHARDREF: {
                    qore_type_result_e rv = QoreTypeInfo::parseAccepts(t.u.ti, u.ti, may_not_match, may_need_filter);
                    if (may_not_match && rv > QTI_NOT_EQUAL) {
                        rv = QTI_NOT_EQUAL;
                    }
                    return rv;
                }
                case QTS_TYPE:
                    return t.u.t == NT_REFERENCE ? QTI_IDENT : QTI_NOT_EQUAL;
                default:
                    break;
            }
        }
        case QTS_HARDREF: {
            return checkMatchType(t, may_not_match);
        }
    }
    return QTI_NOT_EQUAL;
}

qore_type_result_e QoreTypeSpec::checkMatchType(const QoreTypeSpec& t, bool& may_not_match) const {
    qore_type_t ot = t.getType();
    if (u.t == NT_ALL) {
        // issue #3887 if both sides are "ALL" then the match is "NEAR" and not "WILDCARD"
        return ot == NT_ALL ? QTI_NEAR : QTI_WILDCARD;
    }
    if (ot == NT_ALL) {
        // if the type may not match at runtime, then return no match with %strict-types
        if (parse_get_parse_options() & PO_STRICT_TYPES) {
            return QTI_NOT_EQUAL;
        }
        may_not_match = true;
        return QTI_AMBIGUOUS;
    }
    if (u.t == ot) {
        // check special cases
        if ((u.t == NT_LIST || u.t == NT_HASH) && t.typespec != QTS_TYPE && t.typespec != QTS_EMPTYLIST && t.typespec != QTS_EMPTYHASH)
            return QTI_NEAR;
        return QTI_IDENT;
    }
    return QTI_NOT_EQUAL;
}

qore_type_result_e QoreTypeSpec::tryMatchReferenceType(const QoreTypeSpec& t, bool& may_not_match) const {
    switch (t.typespec) {
        case QTS_TYPE: {
            return t.u.t == NT_REFERENCE ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
        }

        case QTS_COMPLEXREF: {
            // check if types match
            if (!t.u.ti || t.u.ti->return_vec.empty()) {
                return QTI_AMBIGUOUS;
            }
            qore_type_result_e rv = QTI_NOT_EQUAL;
            for (auto& rt : t.u.ti->return_vec) {
                qore_type_result_e res = checkMatchType(rt.spec, may_not_match);
                if (res == QTI_NOT_EQUAL && !may_not_match) {
                    may_not_match = true;
                }
                if (res > rv) {
                    rv = res;
                }
            }
            if (t.u.ti->return_vec.size() == 1) {
                return rv;
            }
            if (rv > QTI_AMBIGUOUS) {
                return QTI_AMBIGUOUS;
            }
            return rv;
        }

        default:
            break;
    }
    return QTI_NOT_EQUAL;
}

qore_type_result_e QoreTypeSpec::matchType(qore_type_t t) const {
    if (typespec == QTS_CLASS) {
        return t == NT_OBJECT ? QTI_IDENT : QTI_NOT_EQUAL;
    } else if (typespec == QTS_HASHDECL || typespec == QTS_COMPLEXHASH) {
        return t == NT_HASH ? QTI_IDENT : QTI_NOT_EQUAL;
    } else if (typespec == QTS_COMPLEXLIST || typespec == QTS_COMPLEXSOFTLIST) {
        return t == NT_LIST ? QTI_IDENT : QTI_NOT_EQUAL;
    } else if (typespec == QTS_COMPLEXHARDREF || typespec == QTS_HARDREF) {
        return t == NT_REFERENCE ? QTI_IDENT : QTI_NOT_EQUAL;
    } else if (typespec == QTS_COMPLEXREF) {
        if (t == NT_REFERENCE) {
            return QTI_NEAR;
        }
        if (QoreTypeInfo::hasType(u.ti)) {
            return QoreTypeInfo::parseAcceptsReturns(u.ti, t) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
        }
        return QTI_WILDCARD;
    }
    if (u.t == NT_ALL || u.t == NT_REFERENCE)
        return QTI_WILDCARD;
    return u.t == t ? QTI_IDENT : QTI_NOT_EQUAL;
}

static bool type_spec_accept_object(const QoreClass& type_class, const QoreClass& object_class, bool& priv_error) {
    assert(!priv_error);

    bool priv;
    if (!object_class.getClass(type_class, priv)) {
        return false;
    }
    if (!priv) {
        return true;
    }
    // check access
    if (qore_class_private::runtimeCheckPrivateClassAccess(type_class)) {
        return true;
    }
    priv_error = true;
    return false;
}

bool QoreTypeSpec::isAutoType() const {
    if (typespec == QTS_COMPLEXHASH || typespec == QTS_COMPLEXLIST || typespec == QTS_COMPLEXSOFTLIST) {
        return QoreTypeInfo::isAutoType(u.ti);
    }
    return false;
}

bool QoreTypeSpec::acceptInput(ExceptionSink* xsink, const QoreTypeInfo& typeInfo, q_type_map_t map,
    const char* arg_type, bool obj, int param_num, const char* param_name, QoreValue& n,
    LValueHelper* lvhelper) const {
    bool priv_error = false;
    bool ok = false;

    switch (typespec) {
        case QTS_CLASS: {
            if (n.getType() == NT_OBJECT) {
                ok = type_spec_accept_object(*u.qc, *n.get<const QoreObject>()->getClass(), priv_error);
            } else if (n.getType() == NT_WEAKREF) {
                ok = type_spec_accept_object(*u.qc, *n.get<const WeakReferenceNode>()->get()->getClass(), priv_error);
            }
            break;
        }
        case QTS_HASHDECL: {
            if (n.getType() == NT_HASH) {
                const TypedHashDecl* hd = n.get<const QoreHashNode>()->getHashDecl();
                if (hd && typed_hash_decl_private::get(*hd)->equal(*typed_hash_decl_private::get(*u.hd))) {
                    ok = true;
                    break;
                }
            }
            break;
        }
        case QTS_COMPLEXHASH: {
            if (n.getType() == NT_HASH) {
                if (QoreTypeInfo::isAutoType(&typeInfo)) {
                    ok = true;
                    break;
                }
                QoreHashNode* h = n.get<QoreHashNode>();
                const QoreTypeInfo* ti = h->getValueTypeInfo();
                if (QoreTypeInfo::equal(u.ti, ti)) {
                    ok = true;
                    break;
                }

                // try to fold values into our type; value types are not identical;
                // we have to get a new hash
                if (!h->is_unique()) {
                    AbstractQoreNode* p = n.assign(h = qore_hash_private::get(*h)->copy(&typeInfo));
                    if (lvhelper) {
                        lvhelper->saveTemp(p);
                    } else {
                        discard(p, xsink);
                        if (xsink && *xsink) {
                            return true;
                        }
                    }
                } else {
                    qore_hash_private::get(*h)->complexTypeInfo = &typeInfo;
                }

                // now we have to fold the value types into our type
                HashIterator i(h);
                while (i.next()) {
                    hash_assignment_priv ha(*qore_hash_private::get(*h), *qhi_priv::get(i)->i);
                    QoreValue hn(ha.swap(QoreValue()));
                    u.ti->acceptInputIntern(xsink, arg_type, obj, param_num, param_name, hn, lvhelper);
                    ha.swap(hn);
                    if (xsink && *xsink) {
                        return true;
                    }
                }

                ok = true;
            }
            break;
        }
        case QTS_COMPLEXSOFTLIST:
        case QTS_COMPLEXLIST: {
            if (n.getType() == NT_LIST) {
                if (QoreTypeInfo::isAutoType(&typeInfo)) {
                    ok = true;
                    break;
                }
                QoreListNode* l = n.get<QoreListNode>();
                const QoreTypeInfo* ti = l->getValueTypeInfo();
                if (QoreTypeInfo::equal(u.ti, ti)) {
                    ok = true;
                    break;
                }

                // try to fold values into our type; value types are not identical;
                // we have to get a new list
                qore_list_private* lp;
                if (!l->is_unique()) {
                    AbstractQoreNode* p = n.assign(l = qore_list_private::get(*l)->copy(&typeInfo));
                    if (lvhelper) {
                        lvhelper->saveTemp(p);
                    } else {
                        discard(p, xsink);
                        if (xsink && *xsink) {
                            return true;
                        }
                    }
                    lp = qore_list_private::get(*l);
                } else {
                    lp = qore_list_private::get(*l);
                    lp->complexTypeInfo = &typeInfo;
                }

                // now we have to fold the value types into our type
                for (size_t i = 0; i < l->size(); ++i) {
                    QoreValue ln(lp->takeExists(i));
                    u.ti->acceptInputIntern(xsink, arg_type, obj, param_num, param_name, ln, lvhelper);
                    lp->swap(i, ln);
                    if (xsink && *xsink) {
                        return true;
                    }
                }

                ok = true;
            } else if (typespec == QTS_COMPLEXSOFTLIST) {
                QoreValue val = n;
                n.swap(val);
                n.assign(qore_list_private::newComplexListFromValue(&typeInfo, val, xsink));
                ok = true;
            }
            break;
        }
        case QTS_HARDREF: {
            if (n.getType() == NT_REFERENCE) {
                ok = true;
            }
            break;
        }
        case QTS_COMPLEXHARDREF:
        case QTS_COMPLEXREF: {
            if (n.getType() == NT_REFERENCE) {
                // issue #2889 cannot assign a reference while assigning an lvalue and holding a write lock
                assert(!lvhelper);
                ReferenceNode* r = n.get<ReferenceNode>();
                const QoreTypeInfo* ti = r->getLValueTypeInfo();
                //printd(5, "cr: %p '%s' == %p '%s': %d\n", u.ti, QoreTypeInfo::getName(u.ti), ti, QoreTypeInfo::getName(ti), QoreTypeInfo::outputSuperSetOf(ti, u.ti));
                // first check types before instantiating reference
                if (QoreTypeInfo::outputSuperSetOf(ti, u.ti)) {
                    // issue #2891: do not create a value in the source reference if none already exists
                    // do not process if there is no type restriction
                    LValueHelper lvh(r, xsink, true);
                    //printd(5, "lvh: %d *xsink: %d\n", (bool)lvh, (bool)*xsink);
                    if (lvh) {
                        QoreValue val = lvh.getReferencedValue();
                        if (!val.isNothing()) {
                            lvh.setTypeInfo(u.ti);
                            //printd(5, "ref assign '%s' to '%s'\n", QoreTypeInfo::getName(val.getTypeInfo()), QoreTypeInfo::getName(u.ti));
                            lvh.assign(val, "<reference>");
                        }
                        // we set ok unconditionally here, because any exception thrown above is enough if there is an error
                        ok = true;
                    } else if (!xsink || !*xsink) {
                        // issue #2891 the lvalue may not exist, but we can still perform the assignment
                        ok = true;
                    }
                }
            }
            break;
        }
        case QTS_TYPE:
        case QTS_EMPTYLIST:
        case QTS_EMPTYHASH:
            if (u.t == NT_ALL || u.t == n.getType())
                ok = true;
            break;
    }

    if (ok) {
        assert(!priv_error);
        if (map) {
            map(n, xsink);
            if (xsink && *xsink) {
                xsink->appendLastDescription(" (while converting types for type '%s')",
                    QoreTypeInfo::getName(&typeInfo));
            }
        }
        return true;
    }

    if (priv_error) {
        typeInfo.doAcceptError(true, arg_type, obj, param_num, param_name, n, xsink);
        return true;
    }
    return false;
}

bool QoreTypeSpec::operator==(const QoreTypeSpec& other) const {
    if (typespec != other.typespec)
        return false;
    switch (typespec) {
        case QTS_TYPE:
        case QTS_EMPTYLIST:
        case QTS_EMPTYHASH:
            return u.t == other.u.t;
        case QTS_CLASS:
            return qore_class_private::get(*u.qc)->equal(*qore_class_private::get(*other.u.qc));
        case QTS_HASHDECL:
            return typed_hash_decl_private::get(*u.hd)->equal(*typed_hash_decl_private::get(*other.u.hd));
        case QTS_COMPLEXHASH:
        case QTS_COMPLEXLIST:
        case QTS_COMPLEXSOFTLIST:
        case QTS_COMPLEXHARDREF:
        case QTS_COMPLEXREF:
            return QoreTypeInfo::equal(u.ti, other.u.ti);
        case QTS_HARDREF:
             return true;
    }
    return false;
}

bool QoreTypeSpec::operator!=(const QoreTypeSpec& other) const {
   return !(*this == other);
}

qore_type_result_e QoreTypeSpec::runtimeAcceptsValue(const QoreValue& n, bool exact) const {
    qore_type_t ot = n.getType();
    // issue #2928 we must ensure that each typespec only access its own data in the union
    switch (typespec) {
        case QTS_CLASS:
            if (ot == NT_OBJECT) {
                qore_type_result_e rv = qore_class_private::runtimeCheckCompatibleClass(*u.qc,
                    *n.get<const QoreObject>()->getClass());
                if (rv == QTI_NOT_EQUAL) {
                    return rv;
                }
                // issue #3272: do not return a match for deleted objects
                if (!n.get<const QoreObject>()->isValid()) {
                    return QTI_NOT_EQUAL;
                }
                return (rv == QTI_IDENT && exact) ? QTI_IDENT : QTI_AMBIGUOUS;
            } else if (ot == NT_WEAKREF) {
                qore_type_result_e rv = qore_class_private::runtimeCheckCompatibleClass(*u.qc,
                    *n.get<const WeakReferenceNode>()->get()->getClass());
                if (rv == QTI_NOT_EQUAL) {
                    return rv;
                }
                // issue #3272: do not return a match for deleted objects
                if (!n.get<const WeakReferenceNode>()->get()->isValid()) {
                    return QTI_NOT_EQUAL;
                }
                return (rv == QTI_IDENT && exact) ? QTI_IDENT : QTI_AMBIGUOUS;
            }
            return QTI_NOT_EQUAL;

        case QTS_HASHDECL:
            if (ot == NT_HASH) {
                const TypedHashDecl* hd = n.get<const QoreHashNode>()->getHashDecl();
                if (hd && typed_hash_decl_private::get(*u.hd)->equal(*typed_hash_decl_private::get(*hd)))
                    return exact ? QTI_IDENT : QTI_AMBIGUOUS;
            }
            return QTI_NOT_EQUAL;

        case QTS_COMPLEXHASH:
            if (ot == NT_HASH) {
                if (u.ti == autoTypeInfo) {
                    return QTI_NEAR;
                }
                const QoreTypeInfo* ti = n.get<const QoreHashNode>()->getValueTypeInfo();
                if (ti && QoreTypeInfo::equal(u.ti, ti))
                    return exact ? QTI_IDENT : QTI_AMBIGUOUS;
            }
            return QTI_NOT_EQUAL;

        case QTS_COMPLEXLIST:
        case QTS_COMPLEXSOFTLIST:
            if (ot == NT_LIST) {
                if (u.ti == autoTypeInfo) {
                    return QTI_NEAR;
                }
                const QoreTypeInfo* ti = n.get<const QoreListNode>()->getValueTypeInfo();
                if (ti && QoreTypeInfo::equal(u.ti, ti))
                    return exact ? QTI_IDENT : QTI_AMBIGUOUS;
            }
            return typespec == QTS_COMPLEXSOFTLIST ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;

        case QTS_COMPLEXREF:
            if (ot == NT_REFERENCE) {
                const QoreTypeInfo* ti = n.get<const ReferenceNode>()->getLValueTypeInfo();
                //printd(5, "QoreTypeSpec::runtimeAcceptsValue() cr ti: '%s' typeInfo: '%s' eq: %d ss: %d\n", QoreTypeInfo::getName(ti), QoreTypeInfo::getName(u.ti), QoreTypeInfo::equal(u.ti, ti), QoreTypeInfo::outputSuperSetOf(ti, u.ti));
                if (QoreTypeInfo::equal(u.ti, ti))
                    return QTI_IDENT;
                if (QoreTypeInfo::outputSuperSetOf(ti, u.ti))
                    return exact ? QTI_IDENT : QTI_AMBIGUOUS;
            }
            return QTI_NOT_EQUAL;

        case QTS_EMPTYLIST:
        case QTS_EMPTYHASH:
        case QTS_TYPE:
            // check special cases
            if (u.t == NT_HASH && ot == NT_HASH) {
                const qore_hash_private* h = qore_hash_private::get(*n.get<const QoreHashNode>());
                if (h->hashdecl || h->complexTypeInfo)
                    return QTI_NEAR;
                return exact ? QTI_IDENT : QTI_AMBIGUOUS;
            }
            if (u.t == NT_LIST && ot == NT_LIST) {
                const qore_list_private* l = qore_list_private::get(*n.get<const QoreListNode>());
                if (l->complexTypeInfo)
                    return QTI_NEAR;
                return exact ? QTI_IDENT : QTI_AMBIGUOUS;
            }

            if (u.t == NT_ALL || u.t == ot) {
                return exact ? QTI_IDENT : QTI_AMBIGUOUS;
            }
            break;

        default:
            assert(false);
    }

    return QTI_NOT_EQUAL;
}

qore_type_result_e QoreTypeInfo::runtimeAcceptsValue(const QoreValue& n) const {
    for (auto& t : accept_vec) {
        qore_type_result_e rv = t.spec.runtimeAcceptsValue(n, t.exact);
        if (rv != QTI_NOT_EQUAL)
            return rv;
    }
    return QTI_NOT_EQUAL;
}

void QoreTypeInfo::doNonNumericWarning(const QoreProgramLocation* loc, const char* preface) const {
    QoreStringNode* desc = new QoreStringNode(preface);
    getThisTypeImpl(*desc);
    desc->sprintf(", which does not evaluate to a numeric type, therefore will always evaluate to 0 at runtime");
    qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

void QoreTypeInfo::doNonBooleanWarning(const QoreProgramLocation* loc, const char* preface) const {
    QoreStringNode* desc = new QoreStringNode(preface);
    getThisTypeImpl(*desc);
    desc->sprintf(", which does not evaluate to a numeric or boolean type, therefore will always evaluate to False at runtime");
    qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

void QoreTypeInfo::doNonStringWarning(const QoreProgramLocation* loc, const char* preface) const {
    QoreStringNode* desc = new QoreStringNode(preface);
    getThisTypeImpl(*desc);
    desc->sprintf(", which cannot be converted to a string, therefore will always evaluate to an empty string at runtime");
    qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

void QoreTypeInfo::doNonStringError(const QoreProgramLocation* loc, const char* preface) const {
    QoreStringNode* desc = new QoreStringNode(preface);
    getThisTypeImpl(*desc);
    desc->sprintf(", which cannot be converted to a string, therefore will always evaluate to an empty string at runtime");
    parseException(*loc, "INVALID-OPERATION", desc);
}

void QoreTypeInfo::stripTypeInfo(QoreValue& n, ExceptionSink* xsink, LValueHelper* lvhelper) {
    // strips complex typeinfo for an assignment to an untyped lvalue
    switch (n.getType()) {
        case NT_HASH: {
            if (lvhelper) {
                map_get_plain_hash_lvalue(n, xsink, lvhelper);
            } else {
                map_get_plain_hash(n, xsink);
            }
            break;
        }
        case NT_LIST: {
            if (lvhelper) {
                map_get_plain_list_lvalue(n, xsink, lvhelper);
            } else {
                map_get_plain_list(n, xsink);
            }
            break;
        }
    }
}

const QoreTypeInfo* QoreTypeInfo::getHardReference(const QoreTypeInfo* ti) {
    if (!hasType(ti)) {
        return ti;
    }
    if (!QoreTypeInfo::parseAcceptsReturns(ti, NT_REFERENCE)) {
        return ti;
    }
    if (ti == referenceTypeInfo) {
        return &staticHardReferenceTypeInfo;
    }
    if (ti == referenceOrNothingTypeInfo) {
        return &staticHardReferenceOrNothingTypeInfo;
    }
    {
        const QoreComplexReferenceTypeInfo* type = dynamic_cast<const QoreComplexReferenceTypeInfo*>(ti);
        if (type) {
            return type->getHardReference();
        }
    }
    const QoreComplexReferenceOrNothingTypeInfo* type = dynamic_cast<const QoreComplexReferenceOrNothingTypeInfo*>(ti);
    assert(type);
    return type->getHardReference();
}

#if 0
const QoreTypeInfo* QoreTypeInfo::getRuntimeType(const QoreTypeInfo* ti) {
    if (!hasType(ti)) {
        return ti;
    }
    if (!QoreTypeInfo::parseAcceptsReturns(ti, NT_REFERENCE)) {
        return ti;
    }
    if (dynamic_cast<const QoreReferenceTypeInfo*>(ti) || dynamic_cast<const QoreReferenceOrNothingTypeInfo*>(ti)) {
        return autoTypeInfo;
    }
    {
        const QoreComplexReferenceTypeInfo* type = dynamic_cast<const QoreComplexReferenceTypeInfo*>(ti);
        if (type) {
            return getRuntimeType(type->getRuntimeType());
        }
    }
    const QoreComplexReferenceOrNothingTypeInfo* type = dynamic_cast<const QoreComplexReferenceOrNothingTypeInfo*>(ti);
    assert(type);
    return getRuntimeType(type->getRuntimeType());
}
#endif

template <typename T>
bool typespec_vec_compare(const T& a, const T& b) {
    if (a.size() != b.size())
        return false;
    for (unsigned i = 0; i < a.size(); ++i) {
        if (a[i].spec != b[i].spec)
            return false;
    }
    return true;
}

bool accept_vec_compare(const q_accept_vec_t& a, const q_accept_vec_t& b) {
    return typespec_vec_compare<q_accept_vec_t>(a, b);
}

bool return_vec_compare(const q_return_vec_t& a, const q_return_vec_t& b) {
    return typespec_vec_compare<q_return_vec_t>(a, b);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveRuntime() const {
    if (!subtypes.empty())
        return resolveRuntimeSubtype();

    const QoreTypeInfo* rv = or_nothing ? getBuiltinUserOrNothingTypeInfo(cscope->ostr) : getBuiltinUserTypeInfo(cscope->ostr);
    return rv ? rv : resolveRuntimeClass(*cscope, or_nothing);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveRuntimeSubtype() const {
    if (!strcmp(cscope->ostr, "hash")) {
        if (subtypes.size() == 1) {
            if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
                return or_nothing ? autoHashOrNothingTypeInfo : autoHashTypeInfo;
            // resolve hashdecl
            const qore_ns_private* ns;
            const TypedHashDecl* hd = qore_root_ns_private::get(*getRootNS())->runtimeFindHashDeclIntern(*subtypes[0]->cscope, ns);
            if (!hd)
                return nullptr;
            return hd->getTypeInfo(or_nothing);
        }
        if (subtypes.size() == 2) {
            if (strcmp(subtypes[0]->cscope->ostr, "string")) {
                return nullptr;
            } else {
                if (!strcmp(subtypes[1]->cscope->ostr, "auto"))
                return or_nothing ? autoHashOrNothingTypeInfo : autoHashTypeInfo;

                // resolve value type
                const QoreTypeInfo* valueType = subtypes[1]->resolveRuntime();
                if (!valueType)
                return nullptr;
                if (QoreTypeInfo::hasType(valueType)) {
                return !or_nothing
                    ? qore_get_complex_hash_type(valueType)
                    : qore_get_complex_hash_or_nothing_type(valueType);
                }
            }
        } else {
            return nullptr;
        }
        return or_nothing ? hashOrNothingTypeInfo : hashTypeInfo;
    }
    if (!strcmp(cscope->ostr, "list")) {
        if (subtypes.size() == 1) {
            if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
                return or_nothing ? autoListOrNothingTypeInfo : autoListTypeInfo;
            // resolve value type
            const QoreTypeInfo* valueType = subtypes[0]->resolveRuntime();
            if (!valueType)
                return nullptr;
            if (QoreTypeInfo::hasType(valueType)) {
                return !or_nothing
                ? qore_get_complex_list_type(valueType)
                : qore_get_complex_list_or_nothing_type(valueType);
            }
        } else {
            return nullptr;
        }
        return or_nothing ? listOrNothingTypeInfo : listTypeInfo;
    }
    if (!strcmp(cscope->ostr, "softlist")) {
        if (subtypes.size() == 1) {
            if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
                return or_nothing ? softAutoListOrNothingTypeInfo : softAutoListTypeInfo;
            // resolve value type
            const QoreTypeInfo* valueType = subtypes[0]->resolveRuntime();
            if (!valueType)
                return nullptr;
            if (QoreTypeInfo::hasType(valueType)) {
                return !or_nothing
                ? qore_get_complex_softlist_type(valueType)
                : qore_get_complex_softlist_or_nothing_type(valueType);
            }
        } else {
            return nullptr;
        }
        return or_nothing ? softListOrNothingTypeInfo : softListTypeInfo;
    }
    if (!strcmp(cscope->ostr, "reference")) {
        if (subtypes.size() == 1) {
            if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
                return or_nothing ? referenceOrNothingTypeInfo : referenceTypeInfo;
            // resolve value type
            const QoreTypeInfo* valueType = subtypes[0]->resolveRuntime();
            if (!valueType)
                return nullptr;
            if (QoreTypeInfo::hasType(valueType)) {
                return !or_nothing
                ? qore_get_complex_reference_type(valueType)
                : qore_get_complex_reference_or_nothing_type(valueType);
            }
        } else {
            return nullptr;
        }
        return or_nothing ? referenceOrNothingTypeInfo : referenceTypeInfo;
    }

    if (!strcmp(cscope->ostr, "object")) {
        if (subtypes.size() != 1) {
            return nullptr;
        }

        if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
            return or_nothing ? objectOrNothingTypeInfo : objectTypeInfo;

        // resolve class
        return resolveRuntimeClass(*subtypes[0]->cscope, or_nothing);
    }
    return nullptr;
}

const QoreTypeInfo* QoreParseTypeInfo::resolveRuntimeClass(const NamedScope& cscope, bool or_nothing) {
    // resolve class
    const QoreClass* qc = qore_root_ns_private::get(*getRootNS())->runtimeFindScopedClass(cscope);
    if (!qc)
        return nullptr;

    return or_nothing ? qc->getOrNothingTypeInfo() : qc->getTypeInfo();
}

const QoreTypeInfo* QoreParseTypeInfo::resolveSubtype(const QoreProgramLocation* loc) const {
    if (!strcmp(cscope->ostr, "hash")) {
        if (subtypes.size() == 1) {
            if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
                return or_nothing ? autoHashOrNothingTypeInfo : autoHashTypeInfo;
            // resolve hashdecl
            const TypedHashDecl* hd = qore_root_ns_private::get(*getRootNS())->parseFindHashDecl(loc, *subtypes[0]->cscope);
            //printd(5, "QoreParseTypeInfo::resolveSubtype() this: %p '%s' hd: %p '%s' type: %p (pgm: %p)\n", this, getName(), hd, hd ? hd->getName() : "n/a", hd ? hd->getTypeInfo(false) : nullptr, getProgram());
            return hd ? hd->getTypeInfo(or_nothing) : hashTypeInfo;
        }
        if (subtypes.size() == 2) {
            if (strcmp(subtypes[0]->cscope->ostr, "string")) {
                parseException(*loc, "PARSE-TYPE-ERROR", "invalid complex hash type '%s'; hash key type must be 'string'; cannot declare a hash with key type '%s'", getName(), subtypes[0]->cscope->ostr);
            } else {
                if (!strcmp(subtypes[1]->cscope->ostr, "auto"))
                return or_nothing ? autoHashOrNothingTypeInfo : autoHashTypeInfo;

                // resolve value type
                const QoreTypeInfo* valueType = QoreParseTypeInfo::resolveAny(subtypes[1], loc);
                if (QoreTypeInfo::hasType(valueType)) {
                return !or_nothing
                    ? qore_get_complex_hash_type(valueType)
                    : qore_get_complex_hash_or_nothing_type(valueType);
                }
            }
        } else {
            parseException(*loc, "PARSE-TYPE-ERROR", "cannot resolve '%s' with %d type arguments; base type 'hash' takes a single hashdecl name as a subtype argument or two type names giving the key and value types", getName(), (int)subtypes.size());
        }
        return or_nothing ? hashOrNothingTypeInfo : hashTypeInfo;
    }
    if (!strcmp(cscope->ostr, "list")) {
        if (subtypes.size() == 1) {
            if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
                return or_nothing ? autoListOrNothingTypeInfo : autoListTypeInfo;
            // resolve value type
            const QoreTypeInfo* valueType = QoreParseTypeInfo::resolveAny(subtypes[0], loc);
            if (QoreTypeInfo::hasType(valueType)) {
                return !or_nothing
                ? qore_get_complex_list_type(valueType)
                : qore_get_complex_list_or_nothing_type(valueType);
            }
        } else {
            parseException(*loc, "PARSE-TYPE-ERROR", "cannot resolve '%s' with %d type arguments; base type 'list' takes a single type name giving list element value type", getName(), (int)subtypes.size());
        }
        return or_nothing ? listOrNothingTypeInfo : listTypeInfo;
    }
    if (!strcmp(cscope->ostr, "softlist")) {
        if (subtypes.size() == 1) {
            if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
                return or_nothing ? softAutoListOrNothingTypeInfo : softAutoListTypeInfo;
            // resolve value type
            const QoreTypeInfo* valueType = QoreParseTypeInfo::resolveAny(subtypes[0], loc);
            if (QoreTypeInfo::hasType(valueType)) {
                return !or_nothing
                ? qore_get_complex_softlist_type(valueType)
                : qore_get_complex_softlist_or_nothing_type(valueType);
            }
        } else {
            parseException(*loc, "PARSE-TYPE-ERROR", "cannot resolve '%s' with %d type arguments; base type 'softlist' takes a single type name giving list element value type", getName(), (int)subtypes.size());
        }
        return or_nothing ? softListOrNothingTypeInfo : softListTypeInfo;
    }
    if (!strcmp(cscope->ostr, "reference")) {
        if (subtypes.size() == 1) {
            if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
                return or_nothing ? referenceOrNothingTypeInfo : referenceTypeInfo;
            // resolve value type
            const QoreTypeInfo* valueType = QoreParseTypeInfo::resolveAny(subtypes[0], loc);
            if (QoreTypeInfo::hasType(valueType)) {
                return !or_nothing
                ? qore_get_complex_reference_type(valueType)
                : qore_get_complex_reference_or_nothing_type(valueType);
            }
        } else {
            parseException(*loc, "PARSE-TYPE-ERROR", "cannot resolve '%s' with %d type arguments; base type 'reference' takes a single type name giving referenced lvalue type", getName(), (int)subtypes.size());
        }
        return or_nothing ? referenceOrNothingTypeInfo : referenceTypeInfo;
    }
    if (!strcmp(cscope->ostr, "object")) {
        if (subtypes.size() != 1) {
            parseException(*loc, "PARSE-TYPE-ERROR", "cannot resolve '%s'; base type 'object' takes a single class name as a subtype argument", getName());
            return or_nothing ? objectOrNothingTypeInfo : objectTypeInfo;
        }

        if (!strcmp(subtypes[0]->cscope->ostr, "auto"))
            return or_nothing ? objectOrNothingTypeInfo : objectTypeInfo;

        // resolve class
        return resolveClass(loc, *subtypes[0]->cscope, or_nothing);
    }

    parseException(*loc, "PARSE-TYPE-ERROR", "cannot resolve '%s'; type '%s' does not take subtype declarations", getName(), cscope->getIdentifier());
    return autoTypeInfo;
}

const QoreTypeInfo* QoreParseTypeInfo::resolve(const QoreProgramLocation* loc) const {
    if (!subtypes.empty())
        return resolveSubtype(loc);

    return resolveClass(loc, *cscope, or_nothing);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveAny(const QoreProgramLocation* loc) const {
    if (!subtypes.empty())
        return resolveSubtype(loc);

    const QoreTypeInfo* rv = or_nothing ? getBuiltinUserOrNothingTypeInfo(cscope->ostr) : getBuiltinUserTypeInfo(cscope->ostr);
    return rv ? rv : resolveClass(loc, *cscope, or_nothing);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveAndDelete(const QoreProgramLocation* loc) {
    std::unique_ptr<QoreParseTypeInfo> holder(this);
    return resolve(loc);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveClass(const QoreProgramLocation* loc, const NamedScope& cscope, bool or_nothing) {
    // resolve class
    const QoreClass* qc = qore_root_ns_private::parseFindScopedClass(loc, cscope);

    if (qc && or_nothing) {
        const QoreTypeInfo* rv = qc->getOrNothingTypeInfo();
        if (!rv) {
            parse_error(*loc, "class %s cannot be typed with '*' as the class's type handler has an input filter and the filter does not accept NOTHING", qc->getName());
            return objectOrNothingTypeInfo;
        }
        return rv;
    }

    // qc maybe NULL when the class is not found
    return qc ? qc->getTypeInfo() : objectTypeInfo;
}

QoreValue QoreHashDeclTypeInfo::getDefaultQoreValueImpl() const {
    return qore_hash_private::newHashDecl(accept_vec[0].spec.getHashDecl());
    //return new QoreHashNode(accept_vec[0].spec.getHashDecl(), xsink);
}

QoreComplexSoftListTypeInfo::QoreComplexSoftListTypeInfo(const QoreTypeInfo* vti) : QoreComplexListTypeInfo(q_accept_vec_t {
      {QoreComplexListTypeSpec(vti), nullptr, true},
      {NT_LIST, [vti] (QoreValue& n, ExceptionSink* xsink) {
            QoreValue val;
            n.swap(val);
            n.assign(qore_list_private::newComplexListFromValue(qore_get_complex_list_type(vti), val, xsink));
         }
      },
      {NT_NOTHING, [vti] (QoreValue& n, ExceptionSink* xsink) {
            QoreListNode* l = new QoreListNode(vti);
            n.assign(l);
         }
      },
      {NT_ALL, [vti] (QoreValue& n, ExceptionSink* xsink) {
            QoreValue val;
            n.swap(val);
            n.assign(qore_list_private::newComplexListFromValue(qore_get_complex_list_type(vti), val, xsink));
         }
      },
   }, q_return_vec_t {{QoreComplexListTypeSpec(vti), true}}, QoreStringMaker("softlist<%s>", QoreTypeInfo::getName(vti))) {
   assert(vti);
}

QoreComplexSoftListOrNothingTypeInfo::QoreComplexSoftListOrNothingTypeInfo(const QoreTypeInfo* vti)
    : QoreComplexListOrNothingTypeInfo(q_accept_vec_t {
        {QoreComplexListTypeSpec(vti), nullptr},
        {NT_LIST, [vti] (QoreValue& n, ExceptionSink* xsink) {
                QoreValue val;
                n.swap(val);
                n.assign(qore_list_private::newComplexListFromValue(qore_get_complex_list_type(vti), val, xsink));
            }
        },
        {NT_NOTHING, nullptr},
        {NT_NULL, [] (QoreValue& n, ExceptionSink* xsink) { n.assignNothing(); }},
        {NT_ALL, [vti] (QoreValue& n, ExceptionSink* xsink) {
                QoreValue val;
                n.swap(val);
                n.assign(qore_list_private::newComplexListFromValue(qore_get_complex_list_type(vti), val, xsink));
            }
        },
    },
    q_return_vec_t {{QoreComplexListTypeSpec(vti)}, {NT_NOTHING}},
    QoreStringMaker("*softlist<%s>", QoreTypeInfo::getName(vti))) {
    assert(vti);
}

void map_get_plain_hash_lvalue(QoreValue& n, ExceptionSink* xsink, LValueHelper* lvhelper) {
    // issue #2889 do not pass a QoreValue to LValueHelper::saveTemp() as it will remove the node from the QoreValue
    // instead pass an AbstractQoreNode*
    QoreHashNode* h = n.get<QoreHashNode>();
    lvhelper->saveTemp(h);
    n.assign(copy_strip_complex_types(h));
}

void map_get_plain_list_lvalue(QoreValue& n, ExceptionSink* xsink, LValueHelper* lvhelper) {
    // issue #2889 do not pass a QoreValue to LValueHelper::saveTemp() as it will remove the node from the QoreValue
    // instead pass an AbstractQoreNode*
    QoreListNode* l = n.get<QoreListNode>();
    lvhelper->saveTemp(l);
    n.assign(copy_strip_complex_types(l));
}

void map_get_plain_hash(QoreValue& n, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> h(n.get<QoreHashNode>(), xsink);
    n.assign(copy_strip_complex_types(*h));
}

void map_get_plain_list(QoreValue& n, ExceptionSink* xsink) {
    ReferenceHolder<QoreListNode> l(n.get<QoreListNode>(), xsink);
    n.assign(copy_strip_complex_types(*l));
}
