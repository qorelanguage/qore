/*
  Pseudo_QC_Date.cpp

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

static QoreBigIntNode *n_DATE;

// int <date>.typeCode() {}
static AbstractQoreNode *PSEUDODATE_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return n_DATE->refSelf();
}

// bool <date>.absolute() {}
static AbstractQoreNode *PSEUDODATE_absolute(QoreObject *ignored, DateTimeNode *dt, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(dt->isAbsolute());
}

// bool <date>.relative() {}
static AbstractQoreNode *PSEUDODATE_relative(QoreObject *ignored, DateTimeNode *dt, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(dt->isRelative());
}

QoreClass *initPseudoDateClass(QoreClass *pseudoAll) {   
   n_DATE = Node_NT_Array[NT_DATE];

   QoreClass *QC_PseudoDate = new QoreClass("<date>");

   QC_PseudoDate->addBuiltinVirtualBaseClass(pseudoAll);

   // int <date>.typeCode() {}
   QC_PseudoDate->addMethodExtended("typeCode", (q_method_t)PSEUDODATE_typeCode, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // bool <date>.absolute() {}
   QC_PseudoDate->addMethodExtended("absolute", (q_method_t)PSEUDODATE_absolute, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);

   // bool <date>.relative() {}
   QC_PseudoDate->addMethodExtended("relative", (q_method_t)PSEUDODATE_relative, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);

   return QC_PseudoDate;
}
