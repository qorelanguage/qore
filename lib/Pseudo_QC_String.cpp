/*
  Pseudo_QC_String.cpp

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

static QoreBigIntNode *n_STRING;

// int <string>.typeCode() {}
static AbstractQoreNode *PSEUDOSTRING_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return n_STRING->refSelf();
}

// int <string>.strlen() {}
static AbstractQoreNode *PSEUDOSTRING_strlen(QoreObject *ignored, QoreStringNode *str, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(str->strlen());
}

// int <string>.length() {}
static AbstractQoreNode *PSEUDOSTRING_length(QoreObject *ignored, QoreStringNode *str, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(str->length());
}

// bool <string>.empty() {}
static AbstractQoreNode *PSEUDOSTRING_empty(QoreObject *ignored, QoreStringNode *str, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(str->empty());
}

// string <string>.encoding() {}
static AbstractQoreNode *PSEUDOSTRING_encoding(QoreObject *ignored, QoreStringNode *str, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreStringNode(str->getEncoding()->getCode());
}

QoreClass *initPseudoStringClass(QoreClass *pseudoAll) {   
   n_STRING = Node_NT_Array[NT_STRING];

   QoreClass *QC_PseudoString = new QoreClass("<string>");

   QC_PseudoString->addBuiltinVirtualBaseClass(pseudoAll);

    // int <string>.typeCode() {}
   QC_PseudoString->addMethodExtended("typeCode", (q_method_t)PSEUDOSTRING_typeCode, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // int <string>.strlen() {}
   QC_PseudoString->addMethodExtended("strlen", (q_method_t)PSEUDOSTRING_strlen, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // int <string>.size() {} (alias for <string>.strlen())
   QC_PseudoString->addMethodExtended("size", (q_method_t)PSEUDOSTRING_strlen, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // int <string>.length() {}
   QC_PseudoString->addMethodExtended("length", (q_method_t)PSEUDOSTRING_length, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // int <string>.elements() {}
   QC_PseudoString->addMethodExtended("elements", (q_method_t)PSEUDOSTRING_length, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // bool <string>.empty() {}
   QC_PseudoString->addMethodExtended("empty", (q_method_t)PSEUDOSTRING_empty, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);

   // string <string>.encoding() {}
   QC_PseudoString->addMethodExtended("encoding", (q_method_t)PSEUDOSTRING_encoding, false, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);

   return QC_PseudoString;
}
