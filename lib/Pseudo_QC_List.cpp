/*
  Pseudo_QC_List.cpp

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

// int <list>.size() {}
static AbstractQoreNode *PSEUDOLIST_size(QoreObject *ignored, QoreListNode *l, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(l->size());
}

// bool <list>.empty() {}
static AbstractQoreNode *PSEUDOLIST_empty(QoreObject *ignored, QoreListNode *l, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(l->empty());
}

QoreClass *initPseudoListClass(QoreClass *pseudoAll) {   
   QoreClass *QC_PseudoList = new QoreClass("<list>");

   QC_PseudoList->addBuiltinVirtualBaseClass(pseudoAll);

   // int <list>.size() {}
   QC_PseudoList->addMethodExtended("size", (q_method_t)PSEUDOLIST_size, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // int <list>.elements() {}
   QC_PseudoList->addMethodExtended("elements", (q_method_t)PSEUDOLIST_size, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // bool <list>.empty() {}
   QC_PseudoList->addMethodExtended("empty", (q_method_t)PSEUDOLIST_empty, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);

   return QC_PseudoList;
}
