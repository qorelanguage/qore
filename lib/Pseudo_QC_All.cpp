/*
  Pseudo_QC_All.cpp

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

// int <obj>.typeCode()
static int64 PSEUDOALL_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return get_node_type(node);
}

// string <obj>.type()
static AbstractQoreNode *PSEUDOALL_type(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreStringNode(node ? node->getTypeName() : "nothing");
}

QoreClass *initPseudoAllClass() {
   QoreClass *QC_PseudoAll = new QoreClass("<value>");

   QC_PseudoAll->addMethodExtended("typeCode", (q_method_int64_t)PSEUDOALL_typeCode, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   QC_PseudoAll->addMethodExtended("type", (q_method_t)PSEUDOALL_type, false, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);
   
   return QC_PseudoAll;
}
