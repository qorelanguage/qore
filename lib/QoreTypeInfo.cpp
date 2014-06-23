/*
  QoreTypeInfo.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2014 David Nichols
  
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
#include <qore/intern/QoreNamespaceIntern.h>
#include <qore/intern/qore_number_private.h>

// provides for 2-way compatibility with classes derived from QoreBigIntNode and softint
static BigIntTypeInfo staticBigIntTypeInfo;
const QoreTypeInfo* bigIntTypeInfo = &staticBigIntTypeInfo;

// static reference types
static QoreTypeInfo staticAnyTypeInfo,
   staticStringTypeInfo(NT_STRING),
   staticBoolTypeInfo(NT_BOOLEAN),
   staticBinaryTypeInfo(NT_BINARY),
   staticObjectTypeInfo(NT_OBJECT),
   staticDateTypeInfo(NT_DATE),
   staticHashTypeInfo(NT_HASH),
   staticListTypeInfo(NT_LIST),
   staticNothingTypeInfo(NT_NOTHING),
   staticNullTypeInfo(NT_NULL),
   staticRunTimeClosureTypeInfo(NT_RUNTIME_CLOSURE),
   staticCallReferenceTypeInfo(NT_FUNCREF)
   ;

// const pointers to static reference types
const QoreTypeInfo* anyTypeInfo = &staticAnyTypeInfo,
   *stringTypeInfo = &staticStringTypeInfo,
   *boolTypeInfo = &staticBoolTypeInfo,
   *binaryTypeInfo = &staticBinaryTypeInfo,
   *objectTypeInfo = &staticObjectTypeInfo,
   *dateTypeInfo = &staticDateTypeInfo,
   *hashTypeInfo = &staticHashTypeInfo,
   *listTypeInfo = &staticListTypeInfo,
   *nothingTypeInfo = &staticNothingTypeInfo,
   *nullTypeInfo = &staticNullTypeInfo,
   *runTimeClosureTypeInfo = &staticRunTimeClosureTypeInfo,
   *callReferenceTypeInfo = &staticCallReferenceTypeInfo,
   
   // assigned in init_qore_types()
   *bigIntOrNothingTypeInfo = 0, 
   *stringOrNothingTypeInfo = 0,
   *boolOrNothingTypeInfo = 0,
   *binaryOrNothingTypeInfo = 0,
   *objectOrNothingTypeInfo = 0,
   *dateOrNothingTypeInfo = 0,
   *hashOrNothingTypeInfo = 0,
   *listOrNothingTypeInfo = 0,
   *nullOrNothingTypeInfo = 0
   ;

// reference types
static ReferenceTypeInfo staticReferenceTypeInfo;
const QoreTypeInfo* referenceTypeInfo = &staticReferenceTypeInfo,
      *referenceOrNothingTypeInfo = 0; // assigned in init_qore_types()

// provides limited compatibility with integers
static FloatTypeInfo staticFloatTypeInfo;
static FloatOrNothingTypeInfo staticFloatOrNothingTypeInfo;
const QoreTypeInfo* floatTypeInfo = &staticFloatTypeInfo,
   *floatOrNothingTypeInfo = &staticFloatOrNothingTypeInfo;

// the "number" and "*number" types
static NumberTypeInfo staticNumberTypeInfo;
static NumberOrNothingTypeInfo staticNumberOrNothingTypeInfo;
const QoreTypeInfo* numberTypeInfo = &staticNumberTypeInfo,
   *numberOrNothingTypeInfo = &staticNumberOrNothingTypeInfo;

// provides equal compatibility with closures and all types of code references
static CodeTypeInfo staticCodeTypeInfo;
static CodeOrNothingTypeInfo staticCodeOrNothingTypeInfo;
const QoreTypeInfo* codeTypeInfo = &staticCodeTypeInfo,
   *codeOrNothingTypeInfo = &staticCodeOrNothingTypeInfo;

// either string or binary
static DataTypeInfo staticDataTypeInfo;
static DataOrNothingTypeInfo staticDataOrNothingTypeInfo;
const QoreTypeInfo* dataTypeInfo = &staticDataTypeInfo,
   *dataOrNothingTypeInfo = &staticDataOrNothingTypeInfo;

// provides int compatibility with and conversions from float, string, date, and bool
static SoftBigIntTypeInfo staticSoftBigIntTypeInfo;
static SoftBigIntOrNothingTypeInfo staticBigIntOrNothingTypeInfo;
const QoreTypeInfo* softBigIntTypeInfo = &staticSoftBigIntTypeInfo,
   *softBigIntOrNothingTypeInfo = &staticBigIntOrNothingTypeInfo;

// provides float compatibility with and conversions from int, string, date, and bool
static SoftFloatTypeInfo staticSoftFloatTypeInfo;
static SoftFloatOrNothingTypeInfo staticSoftFloatOrNothingTypeInfo;
const QoreTypeInfo* softFloatTypeInfo = &staticSoftFloatTypeInfo,
   *softFloatOrNothingTypeInfo = &staticSoftFloatOrNothingTypeInfo;

// softnumber and *softnumber
static SoftNumberTypeInfo staticSoftNumberTypeInfo;
static SoftNumberOrNothingTypeInfo staticSoftNumberOrNothingTypeInfo;
const QoreTypeInfo* softNumberTypeInfo = &staticSoftNumberTypeInfo,
   *softNumberOrNothingTypeInfo = &staticSoftNumberOrNothingTypeInfo;

// provides bool compatibility with and conversions from float, string, date, and int
static SoftBoolTypeInfo staticSoftBoolTypeInfo;
static SoftBoolOrNothingTypeInfo staticSoftBoolOrNothingTypeInfo;
const QoreTypeInfo* softBoolTypeInfo = &staticSoftBoolTypeInfo,
   *softBoolOrNothingTypeInfo = &staticSoftBoolOrNothingTypeInfo;

// provides string compatibility with and conversions from float, int, date, and bool
static SoftStringTypeInfo staticSoftStringTypeInfo;
static SoftStringOrNothingTypeInfo staticSoftStringOrNothingTypeInfo;
const QoreTypeInfo* softStringTypeInfo = &staticSoftStringTypeInfo,
   *softStringOrNothingTypeInfo = &staticSoftStringOrNothingTypeInfo;

static SoftDateTypeInfo staticSoftDateTypeInfo;
static SoftDateOrNothingTypeInfo staticSoftDateOrNothingTypeInfo;
const QoreTypeInfo* softDateTypeInfo = &staticSoftDateTypeInfo,
   *softDateOrNothingTypeInfo = &staticSoftDateOrNothingTypeInfo;

// provides automatic conversion to a list
static SoftListTypeInfo staticSoftListTypeInfo;
static SoftListOrNothingTypeInfo staticListOrNothingTypeInfo;
const QoreTypeInfo* softListTypeInfo = &staticSoftListTypeInfo,
   *softListOrNothingTypeInfo = &staticListOrNothingTypeInfo;

// timeout type info accepts int or date and returns an int giving milliseconds
static TimeoutTypeInfo staticTimeoutTypeInfo;
static TimeoutOrNothingTypeInfo staticTimeoutOrNothingTypeInfo;
const QoreTypeInfo* timeoutTypeInfo = &staticTimeoutTypeInfo,
   *timeoutOrNothingTypeInfo = &staticTimeoutOrNothingTypeInfo;

static IntOrFloatTypeInfo staticIntOrFloatTypeInfo;
const QoreTypeInfo* bigIntOrFloatTypeInfo = &staticIntOrFloatTypeInfo;

static IntFloatOrNumberTypeInfo staticIntFloatOrNumberTypeInfo;
const QoreTypeInfo* bigIntFloatOrNumberTypeInfo = &staticIntFloatOrNumberTypeInfo;

static FloatOrNumberTypeInfo staticFloatOrNumberTypeInfo;
const QoreTypeInfo* floatOrNumberTypeInfo = &staticFloatOrNumberTypeInfo;

QoreListNode* emptyList;
QoreHashNode* emptyHash;
QoreStringNode* NullString;
DateTimeNode* ZeroDate;
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

static void do_maps(qore_type_t t, const char* name, const QoreTypeInfo* typeInfo, const QoreTypeInfo* orNothingTypeInfo = 0) {
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

   // static "or nothing" reference types
   bigIntOrNothingTypeInfo    = new OrNothingTypeInfo(staticBigIntTypeInfo, "int"); 
   stringOrNothingTypeInfo    = new OrNothingTypeInfo(staticStringTypeInfo, "string");
   boolOrNothingTypeInfo      = new OrNothingTypeInfo(staticBoolTypeInfo, "bool");
   binaryOrNothingTypeInfo    = new OrNothingTypeInfo(staticBinaryTypeInfo, "binary");
   objectOrNothingTypeInfo    = new OrNothingTypeInfo(staticObjectTypeInfo, "object");
   dateOrNothingTypeInfo      = new OrNothingTypeInfo(staticDateTypeInfo, "date");
   hashOrNothingTypeInfo      = new OrNothingTypeInfo(staticHashTypeInfo, "hash");
   listOrNothingTypeInfo      = new OrNothingTypeInfo(staticListTypeInfo, "list");
   nullOrNothingTypeInfo      = new OrNothingTypeInfo(staticNullTypeInfo, "null");
   referenceOrNothingTypeInfo = new OrNothingTypeInfo(staticReferenceTypeInfo, "reference");

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
   do_maps(NT_NOTHING,     "nothing", nothingTypeInfo);

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
   do_maps(NT_RUNTIME_CLOSURE, "closure", codeTypeInfo);
   do_maps(NT_FUNCREF, "callref", codeTypeInfo);
}

void delete_qore_types() {
   // dereference all values from default value map
   for (def_val_map_t::iterator i = def_val_map.begin(), e = def_val_map.end(); i != e; ++i)
      i->second->deref(0);

   // dereference global default values
   NullString->deref();
   piNumber->deref();
   InfinityNumber->deref();
   NaNumber->deref();
   ZeroNumber->deref();
   ZeroFloat->deref();
   Zero->deref();
   ZeroDate->deref();
   emptyList->deref(0);
   emptyHash->deref(0);

   // delete global typeinfo structures
   delete bigIntOrNothingTypeInfo; 
   delete stringOrNothingTypeInfo;
   delete boolOrNothingTypeInfo;
   delete binaryOrNothingTypeInfo;
   delete objectOrNothingTypeInfo;
   delete dateOrNothingTypeInfo;
   delete hashOrNothingTypeInfo;
   delete listOrNothingTypeInfo;
   delete nullOrNothingTypeInfo;
   delete referenceOrNothingTypeInfo;
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
   qore_type_t t = n ? n->getType() : NT_NOTHING;
   if (t != NT_OBJECT)
      return getTypeInfoForType(t);
   return reinterpret_cast<const QoreObject *>(n)->getClass()->getTypeInfo();
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
   return i != str_typeinfo_map.end() ? i->second : 0;
}

const QoreTypeInfo* getBuiltinUserOrNothingTypeInfo(const char* str) {
   str_typeinfo_map_t::iterator i = str_ornothingtypeinfo_map.find(str);
   return i != str_ornothingtypeinfo_map.end() ? i->second : 0;
}

const char* getBuiltinTypeName(qore_type_t type) {
   type_str_map_t::iterator i = type_str_map.find(type);
   if (i != type_str_map.end())
      return i->second;

   const QoreTypeInfo* typeInfo = getExternalTypeInfoForType(type);
   if (typeInfo)
      return typeInfo->getName();

   // implement for types that should not be available in user code
   //switch (type) {
   //   case NT_DATA:
   // return "string|binary";
   //}

   /*
   printd(0, "type: %d unknown (map size: %d)\n", type, type_str_map.size());
   for (type_str_map_t::iterator i = type_str_map.begin(), e = type_str_map.end(); i != e; ++i)
      printd(0, "map[%d] = %s\n", i->first, i->second);
      
   assert(false);
   */

   return "<unknown type>";
}

int QoreTypeInfo::runtimeAcceptInputIntern(bool &priv_error, AbstractQoreNode* n) const {
   qore_type_t nt = get_node_type(n);

   if (qt != nt)
      return is_int && nt > QORE_NUM_TYPES && dynamic_cast<QoreBigIntNode* >(n) ? 0 : -1;

   if (qt != NT_OBJECT || !qc)
      return 0;

   bool priv;
   if (reinterpret_cast<const QoreObject*>(n)->getClass()->getClass(*qc, priv)) {
      if (!priv)
	 return 0;

      // check private access if required class is privately
      // inherited in the input argument's class
      if (qore_class_private::runtimeCheckPrivateClassAccess(*qc))
	 return 0;
      
      priv_error = true;
   }

   return -1;
}

int QoreTypeInfo::acceptInputDefault(bool& priv_error, AbstractQoreNode* n) const {
   //printd(5, "QoreTypeInfo::acceptInputDefault() this=%p hasType=%d (%s) n=%p (%s)\n", this, hasType(), getName(), n, get_type_name(n));
   if (!hasType())
      return 0;

   if (!accepts_mult)
      return runtimeAcceptInputIntern(priv_error, n);

   const type_vec_t &at = getAcceptTypeList();

   // check all types until one accepts the input
   // priv_error can be set to false more than once; this is OK for error reporting
   for (type_vec_t::const_iterator i = at.begin(), e = at.end(); i != e; ++i) {
      assert((*i)->acceptsSingle());
      if (!(*i)->runtimeAcceptInputIntern(priv_error, n))
	 return 0;
   }

   return runtimeAcceptInputIntern(priv_error, n);
}

bool QoreTypeInfo::isInputIdentical(const QoreTypeInfo* typeInfo) const {
   bool thisnt = (!hasType());
   bool typent = (!typeInfo->hasType());

   if (thisnt && typent)
      return true;

   if (thisnt || typent)
      return false;

   // from this point on, we know that both have types and are not NULL
   if ((accepts_mult && !typeInfo->accepts_mult)
       || (!accepts_mult && typeInfo->accepts_mult))
      return false;

   // from here on, we know either both accept single types or both accept multiple types
   if (!accepts_mult)
      return isTypeIdenticalIntern(typeInfo);

   const type_vec_t &my_at = getAcceptTypeList();
   const type_vec_t &their_at = typeInfo->getAcceptTypeList();

   if (my_at.size() != their_at.size())
      return false;

   // check all types to see if there is an identical type
   for (type_vec_t::const_iterator i = my_at.begin(), e = my_at.end(); i != e; ++i) {
      bool ident = false;
      for (type_vec_t::const_iterator j = their_at.begin(), je = their_at.end(); j != je; ++j) {
	 //printd(5, "QoreTypeInfo::isInputIdentical() this=%p i=%p %s j=%p %s\n", this, *i, (*i)->getName(), *j, (*j)->getName());

	 // if the second type is the original type, skip it
	 if (*j == this)
	    continue;

	 if ((*i) == (*j) || (*i)->isInputIdentical(*j)) {
	    ident = true;
	    break;
	 }
      }
      if (!ident)
	 return false;
   }

   return true;
}

// if the argument's return type is compatible with "this"'s return type
bool QoreTypeInfo::isOutputCompatible(const QoreTypeInfo* typeInfo) const {
   if (!hasType())
      return true;

   if (!typeInfo->hasType())
      return false;

   // from this point on, we know that both have types and are not NULL
   if (!typeInfo->returns_mult) {
      //printd(5, "QoreTypeInfo::isOutputCompatible(%p '%s') this: %p '%s' (qc: %p qt: %d) ti->qc: %p ti->qt: %d\n", typeInfo, typeInfo->getName(), this, getName(), qc, qt, typeInfo->qc, typeInfo->qt);
      return typeInfo->qc ? parseReturnsClass(typeInfo->qc) : parseReturnsType(typeInfo->qt);
   }

   const type_vec_t &their_rt = typeInfo->getReturnTypeList();

   for (type_vec_t::const_iterator j = their_rt.begin(), je = their_rt.end(); j != je; ++j) {
      if (!isOutputCompatible(*j))
         return false;
   }

   return true;
}

bool QoreTypeInfo::isOutputIdentical(const QoreTypeInfo* typeInfo) const {
   bool thisnt = (!hasType());
   bool typent = (!typeInfo->hasType());

   if (thisnt && typent)
      return true;

   if (thisnt || typent)
      return false;

   // from this point on, we know that both have types and are not NULL
   if ((returns_mult && !typeInfo->returns_mult)
       || (!returns_mult && typeInfo->returns_mult)) {
      //printd(5, "QoreTypeInfo::isOutputIdentical() lrm: %d rrm: %d\n", returns_mult, typeInfo->returns_mult);
      return false;
   }

   // from here on, we know either both accept single types or both accept multiple types
   if (!returns_mult)
      return isTypeIdenticalIntern(typeInfo);

   const type_vec_t &my_rt = getReturnTypeList();
   const type_vec_t &their_rt = typeInfo->getReturnTypeList();

   if (my_rt.size() != their_rt.size()) {
      //printd(5, "QoreTypeInfo::isOutputIdentical() lrts: %d rrts: %d\n", my_rt.size(), their_rt.size());
      return false;
   }

   // check all types to see if there is an identical type
   // FIXME: this is not very efficient (also only works properly if all types are unique in the list, which they should be)
   for (type_vec_t::const_iterator i = my_rt.begin(), e = my_rt.end(); i != e; ++i) {
      bool ident = false;
      for (type_vec_t::const_iterator j = their_rt.begin(), je = their_rt.end(); j != je; ++j) {
	 if ((*i)->isOutputIdentical(*j)) {
	    ident = true;
	    break;
	 }
      }
      if (!ident) {
         //printd(5, "QoreTypeInfo::isOutputIdentical() cannot find match for %s in rhs\n", (*i)->getName());
	 return false;
      }
   }

   return true;
}

qore_type_result_e QoreTypeInfo::matchClassIntern(const QoreClass *n_qc) const {
   if (qt == NT_ALL)
      return QTI_AMBIGUOUS;
   
   if (qt != NT_OBJECT)
      return QTI_NOT_EQUAL;

   if (!qc)
      return QTI_AMBIGUOUS;

   qore_type_result_e rc = qore_class_private::parseCheckCompatibleClass(*qc, *n_qc);
   if (rc == QTI_IDENT && !exact_return)
      return QTI_AMBIGUOUS;
   return rc;
}

qore_type_result_e QoreTypeInfo::runtimeMatchClassIntern(const QoreClass *n_qc) const {
   if (qt == NT_ALL)
      return QTI_AMBIGUOUS;

   if (qt != NT_OBJECT)
      return QTI_NOT_EQUAL;

   if (!qc)
      return QTI_AMBIGUOUS;

   qore_type_result_e rc = qore_class_private::runtimeCheckCompatibleClass(*qc, *n_qc);
   if (rc == QTI_IDENT && !exact_return)
      return QTI_AMBIGUOUS;
   return rc;
}

void QoreTypeInfo::doNonNumericWarning(const char* preface) const {
   QoreStringNode* desc = new QoreStringNode(preface);
   getThisType(*desc);
   desc->sprintf(", which does not evaluate to a numeric type, therefore will always evaluate to 0 at runtime");
   qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

void QoreTypeInfo::doNonBooleanWarning(const char* preface) const {
   QoreStringNode* desc = new QoreStringNode(preface);
   getThisType(*desc);
   desc->sprintf(", which does not evaluate to a numeric or boolean type, therefore will always evaluate to False at runtime");
   qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

void QoreTypeInfo::doNonStringWarning(const char* preface) const {
   QoreStringNode* desc = new QoreStringNode(preface);
   getThisType(*desc);
   desc->sprintf(", which cannot be converted to a string, therefore will always evaluate to an empty string at runtime");
   qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
}

const QoreTypeInfo* QoreParseTypeInfo::resolveAndDelete(const QoreProgramLocation& loc) {
   if (!this)
      return 0;

   // resolve class
   const QoreClass *qc = qore_root_ns_private::parseFindScopedClass(loc, *cscope);

   bool my_or_nothing = or_nothing;
   delete this;

   if (qc && my_or_nothing) {
      const QoreTypeInfo* rv = qc->getOrNothingTypeInfo();
      if (!rv) {
	 parse_error(loc, "class %s cannot be typed with '*' as the class' type handler has an input filter and the filter does not accept NOTHING", qc->getName());
	 return objectOrNothingTypeInfo;
      }
      return rv;
   }

   // qc maybe NULL when the class is not found
   return qc ? qc->getTypeInfo() : objectTypeInfo;
}
/*
AbstractVirtualMethod::AbstractVirtualMethod(const char* n_name, bool n_requires_lvalue, const QoreTypeInfo* n_return_type, ...)
   : name(n_name), requires_value(n_requires_lvalue), return_type(n_return_type) {
   va_list args;
   va_start(args, n_return_type);
   while (true) {
      QoreParam p = va_arg(args, QoreParam);
      if (!p.name)
	 break;

      params.push_back(p);
   }
   va_end(args);
}
*/

bool OrNothingTypeInfo::acceptInputImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
   qore_type_t t = get_node_type(n);
   if (n && t == NT_NULL) {
      n = &Nothing;
      return true;
   }
   if (t == NT_NOTHING)
      return true;
   
   if (qc) {
      if (t != NT_OBJECT)
	 return false;
      const QoreClass* n_qc = reinterpret_cast<const QoreObject*>(n)->getClass();

      return qore_class_private::runtimeCheckCompatibleClass(*qc, *n_qc);
   }

   return t == qt;
}
