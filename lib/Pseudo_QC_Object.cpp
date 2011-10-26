/*
  Pseudo_QC_Object.cpp

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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
#include <qore/intern/QoreObjectIntern.h>

// int <object>.typeCode() {}
static int64 PSEUDOOBJECT_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_OBJECT;
}

// list <object>.keys() {}
static AbstractQoreNode *PSEUDOOBJECT_keys(QoreObject *ignored, QoreObject *obj, const QoreListNode *args, ExceptionSink *xsink) {
   return obj->getMemberList(xsink);
}

// *string <object>.firstKey() {}
static AbstractQoreNode *PSEUDOOBJECT_firstKey(QoreObject *ignored, QoreObject *obj, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_object_private::firstKey(obj, xsink);
}

// *string <object>.lastKey() {}
static AbstractQoreNode *PSEUDOOBJECT_lastKey(QoreObject *ignored, QoreObject *obj, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_object_private::lastKey(obj, xsink);
}

// bool <object>.empty() {}
static bool PSEUDOOBJECT_empty(QoreObject *ignored, QoreObject *obj, const QoreListNode *args, ExceptionSink *xsink) {
   return !obj->size(xsink);
}

// int <object>.size() {}
static int64 PSEUDOOBJECT_size(QoreObject *ignored, QoreObject *obj, const QoreListNode *args, ExceptionSink *xsink) {
   return obj->size(xsink);
}

// string <object>.className() {}
static AbstractQoreNode *PSEUDOOBJECT_className(QoreObject *ignored, QoreObject *obj, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreStringNode(obj->getClassName());
}

// bool <object>.isSystem() {}
static bool PSEUDOOBJECT_isSystem(QoreObject *ignored, QoreObject *obj, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(obj->isSystemObject());
}

QoreClass *initPseudoObjectClass(QoreClass *pseudoAll) {   
   QoreClass *QC_PseudoObject = new QoreClass("<object>");

   QC_PseudoObject->addBuiltinVirtualBaseClass(pseudoAll);

   // int <object>.typeCode() {}
   QC_PseudoObject->addMethodExtended("typeCode", (q_method_int64_t)PSEUDOOBJECT_typeCode, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // list <object>.keys() {}
   QC_PseudoObject->addMethodExtended("keys", (q_method_t)PSEUDOOBJECT_keys, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, listTypeInfo);

   // *string <object>.firstKey() {}
   QC_PseudoObject->addMethodExtended("firstKey", (q_method_t)PSEUDOOBJECT_firstKey, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string <object>.lastKey() {}
   QC_PseudoObject->addMethodExtended("lastKey", (q_method_t)PSEUDOOBJECT_lastKey, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

/*
   // bool <object>.hasKey(softstring $key) {}
   QC_PseudoObject->addMethodExtended("hasKey", (q_method_bool_t)PSEUDOOBJECT_hasKey, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo, 1, softStringTypeInfo, NULL);

   // bool <object>.hasKeyValue(softstring $key) {}
   QC_PseudoObject->addMethodExtended("hasKeyValue", (q_method_bool_t)PSEUDOOBJECT_hasKeyValue, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo, 1, softStringTypeInfo, NULL);
*/

   // bool <object>.empty() {}
   QC_PseudoObject->addMethodExtended("empty", (q_method_bool_t)PSEUDOOBJECT_empty, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   // int <object>.size() {}
   QC_PseudoObject->addMethodExtended("size", (q_method_int64_t)PSEUDOOBJECT_size, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // string <object>.className() {}
   QC_PseudoObject->addMethodExtended("className", (q_method_t)PSEUDOOBJECT_className, false, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);

   // bool <object>.isSystem() {}
   QC_PseudoObject->addMethodExtended("isSystem", (q_method_bool_t)PSEUDOOBJECT_isSystem, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);
  
   return QC_PseudoObject;
}
