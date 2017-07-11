/*
  QorePseudoMethods.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
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

#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/QoreLibIntern.h"

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
      qore_class_private::get(*po_list[i])->deref();

   qore_class_private::get(*QC_PSEUDOVALUE)->deref();
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

QoreValue pseudo_classes_eval(const QoreValue n, const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   return qore_class_private::evalPseudoMethod(pseudo_get_class(n.getType()), n, name, args, xsink);
}

const QoreMethod* pseudo_classes_find_method(qore_type_t t, const char *mname, QoreClass* &qc) {
   QoreClass* nqc = pseudo_get_class(t);

   const QoreMethod* m = nqc->findMethod(mname);
   if (m)
      qc = nqc;
   return m;
}

const QoreMethod* pseudo_classes_find_method(const QoreTypeInfo *typeInfo, const char *mname, QoreClass* &qc, bool &possible_match) {
   assert(typeInfo);

   const QoreMethod* m;
   if (typeInfo->return_vec.size() == 1) {
      m = pseudo_classes_find_method(typeInfo->return_vec[0].spec.getType(), mname, qc);
      possible_match = m ? true : false;
      return m;
   }

   possible_match = false;
   QoreClass* nqc;
   for (auto& i : typeInfo->return_vec) {
      if (pseudo_classes_find_method(i.spec.getType(), mname, nqc)) {
         possible_match = true;
         return nullptr;
      }
   }

   return nullptr;
}
