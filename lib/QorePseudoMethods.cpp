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

bool pseudo_classes_find_method(qore_type_t t, const char *mname) {
   po_map_t::iterator i = po_map.find(t);
   QoreClass *qc = i == po_map.end() ? pseudoAll : i->second;

   return qc->findMethod(mname);
}
