/*
 QoreTypeInfo.cc
 
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

// global external type map
static type_info_map_t type_info_map;

// rwlock for global type map
static QoreRWLock type_info_map_lock;

// static reference types
static QoreTypeInfo staticBigIntTypeInfo(NT_INT), 
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

// const pointers to static reference types
const QoreTypeInfo *bigIntTypeInfo = &staticBigIntTypeInfo,
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
   *referenceTypeInfo = &staticReferenceTypeInfo
   ;

void add_to_type_map(qore_type_t t, const QoreTypeInfo *typeInfo) {
   QoreAutoRWWriteLocker al(type_info_map_lock);
   assert(type_info_map.find(t) == type_info_map.end());
   type_info_map[t] = typeInfo;
}

const QoreTypeInfo *getTypeInfoForType(qore_type_t t) {
   switch (t) {
      case NT_INT:
	 return bigIntTypeInfo;
      case NT_STRING:
	 return stringTypeInfo;
      case NT_BOOLEAN:
	 return boolTypeInfo;
      case NT_FLOAT:
	 return floatTypeInfo;
      case NT_BINARY:
	 return binaryTypeInfo;
      case NT_LIST:
	 return listTypeInfo;
      case NT_HASH:
	 return hashTypeInfo;
      case NT_OBJECT:
	 return objectTypeInfo;
      case NT_DATE:
	 return dateTypeInfo;
      case NT_NULL:
	 return nullTypeInfo;
      case NT_NOTHING:
	 return nothingTypeInfo;
   }
   const QoreTypeInfo *rv;
   {
      QoreAutoRWReadLocker al(type_info_map_lock);
      rv = type_info_map[t];
   }

#ifdef DEBUG
   if (!rv) printd(0, "getTypeInfoForValue() %d not found in map\n", t);
   assert(rv);
#endif
   return rv;
}

const QoreTypeInfo *getTypeInfoForValue(const AbstractQoreNode *n) {
   qore_type_t t = n ? n->getType() : NT_NOTHING;
   if (t != NT_OBJECT)
      return getTypeInfoForType(t);
   return reinterpret_cast<const QoreObject *>(n)->getClass()->getTypeInfo();
}

AbstractQoreNode *getDefaultValueForBuiltinValueType(qore_type_t t) {
   switch (t) {
      case NT_INT:
	 return new QoreBigIntNode(0);
      case NT_STRING:
	 return new QoreStringNode;
      case NT_BOOLEAN:
	 return &False;
      case NT_DATE:
	 return new DateTimeNode((int64)0);
      case NT_FLOAT:
	 return new QoreFloatNode(0.0);
      case NT_LIST:
	 return new QoreListNode;
      case NT_HASH:
	 return new QoreHashNode;
      case NT_BINARY:
	 return new BinaryNode;
      case NT_NULL:
	 return &Null;
      case NT_NOTHING:
      case NT_OBJECT:
	 return &Nothing;
   }

   assert(false);
   return 0;
}

qore_type_t getBuiltinType(const char *str) {
   if (!strcmp(str, "int"))
      return NT_INT;
   if (!strcmp(str, "string"))
      return NT_STRING;
   if (!strcmp(str, "bool"))
      return NT_BOOLEAN;
   if (!strcmp(str, "date"))
      return NT_DATE;
   if (!strcmp(str, "float"))
      return NT_FLOAT;
   if (!strcmp(str, "list"))
      return NT_LIST;
   if (!strcmp(str, "hash"))
      return NT_HASH;
   if (!strcmp(str, "object"))
      return NT_OBJECT;
   if (!strcmp(str, "binary"))
      return NT_BINARY;
   // these last two don't make much sense to use, but...
   if (!strcmp(str, "null"))
      return NT_NULL;
   if (!strcmp(str, "nothing"))
      return NT_NOTHING;

   return -1;
}

const char *getBuiltinTypeName(qore_type_t type) {
   switch (type) {
      case NT_INT:
	 return "int";
      case NT_STRING:
	 return "string";
      case NT_BOOLEAN:
	 return "bool";
      case NT_DATE:
	 return "date";
      case NT_FLOAT:
	 return "float";
      case NT_LIST:
	 return "list";
      case NT_HASH:
	 return "hash";
      case NT_OBJECT:
	 return "object";
      case NT_BINARY:
	 return "binary";
	 // these last two don't make much sense to use, but...
      case NT_NULL:
	 return "null";
      case NT_NOTHING:
	 return "nothing";
      case NT_REFERENCE:
	 return "reference to lvalue";
   }

   assert(false);
   return "<unknown>";
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
