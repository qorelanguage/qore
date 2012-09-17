/*
  QorePseudoMethods.cpp

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

#include <map>

#include "Pseudo_QC_All.cpp"
#include "Pseudo_QC_Nothing.cpp"
#include "Pseudo_QC_Bool.cpp"
#include "Pseudo_QC_Int.cpp"
#include "Pseudo_QC_Float.cpp"
#include "Pseudo_QC_Number.cpp"
#include "Pseudo_QC_String.cpp"
#include "Pseudo_QC_List.cpp"
#include "Pseudo_QC_Hash.cpp"
#include "Pseudo_QC_Object.cpp"
#include "Pseudo_QC_Date.cpp"
#include "Pseudo_QC_Binary.cpp"
#include "Pseudo_QC_Callref.cpp"
#include "Pseudo_QC_Closure.cpp"

#include "intern/QoreClassIntern.h"

// list of pseudo-classes for basic types + 2 entries for closures and call references
static QoreClass* po_list[NODE_ARRAY_LEN + 2];

// int <x>.typeCode()
static int64 PSEUDONULL_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_NULL;
}

// create pseudo-class for type
static QoreClass* do_type_code(const char *name, q_method_int64_t f) {
   QoreClass* qc = new QoreClass(name);
   qc->addBuiltinVirtualBaseClass(QC_PSEUDOVALUE);
   qc->addMethodExtended3("typeCode", f, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);
   return qc;
}

void pseudo_classes_init() {
   // root, default pseudo-class
   initPseudoValueClass();
   assert(QC_PSEUDOVALUE);

   // initialize list of pseudo-classes for basic types
   po_list[NT_NOTHING] = initPseudoNothingClass(); //do_type_code("<nothing>", (q_method_int64_t)PSEUDONOTHING_typeCode);
   po_list[NT_NULL] = do_type_code("<null>", (q_method_int64_t)PSEUDONULL_typeCode);

   po_list[NT_INT] = initPseudoIntClass();
   po_list[NT_FLOAT] = initPseudoFloatClass();
   po_list[NT_BOOLEAN] = initPseudoBoolClass();
   po_list[NT_STRING] = initPseudoStringClass();
   po_list[NT_DATE] = initPseudoDateClass();
   po_list[NT_BINARY] = initPseudoBinaryClass();
   po_list[NT_LIST] = initPseudoListClass();
   po_list[NT_HASH] = initPseudoHashClass();
   po_list[NT_OBJECT] = initPseudoObjectClass();
   po_list[NT_NUMBER] = initPseudoNumberClass();

   // + 2 pseudo classes with runtime type values outside the value type range
   po_list[NODE_ARRAY_LEN] = initPseudoCallrefClass();
   po_list[NODE_ARRAY_LEN + 1] = initPseudoClosureClass();
}

void pseudo_classes_del() {
   // delete pseudo-classes
   for (unsigned i = 0; i < NODE_ARRAY_LEN + 2; ++i)
      delete po_list[i];

   delete QC_PSEUDOVALUE;
}

// return the pseudo class for the given type
static QoreClass* pseudo_get_class(qore_type_t t) {
   assert(t >= 0);
   if (t < NODE_ARRAY_LEN)
      return po_list[t];
   if (t == NT_FUNCREF)
      return po_list[NODE_ARRAY_LEN];
   if (t == NT_RUNTIME_CLOSURE)
      return po_list[NODE_ARRAY_LEN + 1];

   return QC_PSEUDOVALUE;
}

// return the pseudo class for the given node
static QoreClass* pseudo_get_class(const AbstractQoreNode *n) {
   return pseudo_get_class(get_node_type(n));
}

AbstractQoreNode *pseudo_classes_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_class_private::evalPseudoMethod(pseudo_get_class(n), n, name, args, xsink);
}

int64 pseudo_classes_int64_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_class_private::bigIntEvalPseudoMethod(pseudo_get_class(n), n, name, args, xsink);
}

int pseudo_classes_int_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_class_private::intEvalPseudoMethod(pseudo_get_class(n), n, name, args, xsink);
}

bool pseudo_classes_bool_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_class_private::boolEvalPseudoMethod(pseudo_get_class(n), n, name, args, xsink);
}

double pseudo_classes_double_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_class_private::floatEvalPseudoMethod(pseudo_get_class(n), n, name, args, xsink);
}

const QoreMethod* pseudo_classes_find_method(qore_type_t t, const char *mname, QoreClass* &qc) {
   QoreClass* nqc = pseudo_get_class(t);

   const QoreMethod* m = nqc->findMethod(mname);
   if (m)
      qc = nqc;
   return m;
}

const QoreMethod* pseudo_classes_find_method(const QoreTypeInfo *typeInfo, const char *mname, QoreClass* &qc, bool &possible_match) {
   assert(typeInfo->hasType());

   const QoreMethod* m;
   if (typeInfo->returnsSingle()) {
      m = pseudo_classes_find_method(typeInfo->getSingleType(), mname, qc);
      possible_match = m ? true : false;
      return m;
   }

   possible_match = false;
   const type_vec_t &tv = typeInfo->getReturnTypeList();
   QoreClass* nqc;
   for (type_vec_t::const_iterator i = tv.begin(), e = tv.end(); i != e; ++i) {
      if (!(*i)->returnsSingle()) {
	 pseudo_classes_find_method(*i, mname, nqc, possible_match);
	 if (possible_match)
	    return 0;
      }
      else {
	 if (pseudo_classes_find_method((*i)->getSingleType(), mname, nqc)) {
	    possible_match = true;
	    return 0;
	 }
      }
   }

   return 0;
}
