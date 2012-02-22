/*
 QoreTypeInfo.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qore/Qore.h>
#include <qore/QoreRWLock.h>
#include <qore/intern/QoreNamespaceIntern.h>

// provides for 2-way compatibility with classes derived from QoreBigIntNode and softint
static BigIntTypeInfo staticBigIntTypeInfo;
const QoreTypeInfo *bigIntTypeInfo = &staticBigIntTypeInfo;

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
   staticCallReferenceTypeInfo(NT_FUNCREF),
   staticReferenceTypeInfo(NT_REFERENCE)
   ;

// const pointers to static reference types
const QoreTypeInfo *anyTypeInfo = &staticAnyTypeInfo,
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
   *referenceTypeInfo = &staticReferenceTypeInfo,
   
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

// provides for run-time assignment capability from any type
static UserReferenceTypeInfo staticUserReferenceTypeInfo;
const QoreTypeInfo *userReferenceTypeInfo = &staticUserReferenceTypeInfo;

// provides limited compatibility with integers
static FloatTypeInfo staticFloatTypeInfo;
static FloatOrNothingTypeInfo staticFloatOrNothingTypeInfo;
const QoreTypeInfo *floatTypeInfo = &staticFloatTypeInfo,
   *floatOrNothingTypeInfo = &staticFloatOrNothingTypeInfo;

// provides equal compatibility with closures and all types of code references
static CodeTypeInfo staticCodeTypeInfo;
static CodeOrNothingTypeInfo staticCodeOrNothingTypeInfo;
const QoreTypeInfo *codeTypeInfo = &staticCodeTypeInfo,
   *codeOrNothingTypeInfo = &staticCodeOrNothingTypeInfo;

// either string or binary
static DataTypeInfo staticDataTypeInfo;
static DataOrNothingTypeInfo staticDataOrNothingTypeInfo;
const QoreTypeInfo *dataTypeInfo = &staticDataTypeInfo,
   *dataOrNothingTypeInfo = &staticDataOrNothingTypeInfo;

// provides int compatibility with and conversions from float, string, date, and bool
static SoftBigIntTypeInfo staticSoftBigIntTypeInfo;
static SoftBigIntOrNothingTypeInfo staticBigIntOrNothingTypeInfo;
const QoreTypeInfo *softBigIntTypeInfo = &staticSoftBigIntTypeInfo,
   *softBigIntOrNothingTypeInfo = &staticBigIntOrNothingTypeInfo;

// provides float compatibility with and conversions from int, string, date, and bool
static SoftFloatTypeInfo staticSoftFloatTypeInfo;
static SoftFloatOrNothingTypeInfo staticSoftFloatOrNothingTypeInfo;
const QoreTypeInfo *softFloatTypeInfo = &staticSoftFloatTypeInfo,
   *softFloatOrNothingTypeInfo = &staticSoftFloatOrNothingTypeInfo;

// provides bool compatibility with and conversions from float, string, date, and int
static SoftBoolTypeInfo staticSoftBoolTypeInfo;
static SoftBoolOrNothingTypeInfo staticSoftBoolOrNothingTypeInfo;
const QoreTypeInfo *softBoolTypeInfo = &staticSoftBoolTypeInfo,
   *softBoolOrNothingTypeInfo = &staticSoftBoolOrNothingTypeInfo;

// provides string compatibility with and conversions from float, int, date, and bool
static SoftStringTypeInfo staticSoftStringTypeInfo;
static SoftStringOrNothingTypeInfo staticSoftStringOrNothingTypeInfo;
const QoreTypeInfo *softStringTypeInfo = &staticSoftStringTypeInfo,
   *softStringOrNothingTypeInfo = &staticSoftStringOrNothingTypeInfo;

static SoftDateTypeInfo staticSoftDateTypeInfo;
static SoftDateOrNothingTypeInfo staticSoftDateOrNothingTypeInfo;
const QoreTypeInfo *softDateTypeInfo = &staticSoftDateTypeInfo,
   *softDateOrNothingTypeInfo = &staticSoftDateOrNothingTypeInfo;

// provides automatic conversion to a list
static SoftListTypeInfo staticSoftListTypeInfo;
static SoftListOrNothingTypeInfo staticListOrNothingTypeInfo;
const QoreTypeInfo *softListTypeInfo = &staticSoftListTypeInfo,
   *softListOrNothingTypeInfo = &staticListOrNothingTypeInfo;

// somethingTypeInfo means "not NOTHING"
static SomethingTypeInfo staticSomethingTypeInfo;
const QoreTypeInfo *somethingTypeInfo = &staticSomethingTypeInfo;

// timeout type info accepts int or date and returns an int giving milliseconds
static TimeoutTypeInfo staticTimeoutTypeInfo;
static TimeoutOrNothingTypeInfo staticTimeoutOrNothingTypeInfo;
const QoreTypeInfo *timeoutTypeInfo = &staticTimeoutTypeInfo,
   *timeoutOrNothingTypeInfo = &staticTimeoutOrNothingTypeInfo;

QoreListNode *emptyList;
QoreHashNode *emptyHash;
QoreStringNode *NullString;
DateTimeNode *ZeroDate;
QoreBigIntNode *Zero;
QoreFloatNode *ZeroFloat;

// map from types to default values
typedef std::map<qore_type_t, AbstractQoreNode *> def_val_map_t;
static def_val_map_t def_val_map;

// map from names used when parsing to types
typedef std::map<const char *, const QoreTypeInfo *, ltstr> str_typeinfo_map_t;
static str_typeinfo_map_t str_typeinfo_map;
static str_typeinfo_map_t str_ornothingtypeinfo_map;

// map from types to type info
typedef std::map<qore_type_t, const QoreTypeInfo *> type_typeinfo_map_t;
static type_typeinfo_map_t type_typeinfo_map;
static type_typeinfo_map_t type_ornothingtypeinfo_map;

// global external type map
static type_typeinfo_map_t extern_type_info_map;

// map from types to names
typedef std::map<qore_type_t, const char *> type_str_map_t;
static type_str_map_t type_str_map;

// rwlock for global type map
static QoreRWLock extern_type_info_map_lock;

static void do_maps(qore_type_t t, const char *name, const QoreTypeInfo *typeInfo, const QoreTypeInfo *orNothingTypeInfo = 0) {
   str_typeinfo_map[name]          = typeInfo;
   str_ornothingtypeinfo_map[name] = orNothingTypeInfo;
   type_typeinfo_map[t]            = typeInfo;
   type_ornothingtypeinfo_map[t]   = orNothingTypeInfo;
   type_str_map[t]                 = name;
}

// at least the NullString must be created after the default character encoding is set
void init_qore_types() {
   // initialize global default values
   NullString    = new QoreStringNode;
   ZeroDate      = DateTimeNode::makeAbsolute(0, 0, 0);
   Zero          = new QoreBigIntNode;
   ZeroFloat     = new QoreFloatNode;

   emptyList     = new QoreListNode;
   emptyHash     = new QoreHashNode;

   def_val_map[NT_INT]     = Zero->refSelf();
   def_val_map[NT_STRING]  = NullString->refSelf();
   def_val_map[NT_BOOLEAN] = &False;
   def_val_map[NT_DATE]    = ZeroDate->refSelf();
   def_val_map[NT_FLOAT]   = ZeroFloat->refSelf();
   def_val_map[NT_LIST]    = emptyList->refSelf();
   def_val_map[NT_HASH]    = emptyHash->refSelf();
   def_val_map[NT_BINARY]  = new BinaryNode;
   def_val_map[NT_NULL]    = &Null;
   def_val_map[NT_NOTHING] = &Nothing;

   // static "or nothing" reference types
   bigIntOrNothingTypeInfo = new OrNothingTypeInfo(staticBigIntTypeInfo, "int"); 
   stringOrNothingTypeInfo = new OrNothingTypeInfo(staticStringTypeInfo, "string");
   boolOrNothingTypeInfo   = new OrNothingTypeInfo(staticBoolTypeInfo, "bool");
   binaryOrNothingTypeInfo = new OrNothingTypeInfo(staticBinaryTypeInfo, "binary");
   objectOrNothingTypeInfo = new OrNothingTypeInfo(staticObjectTypeInfo, "object");
   dateOrNothingTypeInfo   = new OrNothingTypeInfo(staticDateTypeInfo, "date");
   hashOrNothingTypeInfo   = new OrNothingTypeInfo(staticHashTypeInfo, "hash");
   listOrNothingTypeInfo   = new OrNothingTypeInfo(staticListTypeInfo, "list");
   nullOrNothingTypeInfo   = new OrNothingTypeInfo(staticNullTypeInfo, "null");

   do_maps(NT_INT,         "int", bigIntTypeInfo, bigIntOrNothingTypeInfo);
   do_maps(NT_STRING,      "string", stringTypeInfo, stringOrNothingTypeInfo);
   do_maps(NT_BOOLEAN,     "bool", boolTypeInfo, boolOrNothingTypeInfo);
   do_maps(NT_FLOAT,       "float", floatTypeInfo, floatOrNothingTypeInfo);
   do_maps(NT_BINARY,      "binary", binaryTypeInfo, binaryOrNothingTypeInfo);
   do_maps(NT_LIST,        "list", listTypeInfo, listOrNothingTypeInfo);
   do_maps(NT_HASH,        "hash", hashTypeInfo, hashOrNothingTypeInfo);
   do_maps(NT_OBJECT,      "object", objectTypeInfo, objectOrNothingTypeInfo);
   do_maps(NT_ALL,         "any", anyTypeInfo);
   do_maps(NT_DATE,        "date", dateTypeInfo, dateOrNothingTypeInfo);
   do_maps(NT_CODE,        "code", codeTypeInfo, codeOrNothingTypeInfo);
   do_maps(NT_DATA,        "data", dataTypeInfo, dataOrNothingTypeInfo);
   do_maps(NT_REFERENCE,   "reference", referenceTypeInfo, anyTypeInfo);
   do_maps(NT_NULL,        "null", nullTypeInfo, nullOrNothingTypeInfo);
   do_maps(NT_NOTHING,     "nothing", nothingTypeInfo);

   do_maps(NT_SOFTINT,     "softint", softBigIntTypeInfo, softBigIntOrNothingTypeInfo);
   do_maps(NT_SOFTFLOAT,   "softfloat", softFloatTypeInfo, softFloatOrNothingTypeInfo);
   do_maps(NT_SOFTBOOLEAN, "softbool", softBoolTypeInfo, softBoolOrNothingTypeInfo);
   do_maps(NT_SOFTSTRING,  "softstring", softStringTypeInfo, softStringOrNothingTypeInfo);
   do_maps(NT_SOFTDATE,    "softdate", softDateTypeInfo, softDateOrNothingTypeInfo);
   do_maps(NT_SOFTLIST,    "softlist", softListTypeInfo, softListOrNothingTypeInfo);

   do_maps(NT_TIMEOUT,     "timeout", timeoutTypeInfo, timeoutOrNothingTypeInfo);

   // map the closure and callref strings to codeTypeInfo to ensure that these
   // types are always interchangable
   do_maps(NT_RUNTIME_CLOSURE, "closure", codeTypeInfo);
   do_maps(NT_FUNCREF, "callref", codeTypeInfo);
}

void delete_qore_types() {
   // dereference all values from default value map
   for (def_val_map_t::iterator i = def_val_map.begin(), e = def_val_map.end(); i != e; ++i)
      i->second->deref(0);

   // dereference global default values
   NullString->deref();
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
}

void add_to_type_map(qore_type_t t, const QoreTypeInfo *typeInfo) {
   QoreAutoRWWriteLocker al(extern_type_info_map_lock);
   assert(extern_type_info_map.find(t) == extern_type_info_map.end());
   extern_type_info_map[t] = typeInfo;
}

static const QoreTypeInfo *getExternalTypeInfoForType(qore_type_t t) {
   QoreAutoRWReadLocker al(extern_type_info_map_lock);
   type_typeinfo_map_t::iterator i = extern_type_info_map.find(t);
   return (i == extern_type_info_map.end() ? 0 : i->second);
}

const QoreTypeInfo *getTypeInfoForType(qore_type_t t) {
   type_typeinfo_map_t::iterator i = type_typeinfo_map.find(t);
   return i != type_typeinfo_map.end() ? i->second : getExternalTypeInfoForType(t);
}

const QoreTypeInfo *getTypeInfoForValue(const AbstractQoreNode *n) {
   qore_type_t t = n ? n->getType() : NT_NOTHING;
   if (t != NT_OBJECT)
      return getTypeInfoForType(t);
   return reinterpret_cast<const QoreObject *>(n)->getClass()->getTypeInfo();
}

AbstractQoreNode *getDefaultValueForBuiltinValueType(qore_type_t t) {
   def_val_map_t::iterator i = def_val_map.find(t);
   assert(i != def_val_map.end());
   return i->second->refSelf();
}

bool builtinTypeHasDefaultValue(qore_type_t t) {
   return def_val_map.find(t) != def_val_map.end();
}

const QoreTypeInfo *getBuiltinUserTypeInfo(const char *str) {
   // user exceptions here
   if (!strcmp(str, "reference"))
      return userReferenceTypeInfo;

   str_typeinfo_map_t::iterator i = str_typeinfo_map.find(str);
   return i != str_typeinfo_map.end() ? i->second : 0;
}

const QoreTypeInfo *getBuiltinUserOrNothingTypeInfo(const char *str) {
   // user exceptions here
   if (!strcmp(str, "reference"))
      return anyTypeInfo;

   str_typeinfo_map_t::iterator i = str_ornothingtypeinfo_map.find(str);
   return i != str_ornothingtypeinfo_map.end() ? i->second : 0;
}

const char *getBuiltinTypeName(qore_type_t type) {
   type_str_map_t::iterator i = type_str_map.find(type);
   if (i != type_str_map.end())
      return i->second;

   const QoreTypeInfo *typeInfo = getExternalTypeInfoForType(type);
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

int QoreTypeInfo::runtimeAcceptInputIntern(bool &priv_error, AbstractQoreNode *n) const {
   qore_type_t nt = get_node_type(n);

   if (reverse_logic)
      return qt == nt ? -1 : 0;

   if (qt != nt)
      return is_int && nt > QORE_NUM_TYPES && dynamic_cast<QoreBigIntNode *>(n) ? 0 : -1;

   if (qt != NT_OBJECT || !qc)
      return 0;

   bool priv;
   if (reinterpret_cast<const QoreObject *>(n)->getClass(qc->getID(), priv)) {
      if (!priv)
	 return 0;

      // check private access if required class is privately
      // inherited in the input argument's class
      if (runtimeCheckPrivateClassAccess(qc))
	 return 0;
      
      priv_error = true;
   }

   return -1;
}

int QoreTypeInfo::acceptInputDefault(bool &priv_error, AbstractQoreNode *n) const {
   //printd(0, "QoreTypeInfo::acceptInputDefault() this=%p hasType=%d (%s) n=%p (%s)\n", this, hasType(), getName(), n, get_type_name(n));
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

bool QoreTypeInfo::isInputIdentical(const QoreTypeInfo *typeInfo) const {
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

bool QoreTypeInfo::isOutputIdentical(const QoreTypeInfo *typeInfo) const {
   bool thisnt = (!hasType());
   bool typent = (!typeInfo->hasType());

   if (thisnt && typent)
      return true;

   if (thisnt || typent)
      return false;

   // from this point on, we know that both have types and are not NULL
   if ((returns_mult && !typeInfo->returns_mult)
       || (!returns_mult && typeInfo->returns_mult))
      return false;

   // from here on, we know either both accept single types or both accept multiple types
   if (!returns_mult)
      return isTypeIdenticalIntern(typeInfo);

   const type_vec_t &my_rt = getReturnTypeList();
   const type_vec_t &their_rt = typeInfo->getReturnTypeList();

   if (my_rt.size() != their_rt.size())
      return false;

   // check all types to see if there is an identical type
   for (type_vec_t::const_iterator i = my_rt.begin(), e = my_rt.end(); i != e; ++i) {

      bool ident = false;
      for (type_vec_t::const_iterator j = their_rt.begin(), je = their_rt.end(); i != je; ++i) {
	 if ((*i)->isOutputIdentical(*j)) {
	    ident = true;
	    break;
	 }
      }
      if (!ident)
	 return false;
   }

   return true;
}

const QoreTypeInfo *QoreParseTypeInfo::resolveAndDelete() {
   if (!this)
      return 0;

   // resolve class
   const QoreClass *qc = qore_ns_private::parseFindScopedClass(*(getRootNS()), cscope);

   bool my_or_nothing = or_nothing;
   delete this;

   if (qc && my_or_nothing) {
      const QoreTypeInfo *rv = qc->getOrNothingTypeInfo();
      if (!rv) {
	 parse_error("class %s cannot be typed with '*' as the class' type handler has an input filter and the filter does not accept NOTHING", qc->getName());
	 return objectOrNothingTypeInfo;
      }
      return rv;
   }

   // qc maybe NULL when the class is not found
   return qc ? qc->getTypeInfo() : objectTypeInfo;
}
/*
AbstractVirtualMethod::AbstractVirtualMethod(const char *n_name, bool n_requires_lvalue, const QoreTypeInfo *n_return_type, ...) 
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
