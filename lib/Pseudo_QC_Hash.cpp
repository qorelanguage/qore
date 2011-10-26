/*
  Pseudo_QC_Hash.cpp

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

static QoreBigIntNode *n_HASH;

// int <hash>.typeCode() {}
static AbstractQoreNode *PSEUDOHASH_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return n_HASH->refSelf();
}

// list <hash>.keys() {}
static AbstractQoreNode *PSEUDOHASH_keys(QoreObject *ignored, QoreHashNode *h, const QoreListNode *args, ExceptionSink *xsink) {
   return h->getKeys();
}

// *string <hash>.firstKey() {}
static AbstractQoreNode *PSEUDOHASH_firstKey(QoreObject *ignored, QoreHashNode *h, const QoreListNode *args, ExceptionSink *xsink) {
   const char *key = h->getFirstKey();
   return key ? new QoreStringNode(key) : 0;
}

// *string <hash>.lastKey() {}
static AbstractQoreNode *PSEUDOHASH_lastKey(QoreObject *ignored, QoreHashNode *h, const QoreListNode *args, ExceptionSink *xsink) {
   const char *key = h->getLastKey();
   return key ? new QoreStringNode(key) : 0;
}

// bool <hash>.hasKey(softstring $key) {}
static AbstractQoreNode *PSEUDOHASH_hasKey(QoreObject *ignored, QoreHashNode *h, const QoreListNode *args, ExceptionSink *xsink) {
   TempEncodingHelper str(HARD_QORE_STRING(args, 0), QCS_DEFAULT, xsink);
   if (!str)
      return 0;

   return get_bool_node(h->existsKey(str->getBuffer()));
}

// bool <hash>.hasKeyValue(softstring $key) {}
static AbstractQoreNode *PSEUDOHASH_hasKeyValue(QoreObject *ignored, QoreHashNode *h, const QoreListNode *args, ExceptionSink *xsink) {
   TempEncodingHelper str(HARD_QORE_STRING(args, 0), QCS_DEFAULT, xsink);
   if (!str)
      return 0;

   return get_bool_node(h->existsKeyValue(str->getBuffer()));
}

// bool <hash>.empty() {}
static AbstractQoreNode *PSEUDOHASH_empty(QoreObject *ignored, QoreHashNode *h, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(h->empty());
}

// int <hash>.size() {}
static AbstractQoreNode *PSEUDOHASH_size(QoreObject *ignored, QoreHashNode *h, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(h->size());
}

QoreClass *initPseudoHashClass(QoreClass *pseudoAll) {
   n_HASH = Node_NT_Array[NT_HASH];
   
   QoreClass *QC_PseudoHash = new QoreClass("<hash>");

   QC_PseudoHash->addBuiltinVirtualBaseClass(pseudoAll);

   // int <hash>.typeCode() {}
   QC_PseudoHash->addMethodExtended("typeCode", (q_method_t)PSEUDOHASH_typeCode, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // list <hash>.keys() {}
   QC_PseudoHash->addMethodExtended("keys", (q_method_t)PSEUDOHASH_keys, false, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo);

   // *string <hash>.firstKey() {}
   QC_PseudoHash->addMethodExtended("firstKey", (q_method_t)PSEUDOHASH_firstKey, false, QC_CONSTANT, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string <hash>.lastKey() {}
   QC_PseudoHash->addMethodExtended("lastKey", (q_method_t)PSEUDOHASH_lastKey, false, QC_CONSTANT, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // bool <hash>.hasKey(softstring $key) {}
   QC_PseudoHash->addMethodExtended("hasKey", (q_method_t)PSEUDOHASH_hasKey, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo, 1, softStringTypeInfo, NULL);

   // bool <hash>.hasKeyValue(softstring $key) {}
   QC_PseudoHash->addMethodExtended("hasKeyValue", (q_method_t)PSEUDOHASH_hasKeyValue, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo, 1, softStringTypeInfo, NULL);

   // bool <hash>.empty() {}
   QC_PseudoHash->addMethodExtended("empty", (q_method_t)PSEUDOHASH_empty, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);

   // int <hash>.size() {}
   QC_PseudoHash->addMethodExtended("size", (q_method_t)PSEUDOHASH_size, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);
  
   // int <hash>.elements() {}
   QC_PseudoHash->addMethodExtended("elements", (q_method_t)PSEUDOHASH_size, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);
  
   return QC_PseudoHash;
}
