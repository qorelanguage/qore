/*
 QoreTypeInfo.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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

// static reference types
static QoreTypeInfo staticAnyTypeInfo,
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

// provides for run-time assignment capability from any type
static UserReferenceTypeInfo staticUserReferenceTypeInfo;

// provides for 2-way compatibility with classes derived from QoreBigIntNode and softint
static BigIntTypeInfo staticBigIntTypeInfo;

// provides limited compatibility with integers
static FloatTypeInfo staticFloatTypeInfo;

// provides 2-way compatibilty with classes derived from QoreStringNode
static StringTypeInfo staticStringTypeInfo;

// provides equal compatibility with closures and all types of code references
static CodeTypeInfo staticCodeTypeInfo;

// either string or binary
static DataTypeInfo staticDataTypeInfo;

// provides int compatibility with and conversions from float, string, date, and bool
static SoftBigIntTypeInfo staticSoftBigIntTypeInfo;

// provides float compatibility with and conversions from int, string, date, and bool
static SoftFloatTypeInfo staticSoftFloatTypeInfo;

// provides bool compatibility with and conversions from float, string, date, and int
static SoftBoolTypeInfo staticSoftBoolTypeInfo;

// provides string compatibility with and conversions from float, int, date, and bool
static SoftStringTypeInfo staticSoftStringTypeInfo;

// somethingTypeInfo means "not NOTHING"
static SomethingTypeInfo staticSomethingTypeInfo;

// const pointers to static reference types
const QoreTypeInfo *anyTypeInfo = &staticAnyTypeInfo,
   *bigIntTypeInfo = &staticBigIntTypeInfo,
   *floatTypeInfo = &staticFloatTypeInfo,
   *boolTypeInfo = &staticBoolTypeInfo,
   *stringTypeInfo = &staticStringTypeInfo,
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
   *userReferenceTypeInfo = &staticUserReferenceTypeInfo,
   *codeTypeInfo = &staticCodeTypeInfo,
   *softBigIntTypeInfo = &staticSoftBigIntTypeInfo,
   *softFloatTypeInfo = &staticSoftFloatTypeInfo,
   *softBoolTypeInfo = &staticSoftBoolTypeInfo,
   *softStringTypeInfo = &staticSoftStringTypeInfo,
   *somethingTypeInfo = &staticSomethingTypeInfo,
   *dataTypeInfo = &staticDataTypeInfo
   ;

QoreListNode *emptyList;
QoreHashNode *emptyHash;
QoreStringNode *NullString;
DateTimeNode *ZeroDate;
QoreBigIntNode *Zero;

// map from types to default values
typedef std::map<qore_type_t, AbstractQoreNode *> def_val_map_t;
static def_val_map_t def_val_map;

// map from names used when parsing to types
typedef std::map<const char *, const QoreTypeInfo *, ltstr> str_typeinfo_map_t;
static str_typeinfo_map_t str_typeinfo_map;

// map from types to type info
typedef std::map<qore_type_t, const QoreTypeInfo *> type_typeinfo_map_t;
static type_typeinfo_map_t type_typeinfo_map;

// global external type map
static type_typeinfo_map_t extern_type_info_map;

// map from types to names
typedef std::map<qore_type_t, const char *> type_str_map_t;
static type_str_map_t type_str_map;

// rwlock for global type map
static QoreRWLock extern_type_info_map_lock;

static void do_maps(qore_type_t t, const char *name, const QoreTypeInfo *typeInfo) {
   str_typeinfo_map[name] = typeInfo;
   type_typeinfo_map[t]   = typeInfo;
   type_str_map[t]        = name;
}

// at least the NullString must be created after the default character encoding is set
void init_qore_types() {
   // initialize global default values                                                                                                                                    
   NullString    = new QoreStringNode;
   ZeroDate      = DateTimeNode::makeAbsolute(0, 0, 0);
   Zero          = new QoreBigIntNode;

   emptyList     = new QoreListNode;
   emptyHash     = new QoreHashNode;

   def_val_map[NT_INT]     = Zero->refSelf();
   def_val_map[NT_STRING]  = NullString->refSelf();
   def_val_map[NT_BOOLEAN] = &False;
   def_val_map[NT_DATE]    = ZeroDate->refSelf();
   def_val_map[NT_FLOAT]   = new QoreFloatNode;
   def_val_map[NT_LIST]    = emptyList->refSelf();
   def_val_map[NT_HASH]    = emptyHash->refSelf();
   def_val_map[NT_BINARY]  = new BinaryNode;
   def_val_map[NT_NULL]    = &Null;
   def_val_map[NT_NOTHING] = &Nothing;

   do_maps(NT_INT,         "int", bigIntTypeInfo);
   do_maps(NT_STRING,      "string", stringTypeInfo);
   do_maps(NT_BOOLEAN,     "bool", boolTypeInfo);
   do_maps(NT_FLOAT,       "float", floatTypeInfo);
   do_maps(NT_BINARY,      "binary", binaryTypeInfo);
   do_maps(NT_LIST,        "list", listTypeInfo);
   do_maps(NT_HASH,        "hash", hashTypeInfo);
   do_maps(NT_OBJECT,      "object", objectTypeInfo);
   do_maps(NT_ALL,         "any", anyTypeInfo);
   do_maps(NT_DATE,        "date", dateTypeInfo);
   do_maps(NT_CODE,        "code", codeTypeInfo);
   do_maps(NT_REFERENCE,   "reference", referenceTypeInfo);
   do_maps(NT_NULL,        "null", nullTypeInfo);
   do_maps(NT_NOTHING,     "nothing", nothingTypeInfo);
   do_maps(NT_SOFTINT,     "softint", softBigIntTypeInfo);
   do_maps(NT_SOFTFLOAT,   "softfloat", softFloatTypeInfo);
   do_maps(NT_SOFTBOOLEAN, "softbool", softBoolTypeInfo);
   do_maps(NT_SOFTSTRING,  "softstring", softStringTypeInfo);

   // map the closure and callref strings to codeTypeInfo to ensure that these
   // types are always interchangable
   do_maps(NT_RUNTIME_CLOSURE, "closure", codeTypeInfo);
   do_maps(NT_FUNCREF, "callref", codeTypeInfo);
}

void delete_qore_types() {
   // delete all values from default value map
   for (def_val_map_t::iterator i = def_val_map.begin(), e = def_val_map.end(); i != e; ++i)
      i->second->deref(0);

   // dereference global default values
   NullString->deref();
   Zero->deref();
   ZeroDate->deref();
   emptyList->deref(0);
   emptyHash->deref(0);
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

const char *getBuiltinTypeName(qore_type_t type) {
   type_str_map_t::iterator i = type_str_map.find(type);
   if (i != type_str_map.end())
      return i->second;

   const QoreTypeInfo *typeInfo = getExternalTypeInfoForType(type);
   if (typeInfo)
      return typeInfo->getName();

   // implement for types that should not be available in user code
   switch (type) {
      case NT_DATA:
	 return "string|binary";
   }

   /*
   printf("type: %d unknown\n", type);
   for (type_str_map_t::iterator i = type_str_map.begin(), e = type_str_map.end(); i != e; ++i)
      printf("map[%d] = %s\n", i->first, i->second);
      
   assert(false);
   */

   return "<unknown type>";
}

int QoreTypeInfo2::runtimeAcceptInputIntern(bool &priv_error, AbstractQoreNode *n) const {
   assert(!accepts_mult);

   qore_type_t nt = n && n->getType();

   if (qt != nt)
      return -1;

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

int QoreTypeInfo2::acceptInputDefault(bool &priv_error, AbstractQoreNode *n) const {
   //printd(0, "QoreTypeInfo2::acceptInputDefault() this=%p hasType=%d (%s) n=%p (%s)\n", this, hasType(), getName(), n, get_type_name(n));
   if (!hasType())
      return 0;

   if (!accepts_mult)
      return runtimeAcceptInputIntern(priv_error, n);

   const type_vec2_t &at = getAcceptTypeList();

   // check all types until one accepts the input
   // priv_error can be set to false more than once; this is OK for error reporting
   for (type_vec2_t::const_iterator i = at.begin(), e = at.end(); i != e; ++i) {
      assert((*i)->acceptsSingle());
      if (!(*i)->runtimeAcceptInputIntern(priv_error, n))
	 return 0;
   }

   return -1;
}

bool QoreTypeInfo2::isInputIdentical(const QoreTypeInfo2 *typeInfo) const {
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
      return isInputIdenticalIntern(typeInfo);

   const type_vec2_t &my_at = getAcceptTypeList();
   const type_vec2_t &their_at = typeInfo->getAcceptTypeList();

   if (my_at.size() != their_at.size())
      return false;

   // check all types to see if there is an identical type
   for (type_vec2_t::const_iterator i = my_at.begin(), e = my_at.end(); i != e; ++i) {
      assert((*i)->acceptsSingle());

      bool ident = false;
      for (type_vec2_t::const_iterator j = their_at.begin(), e = their_at.end(); i != e; ++i) {
	 assert((*j)->acceptsSingle());

	 if ((*i)->isInputIdenticalIntern(*j)) {
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

   if (cscope) {
      // resolve class
      qc = getRootNS()->parseFindScopedClass(cscope);
      delete cscope;
      cscope = 0;
   }

   const QoreTypeInfo *rv = qc ? qc->getTypeInfo() : getTypeInfoForType(qt);
   delete this;
   return rv;
}

int QoreTypeInfo::checkTypeInstantiationDefault(AbstractQoreNode *n, bool &priv_error) const {
   //printd(0, "QoreTypeInfo::checkTypeInstantiationIntern() this=%p has_type=%d (%s) n=%p (%s)\n", this, this ? has_type : 0, getName(), n, n ? n->getTypeName() : "NOTHING");
   if (!this || !has_type) return 0;
   if (qt == NT_NOTHING && is_nothing(n)) return 0;
   if (is_nothing(n))
      return -1;

   qore_type_t t = n->getType();

   if (qt != t)
      return -1;

   // from here on we know n != 0
   if (qt == NT_OBJECT) {
      if (!qc)
	 return 0;

      bool priv;
      if (reinterpret_cast<const QoreObject *>(n)->getClass(qc->getID(), priv)) {
	 if (!priv)
	    return 0;

	 // check private access
	 if (runtimeCheckPrivateClassAccess(qc))
	    return 0;

	 priv_error = true;
      }

      return -1;
   }

   return 0;
}

int QoreTypeInfo::testTypeCompatibilityDefault(const AbstractQoreNode *n) const {
   if (!this || !has_type) return QTI_AMBIGUOUS;
   if (qt == NT_NOTHING && is_nothing(n)) return QTI_IDENT;
   if (is_nothing(n))
      return QTI_NOT_EQUAL;

   qore_type_t t = n->getType();

   // from here on we know n != 0
   if (qt == NT_OBJECT) {
      if (t != qt)
	 return QTI_NOT_EQUAL;
      if (!qc)
	 return QTI_AMBIGUOUS;

      return testObjectClassAccess(reinterpret_cast<const QoreObject *>(n), qc);
   }

   if (t == qt)
      return QTI_IDENT;

   return QTI_NOT_EQUAL;
}

int QoreTypeInfo::parseEqualDefault(const QoreTypeInfo *typeInfo) const {
   if (!this || !has_type || !typeInfo || !typeInfo->has_type)
      return QTI_AMBIGUOUS;

   // FIXME: when references can be typed, then check the type here
   if (typeInfo->qt == NT_REFERENCE)
      return QTI_AMBIGUOUS;

   if (qt == NT_OBJECT) {
      if (typeInfo->qt != NT_OBJECT)
	 return QTI_NOT_EQUAL;

      if (!qc || !typeInfo->qc)
	 return QTI_AMBIGUOUS;

      if (qc == typeInfo->qc)
	 return QTI_IDENT;

      return parseCheckCompatibleClass(qc, typeInfo->qc) ? QTI_IDENT : QTI_NOT_EQUAL;
   }

   if (typeInfo->qt == qt)
      return QTI_IDENT;

   return typeInfo->compat_qt == qt || typeInfo->qt == compat_qt ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
}

bool QoreTypeInfo::parseTestCompatibleClass(const QoreClass *otherclass) const {
   if (qc == otherclass)
      return true;

   if (!qc)
      return false;

   bool priv;
   if (!qc->getClass(otherclass->getID(), priv))
      return false;

   if (!priv)
      return true;

   return parseCheckPrivateClassAccess(otherclass);
}
