/*
  QoreTypeInfo.cpp

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
#include <qore/QoreRWLock.h>
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/qore_number_private.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/typed_hash_decl_private.h"

const QoreAnyTypeInfo staticAnyTypeInfo;

const QoreBigIntTypeInfo staticBigIntTypeInfo;
const QoreBigIntOrNothingTypeInfo staticBigIntOrNothingTypeInfo;

const QoreStringTypeInfo staticStringTypeInfo;
const QoreStringOrNothingTypeInfo staticStringOrNothingTypeInfo;

const QoreBoolTypeInfo staticBoolTypeInfo;
const QoreBoolOrNothingTypeInfo staticBoolOrNothingTypeInfo;

const QoreBinaryTypeInfo staticBinaryTypeInfo;
const QoreBinaryOrNothingTypeInfo staticBinaryOrNothingTypeInfo;

const QoreObjectTypeInfo staticObjectTypeInfo;
const QoreObjectOrNothingTypeInfo staticObjectOrNothingTypeInfo;

const QoreDateTypeInfo staticDateTypeInfo;
const QoreDateOrNothingTypeInfo staticDateOrNothingTypeInfo;

const QoreHashTypeInfo staticHashTypeInfo;
const QoreHashOrNothingTypeInfo staticHashOrNothingTypeInfo;

const QoreListTypeInfo staticListTypeInfo;
const QoreListOrNothingTypeInfo staticListOrNothingTypeInfo;

const QoreNothingTypeInfo staticNothingTypeInfo;

const QoreNullTypeInfo staticNullTypeInfo;
const QoreNullOrNothingTypeInfo staticNullOrNothingTypeInfo;

const QoreClosureTypeInfo staticClosureTypeInfo;
const QoreClosureOrNothingTypeInfo staticClosureOrNothingTypeInfo;

const QoreCallReferenceTypeInfo staticCallReferenceTypeInfo;
const QoreCallReferenceOrNothingTypeInfo staticCallReferenceOrNothingTypeInfo;

const QoreReferenceTypeInfo staticReferenceTypeInfo;
const QoreReferenceOrNothingTypeInfo staticReferenceOrNothingTypeInfo;

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

const QoreTimeoutTypeInfo staticTimeoutTypeInfo;
const QoreTimeoutOrNothingTypeInfo staticTimeoutOrNothingTypeInfo;

const QoreIntOrFloatTypeInfo staticIntOrFloatTypeInfo;

const QoreIntFloatOrNumberTypeInfo staticIntFloatOrNumberTypeInfo;

const QoreFloatOrNumberTypeInfo staticFloatOrNumberTypeInfo;

const QoreTypeInfo* anyTypeInfo = &staticAnyTypeInfo,
   *bigIntTypeInfo = &staticBigIntTypeInfo,
   *floatTypeInfo = &staticFloatTypeInfo,
   *boolTypeInfo = &staticBoolTypeInfo,
   *stringTypeInfo = &staticStringTypeInfo,
   *binaryTypeInfo = &staticBinaryTypeInfo,
   *dateTypeInfo = &staticDateTypeInfo,
   *objectTypeInfo = &staticObjectTypeInfo,
   *hashTypeInfo = &staticHashTypeInfo,
   *listTypeInfo = &staticListTypeInfo,
   *nothingTypeInfo = &staticNothingTypeInfo,
   *nullTypeInfo = &staticNullTypeInfo,
   *numberTypeInfo = &staticNumberTypeInfo,
   *runTimeClosureTypeInfo = &staticClosureTypeInfo,
   *callReferenceTypeInfo = &staticCallReferenceTypeInfo,
   *referenceTypeInfo = &staticReferenceTypeInfo,
   *codeTypeInfo = &staticCodeTypeInfo,
   *softBigIntTypeInfo = &staticSoftBigIntTypeInfo,
   *softFloatTypeInfo = &staticSoftFloatTypeInfo,
   *softNumberTypeInfo = &staticSoftNumberTypeInfo,
   *softBoolTypeInfo = &staticSoftBoolTypeInfo,
   *softStringTypeInfo = &staticSoftStringTypeInfo,
   *softDateTypeInfo = &staticSoftDateTypeInfo,
   *softListTypeInfo = &staticSoftListTypeInfo,
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
   *listOrNothingTypeInfo = &staticListOrNothingTypeInfo,
   *nullOrNothingTypeInfo = &staticNullOrNothingTypeInfo,
   *codeOrNothingTypeInfo = &staticCodeOrNothingTypeInfo,
   *dataOrNothingTypeInfo = &staticDataOrNothingTypeInfo,
   *referenceOrNothingTypeInfo = &staticReferenceOrNothingTypeInfo,

   *softBigIntOrNothingTypeInfo = &staticSoftBigIntOrNothingTypeInfo,
   *softFloatOrNothingTypeInfo = &staticSoftFloatOrNothingTypeInfo,
   *softNumberOrNothingTypeInfo = &staticSoftNumberOrNothingTypeInfo,
   *softBoolOrNothingTypeInfo = &staticSoftBoolOrNothingTypeInfo,
   *softStringOrNothingTypeInfo = &staticSoftStringOrNothingTypeInfo,
   *softDateOrNothingTypeInfo = &staticSoftDateOrNothingTypeInfo,
   *softListOrNothingTypeInfo = &staticSoftListOrNothingTypeInfo,
   *timeoutOrNothingTypeInfo = &staticTimeoutOrNothingTypeInfo;

QoreListNode* emptyList;
QoreHashNode* emptyHash;
QoreStringNode* NullString;
DateTimeNode* ZeroDate, * OneDate;
QoreBigIntNode* Zero;
QoreFloatNode* ZeroFloat;
QoreNumberNode* ZeroNumber, * NaNumber, * InfinityNumber, * piNumber;

// map from types to default values
typedef std::map<qore_type_t, AbstractQoreNode* > def_val_map_t;
static def_val_map_t def_val_map;

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

// rwlock for global type map
static QoreRWLock extern_type_info_map_lock;

void concatClass(std::string &str, const char *cn) {
   str.append("<class: ");
   str.append(cn);
   str.push_back('>');
}

static void do_maps(qore_type_t t, const char* name, const QoreTypeInfo* typeInfo, const QoreTypeInfo* orNothingTypeInfo) {
   str_typeinfo_map[name]          = typeInfo;
   str_ornothingtypeinfo_map[name] = orNothingTypeInfo;
   type_typeinfo_map[t]            = typeInfo;
   type_ornothingtypeinfo_map[t]   = orNothingTypeInfo;
   type_str_map[t]                 = name;
}

// at least the NullString must be created after the default character encoding is set
void init_qore_types() {
   // initialize global default values
   NullString     = new QoreStringNode;
   ZeroDate       = DateTimeNode::makeAbsolute(0, 0, 0);
   OneDate        = DateTimeNode::makeAbsolute(0, 0, 0, 0, 0, 1);
   Zero           = new QoreBigIntNode;
   ZeroFloat      = new QoreFloatNode;
   ZeroNumber     = new QoreNumberNode;
   NaNumber       = qore_number_private::getNaNumber();
   InfinityNumber = qore_number_private::getInfinity();
   piNumber       = qore_number_private::getPi();

   emptyList      = new QoreListNode;
   emptyHash      = new QoreHashNode;

   def_val_map[NT_INT]     = Zero->refSelf();
   def_val_map[NT_STRING]  = NullString->refSelf();
   def_val_map[NT_BOOLEAN] = &False;
   def_val_map[NT_DATE]    = ZeroDate->refSelf();
   def_val_map[NT_FLOAT]   = ZeroFloat->refSelf();
   def_val_map[NT_NUMBER]  = ZeroNumber->refSelf();
   def_val_map[NT_LIST]    = emptyList->refSelf();
   def_val_map[NT_HASH]    = emptyHash->refSelf();
   def_val_map[NT_BINARY]  = new BinaryNode;
   def_val_map[NT_NULL]    = &Null;
   def_val_map[NT_NOTHING] = &Nothing;

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

   do_maps(NT_TIMEOUT,     "timeout", timeoutTypeInfo, timeoutOrNothingTypeInfo);

   // map the closure and callref strings to codeTypeInfo to ensure that these
   // types are always interchangeable
   do_maps(NT_RUNTIME_CLOSURE, "closure", codeTypeInfo, codeOrNothingTypeInfo);
   do_maps(NT_FUNCREF, "callref", codeTypeInfo, codeOrNothingTypeInfo);
}

void delete_qore_types() {
   // dereference all values from default value map
   for (def_val_map_t::iterator i = def_val_map.begin(), e = def_val_map.end(); i != e; ++i)
      i->second->deref(nullptr);

   // dereference global default values
   NullString->deref();
   piNumber->deref();
   InfinityNumber->deref();
   NaNumber->deref();
   ZeroNumber->deref();
   ZeroFloat->deref();
   Zero->deref();
   OneDate->deref();
   ZeroDate->deref();
   emptyList->deref(0);
   emptyHash->deref(0);
}

void add_to_type_map(qore_type_t t, const QoreTypeInfo* typeInfo) {
   QoreAutoRWWriteLocker al(extern_type_info_map_lock);
   assert(extern_type_info_map.find(t) == extern_type_info_map.end());
   extern_type_info_map[t] = typeInfo;
}

static const QoreTypeInfo* getExternalTypeInfoForType(qore_type_t t) {
   QoreAutoRWReadLocker al(extern_type_info_map_lock);
   type_typeinfo_map_t::iterator i = extern_type_info_map.find(t);
   return (i == extern_type_info_map.end() ? 0 : i->second);
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
      case NT_HASH:
         return static_cast<const QoreHashNode*>(n)->getTypeInfo();
      default:
         break;
   }
   return getTypeInfoForType(t);
}

AbstractQoreNode* getDefaultValueForBuiltinValueType(qore_type_t t) {
   def_val_map_t::iterator i = def_val_map.find(t);
   assert(i != def_val_map.end());
   return i->second->refSelf();
}

bool builtinTypeHasDefaultValue(qore_type_t t) {
   return def_val_map.find(t) != def_val_map.end();
}

const QoreTypeInfo* getBuiltinUserTypeInfo(const char* str) {
   str_typeinfo_map_t::iterator i = str_typeinfo_map.find(str);
   if (i == str_typeinfo_map.end())
      return nullptr;

   const QoreTypeInfo* rv = i->second;
   // return type "any" for reference types if PO_BROKEN_REFERENCES is set
   if (rv == referenceTypeInfo && (getProgram()->getParseOptions64() & PO_BROKEN_REFERENCES))
      rv = anyTypeInfo;
   return rv;
}

const QoreTypeInfo* getBuiltinUserOrNothingTypeInfo(const char* str) {
   str_typeinfo_map_t::iterator i = str_ornothingtypeinfo_map.find(str);
   if (i == str_ornothingtypeinfo_map.end())
      return nullptr;

   const QoreTypeInfo* rv = i->second;
   // return type "any" for reference types if PO_BROKEN_REFERENCES is set
   if (rv == referenceOrNothingTypeInfo && (getProgram()->getParseOptions64() & PO_BROKEN_REFERENCES))
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

qore_type_result_e QoreTypeSpec::match(const QoreTypeSpec& t) const {
   switch (typespec) {
      case QTS_CLASS: {
         switch (t.typespec) {
            case QTS_CLASS:
               return qore_class_private::get(*t.u.qc)->parseCheckCompatibleClass(*qore_class_private::get(*u.qc));
            default: {
               // NOTE: with %strict-types, this must return QTI_NOT_EQUAL
               qore_type_t tt = t.getType();
               if (tt == NT_ALL)
                  return QTI_AMBIGUOUS;
               return tt == NT_OBJECT ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
            }
         }
         return QTI_NOT_EQUAL;
      }
      case QTS_HASHDECL: {
         switch (t.typespec) {
            case QTS_HASHDECL:
               return typed_hash_decl_private::get(*t.u.hd)->parseEqual(*typed_hash_decl_private::get(*u.hd)) ? QTI_IDENT : QTI_NOT_EQUAL;
            default: {
               return QTI_NOT_EQUAL;
            }
         }
         return QTI_NOT_EQUAL;
      }
      case QTS_COMPLEXHASH: {
         switch (t.typespec) {
            case QTS_COMPLEXHASH:
               return QoreTypeInfo::parseAccepts(u.ti, t.u.ti);
            default: {
               return QTI_NOT_EQUAL;
            }
         }
         return QTI_NOT_EQUAL;
      }
      case QTS_TYPE: {
         qore_type_t ot = t.getType();
         if (u.t == NT_ALL || ot == NT_ALL)
            return QTI_AMBIGUOUS;
         return u.t == ot ? QTI_IDENT : QTI_NOT_EQUAL;
      }
   }
   return QTI_NOT_EQUAL;
}

qore_type_result_e QoreTypeSpec::match(const QoreTypeSpec& t, bool& may_not_match) const {
   switch (typespec) {
      case QTS_CLASS: {
         switch (t.typespec) {
            case QTS_CLASS:
               return qore_class_private::get(*t.u.qc)->parseCheckCompatibleClass(*qore_class_private::get(*u.qc), may_not_match);
            default: {
               // NOTE: with %strict-types, anything with may_not_match = true must return QTI_NOT_EQUAL
               qore_type_t tt = t.getType();
               if (tt == NT_ALL || tt == NT_OBJECT) {
                  may_not_match = true;
                  return QTI_AMBIGUOUS;
               }
               return QTI_NOT_EQUAL;
            }
         }
         return QTI_NOT_EQUAL;
      }
      case QTS_HASHDECL: {
         switch (t.typespec) {
            case QTS_HASHDECL:
               return typed_hash_decl_private::get(*t.u.hd)->parseEqual(*typed_hash_decl_private::get(*u.hd)) ? QTI_IDENT : QTI_NOT_EQUAL;
            default: {
               return QTI_NOT_EQUAL;
            }
         }
         return QTI_NOT_EQUAL;
      }
      case QTS_COMPLEXHASH: {
         switch (t.typespec) {
            case QTS_COMPLEXHASH:
               return QoreTypeInfo::equal(u.ti, t.u.ti) ? QTI_IDENT : QTI_NOT_EQUAL;
            default: {
               return QTI_NOT_EQUAL;
            }
         }
         return QTI_NOT_EQUAL;
      }
      case QTS_TYPE: {
         qore_type_t ot = t.getType();
         if (u.t == NT_ALL)
            return QTI_AMBIGUOUS;
         // NOTE: with %strict-types, anything with may_not_match = true must return QTI_NOT_EQUAL
         if (ot == NT_ALL) {
            may_not_match = true;
            return QTI_AMBIGUOUS;
         }
         return u.t == ot ? QTI_IDENT : QTI_NOT_EQUAL;
      }
   }
   return QTI_NOT_EQUAL;
}

bool QoreTypeSpec::runtimeTestMatch(const QoreValue& n, bool& priv_error) const {
   assert(!priv_error);
   switch (typespec) {
      case QTS_CLASS: {
         if (n.getType() == NT_OBJECT) {
            bool priv;
            if (!n.get<const QoreObject>()->getClass()->getClass(*u.qc, priv))
               return false;
            if (!priv)
               return true;
            // check access
            if (qore_class_private::runtimeCheckPrivateClassAccess(*u.qc))
               return true;
            priv_error = true;
            return false;
         }
         return false;
      }
      case QTS_HASHDECL: {
         if (n.getType() == NT_HASH) {
            const TypedHashDecl* hd = n.get<const QoreHashNode>()->getHashDecl();
            return hd && typed_hash_decl_private::get(*hd)->equal(*typed_hash_decl_private::get(*u.hd));
         }
         return false;
      }
      case QTS_COMPLEXHASH: {
         if (n.getType() == NT_HASH) {
            const QoreTypeInfo* ti = n.get<const QoreHashNode>()->getValueTypeInfo();
            return ti && QoreTypeInfo::equal(u.ti, ti);
         }
         return false;
      }
      case QTS_TYPE:
         return u.t == NT_ALL || u.t == n.getType();
   }
   return false;
}

bool QoreTypeSpec::operator==(const QoreTypeSpec& other) const {
   if (typespec != other.typespec)
      return false;
   switch (typespec) {
      case QTS_TYPE:
         return u.t == other.u.t;
      case QTS_CLASS:
         return qore_class_private::get(*u.qc)->equal(*qore_class_private::get(*other.u.qc));
      case QTS_HASHDECL:
         return typed_hash_decl_private::get(*u.hd)->equal(*typed_hash_decl_private::get(*other.u.hd));
      case QTS_COMPLEXHASH:
         return QoreTypeInfo::equal(u.ti, other.u.ti);
   }
   return false;
}

bool QoreTypeSpec::operator!=(const QoreTypeSpec& other) const {
   return (!(*this == other));
}

qore_type_result_e QoreTypeInfo::runtimeAcceptsValue(const QoreValue& n) const {
   if (accept_vec.size() == 1) {
      const QoreTypeSpec& t = accept_vec[0].spec;
      if (n.getType() == NT_OBJECT && t.getTypeSpec() == QTS_CLASS)
         return qore_class_private::runtimeCheckCompatibleClass(*t.getClass(), *n.get<const QoreObject>()->getClass());
      if (n.getType() == NT_HASH) {
         if (t.getTypeSpec() == QTS_HASHDECL) {
            const TypedHashDecl* hd = n.get<const QoreHashNode>()->getHashDecl();
            return hd && typed_hash_decl_private::get(*t.getHashDecl())->equal(*typed_hash_decl_private::get(*hd)) ? QTI_IDENT : QTI_NOT_EQUAL;
         }
         else if (t.getTypeSpec() == QTS_COMPLEXHASH) {
            const QoreTypeInfo* ti = n.get<const QoreHashNode>()->getTypeInfo();
            return ti && QoreTypeInfo::equal(this, ti) ? QTI_IDENT : QTI_NOT_EQUAL;
         }
      }
      qore_type_t at = t.getType();
      if (at == NT_ALL)
         return QTI_AMBIGUOUS;
      return n.getType() == at ? QTI_IDENT : QTI_NOT_EQUAL;
   }

   for (auto& t : accept_vec) {
      if (n.getType() == NT_OBJECT && t.spec.getTypeSpec() == QTS_CLASS) {
         qore_type_result_e rv = qore_class_private::runtimeCheckCompatibleClass(*t.spec.getClass(), *n.get<const QoreObject>()->getClass());
         if (rv != QTI_NOT_EQUAL)
            return QTI_AMBIGUOUS;
      }
      else if (n.getType() == NT_HASH && t.spec.getTypeSpec() == QTS_HASHDECL) {
         const TypedHashDecl* hd = n.get<const QoreHashNode>()->getHashDecl();
         if (hd && typed_hash_decl_private::get(*t.spec.getHashDecl())->equal(*typed_hash_decl_private::get(*hd)))
            return t.exact ? QTI_IDENT : QTI_AMBIGUOUS;
      }
      else if (n.getType() == NT_HASH && t.spec.getTypeSpec() == QTS_COMPLEXHASH) {
         const QoreTypeInfo* ti = n.get<const QoreHashNode>()->getTypeInfo();
         if (ti && QoreTypeInfo::equal(this, ti))
            return t.exact ? QTI_IDENT : QTI_AMBIGUOUS;
      }
      else {
         qore_type_t at = t.spec.getType();
         if (at == NT_ALL || n.getType() == at)
            return t.exact ? QTI_IDENT : QTI_AMBIGUOUS;
      }
   }
   return QTI_NOT_EQUAL;
}

void QoreTypeInfo::doNonNumericWarning(const QoreProgramLocation& loc, const char* preface) const {
   QoreStringNode* desc = new QoreStringNode(preface);
   getThisTypeImpl(*desc);
   desc->sprintf(", which does not evaluate to a numeric type, therefore will always evaluate to 0 at runtime");
   qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

void QoreTypeInfo::doNonBooleanWarning(const QoreProgramLocation& loc, const char* preface) const {
   QoreStringNode* desc = new QoreStringNode(preface);
   getThisTypeImpl(*desc);
   desc->sprintf(", which does not evaluate to a numeric or boolean type, therefore will always evaluate to False at runtime");
   qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

void QoreTypeInfo::doNonStringWarning(const QoreProgramLocation& loc, const char* preface) const {
   QoreStringNode* desc = new QoreStringNode(preface);
   getThisTypeImpl(*desc);
   desc->sprintf(", which cannot be converted to a string, therefore will always evaluate to an empty string at runtime");
   qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

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

const QoreTypeInfo* QoreParseTypeInfo::resolveSubtype(const QoreProgramLocation& loc) const {
   if (!strcmp(cscope->ostr, "hash")) {
      if (subtypes.size() == 1) {
         // resolve hashdecl
         const TypedHashDecl* hd = qore_root_ns_private::get(*getRootNS())->parseFindHashDecl(loc, *subtypes[0]->cscope);
         //printd(5, "QoreParseTypeInfo::resolveSubtype() this: %p '%s' hd: %p '%s' type: %p (pgm: %p)\n", this, getName(), hd, hd ? hd->getName() : "n/a", hd ? hd->getTypeInfo(false) : nullptr, getProgram());
         return hd ? hd->getTypeInfo(or_nothing) : hashTypeInfo;
      }
      if (subtypes.size() == 2) {
         if (strcmp(subtypes[0]->cscope->ostr, "string")) {
            parseException(loc, "PARSE-TYPE-ERROR", "invalid complex hash type '%s'; hash key type must be 'string'; cannot declare a hash with key type '%s'", getName(), subtypes[0]->cscope->ostr);
         }
         else {
            // resolve value type
            const QoreTypeInfo* valueType = QoreParseTypeInfo::resolveAny(subtypes[1], loc);
            if (QoreTypeInfo::hasType(valueType)) {
               QoreStringMaker str("%shash<string, %s>", or_nothing ? "*" : "", QoreTypeInfo::getName(valueType));
               return !or_nothing
                  ? qore_program_private::get(*getProgram())->getComplexHashType(str.c_str(), valueType)
                  : qore_program_private::get(*getProgram())->getComplexHashOrNothingType(str.c_str(), valueType);
            }
         }
      }
      else
         parseException(loc, "PARSE-TYPE-ERROR", "cannot resolve '%s'; base type 'hash' takes a single hashdecl name as a subtype argument or two type names giving the key and value types", getName());
      return or_nothing ? hashOrNothingTypeInfo : hashTypeInfo;
   }
   else if (!strcmp(cscope->ostr, "object")) {
      if (subtypes.size() != 1) {
         parseException(loc, "PARSE-TYPE-ERROR", "cannot resolve '%s'; base type 'object' takes a single class name as a subtype argument", getName());
         return or_nothing ? objectOrNothingTypeInfo : objectTypeInfo;
      }

      // resolve class
      return resolveClass(loc, *subtypes[0]->cscope, or_nothing);
   }

   parseException(loc, "PARSE-TYPE-ERROR", "cannot resolve '%s'; type '%s' does not take subtype declarations", getName(), cscope->getIdentifier());
   return anyTypeInfo;
}

const QoreTypeInfo* QoreParseTypeInfo::resolve(const QoreProgramLocation& loc) const {
   if (!subtypes.empty())
      return resolveSubtype(loc);

   return resolveClass(loc, *cscope, or_nothing);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveAny(const QoreProgramLocation& loc) const {
   if (cscope->size() == 1) {
      const QoreTypeInfo* rv = or_nothing ? getBuiltinUserOrNothingTypeInfo(cscope->ostr) : getBuiltinUserTypeInfo(cscope->ostr);
      if (rv)
         return rv;
   }
   return resolve(loc);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveAndDelete(const QoreProgramLocation& loc) {
   std::unique_ptr<QoreParseTypeInfo> holder(this);
   return resolve(loc);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveClass(const QoreProgramLocation& loc, const NamedScope& cscope, bool or_nothing) {
   // resolve class
   const QoreClass* qc = qore_root_ns_private::parseFindScopedClass(loc, cscope);

   if (qc && or_nothing) {
      const QoreTypeInfo* rv = qc->getOrNothingTypeInfo();
      if (!rv) {
         parse_error(loc, "class %s cannot be typed with '*' as the class's type handler has an input filter and the filter does not accept NOTHING", qc->getName());
         return objectOrNothingTypeInfo;
      }
      return rv;
   }

   // qc maybe NULL when the class is not found
   return qc ? qc->getTypeInfo() : objectTypeInfo;
}

void map_get_plain_hash(QoreValue& n, ExceptionSink* xsink) {
   QoreHashNode* h = n.get<QoreHashNode>();
   qore_hash_private* ph = qore_hash_private::get(*h);
   //printd(5, "map_get_plain_hash ph: %p hd: %p c: %p refs: %d\n", ph, ph->hashdecl, ph->complexTypeInfo, h->reference_count());
   if (!ph->hashdecl && !ph->complexTypeInfo)
      return;

   if (!h->is_unique()) {
      discard(n.assign(ph->copy(true)), xsink);
      return;
   }
   ph->hashdecl = nullptr;
   ph->complexTypeInfo = nullptr;
}