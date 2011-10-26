/*
  QorePseudoMethods.cpp

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

#include <map>

#include "Pseudo_QC_All.cpp"
#include "Pseudo_QC_String.cpp"
#include "Pseudo_QC_List.cpp"
#include "Pseudo_QC_Hash.cpp"
#include "Pseudo_QC_Object.cpp"
#include "Pseudo_QC_Date.cpp"
#include "Pseudo_QC_Binary.cpp"

#include "intern/QoreClassIntern.h"

// list of pseudo-classes for basic types
static QoreClass *po_list[NODE_ARRAY_LEN];

// list of node type values for basic types
QoreBigIntNode *Node_NT_Array[NODE_ARRAY_LEN];

// catch-all/base pseudo class
static QoreClass *pseudoAll;

// node pointers for basic types with pseudo-classes implemented in this file
static QoreBigIntNode *NT_nothing, *NT_null, *NT_int, *NT_float, *NT_boolean;

// int <x>.typeCode()
static AbstractQoreNode *PSEUDONOTHING_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_nothing->refSelf();
}
static AbstractQoreNode *PSEUDONULL_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_null->refSelf();
}
static AbstractQoreNode *PSEUDOINT_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_int->refSelf();
}
static AbstractQoreNode *PSEUDOFLOAT_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_float->refSelf();
}
static AbstractQoreNode *PSEUDOBOOLEAN_typeCode(QoreObject *ignored, AbstractQoreNode *node, const QoreListNode *args, ExceptionSink *xsink) {
   return NT_boolean->refSelf();
}

// create pseudo-class for type
static QoreClass *do_type_code(const char *name, q_method_t f) {
   QoreClass *qc = new QoreClass(name);
   qc->addMethodExtended("typeCode", f, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);
   return qc;
}

void pseudo_classes_init() {
   // initialize array of default node values
   for (unsigned i = 0; i < NODE_ARRAY_LEN; ++i)
      Node_NT_Array[i] = new QoreBigIntNode(i);

   NT_nothing = Node_NT_Array[NT_NOTHING];
   NT_null = Node_NT_Array[NT_NULL];
   NT_int = Node_NT_Array[NT_INT];
   NT_float = Node_NT_Array[NT_FLOAT];
   NT_boolean = Node_NT_Array[NT_BOOLEAN];

   // root, default pseudo-class
   pseudoAll = initPseudoAllClass();

   // initialize list of pseudo-classes for basic types
   po_list[NT_NOTHING] = do_type_code("<nothing>", (q_method_t)PSEUDONOTHING_typeCode);
   po_list[NT_NULL] = do_type_code("<nothing>", (q_method_t)PSEUDONULL_typeCode);
   po_list[NT_INT] = do_type_code("<nothing>", (q_method_t)PSEUDOINT_typeCode);
   po_list[NT_FLOAT] = do_type_code("<nothing>", (q_method_t)PSEUDOFLOAT_typeCode);
   po_list[NT_BOOLEAN] = do_type_code("<nothing>", (q_method_t)PSEUDOBOOLEAN_typeCode);

   po_list[NT_STRING] = initPseudoStringClass(pseudoAll);
   po_list[NT_DATE] = initPseudoDateClass(pseudoAll);
   po_list[NT_BINARY] = initPseudoBinaryClass(pseudoAll);
   po_list[NT_LIST] = initPseudoListClass(pseudoAll);
   po_list[NT_HASH] = initPseudoHashClass(pseudoAll);
   po_list[NT_OBJECT] = initPseudoObjectClass(pseudoAll);   
}

void pseudo_classes_del() {
   // dereference array of default node values
   for (unsigned i = 0; i < NODE_ARRAY_LEN; ++i)
      Node_NT_Array[i]->deref(0);

   // delete pseudo-classes
   for (unsigned i = 0; i < NODE_ARRAY_LEN; ++i)
      delete po_list[i];
}

// return the pseudo class for the given type
static QoreClass *pseudo_get_class(qore_type_t t) {
   assert(t >= 0);
   return t < NODE_ARRAY_LEN ? po_list[t] : pseudoAll;
}

// return the pseudo class for the given node
static QoreClass *pseudo_get_class(const AbstractQoreNode *n) {
   return pseudo_get_class(get_node_type(n));
}

AbstractQoreNode *pseudo_classes_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_class_private::evalPseudoMethod(pseudo_get_class(n), n, name, args, xsink);
}

const QoreMethod *pseudo_classes_find_method(qore_type_t t, const char *mname, QoreClass *&qc) {
   QoreClass *nqc = pseudo_get_class(t);

   const QoreMethod *m = nqc->findMethod(mname);
   if (m)
      qc = nqc;
   return m;
}

const QoreMethod *pseudo_classes_find_method(const QoreTypeInfo *typeInfo, const char *mname, QoreClass *&qc, bool &possible_match) {
   assert(typeInfo->hasType());

   const QoreMethod *m;
   if (typeInfo->returnsSingle()) {
      m = pseudo_classes_find_method(typeInfo->getSingleType(), mname, qc);
      possible_match = m ? true : false;
      return m;
   }

   possible_match = false;
   const type_vec_t &tv = typeInfo->getReturnTypeList();
   QoreClass *nqc;
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
