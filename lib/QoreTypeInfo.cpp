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

// static reference types
static QoreTypeInfo staticAnyTypeInfo,
   staticBigIntTypeInfo(NT_INT), 
   staticBoolTypeInfo(NT_BOOLEAN),
   staticStringTypeInfo(NT_STRING),
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

// provides limited compatibility with integers
static FloatTypeInfo staticFloatTypeInfo;

// provides equal compatibility with closures and all types of code references
static CodeTypeInfo staticCodeTypeInfo;

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
   *codeTypeInfo = &staticCodeTypeInfo
   ;


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
   type_typeinfo_map[t] = typeInfo;
   type_str_map[t] = name;
}

class QoreTypeInitializer {
public:
   DLLLOCAL QoreTypeInitializer() {
      def_val_map[NT_INT] = new QoreBigIntNode;
      def_val_map[NT_STRING] = new QoreStringNode;
      def_val_map[NT_BOOLEAN] = &False;
      def_val_map[NT_DATE] = new DateTimeNode;
      def_val_map[NT_FLOAT] = new QoreFloatNode;
      def_val_map[NT_LIST] = new QoreListNode;
      def_val_map[NT_HASH] = new QoreHashNode;
      def_val_map[NT_BINARY] = new BinaryNode;
      def_val_map[NT_NULL] = &Null;
      def_val_map[NT_NOTHING] = &Nothing;

      do_maps(NT_INT, "int", bigIntTypeInfo);
      do_maps(NT_STRING, "string", stringTypeInfo);
      do_maps(NT_BOOLEAN, "bool", boolTypeInfo);
      do_maps(NT_FLOAT, "float", floatTypeInfo);
      do_maps(NT_BINARY, "binary", binaryTypeInfo);
      do_maps(NT_LIST, "list", listTypeInfo);
      do_maps(NT_HASH, "hash", hashTypeInfo);
      do_maps(NT_OBJECT, "object", objectTypeInfo);
      do_maps(NT_ALL, "any", anyTypeInfo);
      do_maps(NT_DATE, "date", dateTypeInfo);
      do_maps(NT_CODE, "code", codeTypeInfo);
      do_maps(NT_REFERENCE, "reference", referenceTypeInfo);
      do_maps(NT_NULL, "null", nullTypeInfo);
      do_maps(NT_NOTHING, "nothing", nothingTypeInfo);
   }
   DLLLOCAL ~QoreTypeInitializer() {
      // delete all values from default value map
      for (def_val_map_t::iterator i = def_val_map.begin(), e = def_val_map.end(); i != e; ++i)
	 i->second->deref(0);
   }
};

static QoreTypeInitializer qti;

void add_to_type_map(qore_type_t t, const QoreTypeInfo *typeInfo) {
   QoreAutoRWWriteLocker al(extern_type_info_map_lock);
   assert(extern_type_info_map.find(t) == extern_type_info_map.end());
   extern_type_info_map[t] = typeInfo;
}

const QoreTypeInfo *getTypeInfoForType(qore_type_t t) {
   type_typeinfo_map_t::iterator i = type_typeinfo_map.find(t);
   if (i != type_typeinfo_map.end())
      return i->second;

   const QoreTypeInfo *rv;
   {
      QoreAutoRWReadLocker al(extern_type_info_map_lock);
      type_typeinfo_map_t::iterator i = extern_type_info_map.find(t);
      rv = (i == extern_type_info_map.end() ? 0 : i->second);
   }

   //if (!rv) printd(0, "getTypeInfoForValue() %d not found in map\n", t);
   return rv;
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

const QoreTypeInfo *getBuiltinTypeInfo(const char *str) {
   str_typeinfo_map_t::iterator i = str_typeinfo_map.find(str);
   return i != str_typeinfo_map.end() ? i->second : 0;
}

const char *getBuiltinTypeName(qore_type_t type) {
   type_str_map_t::iterator i = type_str_map.find(type);
   assert(i != type_str_map.end());
   return i->second;
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
	 if (!runtimeCheckPrivateClassAccess(qc))
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

   return QTI_NOT_EQUAL;
}
