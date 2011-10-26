/*
  Pseudo_QC_Binary.cpp

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

// int <binary>.typeCode() {}
static int64 PSEUDOBINARY_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_BINARY;
}

// int <binary>.size() {}
static int64 PSEUDOBINARY_size(QoreObject *ignored, BinaryNode *b, const QoreListNode *args, ExceptionSink *xsink) {
   return b->size();
}

// bool <binary>.empty() {}
static bool PSEUDOBINARY_empty(QoreObject *ignored, BinaryNode *b, const QoreListNode *args, ExceptionSink *xsink) {
   return b->empty();
}

QoreClass *initPseudoBinaryClass(QoreClass *pseudoAll) {   
   QoreClass *QC_PseudoBinary = new QoreClass("<binary>");

   QC_PseudoBinary->addBuiltinVirtualBaseClass(pseudoAll);

   // int <binary>.typeCode() {}
   QC_PseudoBinary->addMethodExtended("typeCode", (q_method_int64_t)PSEUDOBINARY_typeCode, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // int <binary>.size() {}
   QC_PseudoBinary->addMethodExtended("size", (q_method_int64_t)PSEUDOBINARY_size, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // int <binary>.elements() {}
   QC_PseudoBinary->addMethodExtended("elements", (q_method_int64_t)PSEUDOBINARY_size, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // bool <binary>.empty() {}
   QC_PseudoBinary->addMethodExtended("empty", (q_method_bool_t)PSEUDOBINARY_empty, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);

   return QC_PseudoBinary;
}
