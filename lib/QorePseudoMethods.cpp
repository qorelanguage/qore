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

#include "intern/QoreClassIntern.h"

// pseudo object map
typedef std::map<qore_type_t, QoreClass *> po_map_t;

static po_map_t po_map;

static QoreClass *pseudoAll;

void pseudo_classes_init() {
   // root, default pseudo-class
   pseudoAll = initPseudoAllClass();
   po_map[NT_STRING] = initPseudoStringClass(pseudoAll);
   po_map[NT_LIST] = initPseudoListClass(pseudoAll);
   po_map[NT_HASH] = initPseudoHashClass(pseudoAll);
   po_map[NT_OBJECT] = initPseudoObjectClass(pseudoAll);   
}

void pseudo_classes_del() {
   for (po_map_t::iterator i = po_map.begin(), e = po_map.end(); i != e; ++i)
      delete i->second;
}

AbstractQoreNode *pseudo_classes_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   po_map_t::iterator i = po_map.find(get_node_type(n));
   QoreClass *qc = i == po_map.end() ? pseudoAll : i->second;

   return qore_class_private::evalPseudoMethod(qc, n, name, args, xsink);
}

const QoreMethod *pseudo_classes_find_method(qore_type_t t, const char *mname, QoreClass *&qc) {
   po_map_t::iterator i = po_map.find(t);
   QoreClass *nqc = i == po_map.end() ? pseudoAll : i->second;

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
