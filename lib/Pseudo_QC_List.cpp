/*
  Pseudo_QC_List.cpp

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

// int <list>.typeCode() {}
static int64 PSEUDOLIST_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_LIST;
}

// int <list>.size() {}
static int64 PSEUDOLIST_size(QoreObject *ignored, QoreListNode *l, const QoreListNode *args, ExceptionSink *xsink) {
   return l->size();
}

// bool <list>.empty() {}
static bool PSEUDOLIST_empty(QoreObject *ignored, QoreListNode *l, const QoreListNode *args, ExceptionSink *xsink) {
   return l->empty();
}

QoreClass *initPseudoListClass(QoreClass *pseudoAll) {   
   QoreClass *QC_PseudoList = new QoreClass("<list>");

   QC_PseudoList->addBuiltinVirtualBaseClass(pseudoAll);

   // int <list>.typeCode() {}
   QC_PseudoList->addMethodExtended("typeCode", (q_method_int64_t)PSEUDOLIST_typeCode, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // int <list>.size() {}
   QC_PseudoList->addMethodExtended("size", (q_method_int64_t)PSEUDOLIST_size, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // bool <list>.empty() {}
   QC_PseudoList->addMethodExtended("empty", (q_method_bool_t)PSEUDOLIST_empty, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);

   return QC_PseudoList;
}
