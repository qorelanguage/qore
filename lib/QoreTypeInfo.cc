/*
 QoreTypeInfo.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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

// global reference types
QoreTypeInfo bigIntTypeInfo(NT_INT), 
   floatTypeInfo(NT_FLOAT),
   boolTypeInfo(NT_BOOLEAN),
   stringTypeInfo(NT_STRING),
   binaryTypeInfo(NT_BINARY),
   objectTypeInfo(NT_OBJECT),
   dateTypeInfo(NT_DATE),
   hashTypeInfo(NT_HASH),
   listTypeInfo(NT_LIST),
   nothingTypeInfo(NT_NOTHING),
   nullTypeInfo(NT_NULL),
   runTimeClosureTypeInfo(NT_RUNTIME_CLOSURE),
   callReferenceTypeInfo(NT_FUNCREF)
   ;

AbstractQoreNode *getDefaultValueForBuitinValueType(qore_type_t t) {
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
   }

   assert(false);
   return "<unknown>";
}

void QoreParseTypeInfo::resolve() {
   if (cscope) {
      // resolve class
      qc = getRootNS()->parseFindScopedClass(cscope);
      delete cscope;
      cscope = 0;
   }
}
